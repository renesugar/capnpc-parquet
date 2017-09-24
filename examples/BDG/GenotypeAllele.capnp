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
#An enumeration that describes the allele that corresponds to a genotype.
#*/
enum GenotypeAllele {

#/**
#The genotype is the reference allele.
#*/
REF @0;

#/**
#The genotype is the alternate allele.
#*/
ALT @1;

#/**
#The genotype is an unspecified other alternate allele. This occurs in our schema
#when we have split a multi-allelic genotype into two genotype records.
#*/
OTHER_ALT @2;

#/**
#The genotype could not be called.
#*/
NO_CALL @3;
}
