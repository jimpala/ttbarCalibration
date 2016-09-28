#/bin/bash

channel=1

workingpoint=$1

startFromNumber=0
#startFromNumber=100

jobNumber=$startFromNumber
let lastJobNumber=$jobNumber+120
#let lastJobNumber=$jobNumber+2
workdir=$PWD
#mkdir submitCals

#source ./../../scripts/setupROOT.sh

while ( test $jobNumber -lt $lastJobNumber); do

    filename=$(echo 'submitJob_'$workingpoint'-'$channel'-'$jobNumber'.sh')

    echo "Create "$filename

    rm -rf submitCals/$filename
    touch submitCals/$filename

    echo "#!/bin/sh" >> submitCals/$filename
    echo "" >> submitCals/$filename
    echo "unset DISPLAY" >> submitCals/$filename
    echo "echo Runnin on host \`hostname\`" >> submitCals/$filename
#    echo "cd "$workdir"/../../scripts;cd "$workdir>>submitCals/$filename
    echo "cd "$workdir"/../../scripts;" >>submitCals/$filename
    echo "source setupROOT.sh;">>submitCals/$filename
    echo "cd "$workdir>>submitCals/$filename
#    echo "cd "$workdir>>submitCals/$filename
#    echo "cd /afs/cern.ch/user/g/giacinto/work/HbbAnalysis/ElectroweakBosons_Feb2013;source scripts/setup_lxplus.sh;cd /afs/cern.ch/user/g/giacinto/work/HbbAnalysis/ElectroweakBosons_Feb2013/AnalysisWZorHbb/macros/ttbarBtagDileptonFit/roofitTestPlain6PT">>submitCals/$filename
    echo "root -b -q runGeneralFit.C\(0,true,false,"$jobNumber",\\\"MV2c2070_tracks\\\"\)" >> submitCals/$filename
    echo "root -b -q runGeneralFit.C\(0,false,true,"$jobNumber",\\\"MV2c2070_tracks\\\"\)" >> submitCals/$filename
    echo "root -b -q runGeneralFit.C\(1,true,false,"$jobNumber",\\\"MV2c2070_tracks\\\"\)" >> submitCals/$filename
    echo "root -b -q runGeneralFit.C\(1,false,true,"$jobNumber",\\\"MV2c2070_tracks\\\"\)" >> submitCals/$filename
    echo "root -b -q runGeneralFit.C\(2,true,false,"$jobNumber",\\\"MV2c2070_tracks\\\"\)" >> submitCals/$filename
    echo "root -b -q runGeneralFit.C\(2,false,true,"$jobNumber",\\\"MV2c2070_tracks\\\"\)" >> submitCals/$filename
    echo "root -b -q runGeneralFit.C\(3,true,false,"$jobNumber",\\\"MV2c2070_tracks\\\"\)" >> submitCals/$filename
    echo "root -b -q runGeneralFit.C\(3,false,true,"$jobNumber",\\\"MV2c2070_tracks\\\"\)" >> submitCals/$filename

    echo "root -b -q runGeneralFit.C\(0,true,false,"$jobNumber",\\\"MV2c2077_tracks\\\"\)" >> submitCals/$filename
    echo "root -b -q runGeneralFit.C\(0,false,true,"$jobNumber",\\\"MV2c2077_tracks\\\"\)" >> submitCals/$filename
    echo "root -b -q runGeneralFit.C\(1,true,false,"$jobNumber",\\\"MV2c2077_tracks\\\"\)" >> submitCals/$filename
    echo "root -b -q runGeneralFit.C\(1,false,true,"$jobNumber",\\\"MV2c2077_tracks\\\"\)" >> submitCals/$filename
    echo "root -b -q runGeneralFit.C\(2,true,false,"$jobNumber",\\\"MV2c2077_tracks\\\"\)" >> submitCals/$filename
    echo "root -b -q runGeneralFit.C\(2,false,true,"$jobNumber",\\\"MV2c2077_tracks\\\"\)" >> submitCals/$filename
    echo "root -b -q runGeneralFit.C\(3,true,false,"$jobNumber",\\\"MV2c2077_tracks\\\"\)" >> submitCals/$filename
    echo "root -b -q runGeneralFit.C\(3,false,true,"$jobNumber",\\\"MV2c2077_tracks\\\"\)" >> submitCals/$filename

    qsub -N $(echo "run-"$jobNumber) -q short $PWD/submitCals/$filename  

#    source $PWD/submitCals/$filename

    let jobNumber=$jobNumber+1

    chmod a+x submitCals/$filename
    


#    root -b -l -q runGeneralFit.C\($channel,true,false,$jobNumber\)

#    let jobNumber=$jobNumber+1

done


