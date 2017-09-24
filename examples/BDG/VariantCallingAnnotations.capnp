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
#This record represents all stats that, inside a VCF, are stored outside of the
#sample but are computed based on the samples. For instance, MAPQ0 is an aggregate
#stat computed from all samples and stored inside the INFO line.
#*/
struct VariantCallingAnnotations {

#/**
#True if filters were applied for this genotype call. FORMAT field "FT" any value other
#than the missing value.
#*/
filtersApplied @0 : Bool $Parquet.OPTIONAL;

#/**
#True if all filters for this genotype call passed. FORMAT field "FT" value PASS.
#*/
filtersPassed @1 : Bool $Parquet.OPTIONAL;

#/**
#Zero or more filters that failed for this genotype call from FORMAT field "FT".
#*/
filtersFailed @2 : List(Text) $Parquet.OPTIONAL;

#/**
#True if the reads covering this site were randomly downsampled to reduce coverage.
#*/
downsampled @3 : Bool $Parquet.OPTIONAL;

#/**
#The Wilcoxon rank-sum test statistic of the base quality scores. The base quality
#scores are separated by whether or not the base supports the reference or the
#alternate allele.
#*/
baseQRankSum @4 : Float64 $Parquet.OPTIONAL;

#/**
#The Fisher's exact test score for the strand bias of the reference and alternate
#alleles. Stored as a phred scaled probability. Thus, if:
#
#* a = The number of positive strand reads covering the reference allele
#* b = The number of positive strand reads covering the alternate allele
#* c = The number of negative strand reads covering the reference allele
#* d = The number of negative strand reads covering the alternate allele
#
#This value takes the score:
#
#-10 log((a + b)! * (c + d)! * (a + c)! * (b + d)! / (a! b! c! d! n!)
#
#Where n = a + b + c + d.
#*/
fisherStrandBiasPValue @5 : Float64 $Parquet.OPTIONAL;

#/**
#The root mean square of the mapping qualities of reads covering this site.
#*/
rmsMapQ @6 : Float64 $Parquet.OPTIONAL;

#/**
#The number of reads at this site with mapping quality equal to 0.
#*/
mapq0Reads @7 : UInt64 $Parquet.OPTIONAL;

#/**
#The Wilcoxon rank-sum test statistic of the mapping quality scores. The mapping
#quality scores are separated by whether or not the read supported the reference or the
#alternate allele.
#*/
mqRankSum @8 : Float64 $Parquet.OPTIONAL;

#/**
#The Wilcoxon rank-sum test statistic of the position of the base in the read at this site.
#The positions are separated by whether or not the base supports the reference or the
#alternate allele.
#*/
readPositionRankSum @9 : Float64 $Parquet.OPTIONAL;

#/**
#The log scale prior probabilities of the various genotype states at this site.
#The number of elements in this array should be equal to the ploidy at this
#site, plus 1.
#*/
genotypePriors @10 : List(Float64) $Parquet.OPTIONAL;

#/**
#The log scaled posterior probabilities of the various genotype states at this site,
#in this sample. The number of elements in this array should be equal to the ploidy at
#this site, plus 1.
#*/
genotypePosteriors @11 : List(Float64) $Parquet.OPTIONAL;

#/**
#The log-odds ratio of being a true vs. false variant under a trained statistical model.
#This model can be a multivariate Gaussian mixture, support vector machine, etc.
#*/
vqslod @12 : Float64 $Parquet.OPTIONAL;

#/**
#If known, the feature that contributed the most to this variant being classified as
#a false variant.
#*/
culprit @13 : Text $Parquet.OPTIONAL;

#/**
#Additional feature info that doesn't fit into the standard fields above.
#They are all encoded as (string, string) key-value pairs.
#*/
attributes @14 : Map(Text, Text) $Parquet.OPTIONAL $Parquet.MAP;
}
