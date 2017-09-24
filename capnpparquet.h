/*
 * Copyright 2017 Rene Sugar
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * @file capnpparquet.h
 * @author Rene Sugar <rene.sugar@gmail.com>
 * @brief Convert a Cap'n Proto schema to a Parquet schema.
 */
#ifndef _CAPNPPARQUET_H_
#define _CAPNPPARQUET_H_

#include <parquet/types.h>
#include <parquet/util/macros.h>
#include <parquet/util/visibility.h>
#include <parquet/schema.h>

#include <algorithm>
#include <cstdint>
#include <cmath>
#include <memory>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "capnpgeneric.h"

/*
 
 Cap'n Proto Types      Parquet Physical Type       Parquet Logical Type        Thrift Type   Arrow Type
 -----------------      ---------------------       --------------------        -----------   -----------------
 VOID,                  FIXED_LEN_BYTE_ARRAY,       NONE,                       VOID,         FIXED_SIZE_BINARY
 BOOL,                  BOOLEAN,                    NONE,                       BOOL,         BOOL
 INT8,                  INT32,                      INT_8,                      I8,           INT8
 INT16,                 INT32,                      INT_16,                     I16,          INT16
 INT32,                 INT32,                      INT_32,                     I32,          INT32
 INT64,                 INT64,                      INT_64,                     I64,          INT64
 UINT8,                 INT32,                      UINT_8,                     I32,          UINT8
 UINT16,                INT32,                      UINT_16,                    I32,          UINT16
 UINT32,                INT64,                      UINT_32,                    I64,          UINT32
 UINT64,                INT64,                      UINT_64,                    I64,          UINT64
 FLOAT32,               FLOAT,                      NONE,                       DOUBLE,       FLOAT
 FLOAT64,               DOUBLE,                     NONE,                       DOUBLE,       DOUBLE
 TEXT,                  BYTE_ARRAY,                 UTF8,                       STRING,       STRING
 DATA,                  BYTE_ARRAY,                 NONE,                       BINARY,       BINARY
 LIST,                  BYTE_ARRAY,                 LIST,                       LIST,         LIST
 ENUM,                  BYTE_ARRAY,                 ENUM,                       ENUM,         STRING
 STRUCT,                BYTE_ARRAY,                 NONE,                       STRUCT,       STRUCT
 INTERFACE,             *                           *                           SERVICE,      *
 ANY_POINTER,           *                           *                           VOID          *
 DATA,                  BYTE_ARRAY,                 BSON,                       BINARY,       BINARY
 TEXT,                  BYTE_ARRAY,                 JSON,                       STRING,       STRING
 SET,                   BYTE_ARRAY,                 LIST,                       SET,          LIST
 MAP,                   BYTE_ARRAY,                 MAP,                        MAP,          DICTIONARY
 FLOAT64,               FIXED_LEN_BYTE_ARRAY,       DECIMAL (see (1), (9))      DOUBLE,       DECIMAL
 INT32,                 INT32,                      DATE,                       INT32,        DATE32
 INT32,                 INT32,                      DATE,                       INT32,        DATE64
 INT32,                 INT32,                      TIME_MILLIS,                INT32,        TIME32
 INT64,                 INT64,                      TIME_MICROS,                INT64,        TIME64
 INT64,                 INT64,                      TIMESTAMP_MILLIS,           INT64,        TIMESTAMP(TimeUnit::MILLI)
 INT64                  INT64,                      TIMESTAMP_MICROS,           INT64,        TIMESTAMP(TimeUnit::MICRO)
 *,                     *,                          *,                          *,            TIMESTAMP(TimeUnit::NANO)
 *,                     *,                          *,                          *,            TIMESTAMP(TimeUnit::SECOND)
 DATA,                  FIXED_LEN_BYTE_ARRAY(12)    INTERVAL,                   BINARY,       INTERVAL
 STRUCT,                BYTE_ARRAY,                 MAP_KEY_VALUE,              STRUCT,       STRUCT
 UNION,                 *,                          *,                          *,            UNION
 *,                     *,                          *,                          *,            NA
 *,                     *,                          *,                          *,            HALF_FLOAT
 
 References:
 
 (1)  https://github.com/apache/parquet-format/blob/master/LogicalTypes.md
 (2)  https://diwakergupta.github.io/thrift-missing-guide/
 (3)  http://thrift.apache.org/docs/idl
 (4)  http://thrift.apache.org/tutorial/
 (5)  https://github.com/apache/thrift/blob/master/compiler/cpp/src/thrift/thriftl.ll
 (6)  https://github.com/apache/thrift/blob/master/compiler/cpp/src/thrift/thrifty.yy
 (7)  https://github.com/apache/parquet-cpp/blob/master/src/parquet/arrow/arrow-reader-writer-test.cc
 (8)  https://gerrit.cloudera.org/#/c/6550/9/fe/src/main/java/org/apache/impala/analysis/CreateTableLikeFileStmt.java
 (9)  https://github.com/apache/spark/blob/master/sql/core/src/main/scala/org/apache/spark/sql/execution/datasources/parquet/ParquetSchemaConverter.scala
 (10) https://github.com/Parquet/parquet-mr/blob/master/parquet-avro/src/main/java/parquet/avro/AvroSchemaConverter.java
 (11) https://avro.apache.org/docs/current/spec.html#Enums
 (12) https://thrift.apache.org/docs/idl
 (13) https://github.com/apache/thrift/blob/master/lib/json/schema.json
 (14) https://github.com/apache/parquet-cpp/blob/master/src/parquet/schema.cc
 (15) https://github.com/Parquet/parquet-mr/blob/master/parquet-avro/src/main/java/parquet/avro/package-info.java
 (16) https://github.com/Parquet/parquet-mr/blob/master/parquet-avro/src/main/java/parquet/avro/AvroWriteSupport.java
 (17) https://www.cloudera.com/documentation/enterprise/5-5-x/topics/impala_complex_types.html#complex_types_nesting
 (18) https://github.com/apache/arrow/blob/master/cpp/src/arrow/type.h
 
  Excerpt from (9) (covers decimal, timestamp, arrays and maps)
 
 DECIMAL
 -------
 // ======================
 // Decimals (legacy mode)
 // ======================
 
 // Spark 1.4.x and prior versions only support decimals with a maximum precision of 18 and
 // always store decimals in fixed-length byte arrays.  To keep compatibility with these older
 // versions, here we convert decimals with all precisions to `FIXED_LEN_BYTE_ARRAY` annotated
 // by `DECIMAL`.

 // ========================
 // Decimals (standard mode)
 // ========================
 
 // Uses INT32 for 1 <= precision <= 9 (if precision <= Decimal.MAX_INT_DIGITS)
 
 // Uses INT64 for 1 <= precision <= 18 (if precision <= Decimal.MAX_LONG_DIGITS)
 
 // Uses FIXED_LEN_BYTE_ARRAY for all other precisions
 
 
 TIMESTAMP
 ---------
 // NOTE: Spark SQL TimestampType is NOT a well defined type in Parquet format spec.
 //
 // As stated in PARQUET-323, Parquet `INT96` was originally introduced to represent nanosecond
 // timestamp in Impala for some historical reasons.  It's not recommended to be used for any
 // other types and will probably be deprecated in some future version of parquet-format spec.
 // That's the reason why parquet-format spec only defines `TIMESTAMP_MILLIS` and
 // `TIMESTAMP_MICROS` which are both logical types annotating `INT64`.
 //
 // Originally, Spark SQL uses the same nanosecond timestamp type as Impala and Hive.  Starting
 // from Spark 1.5.0, we resort to a timestamp type with 100 ns precision so that we can store
 // a timestamp into a `Long`.  This design decision is subject to change though, for example,
 // we may resort to microsecond precision in the future.
 //
 // For Parquet, we plan to write all `TimestampType` value as `TIMESTAMP_MICROS`, but it's
 // currently not implemented yet because parquet-mr 1.8.1 (the version we're currently using)
 // hasn't implemented `TIMESTAMP_MICROS` yet, however it supports TIMESTAMP_MILLIS. We will
 // encode timestamp values as TIMESTAMP_MILLIS annotating INT64 if
 // 'spark.sql.parquet.int64AsTimestampMillis' is set.
 //
 // TODO Converts `TIMESTAMP_MICROS` once parquet-mr implements that.

 
 ARRAYS AND MAPS
 ---------------
 // ===================================
 // ArrayType and MapType (legacy mode)
 // ===================================
 //
 // "bag", "array", "list", "element", "key_value", "map", "key", "value" below are the names
 // used in the Parquet schema nodes.
 //
 // Spark 1.4.x and prior versions convert `ArrayType` with nullable elements into a 3-level
 // `LIST` structure.  This behavior is somewhat a hybrid of parquet-hive and parquet-avro
 // (1.6.0rc3): the 3-level structure is similar to parquet-hive while the 3rd level element
 // field name "array" is borrowed from parquet-avro.
 case ArrayType(elementType, nullable @ true) if writeLegacyParquetFormat =>
 // <list-repetition> group <name> (LIST) {
 //   optional group bag {
 //     repeated <element-type> array;
 //   }
 // }
 //
 // This should not use `listOfElements` here because this new method checks if the
 // element name is `element` in the `GroupType` and throws an exception if not.
 // As mentioned above, Spark prior to 1.4.x writes `ArrayType` as `LIST` but with
 // `array` as its element name as below. Therefore, we build manually
 // the correct group type here via the builder. (See SPARK-16777)
 //
 // Spark 1.4.x and prior versions convert ArrayType with non-nullable elements into a 2-level
 // LIST structure.  This behavior mimics parquet-avro (1.6.0rc3).  Note that this case is
 // covered by the backwards-compatibility rules implemented in `isElementType()`.
 //
 // <list-repetition> group <name> (LIST) {
 //   repeated <element-type> array;
 // }
 //
 // Here too, we should not use `listOfElements`. (See SPARK-16777)
 //
 // "array" is the name chosen by parquet-avro (1.7.0 and prior version)
 //
 // Spark 1.4.x and prior versions convert MapType into a 3-level group annotated by
 // MAP_KEY_VALUE.  This is covered by `convertGroupField(field: GroupType): DataType`.
 //
 // <map-repetition> group <name> (MAP) {
 //   repeated group map (MAP_KEY_VALUE) {
 //     required <key-type> key;
 //     <value-repetition> <value-type> value;
 //   }
 // }
 //
 // =====================================
 // ArrayType and MapType (standard mode)
 // =====================================
 //
 // <list-repetition> group <name> (LIST) {
 //   repeated group list {
 //     <element-repetition> <element-type> element;
 //   }
 // }
 //
 // <map-repetition> group <name> (MAP) {
 //   repeated group key_value {
 //     required <key-type> key;
 //     <value-repetition> <value-type> value;
 //   }
 // }

 Excerpt from (18):
 
 // static inline int decimal_byte_width(int precision) {
 //   if (precision >= 0 && precision < 10) {
 //     return 4;
 //   } else if (precision >= 10 && precision < 19) {
 //    return 8;
 //   } else {
 //     // TODO(phillipc): validate that we can't construct > 128 bit types
 //     return 16;
 //   }
 // }

 // case TimeUnit::SECOND:
 // case TimeUnit::MILLI:
 // case TimeUnit::MICRO:
 // case TimeUnit::NANO:

 */

