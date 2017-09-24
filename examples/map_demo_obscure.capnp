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

@0xc9ae354408c3fbfb;

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
# CREATE TABLE map_demo_obscure
# (
#   id BIGINT,
#   m1 MAP <INT, INT>,
#   m2 MAP <SMALLINT, INT>,
#   m3 MAP <TINYINT, INT>,
#   m4 MAP <TIMESTAMP, INT>,
#   m5 MAP <BOOLEAN, INT>,
#   m6 MAP <CHAR(5), INT>,
#   m7 MAP <VARCHAR(25), INT>,
#   m8 MAP <FLOAT, INT>,
#   m9 MAP <DOUBLE, INT>,
#   m10 MAP <DECIMAL(12,2), INT>
# )
#

struct MapDemoObscure $schema("schema") {
  id  @0 :UInt64 $required;
  m1  @1 :M1;
  m2  @2 :M2;
  m3  @3 :M3;
  m4  @4 :M4;
  m5  @5 :M5;
  m6  @6 :M6;
  m7  @7 :M7;
  m8  @8 :M8;
  m9  @9 :M9;
  m10 @10 :M10;

  struct M1 $map {
    key   @0 :Int32 $required;
    value @1 :Int32;
  }

  struct M2 $map {
    key   @0 :Int16 $required;
    value @1 :Int32;
  }

  struct M3 $map {
    key   @0 :Int8 $required;
    value @1 :Int32;
  }

  struct M4 $map {
    key   @0 :Int64 $timestampMillis $required;
    value @1 :Int32;
  }

  struct M5 $map {
    key   @0 :Bool $required;
    value @1 :Int32;
  }

  struct M6 $map {
    key   @0 :Data $fixed $length(5) $required;
    value @1 :Int32;
  }

  struct M7 $map {
    key   @0 :Text $length(25) $required;
    value @1 :Int32;
  }

  struct M8 $map {
    key   @0 :Float32 $required;
    value @1 :Int32;
  }

  struct M9 $map {
    key   @0 :Float64 $required;
    value @1 :Int32;
  }

  struct M10 $map {
    key   @0 :Data $decimal $precision(12) $scale(2) $required;
    value @1 :Int32;
  }
}

