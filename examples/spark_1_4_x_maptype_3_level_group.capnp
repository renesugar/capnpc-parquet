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

@0x8a2edc2fa684e544;

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
# Spark 1.4.x and prior versions convert MapType into a 3-level group annotated by
# MAP_KEY_VALUE.
#
# <map-repetition> group <name> (MAP) {
#   repeated group map (MAP_KEY_VALUE) {
#     required <key-type> key;
#     <value-repetition> <value-type> value;
#   }
# }
#

struct Menu $schema("schema") {
  id    @0 :UInt64 $required;
  items @1 :MenuItems;

  struct MenuItems $map {
    struct KeyValue $repeated $mapKeyValue {
      key   @0 :Text $required;
      value @1 :Text;
    }
  }
}