/*
    Number of bits required to represent a DECIMAL number for a given precision (number of digits)
 
    bmax = ceil(d*(log(10)/log(2)))
    bmax = ceil(d*log2(10))
 
    Find the log base 2 of an integer with the MSB N set in O(N) operations (the obvious way)
 
    unsigned int v; // 32-bit word to find the log base 2 of
    unsigned int r = 0; // r will be lg(v)
 
    while (v >>= 1) // unroll for more speed...
    {
        r++;
    }
    The log base 2 of an integer is the same as the position of the highest bit set (or most significant bit set, MSB)
    The following log base 2 methods are faster than this one.
 
    References:
 
    (1) http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogObvious
    (2) http://www.exploringbinary.com/number-of-bits-in-a-decimal-integer/
    (3) https://github.com/apache/spark/blob/master/sql/core/src/main/scala/org/apache/spark/sql/execution/datasources/parquet/ParquetSchemaConverter.scala
 */

// TODO(renesugar): Add support for generic structures in capnpgeneric.h
//
// TODO(renesugar): Add support for Cap'n Proto groups in capnpgeneric.h
//       https://capnproto.org/language.html#structs
//

namespace capnpparquet {

// Cog code generation tool: https://nedbatchelder.com/code/cog/
//
// cog.py-2.7 -r capnpparquet.h
//

/*[[[cog
 def capitalize(name):
   return ''.join(s[0].upper() + s[1:] for s in name.split('_'))
 
 dict = {
 'repetition_type': [
 ('is_required', '', 'bool', '', '', 'parquet::Repetition::REQUIRED'),
 ('is_optional', '', 'bool', '', '', 'parquet::Repetition::OPTIONAL'),
 ('is_repeated', '', 'bool', '', '', 'parquet::Repetition::REPEATED'),
 ],
 
 'parquet_node_type': [
 ('is_parquet', 'primitive', 'bool', '', '', 'parquet::schema::Node::PRIMITIVE'),
 ('is_parquet', 'group',     'bool', '', '', 'parquet::schema::Node::GROUP'),
 ],
 
 'value': [
 ('value', 'bool',   'bool',     'bool',     'value', 'b'),
 ('value', 'i8',     'int8_t',   'int8_t',   'value', 'i8'),
 ('value', 'i16',    'int16_t',  'int16_t',  'value', 'i16'),
 ('value', 'i32',    'int32_t',  'int32_t',  'value', 'i32'),
 ('value', 'i64',    'int64_t',  'int64_t',  'value', 'i64'),
 ('value', 'ui8',    'uint8_t',  'uint8_t',  'value', 'ui8'),
 ('value', 'ui16',   'uint16_t', 'uint16_t', 'value', 'ui16'),
 ('value', 'ui32',   'uint32_t', 'uint32_t', 'value', 'ui32'),
 ('value', 'ui64',   'uint64_t', 'uint64_t', 'value', 'ui64'),
 ('value', 'float',  'float',    'float',    'value', 'f'),
 ('value', 'double', 'double',   'double',   'value', 'd'),
 ('value', 'string', 'std::string',             'std::string',             'value', 'string'),
 ('value', 'binary', 'std::vector<uint8_t>&',   'std::vector<uint8_t>&',   'value', 'binary')
 ]
 }
 
 types = [
 # property           get type                      set type                      param name     param value
 # --------           --------                      --------                      ----------     -----------
 ('node_type',       'ASTNode::type',              'ASTNode::type',              'type',         'type'),
 ('name',            'std::string',                'std::string',                'name',         'name'),
 ('capnp_type',      'capnp::schema::Type::Which', 'capnp::schema::Type::Which', 'type',         'capnp::schema::Type::VOID'),
 ('type_length',     'int32_t',                    'int32_t',                    'length',       '-1'),
 ('repetition_type', 'parquet::Repetition::type',  '',                           '',             'parquet::Repetition::OPTIONAL'),
 ('physical_type',   'parquet::Type::type',        'parquet::Type::type',        'type',         'parquet::Type::BYTE_ARRAY'),
 ('logical_type',    'parquet::LogicalType::type',  'parquet::LogicalType::type','type',         'parquet::LogicalType::NONE'),
 ('parquet_node_type', 'parquet::schema::Node::type', '',                        '',             'parquet::schema::Node::PRIMITIVE'),
 ('scale',            'int32_t',                    'int32_t',                   'scale',        '-1'),
 ('precision',        'int32_t',                    'int32_t',                   'precision',    '-1'),
 ('node_id',          'uint64_t',                   'uint64_t',                  'node_id',      '0'),
 ('scope_id',         'uint64_t',                   'uint64_t',                  'scope_id',     '0'),
 ('ordinal',          'uint16_t',                   'uint16_t',                  'ordinal',      '0'),
 ('offset',           'uint32_t',                   'uint32_t',                  'offset',       '0'),
 ('index',            'uint',                       'uint',                      'index',        '0'),
 ('default_value_offset', 'uint32_t',               'uint32_t',                  'offset',       '0'),
 ('had_default_value', '',                          '',                          '',             ''),
 ('unconstrained',     '',                          '',                          '',             ''),
 ('type_id',           'uint64_t',                  'uint64_t',                  'type_id',      '0'),
 ('type_name',         'std::string',               'std::string',               'name',         ''),
 ('enumerant_name',    'std::string',               'std::string',               'name',         ''),
 ('schema_name',       'std::string',               'std::string',               'name',         ''),
 ('parent',            'ASTNode*',                  'ASTNode*',                  'parent',       'nullptr'),
 ('node',              'parquet::schema::NodePtr',  'parquet::schema::NodePtr',  'node',         'nullptr'),
 ('decl',              '',                          '',                          '',             ''),
 ('decimal',           '',                          '',                          '',             ''),
 ('date',              '',                          '',                          '',             ''),
 ('time_millis',       '',                          '',                          '',             ''),
 ('time_micros',       '',                          '',                          '',             ''),
 ('timestamp_millis',  '',                          '',                          '',             ''),
 ('timestamp_micros',  '',                          '',                          '',             ''),
 ('bson',              '',                          '',                          '',             ''),
 ('json',              '',                          '',                          '',             ''),
 ('interval',          '',                          '',                          '',             ''),
 ('fixed_len_byte_array','',                        '',                          '',             ''),
 ('map',               '',                          '',                          '',             ''),
 ('map_key_value',     '',                          '',                          '',             ''),
 ('list',              '',                          '',                          '',             ''),
 ('value',             '',                          '',                          '',             ''),
 
 ]
 cog.outl('typedef struct _ASTNode__isset {')
 cog.outl('  _ASTNode__isset() : ')
 delim = ''
 for property, get_type, set_type, param_name, param_value in types:
   cog.outl('  %s %s(false)' % (delim, property.lower()))
   delim = ','
 cog.outl('    {}')
 for property, get_type, set_type, param_name, param_value in types:
   cog.outl('  bool %s :1;' % (property.lower()))
 cog.outl('} _ASTNode__isset;')
 cog.outl('')
 cog.outl('')
 cog.outl('typedef struct _value {')
 values = dict['value']
 for name, suffix, get_type, set_type, param_name, param_value in values:
   cog.outl('  %s %s;' % (get_type.replace('&', '').ljust(20, ' '), param_value.rjust(10, ' ')))
 cog.outl('} _ASTNodeValue;')
 ]]]*/
typedef struct _ASTNode__isset {
  _ASTNode__isset() : 
   node_type(false)
  , name(false)
  , capnp_type(false)
  , type_length(false)
  , repetition_type(false)
  , physical_type(false)
  , logical_type(false)
  , parquet_node_type(false)
  , scale(false)
  , precision(false)
  , node_id(false)
  , scope_id(false)
  , ordinal(false)
  , offset(false)
  , index(false)
  , default_value_offset(false)
  , had_default_value(false)
  , unconstrained(false)
  , type_id(false)
  , type_name(false)
  , enumerant_name(false)
  , schema_name(false)
  , parent(false)
  , node(false)
  , decl(false)
  , decimal(false)
  , date(false)
  , time_millis(false)
  , time_micros(false)
  , timestamp_millis(false)
  , timestamp_micros(false)
  , bson(false)
  , json(false)
  , interval(false)
  , fixed_len_byte_array(false)
  , map(false)
  , map_key_value(false)
  , list(false)
  , value(false)
    {}
  bool node_type :1;
  bool name :1;
  bool capnp_type :1;
  bool type_length :1;
  bool repetition_type :1;
  bool physical_type :1;
  bool logical_type :1;
  bool parquet_node_type :1;
  bool scale :1;
  bool precision :1;
  bool node_id :1;
  bool scope_id :1;
  bool ordinal :1;
  bool offset :1;
  bool index :1;
  bool default_value_offset :1;
  bool had_default_value :1;
  bool unconstrained :1;
  bool type_id :1;
  bool type_name :1;
  bool enumerant_name :1;
  bool schema_name :1;
  bool parent :1;
  bool node :1;
  bool decl :1;
  bool decimal :1;
  bool date :1;
  bool time_millis :1;
  bool time_micros :1;
  bool timestamp_millis :1;
  bool timestamp_micros :1;
  bool bson :1;
  bool json :1;
  bool interval :1;
  bool fixed_len_byte_array :1;
  bool map :1;
  bool map_key_value :1;
  bool list :1;
  bool value :1;
} _ASTNode__isset;


typedef struct _value {
  bool                          b;
  int8_t                       i8;
  int16_t                     i16;
  int32_t                     i32;
  int64_t                     i64;
  uint8_t                     ui8;
  uint16_t                   ui16;
  uint32_t                   ui32;
  uint64_t                   ui64;
  float                         f;
  double                        d;
  std::string              string;
  std::vector<uint8_t>     binary;
} _ASTNodeValue;
//[[[end]]]

// Convert lowerCamelCase and UpperCamelCase strings to lower_with_underscore.
// https://gist.github.com/rodamber/2558e25d4d8f6b9f2ffdf7bd49471340
std::string convertCamelCase(std::string camelCase) {
  std::string str(1, tolower(camelCase[0]));
  //printf("convertCamelCase: input: %s\n", camelCase.c_str());

  // First place underscores between contiguous lower and upper case letters.
  // For example, `_LowerCamelCase` becomes `_Lower_Camel_Case`.
  for (auto it = camelCase.begin() + 1; it != camelCase.end(); ++it) {
    if (isdigit(*it) && *(it-1) != '_' && islower(*(it-1))) {
      str += "_";
    } else if (isupper(*it) && *(it-1) != '_' && (isdigit(*(it-1)) || islower(*(it-1)))) {
      str += "_";
    }
    str += *it;
  }

  // Then convert it to lower case.
  std::transform(str.begin(), str.end(), str.begin(), ::tolower);
  //printf("convertCamelCase: output: %s\n", str.c_str());
  return str;
}

class ASTNode {
public:
  enum type {
      NONE,
      STRUCT,
      INTERFACE,
      GROUP,
      ENUM,
      FILE,
      FIELD,
      UNION,
      ANNOTATION,
      CONST,
      PARAM,
      METHOD,
      ENUMERANT,
      TYPE,
      VALUE
  };

