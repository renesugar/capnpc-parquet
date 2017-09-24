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

using import "Variant.capnp".Variant;

using import "VariantCallingAnnotations.capnp".VariantCallingAnnotations;

using import "GenotypeAllele.capnp".GenotypeAllele;

#/**
#Genotype.
#*/
struct Genotype {

#/**
#The variant called at this site.
#*/
variant @0 : Variant $Parquet.OPTIONAL;

#/**
#The reference contig that this genotype's variant exists on.
#*/
contigName @1 : Text $Parquet.OPTIONAL;

#/**
#The 0-based start position of this genotype's variant on the reference contig.
#*/
start @2 : UInt64 $Parquet.OPTIONAL;

#/**
#The 0-based, exclusive end position of this genotype's variant on the reference contig.
#*/
union { null, long } end = null;
end @3 : UInt64 $Parquet.OPTIONAL;

#/**
#Statistics collected at this site, if available.
#*/
union { null, VariantCallingAnnotations } variantCallingAnnotations = null;
variantCallingAnnotations @4 : VariantCallingAnnotations $Parquet.OPTIONAL;

#/**
#The unique identifier for this sample.
#*/
sampleId @5 : Text $Parquet.OPTIONAL;

#/**
#A description of this sample.
#*/
sampleDescription @6 : Text $Parquet.OPTIONAL;

#/**
#A string describing the provenance of this sample and the processing applied
#in genotyping this sample.
#*/
processingDescription @7 : Text $Parquet.OPTIONAL;

#/**
#An array describing the genotype called at this site. The length of this
#array is equal to the ploidy of the sample at this site. This array may
#reference OTHER_ALT alleles if this site is multi-allelic in this sample.
#*/
alleles @8 : List(GenotypeAllele) $Parquet.OPTIONAL;

#/**
#The expected dosage of the alternate allele in this sample.
#*/
expectedAlleleDosage @9 : Float64 $Parquet.OPTIONAL;

#/**
#The number of reads that show evidence for the reference at this site.
#*/
referenceReadDepth @10 : UInt64 $Parquet.OPTIONAL;

#/**
#The number of reads that show evidence for this alternate allele at this site.
#*/
alternateReadDepth @11 : UInt64 $Parquet.OPTIONAL;

#/**
#The total number of reads at this site. May not equal (alternateReadDepth +
#referenceReadDepth) if this site shows evidence of multiple alternate alleles.
#Analogous to VCF's DP.
#*/
readDepth @12 : UInt64 $Parquet.OPTIONAL;

#/**
#The minimum number of reads seen at this site across samples when joint
#calling variants. Analogous to VCF's MIN_DP.
#*/
minReadDepth @13 : UInt64 $Parquet.OPTIONAL;

#/**
#The phred-scaled probability that we're correct for this genotype call.
#Analogous to VCF's GQ.
#*/
genotypeQuality @14 : UInt64 $Parquet.OPTIONAL;

#/**
#Log scaled likelihoods that we have n copies of this alternate allele.
#The number of elements in this array should be equal to the ploidy at this
#site, plus 1. Analogous to VCF's PL.
#*/
genotypeLikelihoods @15 : List(Float64) $Parquet.OPTIONAL;

#/**
#Log scaled likelihoods that we have n non-reference alleles at this site.
#The number of elements in this array should be equal to the ploidy at this
#site, plus 1.
#*/
nonReferenceLikelihoods @16 : List(Float64) $Parquet.OPTIONAL;

#/**
#Component statistics which comprise the Fisher's Exact Test to detect strand bias.
#If populated, this element should have length 4.
#*/
strandBiasComponents @17 : List(UInt64) $Parquet.OPTIONAL;

#/**
#We split multi-allelic VCF lines into multiple single-alternate records.
#This bit is set if that happened for this record.
#*/
splitFromMultiAllelic @18 : Bool = false $Parquet.OPTIONAL;

#/**
#True if this genotype is phased.
#*/
phased @19 : Bool = false $Parquet.OPTIONAL;

#/**
#The ID of this phase set, if this genotype is phased. Should only be populated
#if phased == true; else should be null.
#*/
phaseSetId @20 : UInt64 $Parquet.OPTIONAL;

#/**
#Phred scaled quality score for the phasing of this genotype, if this genotype
#is phased. Should only be populated if phased == true; else should be null.
#*/
phaseQuality @21 : UInt64 $Parquet.OPTIONAL;
}
