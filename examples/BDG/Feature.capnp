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

using import "Strand.capnp".Strand;

using import "Dbxref.capnp".Dbxref;

using import "OntologyTerm.capnp".OntologyTerm;

struct Map(Key, Value) {
  entries @0 :List(Entry);
  struct Entry {
    key @0 :Key;
    value @1 :Value;
  }
}

#/**
#Feature, such as those represented in native file formats BED, GFF2/GTF,
#GFF3, IntervalList, and NarrowPeak.
#*/
struct Feature {

#/**
#Identifier for this feature. ID tag in GFF3.
#*/
featureId @0 : Text $Parquet.OPTIONAL;

#/**
#Display name for this feature, e.g. DVL1. Name tag in GFF3, optional column 4 "name"
#in BED format.
#*/
name @1 : Text $Parquet.OPTIONAL;

#/**
#Source of this feature, typically the algorithm or operating procedure that generated
#this feature, e.g. GeneWise. Column 2 "source" in GFF3.
#*/
source @2 : Text $Parquet.OPTIONAL;

#/**
#Feature type, constrained by some formats to a term from the Sequence Ontology (SO),
#e.g. gene, mRNA, exon, or a SO accession number (SO:0000704, SO:0000234, SO:0000147,
#respectively). Column 3 "type" in GFF3.
#*/
featureType @3 : Text $Parquet.OPTIONAL;

#/**
#Contig this feature is located on. Column 1 "seqid" in GFF3, column 1 "chrom"
#in BED format.
#*/
contigName @4 : Text $Parquet.OPTIONAL;

#/**
#Start position for this feature, in 0-based coordinate system with closed-open
#intervals. This may require conversion from the coordinate system of the native
#file format. Column 4 "start" in GFF3, column 2 "chromStart" in BED format.
#*/
start @5 : UInt64 $Parquet.OPTIONAL;

#/**
#End position for this feature, in 0-based coordinate system with closed-open
#intervals. This may require conversion from the coordinate system of the native
#file format. Column 5 "end" in GFF3, column 3 "chromEnd" in BED format.
#*/
end @6 : UInt64 $Parquet.OPTIONAL;

#/**
#Strand for this feature. Column 7 "strand" in GFF3, optional column 6 "strand"
#in BED format.
#*/
strand @7 : Strand $Parquet.OPTIONAL;

#/**
#For features of type "CDS", the phase indicates where the feature begins with reference
#to the reading frame. The phase is one of the integers 0, 1, or 2, indicating the number
#of bases that should be removed from the beginning of this feature to reach the first base
#of the next codon. Column 8 "phase" in GFF3.
#*/
phase @8 : UInt64 $Parquet.OPTIONAL;

#/**
#For features of type "CDS", the frame indicates whether the first base of the CDS segment is
#the first (frame 0), second (frame 1) or third (frame 2) in the codon of the ORF. Column 8
#"frame" in GFF2/GTF format.
#*/
frame @9 : UInt64 $Parquet.OPTIONAL;

#/**
#Score for this feature. Column 6 "score" in GFF3, optional column 5
#"score" in BED format.
#*/
score @10 : Float64 $Parquet.OPTIONAL;

#/**
#Gene identifier, e.g. ENSG00000107404. gene_id tag in GFF2/GTF.
#*/
geneId @11 : Text $Parquet.OPTIONAL;

#/**
#Transcript identifier, e.g. ENST00000378891. transcript_id tag in GFF2/GTF.
#*/
transcriptId @12 : Text $Parquet.OPTIONAL;

#/**
#Exon identifier, e.g. ENSE00001479184. exon_id tag in GFF2/GTF.
#*/
exonId @13 : Text $Parquet.OPTIONAL;

#/**
#Secondary names or identifiers for this feature. Alias tag in GFF3.
#*/
aliases @14 : List(Text) $Parquet.OPTIONAL;

#/**
#Parent feature identifiers. Parent tag in GFF3.
#*/
parentIds @15 : List(Text) $Parquet.OPTIONAL;

#/**
#Target of a nucleotide-to-nucleotide or protein-to-nucleotide alignment
#feature. The format of the value is "target_id start end [strand]", where
#strand is optional and may be "+" or "-". Target tag in GFF3.
#*/
target @16 : Text $Parquet.OPTIONAL;

#/**
#Alignment of the feature to the target in CIGAR format. Gap tag in GFF3.
#*/
gap @17 : Text $Parquet.OPTIONAL;

#/**
#Used to disambiguate the relationship between one feature and another when
#the relationship is a temporal one rather than a purely structural "part of"
#one. Derives_from tag in GFF3.
#*/
derivesFrom @18 : Text $Parquet.OPTIONAL;

#/**
#Notes or comments for this feature. Note tag in GFF3.
#*/
notes @19 : List(Text) $Parquet.OPTIONAL;

#/**
#Database cross references for this feature. Dbxref tag in GFF3.
#*/
dbxrefs @20 : List(Dbxref) $Parquet.OPTIONAL;

#/**
#Ontology term cross references for this feature. Ontology_term tag in GFF3.
#*/
ontologyTerms @21 : List(OntologyTerm) $Parquet.OPTIONAL;

#/**
#True if this feature is circular. Is_circular tag in GFF3.
#*/
circular @22 : Bool $Parquet.OPTIONAL;

#/**
#Additional feature attributes. Column 9 "attributes" in GFF3, excepting those
#reserved tags parsed into other fields, such as parentIds, dbxrefs, and ontologyTerms.
#*/
attributes @23 : Map(Text, Text) $Parquet.OPTIONAL $Parquet.MAP;
}
