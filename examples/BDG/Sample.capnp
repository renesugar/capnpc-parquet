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

using import "ProcessingStep.capnp".ProcessingStep;

struct Map(Key, Value) {
  entries @0 :List(Entry);
  struct Entry {
    key @0 :Key;
    value @1 :Value;
  }
}

#/**
#Sample.
#*/
struct Sample {

#/**
#Identifier for this sample, e.g. IDENTIFIERS &rarr; PRIMARY_ID or other
#subelements of IDENTIFERS in SRA metadata, sample tag SM in read group @RG header lines
#in SAM/BAM files, or sample ID from the header or ##SAMPLE=&lt;ID=S_ID meta-information
#lines in VCF files.
#*/
sampleId @0 : Text $Parquet.OPTIONAL;

#/**
#Descriptive name for this sample, e.g. SAMPLE_NAME &rarr; TAXON_ID, COMMON_NAME,
#INDIVIDUAL_NAME, or other subelements of SAMPLE_NAME in SRA metadata.
#*/
name @1 : Text $Parquet.OPTIONAL;

#/**
#Map of attributes. Common attributes may include: SRA metadata not mentioned above,
#e.g. SAMPLE &rarr; TITLE, SAMPLE &rarr; DESCRIPTION, and SAMPLE_ATTRIBUTES; ENA default
#sample checklist attributes such as cell_type, dev_stage, and germline; and Genomes,
#Mixture, and Description from sample meta-information lines in VCF files.
#*/
attributes @2 : Map(Text, Text) $Parquet.OPTIONAL $Parquet.MAP;

#/**
#The processing steps that have been applied to this sample.
#*/
processingSteps @3 : List(ProcessingStep) $Parquet.OPTIONAL;
}
