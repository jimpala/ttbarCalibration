#!/bin/sh

unset DISPLAY
echo Runnin on host `hostname`
cd /unix/atlasvhbb/abell/mc15data7/roofitTestPlain/roofitTestPlain6PT/../../scripts;
source setupROOT.sh;
cd /unix/atlasvhbb/abell/mc15data7/roofitTestPlain/roofitTestPlain6PT
root -b -q runGeneralFit.C\(0,true,false,0,\"MV2c1070\"\)
root -b -q runGeneralFit.C\(0,false,true,0,\"MV2c1070\"\)
