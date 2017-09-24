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

using import "AlignmentRecord.capnp".AlignmentRecord;

#/**
#The DNA fragment that is was targeted by the sequencer, resulting in
#one or more reads.
#*/
struct Fragment {

#/**
#The name of this fragment.
#*/
readName @0 : Text $Parquet.OPTIONAL;

instrument @1 : Text $Parquet.OPTIONAL;
runId @2: Text $Parquet.OPTIONAL;

#/**
#Fragment's insert size derived from alignment, if the reads have been
#aligned.
#*/
fragmentSize @3 : UInt64 $Parquet.OPTIONAL;

#/**
#The sequences read from this fragment.
#*/
alignments @4 : List(AlignmentRecord) $Parquet.OPTIONAL;
}
