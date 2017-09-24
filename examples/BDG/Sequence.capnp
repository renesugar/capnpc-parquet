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

using import "Alphabet.capnp".Alphabet;

struct Map(Key, Value) {
  entries @0 :List(Entry);
  struct Entry {
    key @0 :Key;
    value @1 :Value;
  }
}

#/**
#Contiguous sequence from an alphabet, e.g. a DNA contig, an RNA transcript,
#or a protein translation.
#*/
struct Sequence {

#/**
#Name of this sequence.
#*/
name @0 : Text $Parquet.OPTIONAL;

#/**
#Description for this sequence.
#*/
description @1 : Text $Parquet.OPTIONAL;

#/**
#Alphabet for this sequence, defaults to Alphabet.DNA.
#*/
alphabet @2 : Alphabet = DNA $Parquet.OPTIONAL;

#/**
#Sequence.
#*/
sequence @3 : Text $Parquet.OPTIONAL;

#/**
#Length of this sequence.
#*/
length @4 : UInt64 $Parquet.OPTIONAL;

#/**
#Map of attributes.
#*/
attributes @5 : Map(Text, Text) $Parquet.OPTIONAL $Parquet.MAP;
}
