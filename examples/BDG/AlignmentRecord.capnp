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
#Alignment record.
#*/
struct AlignmentRecord {

#/**
#Read number within the array of fragment reads.
#*/
readInFragment @0 : UInt64 $Parquet.OPTIONAL;

#/**
#The reference sequence details for the reference chromosome that
#this read is aligned to. If the read is unaligned, this field should
#be null.
#*/
contigName @1 : Text $Parquet.OPTIONAL;

#/**
#0 based reference position for the start of this read's alignment.
#Should be null if the read is unaligned.
#*/
start @2 : UInt64 $Parquet.OPTIONAL;

#/**
#0 based reference position where this read used to start before
#local realignment. Stores the same data as the OP field in the SAM format.
#*/
oldPosition @3 : UInt64 $Parquet.OPTIONAL;

#/**
#0 based reference position for the end of this read's alignment.
#Should be null if the read is unaligned.
#*/
end @4 : UInt64 $Parquet.OPTIONAL;

#/**
#The global mapping quality of this read.
#*/
mapq @5 : UInt64 $Parquet.OPTIONAL;

#/**
#The name of this read. This should be unique within the read group
#that this read is from, and can be used to identify other reads that
#are derived from a single fragment.
#*/
readName @6 : Text $Parquet.OPTIONAL;

#/**
#The bases in this alignment. If the read has been hard clipped, this may
#not represent all the bases in the original read.
#*/
sequence @7 : Text $Parquet.OPTIONAL;

#/**
#The per-base quality scores in this alignment. If the read has been hard
#clipped, this may not represent all the bases in the original read.
#Additionally, if the error scores have been recalibrated, this field
#will not contain the original base quality scores.
#*/
qual @8 : Text $Parquet.OPTIONAL;

#/**
#The Compact Ideosyncratic Gapped Alignment Report (CIGAR) string that
#describes the local alignment of this read. Contains {length, operator}
#pairs for all contiguous alignment operations. The operators include:
#
#* M, ALIGNMENT_MATCH: An alignment match indicates that a sequence can be
#aligned to the reference without evidence of an INDEL. Unlike the
#SEQUENCE_MATCH and SEQUENCE_MISMATCH operators, the ALIGNMENT_MATCH
#operator does not indicate whether the reference and read sequences are an
#exact match.
#* I, INSERT: The insert operator indicates that the read contains evidence of
#bases being inserted into the reference.
#* D, DELETE: The delete operator indicates that the read contains evidence of
#bases being deleted from the reference.
#* N, SKIP: The skip operator indicates that this read skips a long segment of
#the reference, but the bases have not been deleted. This operator is
#commonly used when working with RNA-seq data, where reads may skip long
#segments of the reference between exons.
#* S, CLIP_SOFT: The soft clip operator indicates that bases at the start/end
#of a read have not been considered during alignment. This may occur if the
#majority of a read maps, except for low quality bases at the start/end of
#a read. Bases that are soft clipped will still be stored in the read.
#* H, CLIP_HARD: The hard clip operator indicates that bases at the start/end of
#a read have been omitted from this alignment. This may occur if this linear
#alignment is part of a chimeric alignment, or if the read has been trimmed
#(e.g., during error correction, or to trim poly-A tails for RNA-seq).
#* P, PAD: The pad operator indicates that there is padding in an alignment.
#* =, SEQUENCE_MATCH: This operator indicates that this portion of the aligned
#sequence exactly matches the reference (e.g., all bases are equal to the
#reference bases).
#* X, SEQUENCE_MISMATCH: This operator indicates that this portion of the
#aligned sequence is an alignment match to the reference, but a sequence
#mismatch (e.g., the bases are not equal to the reference). This can
#indicate a SNP or a read error.
#*/
cigar @9 : Text $Parquet.OPTIONAL;

#/**
#Stores the CIGAR string present before local indel realignment.
#Stores the same data as the OC field in the SAM format.
#*/
oldCigar @10 : Text $Parquet.OPTIONAL;

#/**
#The number of bases in this read/alignment that have been trimmed from the
#start of the read. By default, this is equal to 0. If the value is non-zero,
#that means that the start of the read has been hard-clipped.
#*/
basesTrimmedFromStart @11 : UInt64 $Parquet.OPTIONAL;

#/**
#The number of bases in this read/alignment that have been trimmed from the
#end of the read. By default, this is equal to 0. If the value is non-zero,
#that means that the end of the read has been hard-clipped.
#*/
basesTrimmedFromEnd @12 : UInt64 $Parquet.OPTIONAL;

#// Read flags (all default to false)
readPaired @13 : Bool = false $Parquet.OPTIONAL;
properPair @14 : Bool = false $Parquet.OPTIONAL;
readMapped @15 : Bool = false $Parquet.OPTIONAL;
mateMapped @16 : Bool = false $Parquet.OPTIONAL;
failedVendorQualityChecks @17 : Bool = false $Parquet.OPTIONAL;
duplicateRead @17 : Bool = false $Parquet.OPTIONAL;

#/**
#True if this alignment is mapped as a reverse compliment. This field
#defaults to false.
#*/
readNegativeStrand @18 : Bool = false $Parquet.OPTIONAL;

#/**
#True if the mate pair of this alignment is mapped as a reverse compliment.
#This field defaults to false.
#*/
union { boolean, null } mateNegativeStrand = false;
mateNegativeStrand @19 : Bool = false $Parquet.OPTIONAL;

#/**
#This field is true if this alignment is either the best linear alignment,
#or the first linear alignment in a chimeric alignment. Defaults to false.
#*/
primaryAlignment @20 : Bool = false $Parquet.OPTIONAL;

#/**
#This field is true if this alignment is a lower quality linear alignment
#for a multiply-mapped read. Defaults to false.
#*/
secondaryAlignment @20 : Bool = false $Parquet.OPTIONAL;

#/**
#This field is true if this alignment is a non-primary linear alignment in
#a chimeric alignment. Defaults to false.
#*/
supplementaryAlignment @20 : Bool = false $Parquet.OPTIONAL;

#// Commonly used optional attributes
mismatchingPositions @21 : Text $Parquet.OPTIONAL;
origQual @22 : Text $Parquet.OPTIONAL;

#// Remaining optional attributes flattened into a string
attributes @23 : Text $Parquet.OPTIONAL;

#// record group identifer from sequencing run
recordGroupName @24 : Text $Parquet.OPTIONAL;
recordGroupSample @25 : Text $Parquet.OPTIONAL;

#/**
#The start position of the mate of this read. Should be set to null if the
#mate is unaligned, or if the mate does not exist.
#*/
mateAlignmentStart @26 : UInt64 $Parquet.OPTIONAL;

#/**
#The reference contig of the mate of this read. Should be set to null if the
#mate is unaligned, or if the mate does not exist.
#*/
mateContigName @27 : Text $Parquet.OPTIONAL;

#/**
#The distance between this read and it's mate as inferred from alignment.
#*/
inferredInsertSize @26 : UInt64 $Parquet.OPTIONAL;
}
