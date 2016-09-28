#include "TString.h"
#include "RooFitResult.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "RooRealVar.h"
#include <iostream>
#include <iomanip>
#include "TLine.h"
#include "TAxis.h"
#include "TMatrixDSym.h"
#include "TVectorD.h"
#include "TVectorT.h"
#include "TDecompBK.h"
#include "TMatrixDSymEigen.h"
#include "TLegend.h"
#include "TStyle.h"
#include <vector>
#include "TMatrixD.h"
#include "TH2F.h"

#include "Riostream.h"

#include "./../../scripts/atlasstyle/AtlasStyle.C"
#include "./../../scripts/atlasstyle/AtlasLabels.C"
#include "plotUtils.C"


using namespace std;


void checkSystematicsAndCombine(TString tagPoint="MV2c2077",bool allChannels=false,
                                bool oneChannel=false,
                                int whichChannel=0,
                                bool useGeneralBinning=false,
                                bool use6PTbins=true,
                                bool use10PTbins=false)
{

#include "./../../lumi.icc"

  TSubString WPcut = tagPoint.operator()(tagPoint.Length()-2,2);
  double sample_lumi = data_luminosity;
  int sample_energy = 13;
  float y_max = 1.2;
  float y_min = 0.7;

  bool runOnSystematics=true;
  bool mcatnlo = false;

  bool inflateUncByMCtrue=true;

  bool FScorrection=false;
//  bool FScorrectionRef=true;


//  bool FScorrection=false
  bool FScorrectionRef=false;

  bool varyMCefficiencies=true;
  
  bool useBTagSF=false;

//  if (tagPoint=="MV2c2070") tagPoint="";

  //HERE
  bool runOnMC=false;
  //HERE
  bool rescaling=true;
  if  (runOnMC) rescaling=false;

  vector<TString> channels;

  if (!oneChannel)
  {
    
    channels.push_back("emu_2jets");
    channels.push_back("emu_3jets");
    
    if (allChannels)
    {
      channels.push_back("ll_2jets");
      channels.push_back("ll_3jets");
    
//    channels.push_back("emu_1dtagprobe");
//    channels.push_back("ll_1dtagprobe");

//    channels.push_back("emu_1dinclusive");
//    channels.push_back("ll_1dinclusive");
    }
  }
  else
  {
    if (whichChannel==0)
    {
      channels.push_back("emu_2jets");
    }
    else if (whichChannel==1)
    {
      channels.push_back("emu_3jets");
    }
    else if (whichChannel==2)
    {
      channels.push_back("ll_2jets");
    }
    else if (whichChannel==3)
    {
      channels.push_back("ll_3jets");
    }
  }

  int nBins=14;
  if (use6PTbins)
  {
    nBins=7;
  }
  else if (use10PTbins)
  {
    nBins=11;
  }
  else if (!useGeneralBinning)
  {
    nBins=5;
  }

//  Double_t limitsPtEta1d[1000]={20e3,30e3,60e3,90e3,140e3,200e3,300e3,400e3};

//  Double_t limitsPtEta1d[1000]={20e3,30e3,40e3,50e3,60e3,75e3,90e3,110e3,140e3,200e3,300e3,400e3};
//  int nBins=1;1
  

  TString fileName="calibSF_";
  fileName+=tagPoint;
  if (oneChannel) 
  {
    fileName+="_";
    fileName+=channels.at(0);
  }
  if (nBins!=11)
  {
    fileName+="_";
    fileName+=nBins;
    fileName+="bins";
  }

  if (runOnMC) fileName+="_MCclosure";
  fileName+=".txt";


  TString fileName2="resultLatex_true_";
  fileName2+=tagPoint;
  if (runOnMC) fileName2+="_MCclosure";
  if (channels.size()!=4) fileName2+="DUMMY";
  fileName2+=".tex";

  TString fileNameTot="resultLatex";
  fileNameTot+=tagPoint;
  if (runOnMC) fileNameTot+="_MCclosure";
  if (channels.size()!=4) fileNameTot+="DUMMY";
  fileNameTot+=".tex";

  TString fileName3="resultLatex_resultsSepErrors";
  fileName3+=tagPoint;
  if (runOnMC) fileName3+="_MCclosure";
  if (channels.size()!=4) fileName3+="DUMMY";
  fileName3+=".tex";

  TString fileName4="resultLatex_resultsSFSepErrors";
  fileName4+=tagPoint;
  if (runOnMC) fileName4+="_MCclosure";
  if (channels.size()!=4) fileName4+="DUMMY";
  fileName4+=".tex";

  TString fileName5="resultLatex_resultsSFTotErrors";
  fileName5+=tagPoint;
  if (runOnMC) fileName5+="_MCclosure";
  if (channels.size()!=4) fileName5+="DUMMY";
  fileName5+=".tex";

  TString fileNameChan[20];
  
  for (int q=0;q<channels.size();q++)
  {
    fileNameChan[q]="resultLatex";
    fileNameChan[q]+=tagPoint;
    fileNameChan[q]+="_";
    fileNameChan[q]+=q;
    fileNameChan[q]+="Channel";
    
    if (runOnMC) fileNameChan[q]+="_MCclosure";
    if (channels.size()!=4) fileNameChan[q]+="DUMMY";
    fileNameChan[q]+=".tex";
  }
  ofstream cronology(fileName,ios_base::out);//|ios_base::app);
  ofstream latexresults(fileName2,ios_base::out);//|ios_base::app);

  ofstream latexresults3(fileName3,ios_base::out);//|ios_base::app);
  ofstream latexresults4(fileName4,ios_base::out);//|ios_base::app);
  ofstream latexresults5(fileName5,ios_base::out);//|ios_base::app);


  ofstream* latexresultsChan[20];
  for (int q=0;q<channels.size();q++)
  {
    latexresultsChan[q]=new ofstream(fileNameChan[q],ios_base::out);
  }
  

  fileNameTot.ReplaceAll(".tex",".comb.tex");
//  if (channels.size()==2 || runOnMC) fileNameTot.ReplaceAll("DUMMY","");
  ofstream latexresultsTot(fileNameTot,ios_base::out);//|ios_base::app);

  bool useOldSyst=false;
  bool doNotCorrelateDifferentPtBins=false;
  bool onlyStatUncertainty=false;
  bool onlyDiagStatUncertainty=false;
  //HERE
//  bool inflate3jetUnc=true;
  bool inflate3jetUnc=false;
  

  bool useSpecifiedCorrelations=true;

//  int nBins=14;//as usual remove last 300-400 GeV bin

  vector<TString> channelDescription;
  channelDescription.push_back("$e\\mu$ 2 jets");
  channelDescription.push_back("$e\\mu$ 3 jets");
  channelDescription.push_back("$ee+\\mu\\mu$ 2 jets");
  channelDescription.push_back("$ee+\\mu\\mu$ 3 jets");
  channelDescription.push_back("$e\\mu$ tag \\& probe");
  channelDescription.push_back("$ee+\\mu\\mu$ tag \\& probe");
  channelDescription.push_back("$e\\mu$ inclusive");
  channelDescription.push_back("$ee+\\mu\\mu$ inclusive");

     #define first1
//#define second1
//#define third


//  double correlationCoefStdCh=0.5;
//  double correlationCoefStdPt=0.5;
  double correlationCoefStdCh=1.;
  double correlationCoefStdChSpec=0.5;
#ifdef second1
    double correlationCoefStdPt=1.;//do not consider correlations between pT bins
#else
    double correlationCoefStdPt=0.;//do not consider correlations between pT bins
#endif
  if (oneChannel) correlationCoefStdPt=1.;

  vector<TString> systematics;
  vector<TString> systematicsName;
  vector<double> correlationAmongChannels;
  vector<double> correlationAmongPTbins;
  vector<bool> updated;

  systematics.push_back("");
  systematicsName.push_back("");
  correlationAmongChannels.push_back(1);
  correlationAmongPTbins.push_back(1);
  updated.push_back(true);/*

  systematics.push_back("PowhegHerwig");//1
  systematicsName.push_back("Pythia vs Herwig");
  correlationAmongChannels.push_back(1);
  correlationAmongPTbins.push_back(1);
  updated.push_back(true);

  systematics.push_back("MCAtNLO");//2 
  systematicsName.push_back("MCAtNLO vs Powheg");
  correlationAmongChannels.push_back(1);
  correlationAmongPTbins.push_back(1);
  updated.push_back(true);
  /*
  systematics.push_back("PowhegPythia8");//3
  systematicsName.push_back("Pythia vs Pythia8");
  correlationAmongChannels.push_back(1);
  correlationAmongPTbins.push_back(1);
  updated.push_back(true);
  /*
  systematics.push_back("ttbarRadHi");//3
  systematicsName.push_back("ttbar Rad High");
  correlationAmongChannels.push_back(1);
  correlationAmongPTbins.push_back(1);
  updated.push_back(true);
  /*
  systematics.push_back("ttbarRadLo");//3
  systematicsName.push_back("ttbar Rad Low");
  correlationAmongChannels.push_back(1);
  correlationAmongPTbins.push_back(1);
  updated.push_back(true);

  systematics.push_back("SherpaZ");//4
  systematicsName.push_back("Pythia8 vs Sherpa Z Generator");
  correlationAmongChannels.push_back(1);
  correlationAmongPTbins.push_back(1);
  updated.push_back(true);
			  */
  if (!runOnMC && runOnSystematics)
  {
    /*
  systematics.push_back("");//1                   
  systematicsName.push_back("");
  correlationAmongChannels.push_back(correlationCoefStdChSpec);
  correlationAmongPTbins.push_back(correlationCoefStdPt);
  updated.push_back(true);
  
  systematics.push_back("PowhegHerwig");//1
  systematicsName.push_back("Pythia vs Herwig");
  correlationAmongChannels.push_back(correlationCoefStdChSpec);
  correlationAmongPTbins.push_back(correlationCoefStdPt);
  updated.push_back(true);
    */
  if(mcatnlo){
    systematics.push_back("MCAtNLO");//2
    systematicsName.push_back("MCAtNLO vs Powheg");
    correlationAmongChannels.push_back(correlationCoefStdChSpec);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);
  }

  systematics.push_back("PowhegPythia8");//3
  systematicsName.push_back("Pythia vs Pythia8");
  correlationAmongChannels.push_back(correlationCoefStdChSpec);
  correlationAmongPTbins.push_back(correlationCoefStdPt);
  updated.push_back(true);

  systematics.push_back("ttbarRadHi");//4
  systematicsName.push_back("ttbar Rad High");
  correlationAmongChannels.push_back(correlationCoefStdChSpec);
  correlationAmongPTbins.push_back(correlationCoefStdPt);
  updated.push_back(true);

  systematics.push_back("ttbarRadLo");//5
  systematicsName.push_back("ttbar Rad Low");
  correlationAmongChannels.push_back(correlationCoefStdChSpec);
  correlationAmongPTbins.push_back(correlationCoefStdPt);
  updated.push_back(true);
  /*
  systematics.push_back("SherpaZ");//6
  systematicsName.push_back("Z+jets Pythia8 vs Sherpa");
  correlationAmongChannels.push_back(correlationCoefStdChSpec);
  correlationAmongPTbins.push_back(correlationCoefStdPt);
  updated.push_back(true);*/
  {

    systematics.push_back("FlavourTagging_JET_GroupedNP_1__1down");
    systematicsName.push_back("JET Grouped NP 1 Down");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);
    
    systematics.push_back("FlavourTagging_JET_GroupedNP_1__1up");
    systematicsName.push_back("JET Grouped NP 1 Up");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);
    
    systematics.push_back("FlavourTagging_JET_GroupedNP_2__1down");
    systematicsName.push_back("JET Grouped NP 2 Down");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);

    systematics.push_back("FlavourTagging_JET_GroupedNP_2__1up");
    systematicsName.push_back("JET Grouped NP 2 Up");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);
    
    systematics.push_back("FlavourTagging_JET_GroupedNP_3__1down");
    systematicsName.push_back("JET Grouped NP 3 Down");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);

    systematics.push_back("FlavourTagging_JET_GroupedNP_3__1up");
    systematicsName.push_back("JET Grouped NP 2 Up");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);

/*    systematics.push_back("FlavourTagging_JET_GroupedNP_4__1up");
    systematicsName.push_back("JET Grouped NP 4 Up");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);

    systematics.push_back("FlavourTagging_JET_GroupedNP_4__1down");
    systematicsName.push_back("JET Grouped NP 4 Down");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);

    systematics.push_back("FlavourTagging_JET_GroupedNP_5__1up");
    systematicsName.push_back("JET Grouped NP 5 Up");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);

    systematics.push_back("FlavourTagging_JET_GroupedNP_5__1down");
    systematicsName.push_back("JET Grouped NP 5 Down");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);

    systematics.push_back("FlavourTagging_JET_GroupedNP_6__1up");
    systematicsName.push_back("JET Grouped NP 6 Up");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);

    systematics.push_back("FlavourTagging_JET_GroupedNP_6__1down");
    systematicsName.push_back("JET Grouped NP 6 Down");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);

    systematics.push_back("FlavourTagging_JET_GroupedNP_7__1up");
    systematicsName.push_back("JET Grouped NP 7 Up");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);

    systematics.push_back("FlavourTagging_JET_GroupedNP_7__1down");
    systematicsName.push_back("JET Grouped NP 7 Down");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);

    systematics.push_back("FlavourTagging_JET_GroupedNP_8__1up");
    systematicsName.push_back("JET Grouped NP 8 Up");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);

    systematics.push_back("FlavourTagging_JET_GroupedNP_8__1down");
    systematicsName.push_back("JET Grouped NP 8 Down");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);

    systematics.push_back("FlavourTagging_JET_JER_SINGLE_NP__1up");
    systematicsName.push_back("JET Grouped NP 1 Up");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);

    systematics.push_back("FlavourTagging_JET_JER_SINGLE_NP__1down");
    systematicsName.push_back("JET Grouped NP 1 Down");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);
*/
    systematics.push_back("FlavourTagging_MUONS_ID__1down");
    systematicsName.push_back("JET Grouped Muons ID 1 Down");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);

    systematics.push_back("FlavourTagging_MUONS_ID__1up");
    systematicsName.push_back("JET Grouped Muons ID 1 Up");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);

    systematics.push_back("FlavourTagging_MUONS_MS__1down");
    systematicsName.push_back("JET Grouped Muos MS 1 Down");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);

    systematics.push_back("FlavourTagging_MUONS_MS__1up");
    systematicsName.push_back("JET Grouped Muons MS 1 Up");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);

    systematics.push_back("FlavourTagging_MUONS_SCALE__1down");
    systematicsName.push_back("JET Grouped Muons Scale 1 Down");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);

    systematics.push_back("FlavourTagging_MUONS_SCALE__1up");
    systematicsName.push_back("JET Grouped Muons Scale 1 Up");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);

    systematics.push_back("FlavourTagging_EG_RESOLUTION_ALL__1down");
    systematicsName.push_back("EG Resolution All 1 Down");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);

    systematics.push_back("FlavourTagging_EG_RESOLUTION_ALL__1up");
    systematicsName.push_back("EG Resolution All 1 Up");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);

    systematics.push_back("FlavourTagging_EG_SCALE_ALL__1down");
    systematicsName.push_back("EG Scale All 1 Down");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);

    systematics.push_back("FlavourTagging_EG_SCALE_ALL__1up");
    systematicsName.push_back("EG Resolution All 1 Up");
    correlationAmongChannels.push_back(1);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);
  }
  /*  
  systematics.push_back("ZCR");
  systematicsName.push_back("ZCR MET 30");
  correlationAmongChannels.push_back(correlationCoefStdCh);
  correlationAmongPTbins.push_back(correlationCoefStdPt);
  updated.push_back(true);

  systematics.push_back("ZCRLoose");
  systematicsName.push_back("ZCR MET 30 Loose Mass");
  correlationAmongChannels.push_back(correlationCoefStdCh);
  correlationAmongPTbins.push_back(correlationCoefStdPt);
  updated.push_back(true);
  */

  systematics.push_back("fakesDo");
  systematicsName.push_back("Lepton fakes");
  correlationAmongChannels.push_back(correlationCoefStdCh);
  correlationAmongPTbins.push_back(correlationCoefStdPt);
  updated.push_back(true);

  systematics.push_back("fakesUp");
  systematicsName.push_back("Lepton fakes");
  correlationAmongChannels.push_back(correlationCoefStdCh);
  correlationAmongPTbins.push_back(correlationCoefStdPt);
  updated.push_back(true);

  systematics.push_back("ZHFDo");
  systematicsName.push_back("Norm. Z$+b$/$c$");
  correlationAmongChannels.push_back(correlationCoefStdCh);
  correlationAmongPTbins.push_back(correlationCoefStdPt);
  updated.push_back(true);

  systematics.push_back("ZHFUp");
  systematicsName.push_back("Norm. Z$+b$/$c$");
  correlationAmongChannels.push_back(correlationCoefStdCh);
  correlationAmongPTbins.push_back(correlationCoefStdPt);
  updated.push_back(true);

  systematics.push_back("stopDo");
  systematicsName.push_back("Norm. single top");
  correlationAmongChannels.push_back(correlationCoefStdCh);
  correlationAmongPTbins.push_back(correlationCoefStdPt);
  updated.push_back(true);

  systematics.push_back("stopUp");
  systematicsName.push_back("Norm. single top");
  correlationAmongChannels.push_back(correlationCoefStdCh);
  correlationAmongPTbins.push_back(correlationCoefStdPt);
  updated.push_back(true);


   systematics.push_back("dibDo");
   systematicsName.push_back("Norm. diboson");
   correlationAmongChannels.push_back(correlationCoefStdCh);
   correlationAmongPTbins.push_back(correlationCoefStdPt);
   updated.push_back(true);

   systematics.push_back("dibUp");
   systematicsName.push_back("Norm. diboson");
   correlationAmongChannels.push_back(correlationCoefStdCh);
   correlationAmongPTbins.push_back(correlationCoefStdPt);
   updated.push_back(true);

