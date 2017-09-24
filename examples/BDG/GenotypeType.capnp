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
#An enumeration that describes the characteristics of a genotype at a site.
#*/
enum GenotypeType {

#/**
#All genotypes at this site were called as the reference allele.
#*/
HOM_REF @0;

#/**
#Genotypes at this site were called as multiple different alleles. This
#most commonly occurs if a diploid sample's genotype contains one reference
#and one variant allele, but can also occur if the genotype contains multiple
#alternate alleles.
#*/
HET @1;

#/**
#All genotypes at this site were called as a single alternate allele.
#*/
HOM_ALT @2;

#/**
#The genotype could not be called at this site.
#*/
NO_CALL @3;
}