  ASTNode(ASTNode::type type, const std::string& name)
  :
  /*[[[cog
   delim = ''
   for property, get_type, set_type, param_name, param_value in types:
     if (len(get_type) > 0) and (len(param_value) >0):
       cog.outl('  %s%s_(%s)' % (delim, property.lower(), param_value))
     delim = ', '
   cog.outl('  {}')
   ]]]*/
  node_type_(type)
  , name_(name)
  , capnp_type_(capnp::schema::Type::VOID)
  , type_length_(-1)
  , repetition_type_(parquet::Repetition::OPTIONAL)
  , physical_type_(parquet::Type::BYTE_ARRAY)
  , logical_type_(parquet::LogicalType::NONE)
  , parquet_node_type_(parquet::schema::Node::PRIMITIVE)
  , scale_(-1)
  , precision_(-1)
  , node_id_(0)
  , scope_id_(0)
  , ordinal_(0)
  , offset_(0)
  , index_(0)
  , default_value_offset_(0)
  , type_id_(0)
  , parent_(nullptr)
  , node_(nullptr)
  {}
  //[[[end]]]

  ~ASTNode() {
  }

  /*[[[cog
   cog.outl('// Methods to check if a property is set')
   cog.outl('')
   for property, get_type, set_type, param_name, param_value in types:
     cog.outl('bool is_%s() const { return __isset.%s == true; }' % (property.lower(),property.lower()))
   ]]]*/
  // Methods to check if a property is set

  bool is_node_type() const { return __isset.node_type == true; }
  bool is_name() const { return __isset.name == true; }
  bool is_capnp_type() const { return __isset.capnp_type == true; }
  bool is_type_length() const { return __isset.type_length == true; }
  bool is_repetition_type() const { return __isset.repetition_type == true; }
  bool is_physical_type() const { return __isset.physical_type == true; }
  bool is_logical_type() const { return __isset.logical_type == true; }
  bool is_parquet_node_type() const { return __isset.parquet_node_type == true; }
  bool is_scale() const { return __isset.scale == true; }
  bool is_precision() const { return __isset.precision == true; }
  bool is_node_id() const { return __isset.node_id == true; }
  bool is_scope_id() const { return __isset.scope_id == true; }
  bool is_ordinal() const { return __isset.ordinal == true; }
  bool is_offset() const { return __isset.offset == true; }
  bool is_index() const { return __isset.index == true; }
  bool is_default_value_offset() const { return __isset.default_value_offset == true; }
  bool is_had_default_value() const { return __isset.had_default_value == true; }
  bool is_unconstrained() const { return __isset.unconstrained == true; }
  bool is_type_id() const { return __isset.type_id == true; }
  bool is_type_name() const { return __isset.type_name == true; }
  bool is_enumerant_name() const { return __isset.enumerant_name == true; }
  bool is_schema_name() const { return __isset.schema_name == true; }
  bool is_parent() const { return __isset.parent == true; }
  bool is_node() const { return __isset.node == true; }
  bool is_decl() const { return __isset.decl == true; }
  bool is_decimal() const { return __isset.decimal == true; }
  bool is_date() const { return __isset.date == true; }
  bool is_time_millis() const { return __isset.time_millis == true; }
  bool is_time_micros() const { return __isset.time_micros == true; }
  bool is_timestamp_millis() const { return __isset.timestamp_millis == true; }
  bool is_timestamp_micros() const { return __isset.timestamp_micros == true; }
  bool is_bson() const { return __isset.bson == true; }
  bool is_json() const { return __isset.json == true; }
  bool is_interval() const { return __isset.interval == true; }
  bool is_fixed_len_byte_array() const { return __isset.fixed_len_byte_array == true; }
  bool is_map() const { return __isset.map == true; }
  bool is_map_key_value() const { return __isset.map_key_value == true; }
  bool is_list() const { return __isset.list == true; }
  bool is_value() const { return __isset.value == true; }
  //[[[end]]]

  bool is_target(std::string& target) {
      if (targets_.count(target) > 0) {
          return true;
      }
      return false;
  }

  /*[[[cog
   cog.outl('// Methods to get a property')
   for property, get_type, set_type, param_name, param_value in types:
     if len(get_type) > 0:
       cog.outl('')
       cog.outl('%s %s() { return %s_; }' % (get_type, property.lower(), property.lower()))
   cog.outl('')
   ]]]*/
  // Methods to get a property

  ASTNode::type node_type() { return node_type_; }

  std::string name() { return name_; }

  capnp::schema::Type::Which capnp_type() { return capnp_type_; }

  int32_t type_length() { return type_length_; }

  parquet::Repetition::type repetition_type() { return repetition_type_; }

  parquet::Type::type physical_type() { return physical_type_; }

  parquet::LogicalType::type logical_type() { return logical_type_; }

  parquet::schema::Node::type parquet_node_type() { return parquet_node_type_; }

  int32_t scale() { return scale_; }

  int32_t precision() { return precision_; }

  uint64_t node_id() { return node_id_; }

  uint64_t scope_id() { return scope_id_; }

  uint16_t ordinal() { return ordinal_; }

  uint32_t offset() { return offset_; }

  uint index() { return index_; }

  uint32_t default_value_offset() { return default_value_offset_; }

  uint64_t type_id() { return type_id_; }

  std::string type_name() { return type_name_; }

  std::string enumerant_name() { return enumerant_name_; }

  std::string schema_name() { return schema_name_; }

  ASTNode* parent() { return parent_; }

  parquet::schema::NodePtr node() { return node_; }

  //[[[end]]]

  /*[[[cog
   cog.outl('// Methods to get repetition type')
   values = dict['repetition_type']
   for name, suffix, get_type, set_type, param_name, param_value in values:
     cog.outl('')
     cog.outl('%s %s() const { return repetition_type_ == %s; }' %  (get_type, name, param_value))
   ]]]*/
  // Methods to get repetition type

  bool is_required() const { return repetition_type_ == parquet::Repetition::REQUIRED; }

  bool is_optional() const { return repetition_type_ == parquet::Repetition::OPTIONAL; }

  bool is_repeated() const { return repetition_type_ == parquet::Repetition::REPEATED; }
  //[[[end]]]

  /*[[[cog
   cog.outl('// Methods to get Parquet node type')
   values = dict['parquet_node_type']
   for name, suffix, get_type, set_type, param_name, param_value in values:
     cog.outl('')
     cog.outl('%s %s_%s() const { return parquet_node_type_ == %s; }' %  (get_type, name, suffix, param_value))
   ]]]*/
  // Methods to get Parquet node type

  bool is_parquet_primitive() const { return parquet_node_type_ == parquet::schema::Node::PRIMITIVE; }

  bool is_parquet_group() const { return parquet_node_type_ == parquet::schema::Node::GROUP; }
  //[[[end]]]

  /*[[[cog
   cog.outl('// Methods to get value')
   values = dict['value']
   for name, suffix, get_type, set_type, param_name, param_value in values:
     cog.outl('')
     cog.outl('%s get%s%s() {' % (get_type, capitalize(name), suffix.upper()))
     cog.outl('  return value_.%s;' % (param_value))
     cog.outl('}')
   ]]]*/
  // Methods to get value

  bool getValueBOOL() {
    return value_.b;
  }

  int8_t getValueI8() {
    return value_.i8;
  }

  int16_t getValueI16() {
    return value_.i16;
  }

  int32_t getValueI32() {
    return value_.i32;
  }

  int64_t getValueI64() {
    return value_.i64;
  }

  uint8_t getValueUI8() {
    return value_.ui8;
  }

  uint16_t getValueUI16() {
    return value_.ui16;
  }

  uint32_t getValueUI32() {
    return value_.ui32;
  }

  uint64_t getValueUI64() {
    return value_.ui64;
  }

  float getValueFLOAT() {
    return value_.f;
  }

  double getValueDOUBLE() {
    return value_.d;
  }

  std::string getValueSTRING() {
    return value_.string;
  }

  std::vector<uint8_t>& getValueBINARY() {
    return value_.binary;
  }
  //[[[end]]]

  /*[[[cog
   cog.outl('// Methods to set a property')
   for property, get_type, set_type, param_name, param_value in types:
     cog.outl('')
     if len(set_type) > 0:
       cog.outl('void set%s(%s %s) {' % (capitalize(property), set_type, param_name))
       cog.outl('  %s_ = %s;' % (property, param_name))
       cog.outl('  __isset.%s = true;' % (property))
       cog.outl('}')
     elif property == "had_default_value":
       cog.outl('void set%s() {' % (capitalize(property)))
       cog.outl('  __isset.%s = true;' % (property))
       cog.outl('}')
     elif len(get_type) == 0:
       cog.outl('void setIs%s() {' % (capitalize(property)))
       cog.outl('  __isset.%s = true;' % (property))
       cog.outl('}')
   cog.outl('')
   ]]]*/
  // Methods to set a property

  void setNodeType(ASTNode::type type) {
    node_type_ = type;
    __isset.node_type = true;
  }

  void setName(std::string name) {
    name_ = name;
    __isset.name = true;
  }

  void setCapnpType(capnp::schema::Type::Which type) {
    capnp_type_ = type;
    __isset.capnp_type = true;
  }

  void setTypeLength(int32_t length) {
    type_length_ = length;
    __isset.type_length = true;
  }


  void setPhysicalType(parquet::Type::type type) {
    physical_type_ = type;
    __isset.physical_type = true;
  }

  void setLogicalType(parquet::LogicalType::type type) {
    logical_type_ = type;
    __isset.logical_type = true;
  }


  void setScale(int32_t scale) {
    scale_ = scale;
    __isset.scale = true;
  }

  void setPrecision(int32_t precision) {
    precision_ = precision;
    __isset.precision = true;
  }

  void setNodeId(uint64_t node_id) {
    node_id_ = node_id;
    __isset.node_id = true;
  }

  void setScopeId(uint64_t scope_id) {
    scope_id_ = scope_id;
    __isset.scope_id = true;
  }

  void setOrdinal(uint16_t ordinal) {
    ordinal_ = ordinal;
    __isset.ordinal = true;
  }

  void setOffset(uint32_t offset) {
    offset_ = offset;
    __isset.offset = true;
  }

  void setIndex(uint index) {
    index_ = index;
    __isset.index = true;
  }

  void setDefaultValueOffset(uint32_t offset) {
    default_value_offset_ = offset;
    __isset.default_value_offset = true;
  }

  void setHadDefaultValue() {
    __isset.had_default_value = true;
  }

  void setIsUnconstrained() {
    __isset.unconstrained = true;
  }

  void setTypeId(uint64_t type_id) {
    type_id_ = type_id;
    __isset.type_id = true;
  }

  void setTypeName(std::string name) {
    type_name_ = name;
    __isset.type_name = true;
  }

  void setEnumerantName(std::string name) {
    enumerant_name_ = name;
    __isset.enumerant_name = true;
  }

  void setSchemaName(std::string name) {
    schema_name_ = name;
    __isset.schema_name = true;
  }

  void setParent(ASTNode* parent) {
    parent_ = parent;
    __isset.parent = true;
  }

  void setNode(parquet::schema::NodePtr node) {
    node_ = node;
    __isset.node = true;
  }

  void setIsDecl() {
    __isset.decl = true;
  }

  void setIsDecimal() {
    __isset.decimal = true;
  }

  void setIsDate() {
    __isset.date = true;
  }

  void setIsTimeMillis() {
    __isset.time_millis = true;
  }