//only useful after rescaling!
  if (rescaling)
  {
    systematics.push_back("ZDo");
    systematicsName.push_back("Norm. Z$+$jet");
    correlationAmongChannels.push_back(correlationCoefStdCh);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);

    systematics.push_back("ZUp");
    systematicsName.push_back("Norm. Z$+$jet");
    correlationAmongChannels.push_back(correlationCoefStdCh);
    correlationAmongPTbins.push_back(correlationCoefStdPt);
    updated.push_back(true);
   
  }

  systematics.push_back("MistagDo");//to be added directly in the code
  systematicsName.push_back("Mistag rate");
  correlationAmongChannels.push_back(correlationCoefStdCh);
  correlationAmongPTbins.push_back(correlationCoefStdPt);
  updated.push_back(true);

  systematics.push_back("MistagUp");//to be added directly in the code
  systematicsName.push_back("Mistag rate");
  correlationAmongChannels.push_back(correlationCoefStdCh);
  correlationAmongPTbins.push_back(correlationCoefStdPt);
  updated.push_back(true);


  systematics.push_back("NominalBTagSF");
  systematicsName.push_back("Nominal BTag SF");
  correlationAmongChannels.push_back(1);
  correlationAmongPTbins.push_back(correlationCoefStdPt);
  updated.push_back(true);
 }

  else//he expects sth at the end
  {
    cout << "Are we here? Probably" << endl;
      systematics.push_back("");
      systematicsName.push_back("");
      correlationAmongChannels.push_back(1);
      correlationAmongPTbins.push_back(1);
      updated.push_back(true);
  }

  int idxRef=0;
  if (FScorrectionRef)
  {
    idxRef=2;
  }

  
  double values[50][10][100];//bin,channel,systematics
  double errors[50][10][100];
  
  for (unsigned int u=0;u<systematics.size();u++)
  {
    for (unsigned int i=0;i<nBins;i++)
    {
      for (unsigned int j=0;j<channels.size();j++)
      {
        values[i][j][u]=0;
        errors[i][j][u]=0;
      }
    }
  }
     
