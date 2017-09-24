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

using import "QualityScoreVariant.capnp".QualityScoreVariant;

struct Map(Key, Value) {
  entries @0 :List(Entry);
  struct Entry {
    key @0 :Key;
    value @1 :Value;
  }
}

#/**
#Sequence with quality scores.
#*/
struct Read { #// extends Sequence

#/**
#Name of this read.
#*/
name @0 : Text $Parquet.OPTIONAL;

#/**
#Description for this read.
#*/
description @1 : Text $Parquet.OPTIONAL;

#/**
#Alphabet for this read, defaults to Alphabet.DNA.
#*/
alphabet @2 : Alphabet = DNA $Parquet.OPTIONAL;

#/**
#Sequence for this read.
#*/
sequence @3 : Text $Parquet.OPTIONAL;

#/**
#Length of this read.
#*/
length @4 : UInt64 $Parquet.OPTIONAL;

#/**
#Quality scores for this read.
#*/
qualityScores @5 : Text $Parquet.OPTIONAL;

#/**
#Quality score variant for this read, defaults to QualityScoreVariant.FASTQ_SANGER.
#*/
qualityScoreVariant @6 : QualityScoreVariant = FASTQ_SANGER $Parquet.OPTIONAL;

#/**
#Map of attributes.
#*/
attributes @7 : Map(Text, Text) $Parquet.OPTIONAL $Parquet.MAP;
}
