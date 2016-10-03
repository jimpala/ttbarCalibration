#!/bin/sh

echo Runnin on host `hostname`
cd /unix/atlasvhbb/abell/tmvaTraining/roofitTestPlain6PT/..;
source setupROOT.sh;
cd /unix/atlasvhbb/abell/tmvaTraining/roofitTestPlain6PT
root -b -q runGeneralFit.C\(0,true,false,0,\"MV2c10\"\)
root -b -q runGeneralFit.C\(0,false,true,0,\"MV2c10\"\)
root -b -q runGeneralFit.C\(1,true,false,0,\"MV2c10\"\)
root -b -q runGeneralFit.C\(1,false,true,0,\"MV2c10\"\)
root -b -q runGeneralFit.C\(2,true,false,0,\"MV2c10\"\)
root -b -q runGeneralFit.C\(2,false,true,0,\"MV2c10\"\)
root -b -q runGeneralFit.C\(3,true,false,0,\"MV2c10\"\)
root -b -q runGeneralFit.C\(3,false,true,0,\"MV2c10\"\)