//  vector<vector<RooFitResult*> > resultsMC;
  
  for (unsigned int j=0;j<channels.size();j++)
  {

    vector<RooFitResult*> tmp_results;

    for (unsigned int q=0;q<systematics.size();q++)
    {
      TString outputFileName("results");
      if (tagPoint!="")
      {
        outputFileName+="_";
        outputFileName+=tagPoint;
      }
//      if (!varyMCefficiencies || systematics.at(q)=="" ||  systematics.at(q)=="MistagDo" || 
//          systematics.at(q)=="MistagUp")
//      {
        
   if ((systematics.at(q)=="" || systematics.at(q)=="MistagDo" || 
//        systematics.at(q)=="PowhegHerwig" || systematics.at(q)=="SherpaZ" || 
          systematics.at(q)=="MistagUp"))
      {

        if (FScorrectionRef)
        {
          outputFileName+="/PowhegPythiaFS";
        }
        else
        {
          outputFileName+="/MCNominal";
        }
      }
      else
      {
        outputFileName+="/";
        outputFileName+=systematics.at(q);
      }
      int ntot=3;
      if (channels.at(j).Contains("3jets"))
      {
        outputFileName+="_3jets";
        ntot=4;
      }
      if (channels.at(j).Contains("ll"))
      {
        outputFileName+="_sameflav";
      }
      if (channels.at(j).Contains("1dtagprobe"))
      {
        outputFileName+="_1dtagprobe";
      }
      if (channels.at(j).Contains("1dinclusive"))
      {
        outputFileName+="_1dinclusive";
      }
     
      for (unsigned int r=0;r<ntot;r++)
      {
        TString fileName(outputFileName);
        fileName+="_";
        fileName+=r;
        fileName+=".root";
        
        TFile* file=new TFile(fileName);
        RooFitResult* result=(RooFitResult*)file->Get("fitresult_PDFtot_dataset");
        
        cout << " File: " << fileName << endl;
        
        for (unsigned int i=0;i<nBins;i++)
        {
          TString nameOfVar("coefBWeight_b__pTetabin");
          nameOfVar+=i;
          nameOfVar+="_weightbin0";
          
          if (result->floatParsFinal().find(nameOfVar)!=0)
          {
            
            double valueIn=((RooRealVar*)result->floatParsFinal().find(nameOfVar))->getVal();
            double valueInErr=((RooRealVar*)result->floatParsFinal().find(nameOfVar))->getError();
            cout << " chan " << channels.at(j) << " found " << nameOfVar << " val " << valueIn << " +/- " << valueInErr << endl;
            
            values[i][j][q]+=valueIn / valueInErr / valueInErr;
            errors[i][j][q]+=1. / valueInErr / valueInErr;
          }
        }
        file->Close();
      }
    }//iterate on systematics
  }//iterate on channels
  
  double valuesAll[100][20];
  double errorsAll[100][20];

  for (unsigned int q=0;q<systematics.size();q++)
  {
    for (unsigned int i=0;i<nBins;i++)
    {
      valuesAll[i][q]=0;
    }
  }
  

  for (unsigned int q=0;q<systematics.size();q++)
  {
    for (unsigned int j=0;j<channels.size();j++)
    {
      for (unsigned int i=0;i<nBins;i++)
      {
        valuesAll[i][q]+=values[i][j][q];
        errorsAll[i][q]+=errors[i][j][q];
        
        values[i][j][q]=values[i][j][q]/errors[i][j][q];
        errors[i][j][q]=sqrt(1./errors[i][j][q]);
        cout << " chan " << j << " bin " << i << " syst " << q << " eff " << values[i][j][q] << " +/- " << errors[i][j][q] << endl;
      }
    }
  }
  
  for (unsigned int q=0;q<systematics.size();q++)
  {
    for (unsigned int i=0;i<nBins;i++)
    {
      valuesAll[i][q]=valuesAll[i][q]/errorsAll[i][q];
      errorsAll[i][q]=sqrt(1./errorsAll[i][q]);
    }
  }

  for (unsigned int q=0;q<systematics.size();q++)
  {
    cout << " Systematics " << systematics.at(q);
    
    for (unsigned int i=0;i<nBins;i++)
    {
      cout << std::fixed << std::setprecision(3) << " b " << i << " d " << values[i][0][q]-values[i][0][0] ;
      
    }
    cout << endl;
  }


  vector<vector<RooFitResult*> > results;
  

  for (unsigned int i=0;i<channels.size();i++)
  {
    vector<RooFitResult*> tmp_results;
    
    for (unsigned int j=0;j<systematics.size();j++)
    {
      TString outputFileName("results");
      if (updated.at(j)==false)
      {
        outputFileName="../../../../../ElectroweakBosons_September2012_2011analysis/trunk/AnalysisWZorHbb/macros/postProcessingDileptonBugFix/roofitPtBins/results";
      }
      if (tagPoint!="")
      {
        outputFileName+="_";
        outputFileName+=tagPoint;
      }
      outputFileName+="/DataNominal";
      if (runOnMC) 
      {
        outputFileName="results";
        if (tagPoint!="")
        {
          outputFileName+="_";
          outputFileName+=tagPoint;
        }
        outputFileName+="/MCNominal";
        
        if (updated.at(j)==false)
        {
          outputFileName="../../../../../ElectroweakBosons_September2012_2011analysis/trunk/AnalysisWZorHbb/macros/postProcessingDileptonBugFix/roofitPtBins/results_mcWithoutRescaling_closureTest/MCNominal";
        }
      }
      if (rescaling && !runOnMC)
      {
        outputFileName="results";
        if (updated.at(j)==false)
        {
          outputFileName="../../../../../ElectroweakBosons_September2012_2011analysis/trunk/AnalysisWZorHbb/macros/postProcessingDileptonBugFix/roofitPtBins/results";
        }
        if (tagPoint!="")
        {
          outputFileName+="_";
          outputFileName+=tagPoint;
        }
        outputFileName+="/DataNominal";
        if (runOnMC) outputFileName="results_mcWithoutRescaling_closureTest/MCNominal";
      }
      if (channels.at(i).Contains("3jets"))
      {
        outputFileName+="_3jets";
      }
      if (channels.at(i).Contains("ll"))
      {
        outputFileName+="_sameflav";
      }
      if (channels.at(i).Contains("1dtagprobe"))
      {
        outputFileName+="_1dtagprobe";
      }
      if (channels.at(i).Contains("1dinclusive"))
      {
        outputFileName+="_1dinclusive";
      }
      outputFileName+="_";
      if (systematics.at(j)!="")
      {
        outputFileName+="_Sys";
        outputFileName+=systematics.at(j);
      }
      outputFileName+=".root";

      std::cout << "File2: " << outputFileName << std::endl;
      
      TFile* outputResult=new TFile(outputFileName);
      RooFitResult* result=(RooFitResult*)outputResult->Get("fitresult_PDFtot_dataset");
      if (result==0) 
      {
        cout << " couldn't find roofit res in " << outputFileName << endl;
        throw;
      }
      
      tmp_results.push_back(result);
    }
    results.push_back(tmp_results);
  }



  Double_t limitsPtEta1d[1000]={0.,//1.,2.,//20-
                                3.,4.,5.,//1
                                6.,7.,8.,//2
                                9.,10.,11.,//3
                                12.,13.,14.,//4
                                15.,//200-
                                18.,//300-
                                21.};

  if (use6PTbins)
  {
    limitsPtEta1d[0]=20e3;
    limitsPtEta1d[1]=30e3;
    limitsPtEta1d[2]=60e3;
    limitsPtEta1d[3]=90e3;
    limitsPtEta1d[4]=140e3;
    limitsPtEta1d[5]=200e3;
    limitsPtEta1d[6]=300e3;
    limitsPtEta1d[7]=400e3;
  }
  else if (use10PTbins)
  {
    limitsPtEta1d[0]=20e3;
    limitsPtEta1d[1]=30e3;
    limitsPtEta1d[2]=40e3;
    limitsPtEta1d[3]=50e3;
    limitsPtEta1d[4]=60e3;
    limitsPtEta1d[5]=75e3;
    limitsPtEta1d[6]=90e3;
    limitsPtEta1d[7]=110e3;
    limitsPtEta1d[8]=140e3;
    limitsPtEta1d[9]=200e3;
    limitsPtEta1d[10]=300e3;
    limitsPtEta1d[11]=400e3;
  }
  else if (!useGeneralBinning)
  {
    limitsPtEta1d[0]=0;
    limitsPtEta1d[1]=0.5*1e3;
    limitsPtEta1d[2]=1.0*1e3;
    limitsPtEta1d[3]=1.5*1e3;
    limitsPtEta1d[4]=2.0*1e3;
    limitsPtEta1d[5]=2.5*1e3;
  }
  
  
  int npTOnlyBins=6;
  Double_t limitsPt[1000]={20e3,30e3,60e3,90e3,140e3,200e3,300e3,400e3};
  Double_t limitsEta[1000]={0,0.7,1.5,2.5};

  vector<TString> descriptionEtaBinning;
  descriptionEtaBinning.push_back("0<|#eta|<0.7");
  descriptionEtaBinning.push_back("0.7<|#eta|<1.5");
  descriptionEtaBinning.push_back("|#eta|>1.5");

  int nPlots=3;
  vector<vector<int> > binsPlot;
  
  vector<int> binsPlot0;
  binsPlot0.push_back(0);//20-30
  binsPlot0.push_back(1);//30-60
  binsPlot0.push_back(4);//60-90
  binsPlot0.push_back(7);//90-140
  binsPlot0.push_back(10);//140-200
  binsPlot0.push_back(13);//>200

  vector<int> binsPlot1;
  binsPlot1.push_back(0);//20-30
  binsPlot1.push_back(2);//30-60
  binsPlot1.push_back(5);//60-90
  binsPlot1.push_back(8);//90-140
  binsPlot1.push_back(11);//140-200
  binsPlot1.push_back(13);//>200

  vector<int> binsPlot2;
  binsPlot2.push_back(0);//20-30
  binsPlot2.push_back(3);//30-60
  binsPlot2.push_back(6);//60-90
  binsPlot2.push_back(9);//90-140
  binsPlot2.push_back(12);//140-200
  binsPlot2.push_back(13);//>200
  
  binsPlot.push_back(binsPlot0);
  binsPlot.push_back(binsPlot1);
  binsPlot.push_back(binsPlot2);

  //OKOKOKOKOK

  

  Double_t limitsPtDo[1000]={20,
                             30,30,30,
                             60,60,60,
                             90,90,90,
                             140,140,140,
                             200,
                             300};
  
  Double_t limitsPtUp[1000]={30,
                             60,60,60,
                             90,90,90,
                             140,140,140,
                             200,200,200,
                             300,
                             400};
  
  Double_t limitsEtaDo[1000]={0,
                              0,0.7,1.5,
                              0,0.7,1.5,
                              0,0.7,1.5,
                              0,0.7,1.5,
                              0,
                              0};

  Double_t limitsEtaUp[1000]={2.5,
                              0.7,1.5,2.5,
                              0.7,1.5,2.5,
                              0.7,1.5,2.5,
                              0.7,1.5,2.5,
                              2.5,
                              2.5};
                              



  TString* binNames[100];
  binNames[0]=new TString("$[20,30]$");
  binNames[1]=new TString("$[30,60]$");
  binNames[2]=new TString("$[30,60]$");
  binNames[3]=new TString("$[30,60]$");
  binNames[4]=new TString("$[60,90]$");
  binNames[5]=new TString("$[60,90]$");
  binNames[6]=new TString("$[60,90]$");
  binNames[7]=new TString("$[90,140]$");
  binNames[8]=new TString("$[90,140]$");
  binNames[9]=new TString("$[90,140]$");
  binNames[10]=new TString("$[140,200]$");
  binNames[11]=new TString("$[140,200]$");
  binNames[12]=new TString("$[140,200]$");
  binNames[13]=new TString("$[200,300]$");
  binNames[14]=new TString("$[>300]$");

  TString* binNames2[100];
  binNames2[0]=new TString("$[0,2.5]$");
  binNames2[1]=new TString("$[0,0.7]$");
  binNames2[2]=new TString("$[1,1.5]$");
  binNames2[3]=new TString("$[1.5,2.5]$");
  binNames2[4]=new TString("$[0,0.7]$");
  binNames2[5]=new TString("$[1,1.5]$");
  binNames2[6]=new TString("$[1.5,2.5]$");
  binNames2[7]=new TString("$[0,0.7]$");
  binNames2[8]=new TString("$[1,1.5]$");
  binNames2[9]=new TString("$[1.5,2.5]$");
  binNames2[10]=new TString("$[0,0.7]$");
  binNames2[11]=new TString("$[1,1.5]$");
  binNames2[12]=new TString("$[1.5,2.5]$");
  binNames2[13]=new TString("$[0,2.5]$");
  binNames2[14]=new TString("$[0,2.5]$");





  double  valInitial[50][100];
  double differenceToInitialBSF[50][100];
  double  valEnd[50][100];
  double  valError[50][100];
  double  valStatCovariance[50][100][100];//channel, pT bin, pTbin
  double  valStatCovarianceSF[50][100][100];//channel, pT bin, pTbin
  double  x[100];
  double  x_err[100];

  
  double valSystError[50][100];
  double valTotError[50][100];
  double SF[50][100];
  double SF_diffToInitialBSF[50][100];
  double SF_err[50][100];
  double SF_staterr[50][100];

  for (unsigned int i=0;i<100;i++)
  {
    for (unsigned int j=0;j<50;j++)
    {
      valInitial[j][i]=0;
      differenceToInitialBSF[j][i]=0;
      valEnd[j][i]=0;
      valError[j][i]=0;
      for (unsigned int u=0;u<100;u++)
      {
        valStatCovariance[j][i][u]=0;
        valStatCovarianceSF[j][i][u]=0;
      }
      valSystError[j][i]=0;
      valTotError[j][i]=0;
      SF[j][i]=0;
      SF_diffToInitialBSF[j][i]=0;
      SF_err[j][i]=0;
      SF_staterr[j][i]=0;
    }
  }


  for (unsigned int j=0;j<channels.size();j++)
  {
    for (unsigned int i=0;i<nBins;i++)
    {
      valInitial[j][i]=values[i][j][idxRef];
    }
  }

  for (unsigned int j=0;j<channels.size();j++)
  {
    for (unsigned int i=0;i<nBins;i++)
    {
      valInitial[j][i]=values[i][j][idxRef];
    }
  }

  for (unsigned int q=0;q<systematics.size();q++)
  {
    std::cout << " Syst " << systematics.at(q) << "   ";
    for (unsigned int i=0;i<nBins;i++)
    {
      //      valInitial[j][i]=values[i][j][idxRef];
      std::cout << values[i][0][q]  << "+/-" << errors[i][0][q] << " ";
    }
    std::cout << std::endl;
  } 


  for (unsigned int i=0;i<results.size();i++)
  {
    cout << " channel " << channels.at(i) << endl;

    cout << "SF ";

    for (int u=0;u<nBins;u++)
    {
      TString par("coefBWeight_b__pTetabin");
      par+=u;
      par+="_weightbin0";

      int idxToUse=0;
      if (useBTagSF)
      {
        idxToUse=systematics.size()-1;
      }
      if (FScorrectionRef)
      {
        idxToUse=1;
      }
      
      double valueIn=0;
//      for (int r=0;r<channels.size();r++)
//      {
//        valueIn+=((RooRealVar*)results.at(r).at(idxToUse)->floatParsInit().find(par))->getVal();
//      }
      //      valueIn=valueIn/(double)channels.size();
//      valueIn=((RooRealVar*)results.at(i).at(idxToUse)->floatParsInit().find(par))->getVal();

      valueIn=valInitial[i][u];
      
      
      double valueFin=((RooRealVar*)results.at(i).at(0)->floatParsFinal().find(par))->getVal();
      double valueErr=((RooRealVar*)results.at(i).at(0)->floatParsFinal().find(par))->getError();
      /*
      //replace with diboson values
      valueFin=
	//	((RooRealVar*)results.at(i).at(0)->floatParsFinal().find(par))->getVal();
	//          ((RooRealVar*)results.at(i).at(10)->floatParsFinal().find(par))->getVal();
          ((RooRealVar*)results.at(i).at(12)->floatParsFinal().find(par))->getVal();
      */
      if (FScorrection)
      {
        valueFin+=
            ((RooRealVar*)results.at(i).at(1)->floatParsFinal().find(par))->getVal()
            -((RooRealVar*)results.at(i).at(0)->floatParsFinal().find(par))->getVal();
      }
//      
//          

      double addUnc=0;
      if (inflateUncByMCtrue)
      {
        addUnc=errors[u][i][idxRef];
      }
      
      if (inflate3jetUnc)
      {
        if (channels.at(i).Contains("3jets") && u==8) addUnc=0.02;
        if (channels.at(i).Contains("3jets") && u==9) addUnc=0.03;
        if (channels.at(i).Contains("3jets") && u==10) addUnc=0.10;
      }
      valueErr=sqrt(valueErr*valueErr+addUnc*addUnc);

      cout << valueFin/valueIn << " ";
      
      valInitial[i][u]=valueIn;
      valEnd[i][u]=valueFin;
      valError[i][u]=valueErr;

      valStatCovariance[i][u][u]=valueErr*valueErr;
      valStatCovarianceSF[i][u][u]=valueErr*valueErr/valueIn/valueIn;
      if (!onlyDiagStatUncertainty)
      {
        for (int l=0;l<nBins;l++)
        {
          TString par2("coefBWeight_b__pTetabin");
          par2+=l;
          par2+="_weightbin0";
         
          double valueIn2=valInitial[i][l];
              //((RooRealVar*)results.at(i).at(0)->floatParsInit().find(par2))->getVal();
          if (useBTagSF)
          {
            valueIn2=((RooRealVar*)results.at(i).at(systematics.size()-1)->floatParsInit().find(par2))->getVal();
          }
 
          double valueErr2=((RooRealVar*)results.at(i).at(0)->floatParsFinal().find(par2))->getError();
          double addUnc=0;
      
          if (inflateUncByMCtrue)
          {
            addUnc=errors[l][i][idxRef];
          }

          if (inflate3jetUnc)
          {
            if (channels.at(i).Contains("3jets") && l==8) addUnc=0.02;
            if (channels.at(i).Contains("3jets") && l==9) addUnc=0.03;
            if (channels.at(i).Contains("3jets") && l==10) addUnc=0.10;
          }
          valueErr2=sqrt(valueErr2*valueErr2+addUnc*addUnc);
          
          
          double correlation=results.at(i).at(0)->correlation(par,par2);
          valStatCovariance[i][u][l]=valueErr*valueErr2*correlation;
          valStatCovarianceSF[i][u][l]=valueErr*valueErr2*correlation/valueIn/valueIn2;
//        cout << " chan " << i << "["<<u<<","<<l<<"]= " << valueErr*valueErr2*correlation << " corr " << correlation << endl;
        }
      }
      
      x[u]=(limitsPtEta1d[u]+limitsPtEta1d[u+1])/2./1e3;
      x_err[u]=(limitsPtEta1d[u+1]-limitsPtEta1d[u])/2./1e3;
        //x_err[u]=0;
        
    }

    cout << endl;

    cout << " Stat ";
    for (int u=0;u<nBins;u++)
    {
      cout << valError[i][u]/valInitial[i][u] << " ";
    }
    cout << endl;
  }


  std::cout << std::endl;
  
  double delta[4][50][100];//channel,pT bin,systematics
  double deltaSF[4][50][100];
  for (unsigned int i=0;i<100;i++)
  {
    for (unsigned int j=0;j<50;j++)
    {
      for (unsigned int u=0;u<4;u++)
      {
        delta[u][j][i]=0;
        deltaSF[u][j][i]=0;
      }
    }
  }

  for (unsigned int i=0;i<results.size();i++)
  {
    cout << " channel " << channels.at(i) << endl;

    for (unsigned int j=0;j<systematics.size()-1;j++)
    {

      cout << systematics.at(j);
      for (unsigned int u=0;u<nBins;u++)
      {
        TString par("coefBWeight_b__pTetabin");
        par+=u;
        par+="_weightbin0";

        double valueFin=((RooRealVar*)results.at(i).at(j)->floatParsFinal().find(par))->getVal();
        double valueInit=values[u][i][j];
        //((RooRealVar*)results.at(i).at(j)->floatParsFinal().find(par))->getVal();
        //this is the efficiency for channel i and systematic j

        //valueEnd[i][u] is the nominal efficiency for channel i and bin u

        double actu=0;
        double actuSF=0;
//        if (j>=6)
        if (systematics.at(j).Contains("Up")||(systematics.at(j).Contains("up") && !systematics.at(j).Contains("down")))
        {
          double valueFin2=((RooRealVar*)results.at(i).at(j-1)->floatParsFinal().find(par))->getVal();
          double valueInit2=values[u][i][j-1];
          double central=((RooRealVar*)results.at(i).at(0)->floatParsFinal().find(par))->getVal();

          if (systematics.at(j)=="MistagUp")
          {
            valueFin=central + (valueFin-central)/2.;
          }

          actu=(valueFin-valueFin2)/2.;
          actuSF=(valueFin/valueInit-valueFin2/valueInit2)/2.;

          cout << " actu " << actu << " actuSF " << actuSF << endl;


          if (systematics.at(j)=="JetEResolUp")
          {
            cout << " ValFin " << valueFin << " ValFin2 " << valueFin2 << endl;
            valueFin=(valueFin+valueFin2)/2.;
            cout << " Av " << valueFin << endl;
            
            valueInit=(valueInit+valueInit2)/2.;
            cout << " ValInit " << valueInit << " ValInit2 " << valueInit2 << endl;
            valueInit2=values[u][i][0];
            valueFin2=((RooRealVar*)results.at(i).at(0)->floatParsFinal().find(par))->getVal();

            cout << " central init " << valueInit2 << " fin " << valueFin2 << endl;

            actu=(valueFin-valueFin2);
            actuSF=(valueFin/valueInit-valueFin2/valueInit2);

            cout << " actu " << actu << " actuSF " << actuSF << endl;

          }

          cout << std::setprecision(3) << "(+"<< 
             (valueFin-central)/valInitial[i][u] << "/-" <<
              (-valueFin2+central)/valInitial[i][u] << ") ";
/*
          cout << std::setprecision(3) << "(+"<< 
              (valueFin-central)/valInitial[i][u] << "/-" <<
              (-valueFin2+central)/valInitial[i][u] << ") ";
*/

        }
        else if (systematics.at(j)=="MCAtNLO")
        {
          double valueFin2=((RooRealVar*)results.at(i).at(j-1)->floatParsFinal().find(par))->getVal();
          double valueInit2=values[u][i][j-1];
          actu=(valueFin-valueFin2)/1.;
          actuSF=(valueFin/valueInit-valueFin2/valueInit2)/1.;
        }
        else if (systematics.at(j)=="ZCR")
        {
          double valueFin2=((RooRealVar*)results.at(i).at(0)->floatParsFinal().find(par))->getVal();
          double valueInit2=values[u][i][0];
          actu=(valueFin-valueFin2)/1.;
          actuSF=(valueFin/valueInit-valueFin2/valueInit2)/1.;
        }
        else if (systematics.at(j)=="ZCRLoose")
        {
          double valueFin2=((RooRealVar*)results.at(i).at(0)->floatParsFinal().find(par))->getVal();
          double valueInit2=values[u][i][0];
          actu=(valueFin-valueFin2)/1.;
          actuSF=(valueFin/valueInit-valueFin2/valueInit2)/1.;
        }
        else if (systematics.at(j)=="PowhegHerwig")
        {
          double valueFin2=((RooRealVar*)results.at(i).at(0)->floatParsFinal().find(par))->getVal();
          double valueInit2=values[u][i][0];
          actu=(valueFin-valueFin2)/1.;
          actuSF=(valueFin/valueInit-valueFin2/valueInit2)/1.;
        }
        else if (systematics.at(j)=="PowhegPythia8")
        {
          double valueFin2=((RooRealVar*)results.at(i).at(0)->floatParsFinal().find(par))->getVal();
          double valueInit2=values[u][i][0];
          actu=(valueFin-valueFin2)/1.;
          actuSF=(valueFin/valueInit-valueFin2/valueInit2)/1.;
        }
        else if (systematics.at(j)=="ttbarRadLo")
        {
          double valueFin2=((RooRealVar*)results.at(i).at(j-1)->floatParsFinal().find(par))->getVal();
          double valueInit2=values[u][i][j-1];
          //divison by 4 because you only take half of this uncertainty (this is varition up - variation down, so 2 sigma variation)
          actu=(valueFin-valueFin2)/2;
          actuSF=(valueFin/valueInit-valueFin2/valueInit2)/2.;
        }
        else if (systematics.at(j)=="SherpaZ")
        {
          double valueFin2=((RooRealVar*)results.at(i).at(0)->floatParsFinal().find(par))->getVal();
          double valueInit2=values[u][i][0];
          actu=(valueFin-valueFin2)/1.;
          actuSF=(valueFin/valueInit-valueFin2/valueInit2)/1.;
        }
        else
        {
          actu=0;
          actuSF=0;
        }
        cout << std::setprecision(3) << actu/valInitial[i][u] << "  ";
//GP BUUUUUUGGGGG!!!
//        if (systematics.at(j).Contains("Up") j>=7) actu=actu/sqrt(2.);
        //MuonEResolMS buggy, remove for the ee+mumu channels for the moment
        //rescale some systematics to better estimates
        if (systematics.at(j).Contains("Lepton fakes")) actu*=50./30.;
//        if (systematics.at(j)=="ReweightedTopPt") actu*=0.5;
        if (systematics.at(j)=="DibAlpgen"&& channels.at(i).Contains("3jets")) actu*=0.5;
        if (systematics.at(j)=="DibAlpgen"&& channels.at(i).Contains("2jets")) actu*=0.5;
	//	if (systematics.at(j).Contains("MCAtNLO")) actu=0;
//        if (systematics.at(j).Contains("dib") && channels.at(i).Contains("3jets")) actu*=0.41/0.35;
        if (systematics.at(j).Contains("dib") && channels.at(i).Contains("3jets")) actu*=0.41*2./0.35;
//        if (systematics.at(j).Contains("dib") && channels.at(i).Contains("2jets")) actu*=0.34*3./0.35;
        if (systematics.at(j).Contains("dib") && channels.at(i).Contains("2jets")) actu*=0.34/0.35;
        if (systematics.at(j).Contains("stop") && channels.at(i).Contains("3jets")) actu*=0.35/0.25;
        if (systematics.at(j).Contains("stop") && channels.at(i).Contains("2jets")) actu*=0.25/0.25;
        if (systematicsName.at(j).Contains("Z$+$jet") && channels.at(i).Contains("2jets")) actu*=0.2/0.2;


        //rescale some systematics to better estimates
        if (systematics.at(j).Contains("Lepton fakes")) actuSF*=50./30.;
//        if (systematics.at(j)=="ReweightedTopPt") actuSF*=0.5;
        if (systematics.at(j)=="DibAlpgen"&& channels.at(i).Contains("3jets")) actuSF*=0.5;
        if (systematics.at(j)=="DibAlpgen"&& channels.at(i).Contains("2jets")) actuSF*=0.5;
	//	if (systematics.at(j).Contains("MCAtNLO")) actuSF=0;
//        if (systematics.at(j).Contains("dib") && channels.at(i).Contains("3jets")) actuSF*=0.41/0.35;
        if (systematics.at(j).Contains("dib") && channels.at(i).Contains("3jets")) actuSF*=0.41*2./0.35;
//        if (systematics.at(j).Contains("dib") && channels.at(i).Contains("2jets")) actuSF*=0.34*3./0.35;
        if (systematics.at(j).Contains("dib") && channels.at(i).Contains("2jets")) actuSF*=0.34/0.35;
        if (systematics.at(j).Contains("stop") && channels.at(i).Contains("3jets")) actuSF*=0.35/0.25;
        if (systematics.at(j).Contains("stop") && channels.at(i).Contains("2jets")) actuSF*=0.25/0.25;
        if (systematicsName.at(j).Contains("Z$+$jet") && channels.at(i).Contains("2jets")) actuSF*=0.2/0.2;


        if ((!systematics.at(j).Contains("MuonEResolMS")) || channels.at(i).Contains("emu"))
        {
          delta[i][u][j]=actu;
          deltaSF[i][u][j]=actuSF;//ctu/valInitial[i][u];
          valSystError[i][u]+=actu*actu;
        }
        else
        {
          delta[i][u][j]=0;
          deltaSF[i][u][j]=0;
//          valSystError[i][u]=0;
        }

/*        if ((!systematics.at(j).Contains("MuonEResolMS")) || channels.at(i).Contains("emu"))
        {

        }
        else
        {
*/
//        delta[i][u][j]=0;
//        deltaSF[i][u][j]=0;
          //      }
      }

      std::cout << endl;
    }




  }//end of channels (results vector)


  //now show complete results

  for (unsigned int i=0;i<results.size();i++)
  {
    cout << " channel " << channels.at(i) << endl;

    for (int u=0;u<nBins;u++)
    {
      valSystError[i][u]=sqrt(valSystError[i][u]);
      valTotError[i][u]=sqrt(valSystError[i][u]*valSystError[i][u]+valError[i][u]*valError[i][u]);
      SF[i][u]=valEnd[i][u]/valInitial[i][u];
//      cout << " Scale factor is: " << SF[i][u] << endl;
      SF_err[i][u]=valTotError[i][u]/valInitial[i][u];
      SF_staterr[i][u]=valError[i][u]/valInitial[i][u];
    }

    for (int u=0;u<nBins;u++)
    {
      cout << " bin " << u << " x " << x[u] << " err " << x_err[u] << " val truth " << valInitial[i][u] << " valEnd " << valEnd[i][u] << " +/- " << valError[i][u] << " +/- " << valSystError[i][u] <<  " ( " << valTotError[i][u] << " ) "<< " SF " << valEnd[i][u]/valInitial[i][u] << " +/- " << valError[i][u]/valInitial[i][u] << " +/- " << valSystError[i][u]/valInitial[i][u] << " ( " << valTotError[i][u]/valInitial[i][u] << " ) " << endl;
    }
  }

