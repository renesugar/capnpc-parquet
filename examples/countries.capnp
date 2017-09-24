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

@0xc914fe37e9167a6d;

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
# https://impala.incubator.apache.org/docs/build/html/topics/impala_map.html
#
# CREATE TABLE countries (name STRING, famous_leaders MAP < INT, STRING >) STORED AS PARQUET;
# -- A typical row might represent values with different leaders, with key values corresponding to their numeric sequence, such as:
# -- ("United States", { 1: "George Washington", 3: "Thomas Jefferson", 16: "Abraham Lincoln" })
#

struct Countries $schema("schema") {
  name           @0 :Text $required;
  famousLeaders  @1 :FamousLeaders;

  struct FamousLeaders $map {
    struct KeyValue $repeated $mapKeyValue {
      key   @0 :Int32 $required;
      value @1 :Text;
    }
  }
}

