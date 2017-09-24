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

using import "Strand.capnp".Strand;

struct Map(Key, Value) {
  entries @0 :List(Entry);
  struct Entry {
    key @0 :Key;
    value @1 :Value;
  }
}

#/**
#View of a contiguous region of a sequence.
#*/
struct Slice { #// extends Sequence

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
#Start position for this slice on the sequence this slice views, in 0-based coordinate
#system with closed-open intervals.
#*/
start @4 : UInt64 $Parquet.OPTIONAL;

#/**
#End position for this slice on the sequence this slice views, in 0-based coordinate
#system with closed-open intervals.
#*/
end @5 : UInt64 $Parquet.OPTIONAL;

#/**
#Strand for this slice, if any, defaults to Strand.INDEPENDENT.
#*/
strand @6 : Strand = INDEPENDENT $Parquet.OPTIONAL;

#/**
#Length of this slice.
#*/
length @7 : UInt64 $Parquet.OPTIONAL;

#/**
#Length of the sequence this slice views.
#*/
totalLength @8 : UInt64 $Parquet.OPTIONAL;

#/**
#Index of this slice in a set of slices that covers the sequence this slice views, if any.
#*/
index @9 : UInt64 $Parquet.OPTIONAL;

#/**
#Number of slices in a set of slices that covers the sequence this slice views, if any.
#*/
slices @10 : UInt64 $Parquet.OPTIONAL;

#/**
#Map of attributes.
#*/
attributes @11 : Map(Text, Text) $Parquet.OPTIONAL $Parquet.MAP;
}