//  if (false)
  if (channels.size()==4 || channels.size()==8)
  {
    if (channels.size()==4)
    {
      latexresults << "\\begin{tabular}{|l|c|c|c|c|}" << endl;
    }
    else
    {
      latexresults << "\\begin{tabular}{|l|c|c|c|c|c|c|c|c|}" << endl;
    }
    
    latexresults << "\\hline " << endl;
    if (channels.size()==4)
    {
      if (!useGeneralBinning)
      {
        if (use6PTbins||use10PTbins)
        {
          latexresults <<  " $p_{T}$ interval & \\multicolumn{2}{|c|}{e$\\mu$} & \\multicolumn{2}{|c|}{ee$+\\mu\\mu$} \\\\ \\hline" << endl;
        }
        else
        {
          latexresults <<  " $\\eta$ interval & \\multicolumn{2}{|c|}{e$\\mu$} & \\multicolumn{2}{|c|}{ee$+\\mu\\mu$} \\\\ \\hline" << endl;
        }
        
        latexresults << "  (GeV) & 2 jet & 3 jet & 2 jet & 3 jet \\\\ \\hline" << endl;
      }
      else
      {
        latexresults <<  " $p_{T},\\eta$ interval & \\multicolumn{2}{|c|}{e$\\mu$} & \\multicolumn{2}{|c|}{ee$+\\mu\\mu$} \\\\ \\hline" << endl;
        latexresults << "   & 2 jet & 3 jet & 2 jet & 3 jet \\\\ \\hline" << endl;
      }
    }
    else
    {
      if (use6PTbins||use10PTbins)
      {
        latexresults <<  " $p_{T}$ interval & \\multicolumn{2}{|c|}{e$\\mu$} & \\multicolumn{2}{|c|}{ee$+\\mu\\mu$} & \\multicolumn{2}{|c|}{tag \\& probe} & \\multicolumn{2}{|c|}{inclusive}\\\\ \\hline" << endl;
        latexresults << "  (GeV) & 2 jet & 3 jet & 2 jet & 3 jet & e$\\mu$ & ee$+\\mu\\mu$ & e$\\mu$ & ee$+\\mu\\mu$ \\\\ \\hline" << endl;
      }
      else
      {
        latexresults <<  " $\eta$ interval & \\multicolumn{2}{|c|}{e$\\mu$} & \\multicolumn{2}{|c|}{ee$+\\mu\\mu$} & \\multicolumn{2}{|c|}{tag \\& probe} & \\multicolumn{2}{|c|}{inclusive}\\\\ \\hline" << endl;
        latexresults << "       & 2 jet & 3 jet & 2 jet & 3 jet & e$\\mu$ & ee$+\\mu\\mu$ & e$\\mu$ & ee$+\\mu\\mu$ \\\\ \\hline" << endl;
      }
      
    }

    for (int u=0;u<nBins;u++)
    {
      TString binName("$[");
      if (!useGeneralBinning)
      {
        if (use6PTbins||use10PTbins)
        {
          binName+=(int)(limitsPtEta1d[u]/1e3+0.5);
          binName+=";";
          binName+=(int)(limitsPtEta1d[u+1]/1e3+0.5);
          binName+="]$";
          if (nBins>10)
          {
            binName.ReplaceAll("[","");
            binName.ReplaceAll("]","");
          }
        }
        else
        {
          binName+=(limitsPtEta1d[u]/1e3);
          binName+=";";
          binName+=(limitsPtEta1d[u+1]/1e3);
          binName+="]$";
        }
      }
      else
      {
        binName=*binNames[u];
        binName+=",";
        binName+=*binNames2[u];
      }
      
      latexresults << binName;
      
      for (unsigned int i=0;i<results.size();i++)
      {
        latexresults << " & " << std::fixed << std::setprecision(1) << valInitial[i][u]*100. << " $\\pm$ " << errors[u][i][idxRef]*100.;
      }
      latexresults << " \\\\ " << endl;
    }
    latexresults << " \\hline " << endl;
    latexresults << "\\end{tabular}" << endl;
    
    if (channels.size()==4)
    {
      latexresults3 << "\\begin{tabular}{|l|c|c|c|c|}" << endl;
    }
    else
    {
      latexresults3 << "\\begin{tabular}{|l|c|c|c|c|c|c|c|c|}" << endl;
    }
    
    latexresults3 << "\\hline " << endl;
    if (!useGeneralBinning)
    {
      if (channels.size()==4)
      {
        if(use6PTbins||use10PTbins)
        {
          latexresults3 <<  " $p_{T}$ interval & \\multicolumn{2}{|c|}{e$\\mu$} & \\multicolumn{2}{|c|}{ee$+\\mu\\mu$} \\\\ \\hline" << endl;
          latexresults3 << "  (GeV) & 2 jet & 3 jet & 2 jet & 3 jet \\\\ \\hline" << endl;
        }
        else
        {
          latexresults3 <<  " $\\eta$ interval & \\multicolumn{2}{|c|}{e$\\mu$} & \\multicolumn{2}{|c|}{ee$+\\mu\\mu$} \\\\ \\hline" << endl;
          latexresults3 << "       & 2 jet & 3 jet & 2 jet & 3 jet \\\\ \\hline" << endl;
        }
      }
      else
      {
        latexresults3 <<  " $p_{T}$ interval & \\multicolumn{2}{|c|}{e$\\mu$} & \\multicolumn{2}{|c|}{ee$+\\mu\\mu$} & \\multicolumn{2}{|c|}{tag \\& probe} & \\multicolumn{2}{|c|}{inclusive}\\\\ \\hline" << endl;
        latexresults3 << "  (GeV) & 2 jet & 3 jet & 2 jet & 3 jet & e$\\mu$ & ee$+\\mu\\mu$ & e$\\mu$ & ee$+\\mu\\mu$ \\\\ \\hline" << endl;
      }
    }
    else
    {
      latexresults3 <<  " $p_{T},\\eta$ interval & \\multicolumn{2}{|c|}{e$\\mu$} & \\multicolumn{2}{|c|}{ee$+\\mu\\mu$} \\\\ \\hline" << endl;
      latexresults3 << "   & 2 jet & 3 jet & 2 jet & 3 jet \\\\ \\hline" << endl;
    }
      

    for (int u=0;u<nBins;u++)
    {
      TString binName("$[");

      if (!useGeneralBinning)
      {
        if (use6PTbins||use10PTbins)
        {
          binName+=(int)(limitsPtEta1d[u]/1e3+0.5);
          binName+=";";
          binName+=(int)(limitsPtEta1d[u+1]/1e3+0.5);
          binName+="]$";

          if (nBins>10)
          {
            binName.ReplaceAll("[","");
            binName.ReplaceAll("]","");
          }


        }
        else
        {
          binName+=(limitsPtEta1d[u]/1e3);
          binName+=";";
          binName+=(limitsPtEta1d[u+1]/1e3);
          binName+="]$";
        }
      }
      else
      {
        binName=*binNames[u];
        binName+=",";
        binName+=*binNames2[u];
      }
      
      latexresults3 << binName;
      
      for (unsigned int i=0;i<results.size();i++)
      {
        latexresults3 << " & " << std::fixed << std::setprecision(1) << 
            valEnd[i][u]*100. << " $\\pm$ " << valError[i][u]*100. << " $\\pm$ " << valSystError[i][u]*100. <<  " (" << valTotError[i][u]*100. << ")";
      }
      latexresults3 << " \\\\ " << endl;
    }
    latexresults3 << " \\hline " << endl;
    latexresults3 << "\\end{tabular}" << endl;

    if (channels.size()==4)
    {
      latexresults4 << "\\begin{tabular}{|l|c|c|c|c|}" << endl;
    }
    else
    {
      latexresults4 << "\\begin{tabular}{|l|c|c|c|c|c|c|c|c|}" << endl;
    }
    
    latexresults4 << "\\hline " << endl;
    if (!useGeneralBinning)
    {
      if (channels.size()==4)
      {
        if (use6PTbins||use10PTbins)
        {
          latexresults4 <<  " $p_{T}$ interval & \\multicolumn{2}{|c|}{e$\\mu$} & \\multicolumn{2}{|c|}{ee$+\\mu\\mu$} \\\\ \\hline" << endl;
          latexresults4 << "  (GeV) & 2 jet & 3 jet & 2 jet & 3 jet \\\\ \\hline" << endl;
        }
        else
        {
          latexresults4 <<  " $\\eta$ interval & \\multicolumn{2}{|c|}{e$\\mu$} & \\multicolumn{2}{|c|}{ee$+\\mu\\mu$} \\\\ \\hline" << endl;
          latexresults4 << "       & 2 jet & 3 jet & 2 jet & 3 jet \\\\ \\hline" << endl;
        }
        
      }
      else
      {
        latexresults4 <<  " $p_{T}$ interval & \\multicolumn{2}{|c|}{e$\\mu$} & \\multicolumn{2}{|c|}{ee$+\\mu\\mu$} & \\multicolumn{2}{|c|}{tag \\& probe} & \\multicolumn{2}{|c|}{inclusive}\\\\ \\hline" << endl;
        latexresults4 << "  (GeV) & 2 jet & 3 jet & 2 jet & 3 jet & e$\\mu$ & ee$+\\mu\\mu$ & e$\\mu$ & ee$+\\mu\\mu$ \\\\ \\hline" << endl;
      }
    }
    else
    {
      latexresults4 <<  " $p_{T},\\eta$ interval & \\multicolumn{2}{|c|}{e$\\mu$} & \\multicolumn{2}{|c|}{ee$+\\mu\\mu$} \\\\ \\hline" << endl;
      latexresults4 << "   & 2 jet & 3 jet & 2 jet & 3 jet \\\\ \\hline" << endl;
    }
/*
    latexresults << "\\begin{tabular}{|l|c|c|c|c|}" << endl;
    latexresults << "\\hline " << endl;
    latexresults <<  " $p_{T}$ interval & \\multicolumn{2}{|c|}{e$\\mu$} & \\multicolumn{2}{|c|}{ee$+\\mu\\mu$} \\\\ \\hline" << endl;
        latexresults << "  (GeV) & 2 jet & 3 jet & 2 jet & 3 jet \\\\ \\hline" << endl;
*/
    for (int u=0;u<nBins;u++)
    {
      TString binName("$[");
      if (!useGeneralBinning)
      {
        if (use6PTbins||use10PTbins)
        {
          binName+=(int)(limitsPtEta1d[u]/1e3+0.5);
          binName+=";";
          binName+=(int)(limitsPtEta1d[u+1]/1e3+0.5);
          binName+="]$";

          if (nBins>10)
          {
            binName.ReplaceAll("[","");
            binName.ReplaceAll("]","");
          }
          

        }
        else
        {
          binName+=(limitsPtEta1d[u]/1e3);
          binName+=";";
          binName+=(limitsPtEta1d[u+1]/1e3);
          binName+="]$";
        }
      }
      else
      {
        binName=*binNames[u];
        binName+=",";
        binName+=*binNames2[u];
      }

      
      latexresults4 << binName;
      
      for (unsigned int i=0;i<results.size();i++)
      {
        latexresults4 << " & " << std::fixed << std::setprecision(3) << valEnd[i][u]/valInitial[i][u] << " $\\pm$ " << valError[i][u]/valInitial[i][u] << " $\\pm$ " << valSystError[i][u]/valInitial[i][u];
      }
      latexresults4 << " \\\\ " << endl;
    }
    latexresults4 << " \\hline " << endl;
    latexresults4 << "\\end{tabular}" << endl;

   
    if (channels.size()==4)
    {
      latexresults5 << "\\begin{tabular}{|l|c|c|c|c|}" << endl;
    }
    else
    {
      latexresults5 << "\\begin{tabular}{|l|c|c|c|c|c|c|c|c|}" << endl;
    }
    
    latexresults5 << "\\hline " << endl;
    if (channels.size()==4)
    {
      if (!useGeneralBinning)
      {
        if (use6PTbins||use10PTbins)
        {
          latexresults5 <<  " $p_{T}$ interval & \\multicolumn{2}{|c|}{e$\\mu$} & \\multicolumn{2}{|c|}{ee$+\\mu\\mu$} \\\\ \\hline" << endl;
          latexresults5 << "  (GeV) & 2 jet & 3 jet & 2 jet & 3 jet \\\\ \\hline" << endl;
        }
        else
        {
          latexresults5 <<  " $\\eta$ interval & \\multicolumn{2}{|c|}{e$\\mu$} & \\multicolumn{2}{|c|}{ee$+\\mu\\mu$} \\\\ \\hline" << endl;
          latexresults5 << "       & 2 jet & 3 jet & 2 jet & 3 jet \\\\ \\hline" << endl;
        }
        
      }
      else
      {
        latexresults5 <<  " $p_{T},\\eta$ interval & \\multicolumn{2}{|c|}{e$\\mu$} & \\multicolumn{2}{|c|}{ee$+\\mu\\mu$} \\\\ \\hline" << endl;
        latexresults5 << "   & 2 jet & 3 jet & 2 jet & 3 jet \\\\ \\hline" << endl;
      }
    }
    else
    {
      latexresults5 <<  " $p_{T}$ interval & \\multicolumn{2}{|c|}{e$\\mu$} & \\multicolumn{2}{|c|}{ee$+\\mu\\mu$} & \\multicolumn{2}{|c|}{tag \\& probe} & \\multicolumn{2}{|c|}{inclusive}\\\\ \\hline" << endl;
      latexresults5 << "  (GeV) & 2 jet & 3 jet & 2 jet & 3 jet & e$\\mu$ & ee$+\\mu\\mu$ & e$\\mu$ & ee$+\\mu\\mu$ \\\\ \\hline" << endl;
    }
/*
    latexresults << "\\begin{tabular}{|l|c|c|c|c|}" << endl;
    latexresults << "\\hline " << endl;
    latexresults <<  " $p_{T}$ interval & \\multicolumn{2}{|c|}{e$\\mu$} & \\multicolumn{2}{|c|}{ee$+\\mu\\mu$} \\\\ \\hline" << endl;
    latexresults << "  (GeV) & 2 jet & 3 jet & 2 jet & 3 jet \\\\ \\hline" << endl;
*/
    for (int u=0;u<nBins;u++)
    {
      TString binName("$[");
      if (!useGeneralBinning)
      {
        if (use6PTbins||use10PTbins)
        {
          binName+=(int)(limitsPtEta1d[u]/1e3+0.5);
          binName+=";";
          binName+=(int)(limitsPtEta1d[u+1]/1e3+0.5);
          binName+="]$";

          if (nBins>10)
          {
            binName.ReplaceAll("[","");
            binName.ReplaceAll("]","");
          }

        }
        else
        {
          binName+=(limitsPtEta1d[u]/1e3);
          binName+=";";
          binName+=(limitsPtEta1d[u+1]/1e3);
          binName+="]$";  
        }
      }
      else
      {
        binName=*binNames[u];
        binName+=",";
        binName+=*binNames2[u];
      } 
      
      latexresults5 << binName;
      
      for (unsigned int i=0;i<results.size();i++)
      {
        latexresults5 << " & " << std::fixed << std::setprecision(3) << valEnd[i][u]/valInitial[i][u] << " $\\pm$ " << valTotError[i][u]/valInitial[i][u] << " ";
      }
      latexresults5 << " \\\\ " << endl;
    }
    latexresults5 << " \\hline " << endl;
    latexresults5 << "\\end{tabular}" << endl;

    //now single channels scale factors...

    for (unsigned int j=0;j<results.size();j++)
    {

      (*latexresultsChan[j]) << " %";
      (*latexresultsChan[j]) << channelDescription.at(j) << endl;
      

      (*latexresultsChan[j]) << "\\begin{tabular}{|l|";
      
      for (unsigned int i=0;i<nBins;i++)
      {
        (*latexresultsChan[j]) << "c|";
      }
      (*latexresultsChan[j]) << "}" << endl;
      
      (*latexresultsChan[j]) << " " << endl;
    
      (*latexresultsChan[j]) << " \\hline " << endl;

      (*latexresultsChan[j]) << " $p_{T}$ ";

      for (unsigned int u=0;u<nBins;u++)
      {
        
        TString binName("$[");
        if (!useGeneralBinning)
        {
          if (use6PTbins||use10PTbins)
          {
            binName+=(int)(limitsPtEta1d[u]/1e3+0.5);
            binName+=";";
            binName+=(int)(limitsPtEta1d[u+1]/1e3+0.5);
            binName+="]$";
            if (nBins>10)
            {
              binName.ReplaceAll("[","");
              binName.ReplaceAll("]","");
            }

          }
          else
          {
             binName+=(limitsPtEta1d[u]/1e3);
             binName+=";";
             binName+=(limitsPtEta1d[u+1]/1e3);
             binName+="]$";
          }
        }
        else
        {
          binName=*binNames[u];
          if (nBins>10)
          {
            binName.ReplaceAll("[","");
            binName.ReplaceAll("]","");
          }
        }
        
        (*latexresultsChan[j]) << " & " << binName;
        
      }


      (*latexresultsChan[j]) << " \\\\ \\hline " << endl;


      if (useGeneralBinning)
      {

        (*latexresultsChan[j]) << " $\\eta$ ";

        for (unsigned int u=0;u<nBins;u++)
        {
          
          TString binName("$");
          binName=*binNames2[u];
          if (nBins>10)
          {
            binName.ReplaceAll("[","");
            binName.ReplaceAll("]","");
          }

          (*latexresultsChan[j]) << " & " << binName;

        }
        
      (*latexresultsChan[j]) << " \\\\ \\hline " << endl;

      }
      
      (*latexresultsChan[j]) << " SF ";
      for (unsigned int u=0;u<nBins;u++)
      {
        (*latexresultsChan[j]) << " & " << std::fixed << std::setprecision(3) << valEnd[j][u]/valInitial[j][u];
      }
      (*latexresultsChan[j]) << " \\\\ \\hline " << endl;

      (*latexresultsChan[j]) << " Total error ";
      for (unsigned int u=0;u<nBins;u++)
      {
        (*latexresultsChan[j]) << " & " << std::fixed << std::setprecision(3) << valTotError[j][u]/valInitial[j][u];
      }
      (*latexresultsChan[j]) << " \\\\ \\hline " << endl;

      (*latexresultsChan[j]) << " Stat error ";
      for (unsigned int u=0;u<nBins;u++)
      {
        (*latexresultsChan[j]) << " & " << std::fixed << std::setprecision(3) << valError[j][u]/valInitial[j][u];
      }
      (*latexresultsChan[j]) << " \\\\ " << endl;

      (*latexresultsChan[j]) << " Syst error ";
      for (unsigned int u=0;u<nBins;u++)
      {
        (*latexresultsChan[j]) << " & " << std::fixed << std::setprecision(3) << valSystError[j][u]/valInitial[j][u];
      }
      (*latexresultsChan[j]) << " \\\\ \\hline " << endl;
    
      for (unsigned int i=0;i<systematics.size()-1;i++)
      {
        
        bool anyIsDifferentFromZero=false;
        
        for (unsigned int u=0;u<nBins;u++)
        {
          //chan,pT,syst
          if (fabs(deltaSF[j][u][i])>1e-8) anyIsDifferentFromZero=true;
        }

        if (anyIsDifferentFromZero)
        {
          
          (*latexresultsChan[j]) << systematicsName.at(i);

          for (unsigned int u=0;u<nBins;u++)
          {
            (*latexresultsChan[j]) << " & " << std::fixed << std::setprecision(3) << deltaSF[j][u][i];
          }
          (*latexresultsChan[j]) << " \\\\ " << endl;
        }
      }
      (*latexresultsChan[j]) << "\\hline" << endl;

      (*latexresultsChan[j]) << "\\end{tabular}" << endl;
      (*latexresultsChan[j]) << endl;

    }//end channels
    
  }//end if size of channels == 4
  
  


  double SFreduced[10][10][20];//nPlot,nChannel,nBin
  double SFreducedErr[10][10][20];
  double SFreducedStatErr[10][10][20];
  double xreduced[10][10][20];
  double xreduced_err[10][10][20];
  
  for (unsigned r=0;r<nPlots;r++)
  {
    for (unsigned int i=0;i<results.size();i++)
    {
      for (unsigned o=0;o<npTOnlyBins;o++)
      {
        SFreduced[r][i][o]=SF[i][binsPlot.at(r).at(o)];
        SFreducedErr[r][i][o]=SF_err[i][binsPlot.at(r).at(o)];
        SFreducedStatErr[r][i][o]=SF_staterr[i][binsPlot.at(r).at(o)];
        xreduced[r][i][o]=(limitsPt[o]+limitsPt[o+1])/2.;
        xreduced_err[r][i][o]=(limitsPt[o+1]-limitsPt[o])/2.;
      }
    }
  }
  

  TCanvas* c1=new TCanvas("c1","c1",400,400);

  gStyle->SetLegendBorderSize(0);

  TLegend* legend=new TLegend(0.37,0.2,0.67,0.49);
  legend->SetFillStyle(0);
  legend->SetLineStyle(0);
  legend->SetLineColor(0);

  TLegend* legendSF=new TLegend(0.37,0.2,0.67,0.39);
  legendSF->SetFillStyle(0);
  legendSF->SetLineStyle(0);
  legendSF->SetLineColor(0);

  vector<TGraphErrors*> efficienciesSF;

  int nBinsPlot=nBins-1;
  if(!useGeneralBinning && !use6PTbins && !use10PTbins) nBinsPlot=nBins;
  
  for (unsigned int i=0;i<results.size();i++)
  {
    if (!onlyStatUncertainty)
    {
      efficienciesSF.push_back(new TGraphErrors(nBinsPlot,x,SF[i],x_err,SF_err[i]));
    }
    else
    {
      efficienciesSF.push_back(new TGraphErrors(nBinsPlot,x,SF[i],x_err,SF_staterr[i]));
    }
    if (i==0)
    {
      if (use6PTbins||use10PTbins)
      {
        efficienciesSF.back()->GetXaxis()->SetTitle("Jet p_{T} [GeV]");
      }
      else if (!useGeneralBinning)
      {
        efficienciesSF.back()->GetXaxis()->SetTitle("#eta");
      }
      
      efficienciesSF.back()->Print("all");
      efficienciesSF.back()->Draw("ap");
      efficienciesSF.back()->GetYaxis()->SetRangeUser(y_min,y_max);
      if (runOnMC)  efficienciesSF.back()->GetYaxis()->SetRangeUser(0.9,1.05);
      efficienciesSF.back()->SetLineColor(3);
      efficienciesSF.back()->SetMarkerColor(3);
      TLine* a=new TLine(20e3,1,300e3,1);
      a->SetLineStyle(2);
      a->SetLineColor(1);
      a->Draw("same");
//      cout << " SF 1 " << SF[0][0] << " SF 2 " << SF[0][1] << " SF 3 " << SF[0][2] << endl;
    }
    else
    {
      if (3+i<5)
      {
        efficienciesSF.back()->SetLineColor(3+i);
        efficienciesSF.back()->SetMarkerColor(3+i);
      }
      else
      {
        efficienciesSF.back()->SetLineColor(3+i+1);
        efficienciesSF.back()->SetMarkerColor(3+i+1);
      }
      efficienciesSF.back()->Draw("p"); 
    }
  }

  


