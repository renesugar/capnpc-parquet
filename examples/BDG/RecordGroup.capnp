#/**
#* Licensed to Big Data Genomics (BDG) under one
#* or more contributor license agreements.  See the NOTICE file
#* distributed with this work for additional information
#* regarding copyright ownership.  The BDG licenses this file
#* to you under the Apache License, Version 2.0 (the
#* "License"); you may not use this file except in compliance
#* with the License.  You may obtain a copy of the License at
#*
#*     http://www.apache.org/licenses/LICENSE-2.0
#*
#* Unless required by applicable law or agreed to in writing, software
#* distributed under the License is distributed on an "AS IS" BASIS,
#* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#* See the License for the specific language governing permissions and
#* limitations under the License.
#*/

using Parquet = import "Parquet.capnp";

using import "ProcessingStep.capnp".ProcessingStep;

#/**
#Record group metadata.
#*/
struct RecordGroup {

#/**
#Record group identifier.
#*/
name @0 : Text $Parquet.OPTIONAL;

#/**
#Name of the sample that the record group is from.
#*/
sample @1 : Text $Parquet.OPTIONAL;

sequencingCenter @2 : Text $Parquet.OPTIONAL;
description @3 : Text $Parquet.OPTIONAL;
runDateEpoch @4 : UInt64 $Parquet.OPTIONAL;
flowOrder @5 : Text $Parquet.OPTIONAL;
keySequence @6 : Text $Parquet.OPTIONAL;
library @7 : Text $Parquet.OPTIONAL;
predictedMedianInsertSize @8 : UInt64 $Parquet.OPTIONAL;
union { null, string } platform = null;
platform @9 : Text $Parquet.OPTIONAL;
platformUnit @10 : Text $Parquet.OPTIONAL;

#/**
#The processing steps that have been applied to this record group.
#*/
processingSteps @11 : List(ProcessingStep) $Parquet.OPTIONAL;
}
