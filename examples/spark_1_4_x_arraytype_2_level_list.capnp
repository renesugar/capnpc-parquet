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

@0xbc3a181731ec2655;

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
# Spark 1.4.x and prior versions convert ArrayType with non-nullable elements into a 2-level
# LIST structure.  This behavior mimics parquet-avro (1.6.0rc3).  Note that this case is
# covered by the backwards-compatibility rules implemented in `isElementType()`.
#
# <list-repetition> group <name> (LIST) {
#   repeated <element-type> element;
# }
#
# Here too, we should not use `listOfElements`. (See SPARK-16777)
#
# NOTE: Comment in Spark says "array" but description above shows "element":
#
# "array" is the name chosen by parquet-avro (1.7.0 and prior version)
#

struct Todo $schema("schema") {
  id    @0 :UInt64 $required;
  tasks @1 :Tasks;

  struct Tasks $list {
    element @0 :Text $repeated;
  }
}
