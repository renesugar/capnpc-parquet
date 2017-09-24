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
#Quality score variant.
#*/
enum QualityScoreVariant {

#/**
#Sanger and Illumina version &gt;= 1.8 FASTQ quality score variant.
#*/
FASTQ_SANGER @0;

#/**
#Solexa and Illumina version 1.0 FASTQ quality score variant.
#*/
FASTQ_SOLEXA @1;

#/**
#Illumina version &gt;= 1.3 and &lt; 1.8 FASTQ quality score variant.
#*/
FASTQ_ILLUMINA @2;
}
