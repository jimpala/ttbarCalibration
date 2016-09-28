#!/bin/sh

unset DISPLAY
echo Runnin on host `hostname`
cd /unix/atlasvhbb/abell/mc15data7/roofitTestPlain/roofitTestPlain6PT/../../scripts;
source setupROOT.sh;
cd /unix/atlasvhbb/abell/mc15data7/roofitTestPlain/roofitTestPlain6PT
root -b -q runGeneralFit.C\(0,true,false,14,\"MV2c1077\"\)
root -b -q runGeneralFit.C\(0,false,true,14,\"MV2c1077\"\)
root -b -q runGeneralFit.C\(1,true,false,14,\"MV2c1077\"\)
root -b -q runGeneralFit.C\(1,false,true,14,\"MV2c1077\"\)
root -b -q runGeneralFit.C\(2,true,false,14,\"MV2c1077\"\)
root -b -q runGeneralFit.C\(2,false,true,14,\"MV2c1077\"\)
root -b -q runGeneralFit.C\(3,true,false,14,\"MV2c1077\"\)
root -b -q runGeneralFit.C\(3,false,true,14,\"MV2c1077\"\)