//  double delta[4][10][100];//channel,pT bin,systematics
//  double deltaSF[4][10][100];

  vector<TCanvas*> c1_multi;
  vector<vector<TGraphErrors*> > efficienciesSF_multi;

  if (useGeneralBinning)
  {
    
  for (unsigned r=0;r<nPlots;r++)
  {

    TString nameCanvas("c1_EtaBin");
    nameCanvas+=r;

    TCanvas* newCanvas=new TCanvas(nameCanvas,nameCanvas,400,400);
    c1_multi.push_back(newCanvas);
    
//  TLegend* legend=new TLegend(0.37,0.2,0.67,0.49);
//    TLegend* legend=new TLegend(0.17,0.17,0.46,0.47);//0.19,0.5,0.65,0.9);
    
    vector<TGraphErrors*> efficienciesSF_single;
    
//  double SFreduced[10][20];//nPlot,nBin
//  double SFreducedErr[10][20];
//  doube xreduced[10][20];
//  double xreduced_err[10][20];


    for (unsigned int i=0;i<results.size();i++)
    {
      if (!onlyStatUncertainty)
      {
        efficienciesSF_single.push_back(new TGraphErrors(npTOnlyBins,xreduced[r][0],SFreduced[r][i],xreduced_err[r][0],SFreducedErr[r][i]));
      }
      else
      {
        efficienciesSF_single.push_back(new TGraphErrors(npTOnlyBins,xreduced[r][0],SFreduced[r][i],xreduced_err[r][0],SFreducedStatErr[r][i]));
      }
      if (i==0)
      {
        if (!useGeneralBinning)
        {
          if (use6PTbins||use10PTbins)
          {
            efficienciesSF_single.back()->GetXaxis()->SetTitle("Jet p_{T} [GeV]");
          }
          else
          {
            efficienciesSF_single.back()->GetXaxis()->SetTitle("#eta");
          }
        }
        else
        {
          efficienciesSF_single.back()->GetXaxis()->SetTitle("");
        }
        
        efficienciesSF_single.back()->Draw("ap");
        efficienciesSF_single.back()->GetYaxis()->SetRangeUser(0.7,1.2);
        if (runOnMC)  efficienciesSF_single.back()->GetYaxis()->SetRangeUser(0.9,1.05);
        efficienciesSF_single.back()->SetLineColor(3);
        efficienciesSF_single.back()->SetMarkerColor(3);
        TLine* a=new TLine(20e3,1,300e3,1);
        a->SetLineStyle(2);
        a->SetLineColor(1);
        a->Draw();
      }
      else
      {
        efficienciesSF_single.back()->Draw("p"); 
        if (3+i<5)
        {
          efficienciesSF_single.back()->SetLineColor(3+i);
          efficienciesSF_single.back()->SetMarkerColor(3+i);
        }
        else
        {
          efficienciesSF_single.back()->SetLineColor(3+i+1);
          efficienciesSF_single.back()->SetMarkerColor(3+i+1);
        }
      }
    }
    efficienciesSF_multi.push_back(efficienciesSF_single);
  }//end canvases (and plots)
  
  }
  

  int nChannels=channels.size();

  TVectorD sf(nBins*nChannels);
  TMatrixDSym cov(nBins*nChannels);

  for (unsigned int i=0;i<results.size();i++)
  {
    for (unsigned int u=0;u<nBins;u++)
    {
      sf(u*nChannels+i)= valEnd[i][u]/valInitial[i][u];
      
    }
  }


  cout << " SF ";
  for (unsigned int i=0;i<nBins*nChannels;i++)
  {
    cout << " "<<i<<": " << std::setprecision(3)  << sf(i) << endl;
  }
  
  
  for (unsigned int i=0;i<results.size();i++)
  {
    for (unsigned int u=0;u<nBins;u++)
    {

//      cov(u*nChannels+i,u*nChannels+i)=valError[i][u]*valError[i][u];
      
      for (unsigned int i2=0;i2<results.size();i2++)
      {
        if (i==i2)
        {
          cov(u*nChannels+i,u*nChannels+i)+=valStatCovarianceSF[i][u][u];
        }

        for (unsigned int u2=0;u2<nBins;u2++)
        {

          if (i==i2 && u2!=u)
          {
            cov(u*nChannels+i,u2*nChannels+i2)+=valStatCovarianceSF[i][u][u2];
          }

          for (unsigned int j2=0;j2<systematics.size()-1;j2++)
          {
            if (doNotCorrelateDifferentPtBins)
            {
              if (u!=u2)
              {
                continue;
              }
            }
            if (!onlyStatUncertainty)
            {
              if (i==i2 && u==u2)
              {
                cov(u*nChannels+i,u2*nChannels+i2)+=deltaSF[i][u][j2]*deltaSF[i2][u2][j2];
              }
              else
              {
                double correlationToUse=1;
                if (useSpecifiedCorrelations)
                {
                  if (i!=i2)
                  {
                    correlationToUse*=correlationAmongChannels.at(j2);
                  }
                  if (u!=u2)
                  {
                    correlationToUse*=correlationAmongPTbins.at(j2);
                  }
                  cov(u*nChannels+i,u2*nChannels+i2)+=correlationToUse*deltaSF[i][u][j2]*deltaSF[i2][u2][j2];
                }
                else
                {
                  cov(u*nChannels+i,u2*nChannels+i2)+=correlationToUse*deltaSF[i][u][j2]*deltaSF[i2][u2][j2];
                }
              }//end if
            }
          }
        }
      }
    }
  }

  cout << " COV ";

  for (unsigned int i=0;i<nBins*nChannels;i++)
  {
    for (unsigned int j=0;j<nBins*nChannels;j++)
    {
      cout << " ["<<i<<","<<j<<"] "<< cov(i,j);
    }
    cout << endl;
  }

  std::cout << endl;
  std::cout << endl;
  
  cout << " CORR ";

  TMatrixDSym corr(nBins*nChannels);
  for (unsigned int i=0;i<nBins*nChannels;i++)
  {
    for (unsigned int j=0;j<nBins*nChannels;j++)
    {
      corr(i,j)=cov(i,j)/sqrt(cov(i,i)*cov(j,j));
      cout << " ["<<i<<","<<j<<"] "<< corr(i,j);
    }
    cout << endl;
  }

/////START

  vector<TMatrixD*> mixedCov;
  vector<TString> names;

  //add stat uncertainties first
  //create eigenvectors for uncorrelated stuff
  for (unsigned int i=0;i<results.size();i++)
  {  
    for (unsigned int u=0;u<nBins;u++)
    {
      mixedCov.push_back(new TMatrixD(1,nBins*nChannels)); //the n+1 is the uncertainty you want to keep track of correlations   
      
      (*mixedCov.back())[0][u*nChannels+i]=sqrt(valStatCovarianceSF[i][u][u]);
      TString nameSyst("statunc");
      nameSyst+="_uncorrPtBin";
      nameSyst+=u;
      nameSyst+=channels.at(i);
//       nameSyst+="_Ch";
//       nameSyst+=i;
      names.push_back(nameSyst);
    }
  }
  

  for (unsigned int j2=0;j2<systematics.size()-1;j2++)
  {

    //first uncorrelated
    double coefUncorrelated=(1.-correlationAmongChannels.at(j2))*(1.-correlationAmongPTbins.at(j2));
    if (coefUncorrelated>0.)
    {
      //create eigenvectors for uncorrelated stuff
      for (unsigned int i=0;i<results.size();i++)
      {  
        for (unsigned int u=0;u<nBins;u++)
        {

          //u*nChannels+i
//          (*mixedCov.back())[0][nBins*nChannels]=1;
          if (fabs(deltaSF[i][u][j2])>1e-8)
          {
            mixedCov.push_back(new TMatrixD(1,nBins*nChannels)); //the n+1 is the uncertainty you want to keep track of correlations   
            (*mixedCov.back())[0][u*nChannels+i]=sqrt(coefUncorrelated)*deltaSF[i][u][j2];
            TString nameSyst(systematics.at(j2));
            nameSyst+="_uncorrPtBin";
            nameSyst+=u;
	    nameSyst+=channels.at(i);
//             nameSyst+="_Ch";
//             nameSyst+=i;
            names.push_back(nameSyst);
          }
        }
      }
    }
    
    //now correlated among channels
    double coefAmongChannels=correlationAmongChannels.at(j2)*(1.-correlationAmongPTbins.at(j2));
    if (coefAmongChannels>0.)
    {

      for (unsigned int u=0;u<nBins;u++)
      {  

        bool anyDiffFromZero=false;
        for (unsigned int i=0;i<results.size();i++)//channels
        {
          if (fabs(deltaSF[i][u][j2])>1e-8) anyDiffFromZero=true;
        }

        if (anyDiffFromZero)
        {
          mixedCov.push_back(new TMatrixD(1,nBins*nChannels)); //the n+1 is the uncertainty you want to keep track of correlations   
//          (*mixedCov.back())[0][nBins*nChannels]=1;
          TString nameSyst(systematics.at(j2));
          nameSyst+="_PtBin";
          nameSyst+=u;
          names.push_back(nameSyst);
          
          for (unsigned int i=0;i<results.size();i++)//channels
          {
            (*mixedCov.back())[0][u*nChannels+i]=sqrt(coefAmongChannels)*deltaSF[i][u][j2];
          }
        }
      }
    }

    //now correlated among pT bins
    double coefAmongPTbins=(1.-correlationAmongChannels.at(j2))*correlationAmongPTbins.at(j2);
    if (coefAmongPTbins>0.)
    {
      for (unsigned int i=0;i<results.size();i++)//channels
      {  

        bool anyDiffFromZero=false;
        for (unsigned int u=0;u<nBins;u++)
        {
          if (fabs(deltaSF[i][u][j2])>1e-8) anyDiffFromZero=true;
        }

        if (anyDiffFromZero)
        {
          mixedCov.push_back(new TMatrixD(1,nBins*nChannels)); //the n+1 is the uncertainty you want to keep track of correlations   
//          (*mixedCov.back())[0][nBins*nChannels]=1;
          TString nameSyst(systematics.at(j2));
	  nameSyst+=channels.at(i);
 //          nameSyst+="_Ch";
//           nameSyst+=i;
          names.push_back(nameSyst);
          
          for (unsigned int u=0;u<nBins;u++)
          {
            (*mixedCov.back())[0][u*nChannels+i]=sqrt(coefAmongPTbins)*deltaSF[i][u][j2];
          }
        }
      }
    }
    

    double coefAmongPTandChannels=correlationAmongChannels.at(j2)*correlationAmongPTbins.at(j2);

    if (coefAmongPTandChannels>0)
    {
      bool anyDiffFromZero=false;
      for (unsigned int u=0;u<nBins;u++)
      {
        for (unsigned int i=0;i<results.size();i++)//channels
        {
          if (fabs(deltaSF[i][u][j2])>1e-8) anyDiffFromZero=true;
        }
      }
      
      if (anyDiffFromZero)
      {
        mixedCov.push_back(new TMatrixD(1,nBins*nChannels)); //the n+1 is the uncertainty you want to keep track of correlations   
        TString nameSyst(systematics.at(j2));
        nameSyst+="_fullyCorrelated";
        names.push_back(nameSyst);
        for (unsigned int i=0;i<results.size();i++)//channels
        { 
          for (unsigned int u=0;u<nBins;u++)
          {
            if (fabs(deltaSF[i][u][j2])>1e-8)
            {
              (*mixedCov.back())[0][u*nChannels+i]=sqrt(coefAmongPTandChannels)*deltaSF[i][u][j2];
            }
          }
        }
      }
    }
  }//end of systematics

  for (int i=0;i<names.size();i++)
  {
    
    cout << " Systematics: " << names.at(i);
    
    for (int u=0;u<nBins*nChannels;u++)
    {
      cout << " " << (*mixedCov.at(i))[0][u];
    }
    
    cout << endl;

  }
  
 

