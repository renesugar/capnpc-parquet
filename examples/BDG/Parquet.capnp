#/*
#* Copyright 2017 Rene Sugar
#*
#* Licensed under the Apache License, Version 2.0 (the "License");
#* you may not use this file except in compliance with the License.
#* You may obtain a copy of the License at
#*
#*     http://www.apache.org/licenses/LICENSE-2.0
#*
#* Unless required by applicable law or agreed to in writing, software
#* distributed under the License is distributed on an "AS IS" BASIS,
#* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#* See the License for the specific language governing permissions and
#* limitations under the License.
#*/
#
#/*
#* @file Parquet.capnp
#* @author Rene Sugar <rene.sugar@gmail.com>
#* @brief Annotations for converting Cap'n Proto schema to a Parquet schema.
#*/

# Use Cap'n Proto annotations to indicate Parquet schema attributes
#
# https://github.com/Parquet/parquet-format/blob/master/src/thrift/parquet.thrift
#
# https://github.com/Parquet/parquet-format/blob/master/LogicalTypes.md
#
# https://github.com/Parquet/parquet-format/blob/master/Encodings.md
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
