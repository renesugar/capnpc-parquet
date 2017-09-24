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
#Errors, warnings, or informative messages regarding variant annotation accuracy.
#*/
enum VariantAnnotationMessage {

#/**
#Chromosome does not exist in reference genome database. Typically indicates
#a mismatch between the chromosome names in the input file and the chromosome
#names used in the reference genome. Message code E1.
#*/
ERROR_CHROMOSOME_NOT_FOUND @0;

#/**
#The variant's genomic coordinate is greater than chromosome's length.
#Message code E2.
#*/
ERROR_OUT_OF_CHROMOSOME_RANGE @1;

#/**
#The 'REF' field in the input VCF file does not match the reference genome.
#This warning may indicate a conflict between input data and data from
#reference genome (for instance is the input VCF was aligned to a different
#reference genome). Message code W1.
#*/
WARNING_REF_DOES_NOT_MATCH_GENOME @2;

#/**
#Reference sequence is not available, thus no inference could be performed.
#Message code W2.
#*/
WARNING_SEQUENCE_NOT_AVAILABLE @3;

#/**
#A protein coding transcript having a non­multiple of 3 length. It indicates
#that the reference genome has missing information about this particular
#transcript. Message code W3.
#*/
WARNING_TRANSCRIPT_INCOMPLETE @4;

#/**
#A protein coding transcript has two or more STOP codons in the middle of
#the coding sequence (CDS). This should not happen and it usually means the
#reference genome may have an error in this transcript. Message code W4.
#*/
WARNING_TRANSCRIPT_MULTIPLE_STOP_CODONS @5;

#/**
#A protein coding transcript does not have a proper START codon. It is
#rare that a real transcript does not have a START codon, so this probably
#indicates an error or missing information in the reference genome.
#Message code W5.
#*/
WARNING_TRANSCRIPT_NO_START_CODON @6;

#/**
#Variant has been realigned to the most 3­prime position within the
#transcript. This is usually done to to comply with HGVS specification
#to always report the most 3­prime annotation. Message code I1.
#*/
INFO_REALIGN_3_PRIME @7;

#/**
#This effect is a result of combining more than one variants (e.g. two
#consecutive SNPs that conform an MNP, or two consecutive frame_shift
#variants that compensate frame). Message code I2.
#*/
INFO_COMPOUND_ANNOTATION @8;

#/**
#An alternative reference sequence was used to calculate this annotation
#(e.g. cancer sample comparing somatic vs. germline). Message code I3.
#*/
INFO_NON_REFERENCE_ANNOTATION @9;
}
