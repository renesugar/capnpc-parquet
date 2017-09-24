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
#Database cross reference in GFF3 style DBTAG:ID format.
#*/
struct Dbxref {

#/**
#Database tag in GFF3 style DBTAG:ID format, e.g. EMBL in EMBL:AA816246.
#*/
db @0 : Text $Parquet.OPTIONAL;

#/**
#Accession number in GFF3 style DBTAG:ID format, e.g. AA816246 in EMBL:AA816246.
#*/
accession @1 : Text $Parquet.OPTIONAL;
}
