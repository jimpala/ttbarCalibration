#!/bin/sh

unset DISPLAY
echo Running on host `hostname`
cd /unix/atlasvhbb/abell/tmvaTraining/roofitTestPlain6PT/..
source setupROOT.sh;
cd /unix/atlasvhbb/abell/tmvaTraining/roofitTestPlain6PT
root -b -q runGeneralFit.C\(0,true,false,43,\"MV2c1077\"\)
root -b -q runGeneralFit.C\(0,false,true,43,\"MV2c1077\"\)
root -b -q runGeneralFit.C\(1,true,false,43,\"MV2c1077\"\)
root -b -q runGeneralFit.C\(1,false,true,43,\"MV2c1077\"\)
root -b -q runGeneralFit.C\(2,true,false,43,\"MV2c1077\"\)
root -b -q runGeneralFit.C\(2,false,true,43,\"MV2c1077\"\)
root -b -q runGeneralFit.C\(3,true,false,43,\"MV2c1077\"\)
root -b -q runGeneralFit.C\(3,false,true,43,\"MV2c1077\"\)
