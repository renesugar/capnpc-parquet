#
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The ASF licenses this file to You under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with
# the License.  You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

@0xfd2194942e881e06;

# Use Cap'n Proto annotations to indicate Parquet schema attributes
#
# https://capnproto.org/language.html
#
annotation schema(struct)        :Text;
annotation required(*)           :Void;
annotation optional(*)           :Void;
annotation repeated(*)           :Void;
annotation length(*)             :Int32;
annotation scale(*)              :Int32;
annotation precision(*)          :Int32;
annotation decimal(*)            :Void;
annotation date(*)               :Void;
annotation timeMillis(*)         :Void;
annotation timeMicros(*)         :Void;
annotation timestampMillis(*)    :Void;
annotation timestampMicros(*)    :Void;
annotation bson(*)               :Void;
annotation json(*)               :Void;
annotation interval(*)           :Void;
annotation fixed(*)              :Void;
annotation map(*)                :Void;
annotation mapKeyValue(*)        :Void;
annotation list(*)               :Void;

#
# https://github.com/apache/spark/blob/master/sql/core/src/main/scala/org/apache/spark/sql/execution/datasources/parquet/ParquetSchemaConverter.scala
#
# ===================================
# ArrayType and MapType (legacy mode)
# ===================================
#
# Spark 1.4.x and prior versions convert `ArrayType` with nullable elements into a 3-level
# `LIST` structure.  This behavior is somewhat a hybrid of parquet-hive and parquet-avro
# (1.6.0rc3): the 3-level structure is similar to parquet-hive while the 3rd level element
# field name "array" is borrowed from parquet-avro.
# <list-repetition> group <name> (LIST) {
#   optional group bag {
#     repeated <element-type> array;
#   }
# }
#
# This should not use `listOfElements` here because this new method checks if the
# element name is `element` in the `GroupType` and throws an exception if not.
# As mentioned above, Spark prior to 1.4.x writes `ArrayType` as `LIST` but with
# `array` as its element name as below. Therefore, we build manually
# the correct group type here via the builder. (See SPARK-16777)
#
# "array" is the name chosen by parquet-hive (1.7.0 and prior version)
#

struct Todo $schema("schema") {
  id    @0 :UInt64 $required;
  tasks @1 :Tasks;

  struct Tasks $list {
    struct Bag {
      array   @0 :Text $repeated;
    }
  }
}
