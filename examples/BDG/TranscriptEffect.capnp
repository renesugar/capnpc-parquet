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

using import "VariantAnnotationMessage.capnp".VariantAnnotationMessage;


#/**
#Annotation of a variant in the context of a feature, typically a transcript.
#*/
struct TranscriptEffect {

#/**
#Alternate allele for this variant annotation.
#*/
alternateAllele @0 : Text $Parquet.OPTIONAL;

#/**
#One or more annotations (also referred to as effects or consequences) of the
#variant in the context of the feature identified by featureId. Must be
#Sequence Ontology (SO, see http://www.sequenceontology.org) term names, e.g.
#stop_gained, missense_variant, synonymous_variant, upstream_gene_variant.
#*/
effects @1 : List(Text) $Parquet.OPTIONAL;

#/**
#Common gene name (HGNC), e.g. BRCA2. May be closest gene if annotation
#is intergenic.
#*/
geneName @2 : Text $Parquet.OPTIONAL;

#/**
#Gene identifier, e.g. Ensembl Gene identifier, ENSG00000139618. May be
#closest gene if annotation is intergenic.
#*/
geneId @3 : Text $Parquet.OPTIONAL;

#/**
#Feature type, may use Sequence Ontology term names. Typically transcript.
#*/
featureType @4 : Text $Parquet.OPTIONAL;

#/**
#Feature identifier, e.g. Ensembl Transcript identifier and version, ENST00000380152.7.
#*/
featureId @5 : Text $Parquet.OPTIONAL;

#/**
#Feature biotype, e.g. Protein coding or Non coding. See http://vega.sanger.ac.uk/info/about/gene_and_transcript_types.html.
#*/
biotype @6 : Text $Parquet.OPTIONAL;

#/**
#Intron or exon rank.
#*/
rank @7 : UInt64 $Parquet.OPTIONAL;

#/**
#Total number of introns or exons.
#*/
total @8 : UInt64 $Parquet.OPTIONAL;

#/**
#HGVS.g description of the variant. See http://www.hgvs.org/mutnomen/recs-DNA.html.
#*/
genomicHgvs @9 : Text $Parquet.OPTIONAL;

#/**
#HGVS.c description of the variant. See http://www.hgvs.org/mutnomen/recs-DNA.html.
#*/
transcriptHgvs @10 : Text $Parquet.OPTIONAL;

#/**
#HGVS.p description of the variant, if coding. See http://www.hgvs.org/mutnomen/recs-prot.html.
#*/
proteinHgvs @11 : Text $Parquet.OPTIONAL;

#/**
#cDNA sequence position (one based).
#*/
cdnaPosition @12 : UInt64 $Parquet.OPTIONAL;

#/**
#cDNA sequence length in base pairs (one based).
#*/
cdnaLength @13 : UInt64 $Parquet.OPTIONAL;

#/**
#Coding sequence position (one based, includes START and STOP codons).
#*/
cdsPosition @14 : UInt64 $Parquet.OPTIONAL;

#/**
#Coding sequence length in base pairs (one based, includes START and STOP codons).
#*/
cdsLength @15 : UInt64 $Parquet.OPTIONAL;

#/**
#Protein sequence position (one based, includes START but not STOP).
#*/
proteinPosition @16 : UInt64 $Parquet.OPTIONAL;

#/**
#Protein sequence length in amino acids (one based, includes START but not STOP).
#*/
proteinLength @17 : UInt64 $Parquet.OPTIONAL;

#/**
#Distance in base pairs to the feature.
#*/
distance @18 : UInt64 $Parquet.OPTIONAL;

#/**
#Zero or more errors, warnings, or informative messages regarding variant annotation accuracy.
#*/
messages @19 : List(VariantAnnotationMessage) $Parquet.OPTIONAL;
}
