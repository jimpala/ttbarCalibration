#!/bin/bash

channel=1

workingpoint=$1

startFromNumber=0
jobNumber=$startFromNumber
let lastJobNumber=$jobNumber+110

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
    echo "echo Running on host \`hostname\`" >> submitCals/$filename
    echo "cd "$workdir/..>>submitCals/$filename
    echo "source setupROOT.sh;">>submitCals/$filename
    echo "cd "$workdir>>submitCals/$filename



# Many backslashesslashes are for sequential escaping considerations.
    echo "root -b -q runGeneralFit.C\(0,true,false,"$jobNumber",\\\"MV2c1077\\\"\)" >> submitCals/$filename
    echo "root -b -q runGeneralFit.C\(0,false,true,"$jobNumber",\\\"MV2c1077\\\"\)" >> submitCals/$filename
    echo "root -b -q runGeneralFit.C\(1,true,false,"$jobNumber",\\\"MV2c1077\\\"\)" >> submitCals/$filename
    echo "root -b -q runGeneralFit.C\(1,false,true,"$jobNumber",\\\"MV2c1077\\\"\)" >> submitCals/$filename
    echo "root -b -q runGeneralFit.C\(2,true,false,"$jobNumber",\\\"MV2c1077\\\"\)" >> submitCals/$filename
    echo "root -b -q runGeneralFit.C\(2,false,true,"$jobNumber",\\\"MV2c1077\\\"\)" >> submitCals/$filename
    echo "root -b -q runGeneralFit.C\(3,true,false,"$jobNumber",\\\"MV2c1077\\\"\)" >> submitCals/$filename
    echo "root -b -q runGeneralFit.C\(3,false,true,"$jobNumber",\\\"MV2c1077\\\"\)" >> submitCals/$filename


    qsub -N $(echo "run-"$jobNumber) -q short $PWD/submitCals/$filename  

#   source $PWD/submitCals/$filename

    let jobNumber=$jobNumber+1

    chmod a+x submitCals/$filename
    


done


