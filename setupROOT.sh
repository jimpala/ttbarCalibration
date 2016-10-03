export ATLAS_LOCAL_ROOT_BASE="/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/"
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh
localSetupROOT --skipConfirm
export ROOTSYS=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.30/x86_64-slc6-gcc48-opt/root
export PATH=$ROOTSYS/bin:$PATH
export LD_LIBRARY_PATH=$ROOTSYS/lib:$LD_LIBRARY_PATH
source /afs/cern.ch/sw/lcg/external/gcc/4.9.0/x86_64-slc6-gcc48-opt/setup.sh
