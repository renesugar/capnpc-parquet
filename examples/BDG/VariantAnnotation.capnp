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

using import "TranscriptEffect.capnp".TranscriptEffect;

struct Map(Key, Value) {
  entries @0 :List(Entry);
  struct Entry {
    key @0 :Key;
    value @1 :Value;
  }
}

#/**
#Variant annotation.
#*/
struct VariantAnnotation {

#/**
#Ancestral allele, VCF INFO reserved key AA, Number=1, shared across all alternate
#alleles in the same VCF record.
#*/
ancestralAllele @0 : Text $Parquet.OPTIONAL;

#/**
#Allele count, VCF INFO reserved key AC, Number=A, split for multi-allelic sites into
#a single integer value.
#*/
alleleCount @1 : UInt64 $Parquet.OPTIONAL;

#/**
#Total read depth, VCF INFO reserved key AD, Number=R, split for multi-allelic
#sites into single integer values for the reference allele (referenceReadDepth) and
#the alternate allele (readDepth, this field).
#*/
readDepth @2 : UInt64 $Parquet.OPTIONAL;

#/**
#Forward strand read depth, VCF INFO reserved key ADF, Number=R, split for
#multi-allelic sites into single integer values for the reference allele
#(referenceForwardReadDepth) and the alternate allele (forwardReadDepth, this field).
#*/
forwardReadDepth @3 : UInt64 $Parquet.OPTIONAL;

#/**
#Reverse strand read depth, VCF INFO reserved key ADR, Number=R, split for
#multi-allelic sites into single integer values for the reference allele
#(referenceReverseReadDepth) and the alternate allele (reverseReadDepth, this field).
#*/
reverseReadDepth @4 : UInt64 $Parquet.OPTIONAL;

#/**
#Total read depth, VCF INFO reserved key AD, Number=R, split for multi-allelic
#sites into single integer values for the reference allele (referenceReadDepth, this field)
#and the alternate allele (readDepth).
#*/
referenceReadDepth @5 : UInt64 $Parquet.OPTIONAL;

#/**
#Forward strand read depth, VCF INFO reserved key ADF, Number=R, split for
#multi-allelic sites into single integer values for the reference allele
#(referenceForwardReadDepth, this field) and the alternate allele (forwardReadDepth).
#*/
referenceForwardReadDepth @6 : UInt64 $Parquet.OPTIONAL;

#/**
#Reverse strand read depth, VCF INFO reserved key ADR, Number=R, split for
#multi-allelic sites into single integer values for the reference allele
#(referenceReverseReadDepth, this field) and the alternate allele (reverseReadDepth).
#*/
union { null, int } referenceReverseReadDepth = null;
referenceReverseReadDepth @7 : UInt64 $Parquet.OPTIONAL;

#/**
#Minor allele frequency, VCF INFO reserved key AF, Number=A, split for multi-allelic
#sites into a single float value. Use this when frequencies are estimated from primary
#data, not calculated from called genotypes.
#*/
alleleFrequency @8 : Float64 $Parquet.OPTIONAL;

#/**
#CIGAR string describing how to align an alternate allele to the reference
#allele, VCF INFO reserved key CIGAR, Number=A, split for multi-allelic sites into
#a single string value.
#*/
cigar @9 : Text $Parquet.OPTIONAL;

#/**
#Membership in dbSNP, VCF INFO reserved key DB, Number=0. Until Number=A and
#Number=R flags are supported by the VCF specification, this value is shared
#across all alternate alleles in the same VCF record.
#*/
dbSnp @10 : Bool $Parquet.OPTIONAL;

#/**
#Membership in HapMap2, VCF INFO reserved key H2, Number=0. Until Number=A and
#Number=R flags are supported by the VCF specification, this value is shared
#across all alternate alleles in the same VCF record.
#*/
hapMap2 @10 : Bool $Parquet.OPTIONAL;

#/**
#Membership in HapMap3, VCF INFO reserved key H3, Number=0. Until Number=A and
#Number=R flags are supported by the VCF specification, this value is shared
#across all alternate alleles in the same VCF record.
#*/
hapMap3 @11 : Bool $Parquet.OPTIONAL;

#/**
#Validated by follow up experiment, VCF INFO reserved key VALIDATED, Number=0.
#Until Number=A and Number=R flags are supported by the VCF specification, this
#value is shared across all alternate alleles in the same VCF record.
#*/
validated @12 : Bool $Parquet.OPTIONAL;

#/**
#Membership in 1000 Genomes, VCF INFO reserved key 1000G, Number=0. Until
#Number=A and Number=R flags are supported by the VCF specification, this
#value is shared across all alternate alleles in the same VCF record.
#*/
thousandGenomes @13 : Bool $Parquet.OPTIONAL;

#/**
#True if this variant call is somatic; in this case, the reference allele will
#have been observed in another sample. VCF INFO reserved key "SOMATIC", Number=0.
#Until Number=A and Number=R flags are supported by the VCF specification, this value
#is shared across all alleles in the same VCF record.
#*/
somatic @14 : Bool = false $Parquet.OPTIONAL;

#/**
#Zero or more transcript effects, predicted by a tool such as SnpEff or Ensembl VEP,
#one per transcript (or other feature). VCF INFO key ANN, split for multi-allelic
#sites. See http://snpeff.sourceforge.net/VCFannotationformat_v1.0.pdf.
#*/
transcriptEffects @15 : List(TranscriptEffect) $Parquet.OPTIONAL;

#/**
#Additional variant attributes that do not fit into the standard fields above.
#The values are stored as strings, even for flag, integer, and float types. VCF
#INFO key values with Number=., Number=0, Number=1, and Number=[n] are shared across
#all alternate alleles in the same VCF record. VCF INFO key values with Number=A are
#split for multi-allelic sites into a single value. VCF INFO key values with Number=R
#are split into an array of two values, [reference allele, alternate allele], separated
#by commas, e.g. "0,1".
#*/
attributes @16 : Map(Text, Text) $Parquet.OPTIONAL $Parquet.MAP;
}