//////END

  int size=nBins*nChannels;

  TH2F* histoCorrelations=new TH2F("correlations","correlations",size,0,size,size,0,size);
  for (unsigned int i=0;i<nBins*nChannels;i++)
  {
    for (unsigned int j=0;j<nBins*nChannels;j++)
    {
      histoCorrelations->SetBinContent(i+1,j+1,corr(i,j));
    }
  }

  gStyle->SetPalette(1,0);
  
  TCanvas* c6=new TCanvas("c6","c6",400,400);
  
  histoCorrelations->Draw("colz");
  

#ifdef first1
  int nconstraints=nBins*(nChannels-1);
  TMatrixT<double> jacobian(nconstraints,nBins*nChannels);
  for (unsigned int u=0;u<nBins;u++)
  {
    for (unsigned int i=0;i<nChannels-1;i++)
    {
     jacobian((nChannels-1)*u+i,nChannels*u+0)=1;
      jacobian((nChannels-1)*u+i,nChannels*u+i+1)=-1;
    }
  }
#endif

#ifdef second1  
  int nconstraints=nBins*nChannels-1;
  TMatrixT<double> jacobian(nconstraints,nBins*nChannels);
  for (unsigned int u=0;u<nBins*nChannels-1;u++)
  {
    jacobian(u,0)=1;
    jacobian(u,u+1)=-1;
  }
#endif
#ifdef third
  int nconstraints=(nBins-1)*nChannels;
  TMatrixT<double> jacobian(nconstraints,nBins*nChannels);
  int mycount=0;
  for (unsigned int i=0;i<nChannels;i++)
  {
    for (unsigned int u=0;u<nBins-1;u++)
    {
      jacobian(mycount,i)=1;
      jacobian(mycount,nChannels*(u+1)+i)=-1;
      mycount+=1;
    }
  }
#endif  

  std::cout << endl;
  std::cout << endl;

  std::cout << " JACOBIAN " << endl;
  for (unsigned int i=0;i<nconstraints;i++)
  {
    for (unsigned int j=0;j<nBins*nChannels;j++)
    {
      cout << " ["<<i<<","<<j<<"] "<< jacobian(i,j);
    }
    cout << endl;
  }
  
  TMatrixTSym<double> matrixCovariance = cov;
  TMatrixTSym<double> matrixCovariance2 = cov;
  TMatrixTSym<double> matrixCovariance3 = cov;
  TMatrixTSym<double> matrixCovariance4 = cov;

  TMatrixTSym<double> matrixCovariance5 = cov;
  TMatrixTSym<double> matrixCovariance5_inv = matrixCovariance.Invert();

  TMatrixTSym<double> HCH = matrixCovariance2.Similarity(jacobian);
 
  TMatrixTSym<double> HCH_inv=HCH.Invert();

  TMatrixT<double> jacobianT=jacobian;
  jacobianT=jacobianT.T();

  TMatrixT<double> test2 = jacobianT * HCH_inv;
  
  TMatrixT<double> gainMatrix=matrixCovariance3 * test2;

  TVectorT<double> measurementsNew = sf - gainMatrix * (jacobian* sf);
  
  TMatrixT<double> newError = matrixCovariance4 - gainMatrix * jacobian * matrixCovariance4;

  double chi2=(jacobian* sf) * (HCH_inv * (jacobian* sf));
  cout << " chi2 is: " << chi2 << endl;
    
  std::cout << " size of new error matrix " << newError.GetNrows() << std::endl;
  
//now try to understand the errors / profiling

  TMatrixTSym<double> matrixCovarianceFinal(nBins*nChannels);
  TMatrixTSym<double> matrixCovarianceFinalToInv(nBins*nChannels);

  for (int i=0;i<nBins*nChannels;i++)
  {
    for (int u=0;u<nBins*nChannels;u++)
    {
      matrixCovarianceFinal[i][u]=newError[i][u];
    }
  }

 
//  TMatrixTSym<double> matrixCovarianceFinalToInv_inv=matrixCovarianceFinalToInv.Invert();

  double pullVector[1000];
  double nUncertainty[1000];
  double errorVector[1000];

  vector<TVectorD*> mixedCovNew;
  
  
  for (int u=0;u<1000;u++)
  {
    pullVector[u]=0;
    errorVector[u]=0;
    nUncertainty[u]=u;
  }

  int countNotUncorr(0);
  int countAll(0);
  for (int i=0;i<names.size();i++)
  {
    countAll+=1;
    if (!names.at(i).Contains("uncorr"))
    {
      countNotUncorr+=1;
    }
  }

  int nPlotsRequired=countNotUncorr/80+1;

  vector<TH1F*> plotProfiling;
  vector<TCanvas*> canvases;
  
  for (int u=0;u<nPlotsRequired;u++)
  {
    TString name("plotPulls");
    name+=u;
    plotProfiling.push_back(new TH1F(name,name,80,0,80));
    
    name+="_canvas";
    canvases.push_back(new TCanvas(name,name,1000,1000));
  }

  countNotUncorr=0;
  
    TMatrixDSym oldCovCheck(nBins*nChannels);
    TMatrixDSym newCovCheck(nBins*nChannels);
    
    for (int r=0;r<nBins*nChannels;r++)
    {
      for (int s=0;s<nBins*nChannels;s++)
      {
        oldCovCheck[r][s]=0;
        newCovCheck[r][s]=0;
      }
    }

  for (int i=0;i<names.size();i++)
  {

    cout << " Systematics: " << names.at(i);
  
    TMatrixD myVector=*mixedCov.at(i);

    mixedCovNew.push_back(new TVectorD(nBins*nChannels));
    //this will replace the old systematic matrix...


//    for (int u=0;u<nBins*nChannels;u++)
//    {
//      cout << " " << (*mixedCov.at(i))[0][u];
//    }

    TMatrixD diff(nBins*nChannels,1);
    TVectorD vect(nBins*nChannels);

    TVectorD newDifferences(nBins*nChannels);

    TVectorD vectNew(nBins*nChannels);

    for (int r=0;r<nBins*nChannels;r++)
    {
      diff[r][0]=(measurementsNew-sf)[r];
      vect[r]=myVector[0][r];
    }


    double pull=(myVector*matrixCovariance5_inv*diff)[0][0];
    
    double error=1-(matrixCovariance5_inv*(matrixCovariance4-matrixCovarianceFinal)*matrixCovariance5_inv).Similarity(vect);
    
    newDifferences=(matrixCovarianceFinal*matrixCovariance5_inv)*vect;

    (*mixedCovNew.back())=newDifferences;

    cout << names.at(i) << " bef constraint " << endl;
    
    for (int r=0;r<nBins*nChannels;r++)
    {
      cout << " " << r <<  " " << vect[r] << " > " << newDifferences[r];
    }
    cout << endl;

    for (int r=0;r<nBins*nChannels;r++)
    {
      for (int s=0;s<nBins*nChannels;s++)
      {
        oldCovCheck[r][s]+=vect[r]*vect[s];
        newCovCheck[r][s]+=newDifferences[r]*newDifferences[s];
      }
    }

    pullVector[i]=pull;
    errorVector[i]=error;

    int nPlot=countNotUncorr/80;

    if (!names.at(i).Contains("uncorr"))
    {
      plotProfiling.at(nPlot)->SetBinContent(1+(countNotUncorr%80),pull);
      plotProfiling.at(nPlot)->SetBinError(1+(countNotUncorr%80),error);

      plotProfiling.at(nPlot)->GetXaxis()->SetBinLabel(1+(countNotUncorr%80),names.at(i));
      plotProfiling.at(nPlot)->GetXaxis()->SetTitleOffset(30);
      plotProfiling.at(nPlot)->GetXaxis()->SetLabelSize(0.02);
      countNotUncorr+=1;

    }
    
    cout <<  " pull " << pull << " +/- " << error << endl;

  }


  for (int u=0;u<nPlotsRequired;u++)
  {
    canvases.at(u)->cd();
    canvases.at(u)->SetBottomMargin(0.3);
    plotProfiling.at(u)->SetLineColor(2);
    plotProfiling.at(u)->SetMarkerColor(2);
    
    plotProfiling.at(u)->Draw();
  
    TLine* b1=new TLine(0,1,80,1);
    b1->SetLineStyle(2);
    b1->SetLineColor(1);
    b1->Draw();
    
    TLine* b2=new TLine(0,-1,80,-1);
    b2->SetLineStyle(2);
    b2->SetLineColor(1);
    b2->Draw();

    TString name8("combFW_");
    name8+=tagPoint;
    name8+="pulls";
    name8+="_";
    name8+=u;
    name8+=".eps";
//    if (channels.size()==2)
    {
      if (runOnMC) name8.ReplaceAll(".eps","_MCclosure.eps");
      canvases.at(u)->SaveAs(name8);
      name8.ReplaceAll(".eps",".png");
      canvases.at(u)->SaveAs(name8);
    }
  }
  
  TCanvas* c8=new TCanvas("c8","c8",400,400);
//  TGraphErrors* plotProfiling=new TGraphErrors(names.size(),nUncertainty,pullVector,0,errorVector);



  TMatrixDSym newErrorSym(newError.GetNrows());
  for (int u=0;u<newError.GetNrows();u++)
  {
    for (int j=0;j<newError.GetNrows();j++)
    {
      newErrorSym(u,j)=newError(u,j);
    }
  }

    std::cout << " CONSTRAINT " << std::endl;
    for (int u=0;u<size;u++)
    {
      cout << " old meas: " << sf(u) << " new meas: " << measurementsNew(u) << endl;
      cout << " old Error: " << sqrt(cov(u,u)) << " newError: " << sqrt(newError(u,u)) << endl;
    }
    
    std::cout << " errorMatrix " << std::endl;
    for (int i=0;i<newError.GetNrows();i++)
    {
      for (int j=0;j<newError.GetNcols();j++)
      {
        std::cout << " [" << i << "," << j << "]=" << cov(i,j) << "=?" << oldCovCheck[i][j] << "|" << newError(i,j) << "=?" << newCovCheck[i][j];
      }
      cout << endl;


    }

  double SFcomb[100];
  double SFcomb_err[100];
  double Effcomb[100];
  double Effcomb_err[100];
  

  for (unsigned int i=0;i<100;i++)
  {
    SFcomb[i]=0;
    SFcomb_err[i]=0;
    Effcomb[i]=0;
    Effcomb_err[i]=0;
  }

  TH2F* histoCorrelationsAfter=new TH2F("correlationsAfter","correlationsAfter",nBins,0,nBins,nBins,0,nBins);
  
  for (unsigned int j=0;j<nBins;j++)
  {
    std::cout << measurementsNew(j*nChannels) << " +/- " << sqrt(newError(j*nChannels,j*nChannels)) << endl;
    SFcomb[j]=measurementsNew(j*nChannels);
    SFcomb_err[j]=sqrt(newError(j*nChannels,j*nChannels));
    
    for (unsigned int u=0;u<nBins;u++)
    {
      histoCorrelationsAfter->SetBinContent(j+1,u+1,newError(j*nChannels,u*nChannels)/sqrt(newError(j*nChannels,j*nChannels)*newError(u*nChannels,u*nChannels)));
    }

    double average=0;
    for (unsigned int l=0;l<channels.size();l++)
    {
      average+=valInitial[l][j];
    }
    average/=(double)channels.size();
    Effcomb[j]=SFcomb[j]*average;
    Effcomb_err[j]=SFcomb_err[j]*average;
  }

  TCanvas* c7=new TCanvas("c7","c7",400,400);
  histoCorrelationsAfter->Draw("colz");
    



  TCanvas* c2=new TCanvas("c2","c2",400,400);
  
  vector<TGraphErrors*> efficiencies;
  vector<TGraphErrors*> efficienciesTruth;
  

  double valInitialCommon[50];

  for (int u=0;u<nBins;u++)
  {

    TString par("coefBWeight_b__pTetabin");
    par+=u;
    par+="_weightbin0";

    int idxToUse=0;
    if (useBTagSF)
    {
      idxToUse=systematics.size()-1;
    }
    if (FScorrectionRef)
    {
      idxToUse=1;
    }

    double valueIn=0;
    for (int r=0;r<channels.size();r++)
    {
      valueIn+=((RooRealVar*)results.at(r).at(idxToUse)->floatParsInit().find(par))->getVal();
    }
    valueIn=valueIn/(double)channels.size();
    valInitialCommon[u]=valueIn;
  }

  for (unsigned int i=0;i<results.size();i++)
  {

    if (!onlyStatUncertainty)
    {
      efficiencies.push_back(new TGraphErrors(nBinsPlot,x,valEnd[i],x_err,valTotError[i]));
    }
    else
    {
      efficiencies.push_back(new TGraphErrors(nBinsPlot,x,valEnd[i],x_err,valError[i]));
    }
    //    efficienciesTruth.push_back(new TGraphErrors(nBins-1,x,valInitial[i],x_err,0));
    efficienciesTruth.push_back(new TGraphErrors(nBinsPlot,x,valInitialCommon,x_err,0));
    //efficienciesTruth.push_back(new TGraphErrors(nBins-1,x,valInitialCommon,0,0));
    
    if (i==0)
    {
      if (use6PTbins||use10PTbins)
      {
        efficienciesTruth.back()->GetXaxis()->SetTitle("Jet p_{T} [GeV]");
      }
      else if (!useGeneralBinning)
      {
        efficienciesTruth.back()->GetXaxis()->SetTitle("#eta");
      }

      efficienciesTruth.back()->Draw("ap");
      efficienciesTruth.back()->SetLineColor(1);
      efficienciesTruth.back()->SetMarkerColor(1);
      efficienciesTruth.back()->GetYaxis()->SetRangeUser(0.5,0.95);
      if (tagPoint=="MV2c2080")
      {
        efficienciesTruth.back()->GetYaxis()->SetRangeUser(0.55,1.05);
      }
      if (tagPoint=="MV2c2060")
      {
        efficienciesTruth.back()->GetYaxis()->SetRangeUser(0.2,0.8);
      }

      if (tagPoint=="MV2c2050")
      {
        efficienciesTruth.back()->GetYaxis()->SetRangeUser(0.,0.7);
      }
      if (tagPoint=="MV2c2030")
      {
        efficienciesTruth.back()->GetYaxis()->SetRangeUser(0.,0.5);
      }
      
      legend->AddEntry(efficienciesTruth.back(),"MC efficiency (average)","p");
      efficiencies.back()->SetLineColor(3);    
      efficiencies.back()->SetMarkerColor(3);    
      efficiencies.back()->Draw("p");
    }
    else
    {
      if (3+i<5)
      {
        efficiencies.back()->SetLineColor(3+i);
        efficiencies.back()->SetMarkerColor(3+i);
      }
      else
      { 
        efficiencies.back()->SetLineColor(3+i+1);
        efficiencies.back()->SetMarkerColor(3+i+1);
      }
      efficiencies.back()->Draw("p"); 
    }
    legend->AddEntry(efficiencies.back(),channels.at(i),"p");
    legendSF->AddEntry(efficiencies.back(),channels.at(i),"p");
  }


  //similar stuff
  double valEndreduced[10][10][20];//nPlot,nBin
  double valTotErrorreduced[10][10][20];
  double valErrorreduced[10][10][20];
  double valInitialrreduced[10][10][20];
  
  for (unsigned r=0;r<nPlots;r++)
  {
    for (unsigned int i=0;i<results.size();i++)
    {
      for (unsigned o=0;o<npTOnlyBins;o++)
      {
        valEndreduced[r][i][o]=valEnd[i][binsPlot.at(r).at(o)];
        valTotErrorreduced[r][i][o]=valTotError[i][binsPlot.at(r).at(o)];
        valErrorreduced[r][i][o]=valError[i][binsPlot.at(r).at(o)];
        valInitialrreduced[r][i][o]=valInitial[i][binsPlot.at(r).at(o)];
      }
    }
  }