  void setIsTimeMicros() {
    __isset.time_micros = true;
  }

  void setIsTimestampMillis() {
    __isset.timestamp_millis = true;
  }

  void setIsTimestampMicros() {
    __isset.timestamp_micros = true;
  }

  void setIsBson() {
    __isset.bson = true;
  }

  void setIsJson() {
    __isset.json = true;
  }

  void setIsInterval() {
    __isset.interval = true;
  }

  void setIsFixedLenByteArray() {
    __isset.fixed_len_byte_array = true;
  }

  void setIsMap() {
    __isset.map = true;
  }

  void setIsMapKeyValue() {
    __isset.map_key_value = true;
  }

  void setIsList() {
    __isset.list = true;
  }

  void setIsValue() {
    __isset.value = true;
  }

  //[[[end]]]

  /*[[[cog
   cog.outl('// Methods to set repetition type')
   values = dict['repetition_type']
   for name, suffix, get_type, set_type, param_name, param_value in values:
     cog.outl('')
     cog.outl('void set%s%s() {' % (capitalize(name), suffix.upper()))
     cog.outl('  repetition_type_ = %s;' % param_value)
     cog.outl('  __isset.repetition_type = true;')
     cog.outl('}')
   ]]]*/
  // Methods to set repetition type

  void setIsRequired() {
    repetition_type_ = parquet::Repetition::REQUIRED;
    __isset.repetition_type = true;
  }

  void setIsOptional() {
    repetition_type_ = parquet::Repetition::OPTIONAL;
    __isset.repetition_type = true;
  }

  void setIsRepeated() {
    repetition_type_ = parquet::Repetition::REPEATED;
    __isset.repetition_type = true;
  }
  //[[[end]]]

  /*[[[cog
   cog.outl('// Methods to set Parquet node type')
   values = dict['parquet_node_type']
   for name, suffix, get_type, set_type, param_name, param_value in values:
     cog.outl('void set%s%s() {' % (capitalize(name), suffix.upper()))
     cog.outl('  parquet_node_type_ = %s;' % param_value)
     cog.outl('  __isset.parquet_node_type = true;')
     cog.outl('}')
   ]]]*/
  // Methods to set Parquet node type
  void setIsParquetPRIMITIVE() {
    parquet_node_type_ = parquet::schema::Node::PRIMITIVE;
    __isset.parquet_node_type = true;
  }
  void setIsParquetGROUP() {
    parquet_node_type_ = parquet::schema::Node::GROUP;
    __isset.parquet_node_type = true;
  }
  //[[[end]]]

  /*[[[cog
   cog.outl('// Methods to set value')
   values = dict['value']
   for name, suffix, get_type, set_type, param_name, param_value in values:
     cog.outl('')
     cog.outl('void set%s%s(%s value) {' % (capitalize(name), suffix.upper(), set_type))
     cog.outl('  value_.%s = value;' % param_value)
     cog.outl('  __isset.value = true;')
     cog.outl('}')
   ]]]*/
  // Methods to set value

  void setValueBOOL(bool value) {
    value_.b = value;
    __isset.value = true;
  }

  void setValueI8(int8_t value) {
    value_.i8 = value;
    __isset.value = true;
  }

  void setValueI16(int16_t value) {
    value_.i16 = value;
    __isset.value = true;
  }

  void setValueI32(int32_t value) {
    value_.i32 = value;
    __isset.value = true;
  }

  void setValueI64(int64_t value) {
    value_.i64 = value;
    __isset.value = true;
  }

  void setValueUI8(uint8_t value) {
    value_.ui8 = value;
    __isset.value = true;
  }

  void setValueUI16(uint16_t value) {
    value_.ui16 = value;
    __isset.value = true;
  }

  void setValueUI32(uint32_t value) {
    value_.ui32 = value;
    __isset.value = true;
  }

  void setValueUI64(uint64_t value) {
    value_.ui64 = value;
    __isset.value = true;
  }

  void setValueFLOAT(float value) {
    value_.f = value;
    __isset.value = true;
  }

  void setValueDOUBLE(double value) {
    value_.d = value;
    __isset.value = true;
  }

  void setValueSTRING(std::string value) {
    value_.string = value;
    __isset.value = true;
  }

  void setValueBINARY(std::vector<uint8_t>& value) {
    value_.binary = value;
    __isset.value = true;
  }
  //[[[end]]]

  void setValueBINARY(uint8_t* value, size_t length) {
      value_.binary.reserve(length);
      std::copy(value, value + length, std::back_inserter(value_.binary));
      __isset.value = true;
  }

  void appendValueBINARY(const uint8_t* value, size_t length) {
      for (int i = 0; i < length; i++) {
          value_.binary.push_back(value[i]);
      }
      __isset.value = true;
  }

  void addChild(ASTNode* child) {
      if (child != nullptr) {
        child->setParent(this);
        children_.push_back(std::unique_ptr<ASTNode>(child));
      }
  }

  void removeChild(size_t index) {
    if (index < children_.size()) {
      children_.erase(children_.begin() + index);
    }
  }

  void addTarget(std::string target) {
      targets_[target] = true;
  }

  bool Equals(ASTNode* other) {
      return EqualsInternal(other);
  }

  ASTNode* child(int i) { return children_[i].get(); }

  const std::vector<std::unique_ptr<ASTNode>>& children() const { return children_; }

  int num_children() const { return static_cast<int>(children_.size()); }

//protected:
  /*[[[cog
   for property, get_type, set_type, param_name, param_value in types:
     if len(get_type) > 0:
       cog.outl('    %s %s_;' % (get_type.replace('&','').ljust(28, ' '), property.lower()))
   ]]]*/
  ASTNode::type                node_type_;
  std::string                  name_;
  capnp::schema::Type::Which   capnp_type_;
  int32_t                      type_length_;
  parquet::Repetition::type    repetition_type_;
  parquet::Type::type          physical_type_;
  parquet::LogicalType::type   logical_type_;
  parquet::schema::Node::type  parquet_node_type_;
  int32_t                      scale_;
  int32_t                      precision_;
  uint64_t                     node_id_;
  uint64_t                     scope_id_;
  uint16_t                     ordinal_;
  uint32_t                     offset_;
  uint                         index_;
  uint32_t                     default_value_offset_;
  uint64_t                     type_id_;
  std::string                  type_name_;
  std::string                  enumerant_name_;
  std::string                  schema_name_;
  ASTNode*                     parent_;
  parquet::schema::NodePtr     node_;
  //[[[end]]]

  _ASTNodeValue value_;

  _ASTNode__isset __isset;

  std::vector<std::unique_ptr<ASTNode>> children_;

  std::unordered_map<std::string, bool> targets_;

  bool EqualsInternal(ASTNode* other) {
    if (this == other) { return true; }

    bool is_equal = (this->node_type_ == other->node_type_ && name_ == other->name_);

    if (false == is_equal) {
        return false;
    }

    if (this->num_children() != other->num_children()) { return false; }
    for (int i = 0; i < this->num_children(); ++i) {
        if (!this->child(i)->Equals(other->child(i))) { return false; }
    }

    return true;
  }
};

class CapnpcParquet : public BaseGenerator {
public:
  explicit CapnpcParquet(SchemaLoader &schemaLoader)
  : BaseGenerator(schemaLoader), document_(nullptr), currentParent_(nullptr) {
  }

  void finish() override {
    // Analyze Parquet schema

    //printASTNode(0, document_);

    parquet::schema::NodePtr parquet_schema = getDocument();

    std::shared_ptr<parquet::SchemaDescriptor> descr = std::make_shared<parquet::SchemaDescriptor>();

    try {
      descr->Init(std::shared_ptr<parquet::schema::GroupNode>(static_cast<parquet::schema::GroupNode*>(parquet_schema.get())));
    } catch (const std::exception& e) {
      std::cerr << "Parquet schema descriptor error: " << e.what() << std::endl;
      return;
    }

    // Print generated Parquet schema

    try {
      parquet::schema::PrintSchema(descr->schema_root().get(), std::cout);
    } catch (const std::exception& e) {
      std::cerr << "Parquet schema error: " << e.what() << std::endl;
      return;
    }

    // Other Cap'n Proto compiler plugins:
    //
    // https://github.com/capnproto/capnproto/blob/master/c%2B%2B/src/capnp/compiler/capnpc-capnp.c%2B%2B
    // https://github.com/capnproto/capnproto/blob/master/c%2B%2B/src/capnp/compiler/capnpc-c%2B%2B.c%2B%2B
    //

    //
    // Possible uses:
    //
    // 1) Write out a program that reads/writes a Parquet file using the compiled
    // schema. The coded generated could use the Parquet-Cpp or Arrow libraries.
    //
    // 2) Write out a Parquet file filed with randomly generated data using the
    // compiled schema.
    //
  }

  static constexpr const char FILE_SUFFIX[] = ".parquet";
  static const auto TRAVERSAL_LIMIT = 1 << 30;  // Don't limit.
  static constexpr const char *TITLE = "PARQUET Generator";
  static constexpr const char *DESCRIPTION = "PARQUET Generator";
  static const size_t BUFFER_SIZE = 4096;

  parquet::schema::NodePtr getDocument() const {
      // root of the Parquet schema
      return document_->node();
  }

private:
  ASTNode* document_;
  ASTNode* currentParent_;

  kj::String struct_field_reason_;
  kj::String value_reason_;
  constexpr static const char* default_type_reason_ = u8"type";
  const char* type_reason_ = default_type_reason_;

  int32_t minBytesForPrecision(int32_t precision) {
    int32_t numBytes = 1;
    while (pow(2.0, 8.0 * numBytes - 1.0) < pow(10.0, precision)) {
      numBytes += 1;
    }
    return numBytes;
  }

  // Max precision of a decimal value stored in `numBytes` bytes
  int32_t maxPrecisionForBytes(int32_t numBytes) {
    return round(                                   // convert double to int32_t
             floor(log10(                           // number of base-10 digits
            pow(2, 8.0 * numBytes - 1.0) - 1.0)));  // max value stored in numBytes
  }

  int32_t getAnnotationValueI32(ASTNode* node) {
    int num_children = node->num_children();
    int32_t value = 0;

    ASTNode* child;

    for (int i = 0; i < num_children; i++) {
      child = node->child(i);

      if ((child->node() != nullptr) &&
          (child->node_type() == ASTNode::type::VALUE) &&
          (child->is_value())) {
        switch (child->capnp_type()) {
            case schema::Type::INT64:
              value = static_cast<int32_t>(child->getValueI64());
              break;
            case schema::Type::UINT64:
              value = static_cast<int32_t>(child->getValueUI64());
              break;
            case schema::Type::INT8:
              value = static_cast<int32_t>(child->getValueI8());
              break;
            case schema::Type::INT16:
              value = static_cast<int32_t>(child->getValueI16());
              break;
            case schema::Type::INT32:
              value = child->getValueI32();
              break;
            case schema::Type::UINT8:
              value = static_cast<int32_t>(child->getValueUI8());
              break;
            case schema::Type::UINT16:
              value = static_cast<int32_t>(child->getValueUI16());
              break;
            case schema::Type::UINT32:
              value = static_cast<int32_t>(child->getValueUI32());
              break;
            default:
              value = 0;
        }
      }
    }

    return value;
  }

