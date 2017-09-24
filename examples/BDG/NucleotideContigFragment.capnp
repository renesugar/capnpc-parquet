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
#Stores a contig of nucleotides; this may be a reference chromosome, may be an
#assembly, may be a BAC. Very long contigs (>1Mbp) need to be split into fragments.
#It seems that they are too long to load in a single go. For best performance,
#it seems like 10kbp is a good point at which to start splitting contigs into
#fragments.
#*/
struct NucleotideContigFragment {

#/**
#The name of this contig in the assembly (e.g., "1").
#*/
contigName @0 : Text $Parquet.OPTIONAL;

#/**
#The total length of the contig this fragment is from.
#*/
contigLength @1 : UInt64 $Parquet.OPTIONAL;

#/**
#A description for this contig. When importing from FASTA, the FASTA header
#description line should be stored here.
#*/
description @2 : Text $Parquet.OPTIONAL;

#/**
#The sequence of bases in this fragment.
#*/
sequence @3 : Text $Parquet.OPTIONAL;

#/**
#In a fragmented contig, the index of this fragment in the set of fragments.
#Can be null if the contig is not fragmented.
#*/
index @4 : UInt64 $Parquet.OPTIONAL;

#/**
#The position of the first base of this fragment in the overall contig. E.g.,
#if all fragments are 10kbp and this is the third fragment in the contig,
#the start position would be 20000L.
#*/
start @5 : UInt64 $Parquet.OPTIONAL;

#/**
#The position of the last base of this fragment in the overall contig. E.g.,
#if all fragments are 10kbp and this is the third fragment in the contig,
#the end position would be 29999L.
#*/
end @6 : UInt64 $Parquet.OPTIONAL;

#/**
#The length of this fragment.
#*/
length @7 : UInt64 $Parquet.OPTIONAL;

#/**
#The total count of fragments that this contig has been broken into. Can be
#null if the contig is not fragmented.
#*/
fragments @8 : UInt64 $Parquet.OPTIONAL;
}