//end similar stuff

  vector<TCanvas*> c2_multi;
  vector<vector<TGraphErrors*> > efficiencies_multi;
  vector<vector<TGraphErrors*> > efficienciesTruth_multi;


  if (useGeneralBinning)
  {
    
  for (unsigned r=0;r<nPlots;r++)
  {

    TString nameCanvas("c2_EtaBin");
    nameCanvas+=r;
   
    TCanvas* newCanvas=new TCanvas(nameCanvas,nameCanvas,400,400);
    c2_multi.push_back(newCanvas);
  
    vector<TGraphErrors*> efficiencies_single;
    vector<TGraphErrors*> efficiencies_singleTruth;
  

    for (unsigned int i=0;i<results.size();i++)
    {
      
      if (!onlyStatUncertainty)
      {
        efficiencies_single.push_back(new TGraphErrors(npTOnlyBins,xreduced[r][i],valEndreduced[r][i],xreduced_err[r][i],valTotErrorreduced[r][i]));
      }
      else
      {
        efficiencies_single.push_back(new TGraphErrors(npTOnlyBins,xreduced[r][i],valEndreduced[r][i],xreduced_err[r][i],valErrorreduced[r][i]));
      }
      efficiencies_singleTruth.push_back(new TGraphErrors(npTOnlyBins,xreduced[r][i],valInitialrreduced[r][i],xreduced_err[r][i],0));
      
      if (i==0)
      {
        efficiencies_singleTruth.back()->GetXaxis()->SetTitle("Jet p_{T} [GeV]");
        efficiencies_singleTruth.back()->Draw("ap");
        efficiencies_singleTruth.back()->SetLineColor(1);
        efficiencies_singleTruth.back()->SetMarkerColor(1);
        if (tagPoint=="MV2c2070")
        {
          efficiencies_singleTruth.back()->GetYaxis()->SetRangeUser(0.25,0.95);
        }
        if (tagPoint=="MV2c2080")
        {
          efficiencies_singleTruth.back()->GetYaxis()->SetRangeUser(0.55,1.05);
        }
        if (tagPoint=="MV2c2060")
        {
          efficiencies_singleTruth.back()->GetYaxis()->SetRangeUser(0.2,0.8);
        }
        if (tagPoint=="MV2c2050")
        {
          efficiencies_singleTruth.back()->GetYaxis()->SetRangeUser(0.,0.7);
        }
        if (tagPoint=="MV2c2030")
        {
          efficiencies_singleTruth.back()->GetYaxis()->SetRangeUser(0.,0.5);
        }

        
//        legend->AddEntry(efficiencies_singleTruth.back(),"MC eff. (1. chan.)","p");
        efficiencies_single.back()->SetLineColor(3);    
        efficiencies_single.back()->SetMarkerColor(3);    
        efficiencies_single.back()->Draw("p");
      }
      else
      {
        if (3+i<5)
        {
          efficiencies_single.back()->SetLineColor(3+i);
          efficiencies_single.back()->SetMarkerColor(3+i);
        }
        else
        { 
          efficiencies_single.back()->SetLineColor(3+i+1);
          efficiencies_single.back()->SetMarkerColor(3+i+1);
        }
        efficiencies_single.back()->Draw("p"); 
      }
//      legend->AddEntry(efficiencies_single.back(),channels.at(i),"p");
    }

    efficiencies_multi.push_back(efficiencies_single);
    efficienciesTruth_multi.push_back(efficiencies_singleTruth);

  }//end nPlots
  
  
  }
  

  std::cout << " B " << std::endl;

  c2->cd();
  

  //similar stuff
  double Effcomb_reduced[10][20];//nPlot,nBin
  double Effcomb_err_reduced[10][20];
  
  if (useGeneralBinning)
  {
    

  for (unsigned r=0;r<nPlots;r++)
  {
    for (unsigned o=0;o<npTOnlyBins;o++)
    {
      Effcomb_reduced[r][o]=Effcomb[binsPlot.at(r).at(o)];
      Effcomb_err_reduced[r][o]=Effcomb_err[binsPlot.at(r).at(o)];
    }
  }
  
  }
  
//end similar stuff

  


  

    TGraphErrors* combinedResultEff=new TGraphErrors(nBins,x,Effcomb,x_err,Effcomb_err);
//    TGraphErrors* combinedResultEff=new TGraphErrors(nBins,x,Effcomb,0,Effcomb_err);

    combinedResultEff->SetLineColor(2);
    combinedResultEff->SetMarkerColor(2);
    combinedResultEff->Draw("p"); 

    legend->AddEntry(combinedResultEff,"Combination","p");
    legendSF->AddEntry(combinedResultEff,"Combination","p");

    legend->Draw();
    
  float xpos=0.45;
  float yposlabel=0.87;
  float yposlumi=0.80;
  float yposchan=0.82;


  if (!runOnMC) ATLASLumi(xpos,yposlumi,  sample_lumi, sample_energy);
  ATLASLabel(xpos,yposlabel,true);

    TString name3("combFW_");
    name3+=tagPoint;
    name3+="singleEfficiencies.eps";

    if (channels.size()==4 || channels.size()==8)
    {
      if (runOnMC) name3.ReplaceAll(".eps","_MCclosure.eps");
      c2->SaveAs(name3);
      name3.ReplaceAll(".eps",".png");
      c2->SaveAs(name3);
    }

  vector<TGraphErrors*> combinedResultEff_multi;

  if (useGeneralBinning)
  {
    
 

  for (unsigned r=0;r<nPlots;r++)
  {

      TString name3("combFW_");
      name3+=tagPoint;
      name3+="singleEfficiencies_etabin";
      name3+=r;
      name3+=".eps";


    c2_multi.at(r)->cd();
//HERE
    
//    TGraphErrors* combinedResultEff=new TGraphErrors(nBins,x,Effcomb,x_err,Effcomb_err);
    TGraphErrors* combinedResultEff=new TGraphErrors(npTOnlyBins,xreduced[r][0],Effcomb_reduced[r],0,Effcomb_err_reduced[r]);
    
    combinedResultEff_multi.push_back(combinedResultEff);

    combinedResultEff->SetLineColor(2);
    combinedResultEff->SetMarkerColor(2);
    combinedResultEff->Draw("p"); 
    
//    legend->AddEntry(combinedResultEff,"Combination","p");
    
    legend->Draw();
    
//  float xpos=0.45;
    float xpos=0.5;
    float yposlabel=0.9;
    float yposlumi=0.83;
    float yposchan=0.82;
    
    
    if (!runOnMC) ATLASLumi(xpos,yposlumi,  sample_lumi, sample_energy);
    ATLASLabel(xpos,yposlabel,true);
    
    TLatex p; 
    p.SetNDC();
    p.SetTextFont(42);
//    p.SetTextColor(color);
    p.DrawLatex(0.23,0.83,descriptionEtaBinning.at(r));





    if (channels.size()==4)
//    if (channels.size()==2)
    {
      if (runOnMC) name3.ReplaceAll(".eps","_MCclosure.eps");
      c2_multi.at(r)->SaveAs(name3);
      name3.ReplaceAll(".eps",".png");
      c2_multi.at(r)->SaveAs(name3);
    }
  }
  
  }
  

//END




  c1->cd();
  TGraphErrors* combinedResult=new TGraphErrors(nBinsPlot,x,SFcomb,x_err,SFcomb_err);
  combinedResult->GetYaxis()->SetRangeUser(0.6,1.4);
  combinedResult->SetLineColor(2);
  combinedResult->SetMarkerColor(2);
  combinedResult->SetLineWidth(2);

  combinedResult->Draw("p");
    
  legendSF->Draw();

  if (!runOnMC) ATLASLumi(xpos,yposlumi,  sample_lumi, sample_energy);
  ATLASLabel(xpos,yposlabel,true);

  TString name1("combFW_");
  name1+=tagPoint;
  name1+="singleSFs.eps";
  if (channels.size()==4 || channels.size()==8)
  {
    if (runOnMC) name1.ReplaceAll(".eps","_MCclosure.eps");
    c1->SaveAs(name1);
    name1.ReplaceAll(".eps",".png");
    c1->SaveAs(name1);
  }



  //similar stuff
  double SFcomb_reduced[10][20];//nPlot,nBin
  double SFcomb_err_reduced[10][20];//nPlot,nBin

  if (useGeneralBinning)
  {
    
  
  for (unsigned r=0;r<nPlots;r++)
  {
    for (unsigned o=0;o<npTOnlyBins;o++)
    {
      SFcomb_reduced[r][o]=SFcomb[binsPlot.at(r).at(o)];
      SFcomb_err_reduced[r][o]=SFcomb_err[binsPlot.at(r).at(o)];
    }
  }

  
  }
  
  vector<TGraphErrors*> combinedResult_multi;

  if (useGeneralBinning)
  {
    

  for (unsigned r=0;r<nPlots;r++)
  {
    c1_multi.at(r)->cd();

    TGraphErrors* combinedResult=new TGraphErrors(npTOnlyBins,xreduced[r][0],SFcomb_reduced[r],xreduced_err[r][0],SFcomb_err_reduced[r]);
    combinedResult->GetYaxis()->SetRangeUser(0.2,1);
    combinedResult->SetLineColor(2);
    combinedResult->SetMarkerColor(2);
    combinedResult->SetLineWidth(2);

    combinedResult_multi.push_back(combinedResult);
    
    combinedResult->Draw("p");
    
    legendSF->Draw();
    
    if (!runOnMC) ATLASLumi(xpos,yposlumi,  sample_lumi, sample_energy);
    ATLASLabel(xpos,yposlabel,true);
    
    TLatex p2; 
    p2.SetNDC();
    p2.SetTextFont(42);
//    p2.SetTextColor(color);
    p2.DrawLatex(0.23,0.83,descriptionEtaBinning.at(r));

    TString name1("combFW_");
    name1+=tagPoint;
    name1+="singleSFs_etabin";
    name1+=r;
    name1+=".eps";
    if (channels.size()==4)
//      if (channels.size()==2)
    {
      if (runOnMC) name1.ReplaceAll(".eps","_MCclosure.eps");
      c1_multi.at(r)->SaveAs(name1);
      name1.ReplaceAll(".eps",".png");
      c1_multi.at(r)->SaveAs(name1);
    }
  }
  
  }
  
  //END





  combinedResult->SetLineColor(1);
  combinedResult->SetMarkerColor(1);

    combinedResultEff->SetLineColor(1);
    combinedResultEff->SetMarkerColor(1);

   
    SetAtlasStyle();
  

    TCanvas* c4=new TCanvas("c4","c4",400,400);

    efficienciesTruth.at(0)->SetLineColor(2);    
    efficienciesTruth.at(0)->SetMarkerStyle(4);
    efficienciesTruth.at(0)->SetMarkerColor(2);    

    if (use6PTbins||use10PTbins)
    {
      efficienciesTruth.at(0)->GetXaxis()->SetTitle("Jet p_{T} [GeV]");
    }
    else if (!useGeneralBinning)
    {
      efficienciesTruth.at(0)->GetXaxis()->SetTitle("#eta");
    }

    efficienciesTruth.at(0)->GetXaxis()->SetTitle("Jet p_{T} [GeV]");
    efficienciesTruth.at(0)->GetYaxis()->SetTitle("b-jet efficiency");  
    efficienciesTruth.at(0)->Draw("ap");
    efficienciesTruth.at(0)->GetYaxis()->SetRangeUser(0.25,1);
      if (tagPoint=="MV2c2080")
      {
        efficienciesTruth.at(0)->GetYaxis()->SetRangeUser(0.55,1.05);
      }
      if (tagPoint=="MV2c2060")
      {
        efficienciesTruth.at(0)->GetYaxis()->SetRangeUser(0.2,0.8);
      }

      if (tagPoint=="MV2c2050")
      {
        efficienciesTruth.at(0)->GetYaxis()->SetRangeUser(0.,0.7);
      }
      if (tagPoint=="MV2c2030")
      {
        efficienciesTruth.at(0)->GetYaxis()->SetRangeUser(0.,0.5);
      }


    combinedResultEff->Draw("p"); 

    TLegend* legend2=new TLegend(0.6,0.2,0.87,0.4);
    legend2->SetFillStyle(0);
    legend2->SetLineStyle(0);
    legend2->SetLineColor(0);
    legend2->AddEntry(efficienciesTruth.at(0),"MC (average)","p");
    legend2->AddEntry(combinedResultEff,"Data","p");
    legend2->Draw();

    TString name4("combFW_");
    name4+=tagPoint;
    name4+="combinedEfficiencies.eps";

    if (!runOnMC)     ATLASLumi(xpos,yposlumi,  sample_lumi, sample_energy);
    ATLASLabel(xpos,yposlabel,true);

    TLatex *CP1 = new TLatex();
    CP1->SetNDC(kTRUE);
    //    CP1->SetTextAlign(31);
    CP1->SetTextFont(42);
    CP1->SetTextSize(0.038);
    CP1->DrawLatex(0.21,0.21,"MV2c20, #epsilon_{b}="+WPcut+"%");



