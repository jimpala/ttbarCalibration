#ifndef BUILDUPMULTISAMPLEMODEL_H
#define BUILDUPMULTISAMPLEMODEL_H

#include "TString.h"

class RooAbsPdf;

RooAbsPdf* buildUpMultiSampleModel(TString tagPoint="MV2c2070",
                                   int channel=0,
                                   int nCombinationsModel=1,bool addWeight=false,
                                   int optionSample=-1,TString systematicsToLoad="");

#endif