  std::string getAnnotationValueTEXT(ASTNode* node) {
    int num_children = node->num_children();
    std::string value;

    ASTNode* child;

    for (int i = 0; i < num_children; i++) {
      child = node->child(i);

      if ((child->node() != nullptr) &&
          (child->node_type() == ASTNode::type::VALUE) &&
          (child->is_value())) {
        switch (child->capnp_type()) {
          case schema::Type::TEXT:
            value = child->getValueSTRING();
            break;
          default:
            break;
        }
      }
    }
    
    return value;
  }

  void applyAnnotations(ASTNode* node) {
    int num_children = node->num_children();

    ASTNode* child;

    for (int i = 0; i < num_children; i++) {
      child = node->child(i);

      //printf("applyAnnotations: %s %d\n", child->name().c_str(), child->is_decl() ? 1 : 0);

      if ((!child->is_decl()) &&
          (child->node_type() == ASTNode::type::ANNOTATION)) {
        // Use Cap'n Proto annotations to indicate Parquet schema attributes
        //
        // https://capnproto.org/language.html
        //
        // annotation schema(struct)        :Text;
        // annotation required(*)           :Void;
        // annotation optional(*)           :Void;
        // annotation repeated(*)           :Void;
        // annotation length(*)             :Int32;
        // annotation scale(*)              :Int32;
        // annotation precision(*)          :Int32;
        // annotation decimal(*)            :Void;
        // annotation date(*)               :Void;
        // annotation timeMillis(*)         :Void;
        // annotation timeMicros(*)         :Void;
        // annotation timestampMillis(*)    :Void;
        // annotation timestampMicros(*)    :Void;
        // annotation bson(*)               :Void;
        // annotation json(*)               :Void;
        // annotation interval(*)           :Void;
        // annotation fixed(*)              :Void;
        // annotation map(*)                :Void;
        // annotation mapKeyValue(*)        :Void;
        // annotation list(*)               :Void;

        if (child->name() == "schema") {
          node->setSchemaName(getAnnotationValueTEXT(child));
        } else if (child->name() == "required") {
          node->setIsRequired();
        } else if (child->name() == "optional") {
          node->setIsOptional();
        } else if (child->name() == "repeated") {
          node->setIsRepeated();
        } else if (child->name() == "length") {
          node->setTypeLength(getAnnotationValueI32(child));
        } else if (child->name() == "scale") {
          node->setScale(getAnnotationValueI32(child));
        } else if (child->name() == "precision") {
          node->setPrecision(getAnnotationValueI32(child));
        } else if (child->name() == "decimal") {
          node->setIsDecimal();
        } else if (child->name() == "date") {
          node->setIsDate();
        } else if (child->name() == "timeMillis") {
          node->setIsTimeMillis();
        } else if (child->name() == "timeMicros") {
          node->setIsTimeMicros();
        } else if (child->name() == "timestampMillis") {
          node->setIsTimestampMillis();
        } else if (child->name() == "timestampMicros") {
          node->setIsTimestampMicros();
        } else if (child->name() == "bson") {
          node->setIsBson();
        } else if (child->name() == "json") {
          node->setIsJson();
        } else if (child->name() == "interval") {
          node->setIsInterval();
        } else if (child->name() == "fixed") {
          node->setIsFixedLenByteArray();
        } else if (child->name() == "map") {
          node->setIsMap();
        } else if (child->name() == "mapKeyValue") {
          node->setIsMapKeyValue();
        } else if (child->name() == "list") {
          node->setIsList();
        }
      }
    }
  }

  void applyLogicalType(ASTNode* node) {
    // https://github.com/apache/parquet-format/blob/master/LogicalTypes.md
    if (node->is_capnp_type()) {
      capnp::schema::Type::Which capnp_type = node->capnp_type();

      switch (capnp_type) {
        case capnp::schema::Type::VOID:
          node->setLogicalType(parquet::LogicalType::NONE);
          break;
        case capnp::schema::Type::BOOL:
          node->setLogicalType(parquet::LogicalType::NONE);
          break;
        case capnp::schema::Type::INT8:
          node->setLogicalType(parquet::LogicalType::INT_8);
          break;
        case capnp::schema::Type::INT16:
          node->setLogicalType(parquet::LogicalType::INT_16);
          break;
        case capnp::schema::Type::INT32:
          node->setLogicalType(parquet::LogicalType::INT_32);
          break;
        case capnp::schema::Type::INT64:
          node->setLogicalType(parquet::LogicalType::INT_64);
          break;
        case capnp::schema::Type::UINT8:
          node->setLogicalType(parquet::LogicalType::UINT_8);
          break;
        case capnp::schema::Type::UINT16:
          node->setLogicalType(parquet::LogicalType::UINT_16);
          break;
        case capnp::schema::Type::UINT32:
          node->setLogicalType(parquet::LogicalType::UINT_32);
          break;
        case capnp::schema::Type::UINT64:
          node->setLogicalType(parquet::LogicalType::UINT_64);
          break;
        case capnp::schema::Type::FLOAT32:
          node->setLogicalType(parquet::LogicalType::NONE);
          break;
        case capnp::schema::Type::FLOAT64:
          node->setLogicalType(parquet::LogicalType::NONE);
          break;
        case capnp::schema::Type::TEXT:
          node->setLogicalType(parquet::LogicalType::UTF8);
          break;
        case capnp::schema::Type::DATA:
          node->setLogicalType(parquet::LogicalType::NONE);
          break;
        case capnp::schema::Type::LIST:
          node->setLogicalType(parquet::LogicalType::LIST);
          break;
        case capnp::schema::Type::ENUM:
          node->setLogicalType(parquet::LogicalType::ENUM);
          break;
        case capnp::schema::Type::STRUCT:
          // Represented by parquet::schema::Node:GROUP in Parquet
          if (node->is_map()) {
            // TODO(renesugar): In parquet-cpp, LogicalTypeToString doesn't have
            //                  a case for parquet::LogicalType::MAP so "UNKNOWN"
            //                  is printed instead of "MAP" when PrintSchema
            //                  is called.
            //
            node->setLogicalType(parquet::LogicalType::MAP);
          } else if (node->is_map_key_value()) {
            node->setLogicalType(parquet::LogicalType::MAP_KEY_VALUE);
          } else if (node->is_list()) {
            node->setLogicalType(parquet::LogicalType::LIST);
          } else {
            node->setLogicalType(parquet::LogicalType::NONE);
          }
          break;
        case capnp::schema::Type::INTERFACE:
        case capnp::schema::Type::ANY_POINTER:
          // Not represented in Parquet
          node->setLogicalType(parquet::LogicalType::NONE);
          break;
        default:
          node->setLogicalType(parquet::LogicalType::NONE);
          break;
      }
    }

    if (node->is_decimal()) {
      node->setLogicalType(parquet::LogicalType::DECIMAL);
    } else if (node->is_date()) {
      node->setLogicalType(parquet::LogicalType::DATE);
    } else if (node->is_time_millis()) {
      node->setLogicalType(parquet::LogicalType::TIME_MILLIS);
    } else if (node->is_time_micros()) {
      node->setLogicalType(parquet::LogicalType::TIME_MICROS);
    } else if (node->is_timestamp_millis()) {
      node->setLogicalType(parquet::LogicalType::TIMESTAMP_MILLIS);
    } else if (node->is_timestamp_micros()) {
      node->setLogicalType(parquet::LogicalType::TIMESTAMP_MICROS);
    } else if (node->is_bson()) {
      node->setLogicalType(parquet::LogicalType::BSON);
    } else if (node->is_json()) {
      node->setLogicalType(parquet::LogicalType::JSON);
    } else if (node->is_interval()) {
      node->setLogicalType(parquet::LogicalType::INTERVAL);
    }
  }

  void applyPhysicalType(ASTNode* node) {
    if (node->is_capnp_type()) {
      capnp::schema::Type::Which capnp_type = node->capnp_type();

      switch (capnp_type) {
        case capnp::schema::Type::VOID:
          node->setPhysicalType(parquet::Type::BYTE_ARRAY);
          break;
        case capnp::schema::Type::BOOL:
          node->setPhysicalType(parquet::Type::BOOLEAN);
          break;
        case capnp::schema::Type::INT8:
          node->setPhysicalType(parquet::Type::INT32);
          break;
        case capnp::schema::Type::INT16:
          node->setPhysicalType(parquet::Type::INT32);
          break;
        case capnp::schema::Type::INT32:
          node->setPhysicalType(parquet::Type::INT32);
          break;
        case capnp::schema::Type::INT64:
          node->setPhysicalType(parquet::Type::INT64);
          break;
        case capnp::schema::Type::UINT8:
          node->setPhysicalType(parquet::Type::INT32);
          break;
        case capnp::schema::Type::UINT16:
          node->setPhysicalType(parquet::Type::INT32);
          break;
        case capnp::schema::Type::UINT32:
          node->setPhysicalType(parquet::Type::INT32);
          break;
        case capnp::schema::Type::UINT64:
          node->setPhysicalType(parquet::Type::INT64);
          break;
        case capnp::schema::Type::FLOAT32:
          node->setPhysicalType(parquet::Type::FLOAT);
          break;
        case capnp::schema::Type::FLOAT64:
          node->setPhysicalType(parquet::Type::DOUBLE);
          break;
        case capnp::schema::Type::TEXT:
          node->setPhysicalType(parquet::Type::BYTE_ARRAY);
          break;
        case capnp::schema::Type::DATA:
          node->setPhysicalType(parquet::Type::BYTE_ARRAY);
          break;
        case capnp::schema::Type::LIST:
          // Represented by parquet::schema::Node:GROUP in Parquet
          break;
        case capnp::schema::Type::ENUM:
          node->setPhysicalType(parquet::Type::BYTE_ARRAY);
          break;
        case capnp::schema::Type::STRUCT:
          // Represented by parquet::schema::Node:GROUP in Parquet
          break;
        case capnp::schema::Type::INTERFACE:
        case capnp::schema::Type::ANY_POINTER:
          // Not represented in Parquet
          break;
        default:
          break;
      }
    }

    if (node->is_decimal()) {
      int32_t precision = node->precision();

      if (precision <= 9) {
        // Uses INT32 for 1 <= precision <= 9
        node->setPhysicalType(parquet::Type::INT32);
      } else if (precision <= 18) {
        // Uses INT64 for 1 <= precision <= 18
        node->setPhysicalType(parquet::Type::INT64);
      } else {
        // Uses FIXED_LEN_BYTE_ARRAY for all other precisions
        int32_t numBytes = 0;
        node->setPhysicalType(parquet::Type::FIXED_LEN_BYTE_ARRAY);
        numBytes = minBytesForPrecision(precision);
        node->setTypeLength(numBytes);
      }
    } else if (node->is_date()) {
      node->setPhysicalType(parquet::Type::INT32);
    } else if (node->is_time_millis()) {
      node->setPhysicalType(parquet::Type::INT32);
    } else if (node->is_time_micros()) {
      node->setPhysicalType(parquet::Type::INT32);
    } else if (node->is_timestamp_millis()) {
      node->setPhysicalType(parquet::Type::INT64);
    } else if (node->is_timestamp_micros()) {
      node->setPhysicalType(parquet::Type::INT64);
    } else if (node->is_bson()) {
      node->setPhysicalType(parquet::Type::BYTE_ARRAY);
    } else if (node->is_json()) {
      node->setPhysicalType(parquet::Type::BYTE_ARRAY);
    } else if (node->is_interval()) {
      node->setPhysicalType(parquet::Type::FIXED_LEN_BYTE_ARRAY);
      node->setTypeLength(12);
    } else if (node->is_fixed_len_byte_array()) {
      node->setPhysicalType(parquet::Type::FIXED_LEN_BYTE_ARRAY);
    }
  }

