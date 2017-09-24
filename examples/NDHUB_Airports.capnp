# Copyright (c) 2017 Rene Sugar. All Rights Reserved.
#
# Licensed under the MIT License:
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

@0xa792113077148c12;

# Source: State of North Dakota — The Airports database
#         https://catalog.data.gov/dataset?res_format=CSV
#         https://gishubdata.nd.gov/sites/default/files/NDHUB.Airports.csv

# Use Cap'n Proto annotations to indicate Parquet schema attributes
#
# https://capnproto.org/language.html
#
annotation schema(struct)        :Text;
annotation required(*)           :Void;
annotation optional(*)           :Void;
annotation repeated(*)           :Void;
annotation length(*)             :Int32;
annotation scale(*)              :Int32;
annotation precision(*)          :Int32;
annotation decimal(*)            :Void;
annotation date(*)               :Void;
annotation timeMillis(*)         :Void;
annotation timeMicros(*)         :Void;
annotation timestampMillis(*)    :Void;
annotation timestampMicros(*)    :Void;
annotation bson(*)               :Void;
annotation json(*)               :Void;
annotation interval(*)           :Void;
annotation fixed(*)              :Void;
annotation map(*)                :Void;
annotation mapKeyValue(*)        :Void;
annotation list(*)               :Void;

struct Airport $schema("schema") {
  objectid   @0 :UInt32 $required;
  siteNo     @1 :Text;
  lanFaTy    @2 :Text;
  locid      @3 :Text;
  effDate    @4 :Text $date;
  faaRegion  @5 :Text;
  faaDistri  @6 :Text;
  faaSt      @7 :Text;
  stateName  @8 :Text;
  countyNam  @9 :Text;
  countySt   @10 :Text;
  cityName   @11 :Text;
  fullname   @12 :Text;
  ownerType  @13 :Text;
  facUse     @14 :Text;
  facCystzp  @15 :Text;
  elev       @16 :UInt32;
  aeroChart  @17 :Text;
  cbdDist    @18 :Text;
  cbdDir     @19 :Text;
  actDate    @20 :Text $date;
  certType   @21 :Text;
  fedAgree   @22 :Text;
  internatio @23 :Text;
  custLndg   @24 :Text;
  jointUse   @25 :Text;
  milLndgR   @26 :Text;
  milInt     @27 :Text;
  cntlTwr    @28 :Text;
  sEngGa     @29 :Text;
  mEngGa     @30 :Text;
  jetEnGa    @31 :Text;
  helicopter @32 :Text;
  operGlide  @33 :Text;
  operMil    @34 :Text;
  ultralight @35 :Text;
  commServ   @36 :Text;
  airTaxi    @37 :Text;
  localOps   @38 :Text;
  itinOpsG   @39 :Text;
  milOps     @40 :Text;
  latitude   @41 :Float64;
  longitude  @42 :Float64;
  stfips     @43 :UInt32;
  stPostal   @44 :Text;
  cy05Enp    @45 :UInt32;
  version    @46 :UInt32;
}
