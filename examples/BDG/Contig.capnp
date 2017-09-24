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

#/**
#Record for describing a reference assembly. Not used for storing the contents
#of said assembly.
#*/
struct Contig {

#/**
#The name of this contig in the assembly (e.g., "1").
#*/
contigName @0 : Text $Parquet.OPTIONAL;

#/**
#The length of this contig.
#*/
contigLength @1 : UInt64 $Parquet.OPTIONAL;

#/**
#The MD5 checksum of the assembly for this contig.
#*/
contigMD5 @2 : Text $Parquet.OPTIONAL;

#/**
#The URL at which this reference assembly can be found.
#*/
referenceURL @3 : Text $Parquet.OPTIONAL;

#/**
#The name of this assembly (e.g., "hg19").
#*/
union { null, string } assembly = null;
assembly @4 : Text $Parquet.OPTIONAL;

#/**
#The species that this assembly is for.
#*/
species @5 : Text $Parquet.OPTIONAL;

#/**
#Optional 0-based index of this contig in a SAM file header that it was read
#from; helps output SAMs/BAMs with headers in the same order as they started
#with, before a conversion to ADAM.
#*/
referenceIndex @6 : UInt64 $Parquet.OPTIONAL;
}