  void applyParquetNodeType(ASTNode* node) {
    if (node->is_capnp_type()) {
      capnp::schema::Type::Which capnp_type = node->capnp_type();

      switch (capnp_type) {
        case capnp::schema::Type::VOID:
          node->setIsParquetPRIMITIVE();
          break;
        case capnp::schema::Type::BOOL:
          node->setIsParquetPRIMITIVE();
          break;
        case capnp::schema::Type::INT8:
          node->setIsParquetPRIMITIVE();
          break;
        case capnp::schema::Type::INT16:
          node->setIsParquetPRIMITIVE();
          break;
        case capnp::schema::Type::INT32:
          node->setIsParquetPRIMITIVE();
          break;
        case capnp::schema::Type::INT64:
          node->setIsParquetPRIMITIVE();
          break;
        case capnp::schema::Type::UINT8:
          node->setIsParquetPRIMITIVE();
          break;
        case capnp::schema::Type::UINT16:
          node->setIsParquetPRIMITIVE();
          break;
        case capnp::schema::Type::UINT32:
          node->setIsParquetPRIMITIVE();
          break;
        case capnp::schema::Type::UINT64:
          node->setIsParquetPRIMITIVE();
          break;
        case capnp::schema::Type::FLOAT32:
          node->setIsParquetPRIMITIVE();
          break;
        case capnp::schema::Type::FLOAT64:
          node->setIsParquetPRIMITIVE();
          break;
        case capnp::schema::Type::TEXT:
          node->setIsParquetPRIMITIVE();
          break;
        case capnp::schema::Type::DATA:
          node->setIsParquetPRIMITIVE();
          break;
        case capnp::schema::Type::LIST:
          // Represented by parquet::schema::Node:GROUP in Parquet
          node->setIsParquetGROUP();
          break;
        case capnp::schema::Type::ENUM:
          node->setIsParquetPRIMITIVE();
          break;
        case capnp::schema::Type::STRUCT:
          // Represented by parquet::schema::Node:GROUP in Parquet
          node->setIsParquetGROUP();
          break;
        case capnp::schema::Type::INTERFACE:
        case capnp::schema::Type::ANY_POINTER:
          // Not represented in Parquet
          node->setIsParquetPRIMITIVE();
          break;
        default:
          break;
      }
    }

    if (node->is_decimal()) {
      node->setIsParquetPRIMITIVE();
    } else if (node->is_date()) {
      node->setIsParquetPRIMITIVE();
    } else if (node->is_time_millis()) {
      node->setIsParquetPRIMITIVE();
    } else if (node->is_time_micros()) {
      node->setIsParquetPRIMITIVE();
    } else if (node->is_timestamp_millis()) {
      node->setIsParquetPRIMITIVE();
    } else if (node->is_timestamp_micros()) {
      node->setIsParquetPRIMITIVE();
    } else if (node->is_bson()) {
      node->setIsParquetPRIMITIVE();
    } else if (node->is_json()) {
      node->setIsParquetPRIMITIVE();
    } else if (node->is_interval()) {
      node->setIsParquetPRIMITIVE();
    } else if (node->is_fixed_len_byte_array()) {
      node->setIsParquetPRIMITIVE();
    }
  }

  void printIndent(int indent) {
    for (int i = 0; i < indent; i++) {
      printf("  ");
    }
  }

  void printASTNode(int indent, ASTNode* element) {
    printIndent(indent);

    printf("node: element %s node_type=%d num_children=%d is_decl=%d capnp_type=%d type_name=%s schema_name=%s\n", element->name().c_str(), element->node_type(), element->num_children(), element->is_decl() ? 1 : 0, element->capnp_type(), element->type_name().c_str(), element->schema_name().c_str());

    for (int i = 0; i < element->num_children(); i++) {
      printIndent(indent+1);
      //printf("\tchild: %d %d %s='%s'\n", element->child(i)->node_type(), element->child(i)->is_decl() ? 1 : 0, element->child(i)->name().c_str(), getAnnotationValueTEXT(element->child(i)).c_str());
      printf("child: element %s node_type=%d num_children=%d is_decl=%d capnp_type=%d type_name=%s schema_name=%s\n", element->child(i)->name().c_str(), element->child(i)->node_type(), element->child(i)->num_children(), element->child(i)->is_decl() ? 1 : 0, element->child(i)->capnp_type(), element->child(i)->type_name().c_str(), element->child(i)->schema_name().c_str());
      if (element->child(i)->num_children()) {
        printASTNode(indent+2, element->child(i));
      }
    }
  }

  void buildParquetNode(ASTNode* element) {
    parquet::schema::NodePtr    node;
    parquet::schema::NodePtr    child_node;
    parquet::schema::NodeVector children;
    std::string                 name;
    std::vector<uint64_t>       decl_nodeid;

    applyAnnotations(element);

    // For the AST file node, instead of creating a Parquet node, get the
    // root schema node from the child nodes.

    if (element->node_type() == ASTNode::type::FILE) {
      // Get Parquet schema nodes from children
      for (int i = 0; i < element->num_children(); i++) {
        if ((element->child(i)->node() != nullptr) &&
            (element->child(i)->is_schema_name()) &&
            (element->child(i)->node()->is_group()) &&
            (element->child(i)->node_type() != ASTNode::type::ANNOTATION)) {
          // Set the Parquet node
          element->setNode(element->child(i)->node());
          return;
        }
      }
      return;
    }

    // parquet-cpp only lets child nodes to be set when creating a Parquet node.
    //
    // Parquet nodes are built after parsing an element is complete and set on the node.
    //
    // After retrieving the Parquet nodes from the AST node's children, the Parquet node is created.

    applyLogicalType(element);

    applyPhysicalType(element);

    applyParquetNodeType(element);

   /* // Get Parquet schema nodes from children
    for (int i = 0; i < element->num_children(); i++) {
      if ((element->child(i)->node() != nullptr) &&
          (element->child(i)->node_type() != ASTNode::type::ANNOTATION)) {
        children.push_back(parquet::schema::NodePtr(element->child(i)->node()));
      }
    }*/

    // Get Parquet schema nodes from children
    for (int i = 0; i < element->num_children(); i++) {
      if ((element->child(i)->node() != nullptr) &&
          (element->child(i)->node_type() != ASTNode::type::ANNOTATION)) {

        //printf("****parquet node: %s is_decl: %d type_name=%s\n", element->child(i)->name().c_str(), element->child(i)->is_decl() ? 1 : 0, element->child(i)->type_name().c_str());

        ASTNode* child = element->child(i);
        child_node = child->node();
        if ((element->child(i)->node_type() == ASTNode::type::FIELD)) {
          ASTNode* type = nullptr;
          for (int k = 0; k < element->child(i)->num_children(); k++) {
            if (element->child(i)->child(k)->node_type() == ASTNode::type::TYPE) {
              type = element->child(i)->child(k);
            }
          }
          for (int j = 0; j < element->num_children(); j++) {
            //printf("\t****parquet node: %s\n", element->child(j)->name().c_str());
            if ((element->child(j)->is_decl()) &&
                (element->child(j)->capnp_type() == type->capnp_type()) &&
                (element->child(j)->name() == type->type_name())) {
              child = element->child(j);
              child_node = child->node();
              parquet::schema::NodeVector decl_children;
              parquet::schema::GroupNode* group_node = static_cast<parquet::schema::GroupNode*>(child_node.get());

              for (int m = 0; m < group_node->field_count(); m++) {
                decl_children.push_back(group_node->field(m));
              }
              child_node = parquet::schema::GroupNode::Make(
                                                      convertCamelCase(element->child(i)->name()),
                                                      child_node->repetition(),
                                                      decl_children, child_node->logical_type());

              element->child(i)->setNode(child_node);

              decl_nodeid.push_back(element->child(j)->node_id());
              break;
            }
          }
        }

        // children.push_back(parquet::schema::NodePtr(child_node));
      }
    }

    if (decl_nodeid.size() > 0) {
      uint64_t node_id = 0;

      for (int i = 0; i < decl_nodeid.size(); i++) {
        node_id = decl_nodeid[i];

        for (int j = 0; j < element->num_children(); j++) {
          if (element->child(j)->node_id() == node_id) {
            // found node to delete
            element->removeChild(j);
            break;
          }
        }
      }
    }

    for (int i = 0; i < element->num_children(); i++) {
      if ((element->child(i)->node() != nullptr) &&
          (element->child(i)->node_type() != ASTNode::type::ANNOTATION)) {
        children.push_back(parquet::schema::NodePtr(element->child(i)->node()));
      }
    }

    // Create the Parquet node

    // TODO(renesugar): There is no way to set the id on a GroupNode
    //       (Cap'n Proto uses uint64_t; Parquet uses int)
    //
    // PrimitiveNode and GroupNode do not have an "id" constructor parameter
    // for the base class Node "id" constructor parameter.

    if (element->is_enumerant_name()) {
      // NOTE: Parquet doesn't store constants or default values
      name = element->enumerant_name();
    } else if (element->is_type_name()) {
      name = element->type_name();
    } else if (element->is_schema_name()) {
      name = element->schema_name();
    } else {
      name = element->name();
    }

    name = convertCamelCase(name);

    //printf("name = '%s'\n", name.c_str());

    if (element->is_parquet_group()) {
      node = parquet::schema::GroupNode::Make(name, element->repetition_type(),
                                              children, element->logical_type());
      //parquet::schema::GroupNode* group_node = static_cast<parquet::schema::GroupNode*>(node.get());
      //printf("\tfields=%d\n", group_node->field_count());
    } else {
      if (element->is_decimal()) {
        node = parquet::schema::PrimitiveNode::Make(name /*element->name()*/, element->repetition_type(),
                                                    element->physical_type(), element->logical_type(),
                                                    -1, element->precision(), element->scale());
      } else {
        node = parquet::schema::PrimitiveNode::Make(name /*element->name()*/, element->repetition_type(),
                                              element->physical_type(), element->logical_type(),
                                              element->type_length());
      }
    }
    // Set the Parquet node

    element->setNode(std::move(node));
  }

  bool pre_visit_file(const Schema& schema, const schema::CodeGeneratorRequest::RequestedFile::Reader& decl) override {
    // auto inputFilename = decl.getFilename();

    auto proto = schema.getProto();

    ASTNode* element = new ASTNode(ASTNode::type::FILE, proto.getDisplayName().cStr());

    element->setNodeId(proto.getId());

    if (document_ == nullptr) {
      document_ = element;
    } else {
      if (currentParent_ == nullptr) {
        document_->addChild(element);
      } else {
        currentParent_->addChild(element);
      }
      currentParent_ = element;
    }

    return false;
  }

