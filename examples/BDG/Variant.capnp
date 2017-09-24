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

using import "VariantAnnotation.capnp".VariantAnnotation;


#/**
#Variant.
#*/
struct Variant {

#/**
#The reference contig this variant exists on. VCF column 1 "CONTIG".
#*/
contigName @0 : Text $Parquet.OPTIONAL;

#/**
#The zero-based start position of this variant on the reference contig.
#VCF column 2 "POS" converted to zero-based coordinate system, closed-open intervals.
#*/
start @1 : UInt64 $Parquet.OPTIONAL;

#/**
#The zero-based, exclusive end position of this variant on the reference contig.
#Calculated by start + referenceAllele.length().
#*/
end @2 : UInt64 $Parquet.OPTIONAL;

#/**
#Zero or more unique names or identifiers for this variant. If this is a dbSNP
#variant it is encouraged to use the rs number(s). VCF column 3 "ID" shared across
#all alleles in the same VCF record.
#*/
names @3 : List(Text) $Parquet.OPTIONAL;

#/**
#A string describing the reference allele at this site. VCF column 4 "REF".
#*/
referenceAllele @4 : Text $Parquet.OPTIONAL;

#/**
#A string describing the alternate allele at this site. VCF column 5 "ALT" split
#for multi-allelic sites.
#*/
alternateAllele @5 : Text $Parquet.OPTIONAL;

#/**
#True if filters were applied for this variant. VCF column 7 "FILTER" any value other
#than the missing value.
#*/
filtersApplied @6 : Bool $Parquet.OPTIONAL;

#/**
#True if all filters for this variant passed. VCF column 7 "FILTER" value PASS.
#*/
filtersPassed @7 : Bool $Parquet.OPTIONAL;

#/**
#Zero or more filters that failed for this variant. VCF column 7 "FILTER" shared across
#all alleles in the same VCF record.
#*/
filtersFailed @8 : List(Text) $Parquet.OPTIONAL;

#/**
#Annotation for this variant, if any.
#*/
annotation @9 : VariantAnnotation $Parquet.OPTIONAL;
}