//    if (channels.size()==2 || runOnMC)
    {
      if (runOnMC) name4.ReplaceAll(".eps","_MCclosure.eps");
      c4->SaveAs(name4);
      name4.ReplaceAll(".eps",".C");
      c4->SaveAs(name4);
      name4.ReplaceAll(".C",".png");
      c4->SaveAs(name4);
    }
  


    //HERE

    if (useGeneralBinning)
    {
      

    for (unsigned r=0;r<nPlots;r++)
    {

      combinedResultEff_multi.at(r)->SetLineColor(1);
      combinedResultEff_multi.at(r)->SetMarkerColor(1);


      efficienciesTruth_multi.at(r).at(0)->SetLineColor(2);    
      efficienciesTruth_multi.at(r).at(0)->SetMarkerColor(2);    
      efficienciesTruth_multi.at(r).at(0)->SetMarkerStyle(4);
      efficienciesTruth_multi.at(r).at(0)->GetXaxis()->SetTitle("");
      efficienciesTruth_multi.at(r).at(0)->Draw("ap");

        if (tagPoint=="MV2c2070")
        {
          efficiencies_multi.at(r).at(0)->GetYaxis()->SetRangeUser(0.25,0.95);
        }
        if (tagPoint=="MV2c2080")
        {
          efficiencies_multi.at(r).at(0)->GetYaxis()->SetRangeUser(0.55,1.05);
        }
        if (tagPoint=="MV2c2060")
        {
          efficiencies_multi.at(r).at(0)->GetYaxis()->SetRangeUser(0.2,0.8);
        }
        if (tagPoint=="MV2c2050")
        {
          efficiencies_multi.at(r).at(0)->GetYaxis()->SetRangeUser(0.,0.7);
        }
        if (tagPoint=="MV2c2030")
        {
          efficiencies_multi.at(r).at(0)->GetYaxis()->SetRangeUser(0.,0.5);
        }

      
      combinedResultEff_multi.at(r)->Draw("p"); 
      
      TString name4("combFW_");
      name4+=tagPoint;
      name4+="combinedEfficiencies_etabin";
      name4+=r;
      name4+=".eps";
      
      if (!runOnMC)     ATLASLumi(xpos,yposlumi,  sample_lumi, sample_energy);
      ATLASLabel(xpos,yposlabel,true);
      
      TLatex p3; 
      p3.SetNDC();
      p3.SetTextFont(42);
      p3.DrawLatex(0.23,0.83,descriptionEtaBinning.at(r));

//      if (channels.size()==2 || runOnMC)
      {
        if (runOnMC) name4.ReplaceAll(".eps","_MCclosure.eps");
        c4->SaveAs(name4);
        name4.ReplaceAll(".eps",".png");
        c4->SaveAs(name4);
      }
    }
    
    }
    

    TVectorT<double> statUncertaintyNew(nBins*nChannels);
    TVectorT<double> sysUncertaintyNew(nBins*nChannels);
    for (unsigned int i=0;i<nBins*nChannels;i++)
    {
      statUncertaintyNew[i]=0;
      sysUncertaintyNew[i]=0;
    }

    for (unsigned int u=0;u<names.size();u++)
    {
      if (names.at(u).Contains("statunc"))
      {
        for (unsigned int i=0;i<nBins*nChannels;i++)
        {
          statUncertaintyNew[i]+=
              (*mixedCovNew.at(u))[i,i]*
              (*mixedCovNew.at(u))[i,i];
        }
      }
      else
      {
        for (unsigned int i=0;i<nBins*nChannels;i++)
        {
          sysUncertaintyNew[i]+=
              (*mixedCovNew.at(u))[i,i]*
              (*mixedCovNew.at(u))[i,i];
        }
      }
    }

    for (unsigned int i=0;i<nBins;i++)
    {

      std::cout << measurementsNew(i*nChannels) << 
          " +/- " << sqrt(statUncertaintyNew[i*nChannels]) <<
          " +/- " << sqrt(newError(i*nChannels,i*nChannels)-statUncertaintyNew[i*nChannels]) <<
          " [== " << sqrt(sysUncertaintyNew[i*nChannels]) << " ] " << 
          " ( " << sqrt(newError(i*nChannels,i*nChannels)) << " ) " << endl;
    }

    latexresultsTot << "\\begin{tabular}{|l|c|c|c|c|}" << endl;
    latexresultsTot << "\\hline " << endl;
    if (!useGeneralBinning)
    {
      if (use6PTbins||use10PTbins)
      {
        latexresultsTot <<  " $p_{T}$ interval &  Combined SF & Stat error & Syst error & Tot error \\\\" << endl;
      }
      else
      {
        latexresultsTot <<  " $\\eta$ interval &  Combined SF & Stat error & Syst error & Tot error \\\\" << endl;
      }
    }
    else
    {
      latexresultsTot <<  " $p_{T},\\eta$ interval &  Combined SF & Stat error & Syst error & Tot error \\\\" << endl;
    }
    latexresultsTot << "\\hline " << endl;

    for (int u=0;u<nBins;u++)
    {
      TString binName("$[");
      if (!useGeneralBinning)
      {
        if (use6PTbins||use10PTbins)
        {
          binName+=(int)(limitsPtEta1d[u]/1e3+0.5);
          binName+=";";
          binName+=(int)(limitsPtEta1d[u+1]/1e3+0.5);
          binName+="]$";

          if (nBins>10)
          {
            binName.ReplaceAll("[","");
            binName.ReplaceAll("]","");
          }

        }
        else
        {
          binName+=(limitsPtEta1d[u]/1e3);
          binName+=";";
          binName+=(limitsPtEta1d[u+1]/1e3);
          binName+="]$";
        }
      }
      else
      {
         binName=*binNames[u];
        binName+=",";
        binName+=*binNames2[u];
      }
 
      latexresultsTot << binName << " & " 
                    << std::fixed << std::setprecision(3) << 
          measurementsNew(u*nChannels) << " & " << 
          sqrt(statUncertaintyNew[u*nChannels]) << " & " << 
          sqrt(sysUncertaintyNew[u*nChannels]) << " & " <<
          sqrt(newError(u*nChannels,u*nChannels)) << " \\\\ " << endl;
    }
    latexresultsTot << "\\hline " << endl;
    latexresultsTot << "\\end{tabular} " << endl;

    //choose cutValue
    string cutValue="";
    double cutValuedbl=0;
    int prec=4;
    if (tagPoint=="MV2c2090")
      {
	cutValue="";
      }
    if (tagPoint=="MV2c2085")
      {
	cutValue="FixedCutBeff_85";
	cutValuedbl=-0.7887;
      }
    if (tagPoint=="MV2c2080")
      {
	cutValue="";
      }
    if (tagPoint=="MV2c2077")
      {
        cutValue="FixedCutBeff_77";
	cutValuedbl=-0.4434;
      }
    if (tagPoint=="MV2c2070_tracks")
      {
	cutValue="FixedCutBeff_70";
      }
    if (tagPoint=="MV2c2070")
      {
	cutValue="FixedCutBeff_70";
	cutValuedbl=-0.0436;
      }
    if (tagPoint=="MV2c2060")
      {
	cutValue="FixedCutBeff_60";
	cutValuedbl=0.4496;
      }
    if (tagPoint=="MV2c2050")
      {
	cutValue="FixedCutBeff_50";
      }
    if (tagPoint=="MV2c2030")
      {
	cutValue="FixedCutBeff_30";
      }

    //    cutValue = (cutValue + 1) /2;

    //fileName

    if (!oneChannel)
    {
      cronology << std::fixed << std::setprecision(prec) << "Analysis(PDF_6bins_2and3j,bottom,MV2c20," << cutValue << ",AntiKt4EMTopoJets)" << endl;
      //      cronology << std::fixed << std::setprecision(prec) << "      meta_data_s (Hadronization, PythiaEvtGen)" << endl;
    }
    else 
    {
      cronology << std::fixed << std::setprecision(prec) << "Analysis(PDF_6bins_";
//       if (nBins!=11)
//       {
//         cronology << nBins << "bins_";
//       }
      if (whichChannel<=1)
      {
        cronology << "emu_";
      }
      else
      {
        cronology << "ll_";
      }
      if (whichChannel%2==0)
      {
        cronology<<"2j,bottom,MV2c20," << cutValue << ",AntiKt4EMTopoJets)" << endl;
      }
      else
      {
        cronology<<"3j,bottom,MV2c20," << cutValue << ",AntiKt4EMTopoJets)" << endl;
      }
    }

    cronology << "{" << endl;
    cronology << std::fixed << std::setprecision(prec) << "      meta_data_s (Hadronization, PythiaEvtGen)" << endl;
    cronology << "      meta_data_s (OperatingPoint," << cutValuedbl  << ")" << endl;

    //replace last with 300GeV
    //    limitsPtEta1d[10]=300e3;

    for (unsigned int i=0;i<nBins-1;i++)
    {
      cronology << endl;

      if (useGeneralBinning)
      {
        cronology << std::fixed << std::setprecision(6) << "      bin(" << (int)(limitsPtDo[i]+0.5) << 
            "<pt<" << (int)(limitsPtUp[i]+0.5) << std::setprecision(1) << ","<<limitsEtaDo[i]<<"<abseta<"<<limitsEtaUp[i]<<")" << endl;
      }
      else if (use6PTbins || use10PTbins)
      {
        cronology << std::fixed << std::setprecision(6) << 
            "      bin(" << (int)(limitsPtEta1d[i]/1e3+0.5) << 
            "<pt<" << (int)(limitsPtEta1d[i+1]/1e3+0.5) << ",0<abseta<2.5)" << endl;
      }
      else
      {
        cronology << std::fixed << std::setprecision(6) << 
            "      bin(0<pt<400),"<<std::setprecision(1) << 
            limitsPtEta1d[i]/1e3<<"<abseta<"<<limitsPtEta1d[i+1]/1e3<<")" << endl;
      }

      cronology << std::setprecision(6) << "      {" << endl;
      
      cronology << "         central_value(" << measurementsNew(i*nChannels) << "," << sqrt(statUncertaintyNew[i*nChannels]) << ")" << endl;
      
      for (unsigned int u=0;u<names.size();u++)
      {
        if (!names.at(u).Contains("statunc"))
        {
          double variation=(*mixedCovNew.at(u))[i*nChannels];
///XXX Changes here
          if (fabs(variation*100)>0)
          {
            TString nameToUse=names.at(u);
            nameToUse.ReplaceAll("Up","");
            if (oneChannel) nameToUse.ReplaceAll("_fullyCorrelated","");
            if (oneChannel) nameToUse.ReplaceAll("PowhegPythiaFS","FullsimVsFastsim");
            if (oneChannel) nameToUse.ReplaceAll("PowhegHerwig","HerwigVsPythia");
            if (oneChannel) nameToUse.ReplaceAll("TopAcerMCMorePS","ISRFSR");
            if (oneChannel) nameToUse.ReplaceAll("TopAlpgen","Alpgen");
            if (oneChannel) nameToUse.ReplaceAll("StopAcerMCMorePS","ISRFSRWt");
            if (oneChannel) nameToUse.ReplaceAll("ZAlpgen","AlpgenZ");
            if (oneChannel) nameToUse.ReplaceAll("DibAlpgen","HerwigVsAlpgenDib");
            if (oneChannel) nameToUse.ReplaceAll("ReweightedTopPt","TopPtReweighting");
            if (oneChannel) nameToUse.ReplaceAll("JetEResol","jet energy resolution");
            if (oneChannel) nameToUse.ReplaceAll("JetEResol","jet energy resolution");
	    nameToUse.ReplaceAll("FlavourTagging_","");
	    nameToUse.ReplaceAll("__1up","");        

            cronology << "         sys(FT_EFF_" << nameToUse << "," << variation*100 << "%)" << endl;
          }
        }//end if
      }//end systematics
      cronology << "      }" << endl;
    }//end pt bins

    cronology << "}" << endl;

    cronology.close();
   
    cout << " CORR ";

    TMatrixDSym corrNew(nBins*nChannels);

    for (unsigned int i=0;i<nBins;i++)
    {
      cout << " ["<<i<<"] ";
    }
    cout << endl;
    for (unsigned int i=0;i<nBins;i++)
    {
      cout << "["<<i<<"] ";
      for (unsigned int j=0;j<nBins;j++)
      {
        corrNew(i,j)=newError(i*nChannels,j*nChannels)/sqrt(newError(i*nChannels,i*nChannels)*newError(j*nChannels,j*nChannels));
        cout << std::fixed << std::setprecision(2) << corrNew(i,j) << " ";
      }
      cout << endl;
    }
    cout << endl;

    cout << endl;

    for (unsigned int i=0;i<nBins*nChannels;i++)
    {

      std::cout << std::fixed << std::setprecision(3) <<  measurementsNew(i) << 
          " +/- " << sqrt(statUncertaintyNew[i]) <<
          " +/- " << sqrt(newError(i,i)-statUncertaintyNew[i]) <<
          " [== " << sqrt(sysUncertaintyNew[i]) << " ] " << 
          " ( " << sqrt(newError(i,i)) << " ) " << endl;
    }


    cout << " CORR complete " << endl;

    for (unsigned int i=0;i<nBins*nChannels;i++)
    {
      cout <<  std::fixed << std::setprecision(2) <<  " ["<<i<<"] ";
    }
    cout << endl;
    for (unsigned int i=0;i<nBins*nChannels;i++)
    {
      cout << "["<<i<<"] ";
      for (unsigned int j=0;j<nBins*nChannels;j++)
      {
        corrNew(i,j)=newError(i,j)/sqrt(newError(i,i)*newError(j,j));
        cout << std::fixed << std::setprecision(2) << corrNew(i,j) << " ";
      }
      cout << endl;
    }
    cout << endl;


   
    double statUncPlot[200];
    
    for (unsigned int i=0;i<nBins;i++)
    {
      statUncPlot[i]= sqrt(statUncertaintyNew[i*nChannels]);
    }


  TCanvas* c3=new TCanvas("c3","c3",400,400);
  TGraphErrors* combinedResultStat=new TGraphErrors(nBinsPlot,x,SFcomb,x_err,statUncPlot);
  combinedResult->GetYaxis()->SetRangeUser(y_min,y_max);
  if (runOnMC)   combinedResultStat->GetYaxis()->SetRangeUser(0.95,1.05);

  if (use6PTbins||use10PTbins)
  {
    combinedResult->GetXaxis()->SetTitle("Jet p_{T} [GeV]");
  }
  else if (!useGeneralBinning)
  {
    combinedResult->GetXaxis()->SetTitle("#eta");
  }


  combinedResult->GetYaxis()->SetTitle("Data / MC");
  combinedResult->SetFillColor(kGreen);
//  combinedResult->SetFillStyle(10);
//  combinedResult->Draw("ae2");
  if (!runOnMC)
  {
    combinedResult->Draw("ae2");
    combinedResultStat->Draw("p");
  }
  else
  {
    combinedResultStat->Draw("ap");
  }
  
  if (runOnMC)
  {
    combinedResultStat->Fit("pol0");
    
    gStyle->SetOptFit(1111);
  }

  TLine b(20e3,1,300e3,1);
  b.SetLineStyle(2);
  b.SetLineColor(2);
  b.Draw();

  if (!runOnMC) ATLASLumi(xpos,yposlumi,  sample_lumi, sample_energy);
  if (!runOnMC)
  {
    ATLASLabel(xpos,yposlabel,true);
  }
  else
  {
    ATLASLabel(xpos-0.25,yposlabel,true);
  }
  TLatex *CP2 = new TLatex();
  CP2->SetNDC(kTRUE);
  //    CP1->SetTextAlign(31);
  CP2->SetTextFont(42);
  CP2->SetTextSize(0.038);
  CP2->DrawLatex(0.7,0.21,"MV2c20, #epsilon_{b}="+WPcut+"%");

  TString name2("combFW_");
  name2+=tagPoint;
  name2+="combinedSF.eps";

//  if (channels.size()==2 || runOnMC)
  {
    if (runOnMC) name2.ReplaceAll(".eps","_MCclosure.eps");
    c3->SaveAs(name2);
    name2.ReplaceAll(".eps",".png");
    c3->SaveAs(name2);
  }

//  TMatrixTSym<double> matrix1
//  TMatrixTSym<double> matrix2 = matrix1.Invert();


  std::cout << " C " << std::endl;

  double statUncPlotReduced[10][20];//nPlot,nChannel,nBin

  if (useGeneralBinning)
  {
    
  for (unsigned r=0;r<nPlots;r++)
  {
    for (unsigned o=0;o<npTOnlyBins;o++)
    {
      statUncPlotReduced[r][o]=statUncPlot[binsPlot.at(r).at(o)];
    }
  }

//begin eta dep
  
  for (unsigned r=0;r<nPlots;r++)
  {

    TString nameCanvas("c4_EtaBin");
    nameCanvas+=r;

    TCanvas* newCanvas=new TCanvas(nameCanvas,nameCanvas,400,400);

    TGraphErrors* combinedResultStat=new TGraphErrors(npTOnlyBins,xreduced[r][0],SFcomb_reduced[r],xreduced_err[r][0],statUncPlotReduced[r]);
    combinedResult_multi.at(r)->GetYaxis()->SetRangeUser(0.2,1);
    if (runOnMC)   combinedResultStat->GetYaxis()->SetRangeUser(0.95,1.05);
//    combinedResult_multi.at(r)->GetXaxis()->SetTitle("");
    combinedResult_multi.at(r)->GetXaxis()->SetTitle("Jet p_{T} [GeV]");
    combinedResult_multi.at(r)->SetFillColor(kGreen);
    combinedResult_multi.at(r)->SetLineColor(1);
//    combinedResult_multi.at(r)->SetFillStyle(1);
//  combinedResult_multi.at(r)->Draw("ae2");
    if (!runOnMC)
    {
      combinedResult_multi.at(r)->Draw("ae2");
      combinedResultStat->Draw("p");
    }
    else
    {
      combinedResult_multi.at(r)->Draw("ap");
    }
    
    if (runOnMC)
    {
      combinedResultStat->Fit("pol0");
      
      gStyle->SetOptFit(1111);
    }
    
    TLine b(20e3,1,300e3,1);
    b.SetLineStyle(2);
    b.SetLineColor(2);
    b.Draw();
    
    if (!runOnMC) ATLASLumi(xpos,yposlumi,  sample_lumi, sample_energy);
    if (!runOnMC)
    {
      ATLASLabel(xpos,yposlabel,true);
    }
    else
    {
      ATLASLabel(xpos-0.25,yposlabel,true);
    }
    
    TLatex p; 
    p.SetNDC();
    p.SetTextFont(42);
//    p.SetTextColor(color);
    p.DrawLatex(0.23,0.83,descriptionEtaBinning.at(r));
    
    TString name2("combFW_");
    name2+=tagPoint;
    name2+="combinedSF_etabin";
    name2+=r;
    name2+=".eps";
    
//    if (channels.size()==2 || runOnMC)
    {
      if (runOnMC) name2.ReplaceAll(".eps","_MCclosure.eps");
      newCanvas->SaveAs(name2);
      name2.ReplaceAll(".eps",".png");
      newCanvas->SaveAs(name2);
    }
  }
  
  }
  

}//full routine for combining