  bool post_visit_file(const Schema& schema, const schema::CodeGeneratorRequest::RequestedFile::Reader& decl) override {
    ASTNode* element = ((currentParent_ == nullptr) ? document_ : currentParent_);

    // Build the Parquet schema node
    buildParquetNode(element);

    if (currentParent_ != nullptr) {
      currentParent_ = currentParent_->parent();
    }
    return false;
  }

  bool pre_visit_nested_decls(const Schema& schema) override {
    // called for every type with nested declarations
    return false;
  }

  bool post_visit_nested_decls(const Schema& schema) override {
    // called for every type with nested declarations
    return false;
  }

  bool pre_visit_decl(const Schema& schema, const schema::Node::NestedNode::Reader& decl) override {
    uint64_t node_id  = decl.getId();
    auto     proto    = schema.getProto();
    uint64_t scope_id = proto.getScopeId();

    //printf("decl.getId = %llu\n", node_id);

    capnp::schema::Node::Which node_capnp_type = proto.which();

    capnp::schema::Type::Which capnp_type = schema::Type::VOID;

    ASTNode::type node_type = ASTNode::NONE;

    // case schema::Node::ENUM: See NOTE(1) in this file.
    switch (node_capnp_type) {
      /*[[[cog
       types = ['struct', 'enum', 'interface', 'file', 'const', 'annotation'];
       for type in types:
         cog.outl('case schema::Node::%s:' % type.upper())
         cog.outl('  node_type = ASTNode::%s;' % type.upper())
         if type in ('struct', 'enum', 'interface'):
           cog.outl('  capnp_type = schema::Type::%s;' % type.upper())
         cog.outl('  break;')
       ]]]*/
      case schema::Node::STRUCT:
        node_type = ASTNode::STRUCT;
        capnp_type = schema::Type::STRUCT;
        break;
      case schema::Node::ENUM:
        node_type = ASTNode::ENUM;
        capnp_type = schema::Type::ENUM;
        break;
      case schema::Node::INTERFACE:
        node_type = ASTNode::INTERFACE;
        capnp_type = schema::Type::INTERFACE;
        break;
      case schema::Node::FILE:
        node_type = ASTNode::FILE;
        break;
      case schema::Node::CONST:
        node_type = ASTNode::CONST;
        break;
      case schema::Node::ANNOTATION:
        node_type = ASTNode::ANNOTATION;
        break;
      //[[[end]]]
      default:
          break;
    }

    ASTNode* element = new ASTNode(node_type, schema.getShortDisplayName().cStr());

    element->setNodeId(node_id);
    element->setScopeId(scope_id);
    element->setIsDecl();
    if (capnp_type != schema::Type::VOID) {
      element->setCapnpType(capnp_type);
    }

    if (currentParent_ == nullptr) {
      document_->addChild(element);
    } else {
      currentParent_->addChild(element);
    }
    currentParent_ = element;

    return false;
  }

  bool post_visit_decl(const Schema& schema, const schema::Node::NestedNode::Reader& decl) override {
    ASTNode* element = ((currentParent_ == nullptr) ? document_ : currentParent_);

    // Build the Parquet schema node
    buildParquetNode(element);

    if (currentParent_ != nullptr) {
      currentParent_ = currentParent_->parent();
    }
    return false;
  }

  bool pre_visit_const_decl(const Schema& schema, const schema::Node::NestedNode::Reader& decl) override {
    value_reason_ = kj::str("value");
    return false;
  }

  bool post_visit_const_decl(const Schema& schema, const schema::Node::NestedNode::Reader& decl) override {
    return false;
  }

  bool pre_visit_enum_decl(const Schema& schema, const schema::Node::NestedNode::Reader& decl) override {
    // See NOTE(1) in this file.
    return false;
  }

  bool post_visit_enum_decl(const Schema& schema, const schema::Node::NestedNode::Reader& decl) override {
    // See NOTE(1) in this file.
    return false;
  }

  bool pre_visit_enumerants(const Schema& schema, const EnumSchema::EnumerantList& enumerants) override {
    // See NOTE(1) in this file.
    return false;
  }

  bool post_visit_enumerants(const Schema& schema, const EnumSchema::EnumerantList& enumerants) override {
    // See NOTE(1) in this file.
    return false;
  }

  bool pre_visit_enumerant(const Schema& schema, const EnumSchema::Enumerant& enumerant) override {
    // NOTE(1):
    //
    // Parquet only defines that an enum type is stored as a binary string.
    //
    // The format of the binary string is not specified.
    //
    // This is because Parquet drops the enum type information making it impossible to round-trip
    // a Parquet schema.
    //
    // Parquet could store enum types as a dictionary encoded string column.
    //
    // Avro gets around this lossy conversion by storing the original Avro schema as a string in
    // the Parquet file's metadata.
    // (see:
    //  https://github.com/Parquet/parquet-mr/blob/master/parquet-avro/src/main/java/parquet/avro/AvroWriteSupport.java
    //    private static final String AVRO_SCHEMA = "parquet.avro.schema";
    //  https://github.com/Parquet/parquet-mr/blob/master/parquet-avro/src/main/java/parquet/avro/AvroReadSupport.java
    //    private static final String AVRO_READ_SCHEMA = "parquet.avro.read.schema";
    //    static final String AVRO_SCHEMA_METADATA_KEY = "avro.schema";
    //    private static final String AVRO_READ_SCHEMA_METADATA_KEY = "avro.read.schema";
    // )
    //
    // Avro uses the first schema it finds or does a conversion from Parquet to Avro schema:
    //
    // (1) use the Avro read schema provided by the user set in "avro.read.schema" if present
    // (2) use the Avro schema from the file metadata in "avro.schema" if present
    // (3) default to converting the Parquet schema into an Avro schema
    //
    // Storing enum column values as enumerant strings makes more sense than storing integers
    // since Parquet does not store the enum type values as metadata.
    //
    // e.g. a column named "animal" containing enum values "cat", "dog", etc. is more useful
    //      than storing 0, 1, etc. with no type information mapping integers to enum values.
    //
    // Parquet file format could be updated to store the enum type metadata as an array of
    // enumerant/ordinal values.
    //

    ASTNode* element = new ASTNode(ASTNode::type::ENUMERANT,
                                       enumerant.getProto().getName().cStr());

    element->setOrdinal(enumerant.getOrdinal());

    if (currentParent_ == nullptr) {
      document_->addChild(element);
    } else {
      currentParent_->addChild(element);
    }
    currentParent_ = element;

    return false;
  }

  bool post_visit_enumerant(const Schema& schema, const EnumSchema::Enumerant& enumerant) override {
    // See NOTE(1) in this file.
    if (currentParent_ != nullptr) {
      currentParent_ = currentParent_->parent();
    }
    return false;
  }

  bool pre_visit_annotation_decl(const Schema& schema, const schema::Node::NestedNode::Reader& decl) override {
    auto proto = schema.getProto().getAnnotation();

    /*[[[cog
     targets = [
     'struct', 'interface', 'group', 'enum', 'file', 'field', 'union',
     'enumerant', 'annotation', 'const', 'param', 'method',
     ]
     for target in targets:
       cog.outl('if (proto.getTargets%s()) {' % target.title())
       cog.outl('  currentParent_->addTarget(std::string("%s"));' % target.lower())
       cog.outl('}')
     ]]]*/
    if (proto.getTargetsStruct()) {
      currentParent_->addTarget(std::string("struct"));
    }
    if (proto.getTargetsInterface()) {
      currentParent_->addTarget(std::string("interface"));
    }
    if (proto.getTargetsGroup()) {
      currentParent_->addTarget(std::string("group"));
    }
    if (proto.getTargetsEnum()) {
      currentParent_->addTarget(std::string("enum"));
    }
    if (proto.getTargetsFile()) {
      currentParent_->addTarget(std::string("file"));
    }
    if (proto.getTargetsField()) {
      currentParent_->addTarget(std::string("field"));
    }
    if (proto.getTargetsUnion()) {
      currentParent_->addTarget(std::string("union"));
    }
    if (proto.getTargetsEnumerant()) {
      currentParent_->addTarget(std::string("enumerant"));
    }
    if (proto.getTargetsAnnotation()) {
      currentParent_->addTarget(std::string("annotation"));
    }
    if (proto.getTargetsConst()) {
      currentParent_->addTarget(std::string("const"));
    }
    if (proto.getTargetsParam()) {
      currentParent_->addTarget(std::string("param"));
    }
    if (proto.getTargetsMethod()) {
      currentParent_->addTarget(std::string("method"));
    }
    //[[[end]]]
    return false;
  }

  bool post_visit_annotation_decl(const Schema& schema, const schema::Node::NestedNode::Reader& decl) override {
    return false;
  }

  bool pre_visit_struct_decl(const Schema& schema, const schema::Node::NestedNode::Reader& decl) override {
    struct_field_reason_ = kj::str("fields");
    return false;
  }

  bool post_visit_struct_decl(const Schema& schema, const schema::Node::NestedNode::Reader& decl) override {
    return false;
  }

  bool pre_visit_type(const Schema& schema, const schema::Type::Reader& type) override {
    capnp::schema::Type::Which capnp_type = type.which();

    std::string name;

    ASTNode* element = nullptr;

    //printf("pre_visit_type: capnp_type=%d\n", capnp_type);
    switch (capnp_type) {
      /*[[[cog
       types = ['void', 'bool', 'text', 'data', 'float32', 'float64']
       types.extend('int%s' % size for size in [8, 16, 32, 64])
       types.extend('uint%s' % size for size in [8, 16, 32, 64])
       for type in types:
         cog.outl('case schema::Type::%s:' % type.upper())
         cog.outl('  name = "%s";' % type.lower())
         cog.outl('  element = new ASTNode(ASTNode::TYPE, name);')
         cog.outl('  break;')
       ]]]*/
      case schema::Type::VOID:
        name = "void";
        element = new ASTNode(ASTNode::TYPE, name);
        break;
      case schema::Type::BOOL:
        name = "bool";
        element = new ASTNode(ASTNode::TYPE, name);
        break;
      case schema::Type::TEXT:
        name = "text";
        element = new ASTNode(ASTNode::TYPE, name);
        break;
      case schema::Type::DATA:
        name = "data";
        element = new ASTNode(ASTNode::TYPE, name);
        break;
      case schema::Type::FLOAT32:
        name = "float32";
        element = new ASTNode(ASTNode::TYPE, name);
        break;
      case schema::Type::FLOAT64:
        name = "float64";
        element = new ASTNode(ASTNode::TYPE, name);
        break;
      case schema::Type::INT8:
        name = "int8";
        element = new ASTNode(ASTNode::TYPE, name);
        break;
      case schema::Type::INT16:
        name = "int16";
        element = new ASTNode(ASTNode::TYPE, name);
        break;
      case schema::Type::INT32:
        name = "int32";
        element = new ASTNode(ASTNode::TYPE, name);
        break;
      case schema::Type::INT64:
        name = "int64";
        element = new ASTNode(ASTNode::TYPE, name);
        break;
      case schema::Type::UINT8:
        name = "uint8";
        element = new ASTNode(ASTNode::TYPE, name);
        break;
      case schema::Type::UINT16:
        name = "uint16";
        element = new ASTNode(ASTNode::TYPE, name);
        break;
      case schema::Type::UINT32:
        name = "uint32";
        element = new ASTNode(ASTNode::TYPE, name);
        break;
      case schema::Type::UINT64:
        name = "uint64";
        element = new ASTNode(ASTNode::TYPE, name);
        break;
      //[[[end]]]
      case schema::Type::LIST:
        name = "list";
        element = new ASTNode(ASTNode::TYPE, name);
        break;
      case schema::Type::ENUM: {
        auto enumSchema = schemaLoader.get(
                                           type.getEnum().getTypeId(), type.getEnum().getBrand(), schema);
        name = "enum";
        element = new ASTNode(ASTNode::TYPE, name);

        element->setTypeId(enumSchema.getProto().getId());
        element->setTypeName(enumSchema.getShortDisplayName().cStr());
        break;
      }
      case schema::Type::STRUCT: {
        auto structSchema = schemaLoader.get(
                                             type.getStruct().getTypeId(), type.getStruct().getBrand(), schema);

        // TODO(renesugar): Can previously declared STRUCT types
        //                  be used as fields in other STRUCT
        //                  declarations in Parquet?
        //printf("struct: %s\n", structSchema.getShortDisplayName().cStr());
        name = "struct";
        element = new ASTNode(ASTNode::TYPE, name);

        element->setTypeId(structSchema.getProto().getId());
        element->setTypeName(structSchema.getShortDisplayName().cStr());
        break;
      }
      case schema::Type::INTERFACE: {
        auto ifaceSchema = schemaLoader.get(
                                      type.getInterface().getTypeId(),
                                      type.getInterface().getBrand(),
                                      schema);
        name = "interface";
        element = new ASTNode(ASTNode::TYPE, name);

        element->setTypeId(ifaceSchema.getProto().getId());
        element->setTypeName(ifaceSchema.getShortDisplayName().cStr());
        break;
      }
      case schema::Type::ANY_POINTER:
        name = "anypointer";
        element = new ASTNode(ASTNode::TYPE, name);

        if (type.getAnyPointer().isUnconstrained()) {
            element->setIsUnconstrained();
        }
        break;
      default:
        break;
    }

    element->setCapnpType(capnp_type);

    if (currentParent_ == nullptr) {
      document_->addChild(element);
    } else {
      currentParent_->addChild(element);
    }
    currentParent_ = element;

    switch (capnp_type) {
      case schema::Type::LIST: {
        {
            type_reason_ = "elementType";
            auto _ = Finally([&](){type_reason_ = default_type_reason_;});

            TRAVERSE(type, schema, type.getList().getElementType());
        }
        return true;
      }
      case schema::Type::ENUM:
      case schema::Type::STRUCT:
      case schema::Type::INTERFACE:
        break;
      case schema::Type::ANY_POINTER:
        break;
      default:
        break;
    }

    return false;
  }

  bool post_visit_type(const Schema& schema, const schema::Type::Reader& type) override {
    ASTNode* element = ((currentParent_ == nullptr) ? document_ : currentParent_);

    // Build the Parquet schema node
    buildParquetNode(element);

    if (currentParent_ != nullptr) {
      currentParent_ = currentParent_->parent();
    }
    return false;
  }

  bool pre_visit_dynamic_value(const Schema& schema, const Type& type, const DynamicValue::Reader& value) override {
      capnp::schema::Type::Which capnp_type = type.which();

      ASTNode* element = new ASTNode(ASTNode::type::VALUE, schema.getShortDisplayName().cStr());

      /*value_reason_.cStr();*/
      value_reason_ = kj::str("ERROR");
      switch (capnp_type) {
        /*[[[cog
         sizes32 = [8, 16, 32]
         types = [
         ('bool', 'bool', 'bool'),
         ('int64', 'int64_t', 'i64'),
         ('uint64', 'uint64_t', 'ui64'),
         ('float32', 'float', 'float'),
         ('float64', 'double', 'double')
         ] + [
         ('int%d' % size, 'int%d_t' % size, 'i%d' % size) for size in sizes32
         ] + [
         ('uint%d' % size, 'uint%d_t' % size, 'ui%d' % size) for size in sizes32
         ]
         for type, ctype, writer in types:
           cog.outl('case schema::Type::%s:' % type.upper())
           cog.outl('  element->setValue%s(value.as<%s>());' % (writer.upper(), ctype))
           cog.outl('  break;')
         ]]]*/
        case schema::Type::BOOL:
          element->setValueBOOL(value.as<bool>());
          break;
        case schema::Type::INT64:
          element->setValueI64(value.as<int64_t>());
          break;
        case schema::Type::UINT64:
          element->setValueUI64(value.as<uint64_t>());
          break;
        case schema::Type::FLOAT32:
          element->setValueFLOAT(value.as<float>());
          break;
        case schema::Type::FLOAT64:
          element->setValueDOUBLE(value.as<double>());
          break;
        case schema::Type::INT8:
          element->setValueI8(value.as<int8_t>());
          break;
        case schema::Type::INT16:
          element->setValueI16(value.as<int16_t>());
          break;
        case schema::Type::INT32:
          element->setValueI32(value.as<int32_t>());
          break;
        case schema::Type::UINT8:
          element->setValueUI8(value.as<uint8_t>());
          break;
        case schema::Type::UINT16:
          element->setValueUI16(value.as<uint16_t>());
          break;
        case schema::Type::UINT32:
          element->setValueUI32(value.as<uint32_t>());
          break;
        //[[[end]]]
        case schema::Type::VOID:
          break;
        case schema::Type::TEXT:
          element->setValueSTRING(value.as<Text>().cStr());
          break;
        case schema::Type::DATA:
          for (auto data : value.as<Data>()) {
              element->appendValueBINARY(reinterpret_cast<const uint8_t*>(&data), 1);
          }
          break;
        case schema::Type::LIST: {
          break;
        }
        case schema::Type::STRUCT: {
          break;
        }
        case schema::Type::ENUM: {
          auto enumValue = value.as<DynamicEnum>();
          element->setOrdinal(enumValue.getRaw());
          KJ_IF_MAYBE(enumerant, enumValue.getEnumerant()) {
              element->setEnumerantName(enumerant->getProto().getName().cStr());
          }
          break;
        }
        case schema::Type::INTERFACE:
        case schema::Type::ANY_POINTER:
          // Cannot exist in a schema file.
          break;
        default:
          break;
      }

      element->setCapnpType(capnp_type);

      if (currentParent_ == nullptr) {
        document_->addChild(element);
      } else {
        currentParent_->addChild(element);
      }
      currentParent_ = element;

      return false;
  }

  bool post_visit_dynamic_value(const Schema& schema, const Type& type, const DynamicValue::Reader& value) override {
    /*switch (type.which()) {
        case schema::Type::LIST: {
            break;
        }
        case schema::Type::STRUCT: {
            break;
        }
        default: break;
    }*/
    ASTNode* element = ((currentParent_ == nullptr) ? document_ : currentParent_);

    // Build the Parquet schema node
    buildParquetNode(element);

    if (currentParent_ != nullptr) {
      currentParent_ = currentParent_->parent();
    }
    return false;
  }

  bool pre_visit_struct_fields(const StructSchema& schema) override {
    return false;
  }

  bool post_visit_struct_fields(const StructSchema& schema) override {
    return false;
  }

  bool pre_visit_struct_field(const StructSchema& schema, const StructSchema::Field& field) override {
    auto proto = field.getProto();

    ASTNode* element = new ASTNode(ASTNode::type::FIELD, proto.getName().cStr());

    auto ord = field.getProto().getOrdinal();

    if (ord.isExplicit()) {
        element->setOrdinal(ord.getExplicit());
    }

    if (currentParent_ == nullptr) {
      document_->addChild(element);
      //printf("parent: %s ", document_->name().c_str());
    } else {
      currentParent_->addChild(element);
      //printf("parent: %s ", document_->name().c_str());
    }
    currentParent_ = element;

    //printf("field: %s\n", proto.getName().cStr());
    return false;
  }

  bool pre_visit_struct_field_slot(const StructSchema& schema, const StructSchema::Field& field, const schema::Field::Slot::Reader& slot) override {
    capnp::schema::Type::Which capnp_type = slot.getType().which();

    currentParent_->setOffset(slot.getOffset());
    if (slot.getHadExplicitDefault()) {
      currentParent_->setHadDefaultValue();
    }
    currentParent_->setCapnpType(capnp_type);

    return false;
  }

  bool pre_visit_struct_default_value(const StructSchema& schema, const StructSchema::Field& field) override {
    value_reason_ = kj::str("default");
    return false;
  }

  bool post_visit_struct_field(const StructSchema& schema, const StructSchema::Field& field) override {
    ASTNode* element = ((currentParent_ == nullptr) ? document_ : currentParent_);

    // Build the Parquet schema node
    buildParquetNode(element);

    if (currentParent_ != nullptr)  {
      currentParent_ = currentParent_->parent();
    }
    return false;
  }

  bool pre_visit_interface_decl(const Schema& schema, const schema::Node::NestedNode::Reader& decl) override {
    return false;
  }

  bool post_visit_interface_decl(const Schema& schema, const schema::Node::NestedNode::Reader& decl) override {
    return false;
  }

  bool pre_visit_param_list(const InterfaceSchema& interfaceSchema, const kj::String& name, const StructSchema& schema) override {
    struct_field_reason_ = kj::str(name);
    return false;
  }

  bool post_visit_param_list(const InterfaceSchema& interfaceSchema, const kj::String& name, const StructSchema& schema) override {
    return false;
  }

  bool pre_visit_methods(const InterfaceSchema& interfaceSchema) override {
    return false;
  }

  bool post_visit_methods(const InterfaceSchema& interfaceSchema) override {
    return false;
  }

  bool pre_visit_method(const InterfaceSchema& interfaceSchema, const InterfaceSchema::Method& method) override {
    /*method.getProto().getName().cStr();*/
    /*method.getOrdinal();*/
    return false;
  }

  bool post_visit_method(const InterfaceSchema& interfaceSchema, const InterfaceSchema::Method& method) override {
    return false;
  }

  bool pre_visit_method_implicit_params(const InterfaceSchema& interfaceSchema,
                                        const InterfaceSchema::Method& method,
                                        const capnp::List<capnp::schema::Node::Parameter>::Reader& params) override {
/*
    for (auto param : params) {
        writer->String(param.getName().cStr());
    }
*/
    return false;
  }

  bool pre_visit_annotations(const Schema& schema) override {
    return false;
  }

  bool post_visit_annotations(const Schema& schema) override {
    return false;
  }

  bool pre_visit_annotation(const schema::Annotation::Reader& annotation, const Schema& schema) override {
    auto decl = schemaLoader.get(annotation.getId(), annotation.getBrand(), schema);

    //printf("pre_visit_annotation: %s\n", decl.getShortDisplayName().cStr());

    ASTNode* element = new ASTNode(ASTNode::type::ANNOTATION, decl.getShortDisplayName().cStr());

    element->setNodeId(annotation.getId());

    if (currentParent_ == nullptr) {
      document_->addChild(element);
    } else {
      currentParent_->addChild(element);
    }
    currentParent_ = element;

    value_reason_ = kj::str("value");
    return false;
  }

  bool post_visit_annotation(const schema::Annotation::Reader& annotation, const Schema& schema) override {
    if (currentParent_ != nullptr) {
      currentParent_ = currentParent_->parent();
    }
    return false;
  }
};

};  // namespace capnpparquet

#endif  // _CAPNPPARQUET_H_
