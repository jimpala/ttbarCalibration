#include "./tracks.icc"
#include "TString.h"
#include "vector"
#include <iostream>

void runGeneralFit(int channel=0,
                   bool runCal=true,
                   bool runFitData=false,
                   int runItem=-1,
                   TString tagPoint="MV2c1077",
//		   bool runFit=true) { //for MC closure test
                 bool runFit=false) { //for data SFs

  bool rescaling=true;
  
//0=emu 2 jet
//1=ll 2 jet
//2=emu 3 jet
//3=ll 3 jet

//  gSystem->Load("./jetnet/libTJetNet.so");
//  gSystem->Load("libFoam");

  gSystem->Load("libRooFit");
//  gROOT->ProcessLine(".L createDatasetGeneral.C+");
  gROOT->ProcessLine(".L createDatasetGeneral_MVA.C++");
  cout << "Hello" << endl;
//  gROOT->ProcessLine(".L DataObject.cxx+");
  gROOT->ProcessLine(".L RhhBinnedPdf.cc++");
  gROOT->ProcessLine(".L RhhBinnedPdfSymmetricMatrix.cc++");
  gROOT->ProcessLine(".L RhhBinnedPdfInCategories.cc++");
  gROOT->ProcessLine(".L RhhBinnedPdfInPtCategories.cc++");
//  gROOT->ProcessLine(".L CombinatorialTTbarPDF.cxx+");
//  gROOT->ProcessLine(".L buildUpMultiSampleModel.C+g");
  gROOT->ProcessLine(".L buildUpMultiSampleModel_MVA.C++g");
  gROOT->ProcessLine(".L doGeneralFit.C++g");



//int channel=3;


  int nCombinationsToUse=2;
  if (channel>=2)
  {
    nCombinationsToUse=6;
  }
  if (channel>=4)
  {
    nCombinationsToUse=1;
  }

  vector<TString> topSystematics;
  topSystematics.push_back("MCAtNLO");
  topSystematics.push_back("PowhegHerwig");
  //  topSystematics.push_back("PowhegPythia8"); 
  topSystematics.push_back("ttbarRadHi");
  topSystematics.push_back("ttbarRadLo");
//  topSystematics.push_back("ttbarPDFRW");
//  topSystematics.push_back("ttbar_FastSim");

  vector<TString> singletopSystematics;

  singletopSystematics.push_back("stop_Radhi");
  singletopSystematics.push_back("stop_RadLo");
  singletopSystematics.push_back("stop_Herwig");

  vector<TString> dibosonSystematics;
  dibosonSystematics.push_back("Powheg_Diboson");


  vector<TString> zSystematics;
  if(tracks) zSystematics.push_back("SherpaZ");  
  else zSystematics.push_back("PowhegPythia8Z");
//  zSystematics.push_back("MadGraphZ");

  vector<TString> mcsystematics;

  mcsystematics.push_back("FlavourTagging_JET_RelativeNonClosure_MC15__1up");
  mcsystematics.push_back("FlavourTagging_JET_RelativeNonClosure_MC15__1down");
  mcsystematics.push_back("FlavourTagging_JET_BJES_Response__1up");
  mcsystematics.push_back("FlavourTagging_JET_BJES_Response__1down");
  mcsystematics.push_back("FlavourTagging_JET_EffectiveNP_1__1up");
  mcsystematics.push_back("FlavourTagging_JET_EffectiveNP_1__1down");
  mcsystematics.push_back("FlavourTagging_JET_EffectiveNP_2__1up");
  mcsystematics.push_back("FlavourTagging_JET_EffectiveNP_2__1down");
  mcsystematics.push_back("FlavourTagging_JET_EffectiveNP_3__1up");
  mcsystematics.push_back("FlavourTagging_JET_EffectiveNP_3__1down");
  mcsystematics.push_back("FlavourTagging_JET_EffectiveNP_4__1up");
  mcsystematics.push_back("FlavourTagging_JET_EffectiveNP_4__1down");
  mcsystematics.push_back("FlavourTagging_JET_EffectiveNP_5__1up");
  mcsystematics.push_back("FlavourTagging_JET_EffectiveNP_5__1down");
  mcsystematics.push_back("FlavourTagging_JET_EffectiveNP_6restTerm__1up");
  mcsystematics.push_back("FlavourTagging_JET_EffectiveNP_6restTerm__1down");
  mcsystematics.push_back("FlavourTagging_JET_EtaIntercalibration_Modelling__1up");
  mcsystematics.push_back("FlavourTagging_JET_EtaIntercalibration_Modelling__1down");
  mcsystematics.push_back("FlavourTagging_JET_EtaIntercalibration_TotalStat__1up");
  mcsystematics.push_back("FlavourTagging_JET_EtaIntercalibration_TotalStat__1down");
  mcsystematics.push_back("FlavourTagging_JET_Flavor_Composition__1up");
  mcsystematics.push_back("FlavourTagging_JET_Flavor_Composition__1down");
  mcsystematics.push_back("FlavourTagging_JET_Flavor_Response__1up");
  mcsystematics.push_back("FlavourTagging_JET_Flavor_Response__1down");
//  mcsystematics.push_back("FlavourTagging_JET_GroupedNP_1__1up");
//  mcsystematics.push_back("FlavourTagging_JET_GroupedNP_1__1down");
  mcsystematics.push_back("FlavourTagging_JET_Pileup_OffsetMu__1up");
  mcsystematics.push_back("FlavourTagging_JET_Pileup_OffsetMu__1down");
  mcsystematics.push_back("FlavourTagging_JET_Pileup_OffsetNPV__1up");
  mcsystematics.push_back("FlavourTagging_JET_Pileup_OffsetNPV__1down");
  mcsystematics.push_back("FlavourTagging_JET_Pileup_PtTerm__1up");
  mcsystematics.push_back("FlavourTagging_JET_Pileup_PtTerm__1down");
  mcsystematics.push_back("FlavourTagging_JET_Pileup_RhoTopology__1up");
  mcsystematics.push_back("FlavourTagging_JET_Pileup_RhoTopology__1down");
  mcsystematics.push_back("FlavourTagging_JET_PunchThrough_MC15__1up");
  mcsystematics.push_back("FlavourTagging_JET_PunchThrough_MC15__1down");
  mcsystematics.push_back("FlavourTagging_JET_SingleParticle_HighPt__1up");
  mcsystematics.push_back("FlavourTagging_JET_SingleParticle_HighPt__1down");
  mcsystematics.push_back("FlavourTagging_JET_JER_SINGLE_NP__1up");
  mcsystematics.push_back("FlavourTagging_MUONS_SCALE__1up");
  mcsystematics.push_back("FlavourTagging_MUONS_SCALE__1down");
  mcsystematics.push_back("FlavourTagging_MUONS_ID__1up");
  mcsystematics.push_back("FlavourTagging_MUONS_ID__1down");
  mcsystematics.push_back("FlavourTagging_MUONS_MS__1up");
  mcsystematics.push_back("FlavourTagging_MUONS_MS__1down");
  mcsystematics.push_back("FlavourTagging_EG_RESOLUTION_ALL__1up");
  mcsystematics.push_back("FlavourTagging_EG_RESOLUTION_ALL__1down");
  mcsystematics.push_back("FlavourTagging_EG_SCALE_ALL__1up");
  mcsystematics.push_back("FlavourTagging_EG_SCALE_ALL__1down");
  mcsystematics.push_back("FlavourTagging_MUON_EFF_STAT__1up");
  mcsystematics.push_back("FlavourTagging_MUON_EFF_STAT__1down");
  mcsystematics.push_back("FlavourTagging_MUON_EFF_SYS__1up");
  mcsystematics.push_back("FlavourTagging_MUON_EFF_SYS__1down");
  mcsystematics.push_back("FlavourTagging_MUON_ISO_STAT__1up");
  mcsystematics.push_back("FlavourTagging_MUON_ISO_STAT__1down");
  mcsystematics.push_back("FlavourTagging_MUON_ISO_SYS__1up");
  mcsystematics.push_back("FlavourTagging_MUON_ISO_SYS__1down");
  mcsystematics.push_back("FlavourTagging_EL_EFF_ID_TotalCorrUncertainty__1up");
  mcsystematics.push_back("FlavourTagging_EL_EFF_ID_TotalCorrUncertainty__1down");
  mcsystematics.push_back("FlavourTagging_EL_EFF_Trigger_TotalCorrUncertainty__1up");
  mcsystematics.push_back("FlavourTagging_EL_EFF_Trigger_TotalCorrUncertainty__1down");
  mcsystematics.push_back("FlavourTagging_EL_EFF_Reco_TotalCorrUncertainty__1up");
  mcsystematics.push_back("FlavourTagging_EL_EFF_Reco_TotalCorrUncertainty__1down");
  mcsystematics.push_back("FlavourTagging_EL_EFF_Iso_TotalCorrUncertainty__1up");
  mcsystematics.push_back("FlavourTagging_EL_EFF_Iso_TotalCorrUncertainty__1down");
  mcsystematics.push_back("FlavourTagging_MUON_TTVA_STAT__1up");
  mcsystematics.push_back("FlavourTagging_MUON_TTVA_STAT__1down");
  mcsystematics.push_back("FlavourTagging_MUON_TTVA_SYS__1up");
  mcsystematics.push_back("FlavourTagging_MUON_TTVA_SYS__1down");

//Muon trigger uncertainties

  mcsystematics.push_back("MUON_EFF_TrigSystUncertainty_1up");
  mcsystematics.push_back("MUON_EFF_TrigSystUncertainty_1down");
  mcsystematics.push_back("MUON_EFF_TrigStatUncertainty_1up");
  mcsystematics.push_back("MUON_EFF_TrigStatUncertainty_1down");

// PU systematics                                                                                                                                                                                                                     
  mcsystematics.push_back("PRW_DATASF_1up");
  mcsystematics.push_back("PRW_DATASF_1down");

  vector<TString> systematics;

  for (unsigned int r=0;r<topSystematics.size();r++)
  {
    systematics.push_back(topSystematics.at(r));
  }
  for (unsigned int r=0;r<dibosonSystematics.size();r++)
  {
    systematics.push_back(dibosonSystematics.at(r));
  }
  for (unsigned int r=0;r<singletopSystematics.size();r++)
  {
    systematics.push_back(singletopSystematics.at(r));
  }
  for (unsigned int r=0;r<zSystematics.size();r++)
  {
    systematics.push_back(zSystematics.at(r));
  }

//  systematics.push_back("high_pt_cut");

  systematics.push_back("ttbarNNLO");
  systematics.push_back("fakesUp");
  systematics.push_back("fakesDo");
  systematics.push_back("ZHFUp");
  systematics.push_back("ZHFDo");
  systematics.push_back("stopUp");
  systematics.push_back("stopDo");
  systematics.push_back("dibUp");
  systematics.push_back("dibDo");
  systematics.push_back("ZUp");
  systematics.push_back("ZDo");


  systematics.push_back("LumiUp");
  systematics.push_back("LumiDo");

  // Not there before
  systematics.push_back("MistagUp");//to be added directly in the code
  systematics.push_back("MistagDo");
  systematics.push_back("MistagCUp");//to be added directly in the code
  systematics.push_back("MistagCDo");

  systematics.push_back("ttbarRW");
  systematics.push_back("ZPTRW");

  systematics.push_back("NominalBTagSF");

  for (unsigned int r=0;r<mcsystematics.size();r++)
  {
    systematics.push_back(mcsystematics.at(r));
  }

  for (int i=0;i<systematics.size();i++)
  {
    cout << " N. " << i << " " << systematics.at(i) << endl;
  }

  TString name("fileRoofitGeneral");
  name+=channel;
  name+=".root";
  //GP NOT USED ANYMORE --> FAKE ARGUMENT

  if (runCal)
  {

    if (runItem<=0)
    {

      createDatasetGeneral(tagPoint,channel,1,false,false,false,"",0);
      doGeneralFit(tagPoint,channel,1,"MCNominal",name,0);
      
      createDatasetGeneral(tagPoint,channel,1,false,false,false,"",1);
      doGeneralFit(tagPoint,channel,1,"MCNominal",name,1);
      
      if (channel<4)
      {
        createDatasetGeneral(tagPoint,channel,1,false,false,false,"",2);
        doGeneralFit(tagPoint,channel,1,"MCNominal",name,2);
      }
      
      if (channel>=2 && channel<4)
      {
        createDatasetGeneral(tagPoint,channel,1,false,false,false,"",3);
        doGeneralFit(tagPoint,channel,1,"MCNominal",name,3);
        
      }
    }

    int size=systematics.size();
    
    for (int i=0;i<size;i++)
    {

      if (!(runItem==-1 || i==runItem-1)) continue;

//      if (systematics.at(i).Contains("Mistag")&&!tracks) continue;
      if (systematics.at(i).Contains("ttBB")) continue;

      createDatasetGeneral(tagPoint,channel,1,false,false,false,systematics.at(i),0);
      doGeneralFit(tagPoint,channel,1,systematics.at(i),name,0,systematics.at(i));
      createDatasetGeneral(tagPoint,channel,1,false,false,false,systematics.at(i),1);
      doGeneralFit(tagPoint,channel,1,systematics.at(i),name,1,systematics.at(i));
      if (channel<4)
      {
        createDatasetGeneral(tagPoint,channel,1,false,false,false,systematics.at(i),2);
        doGeneralFit(tagPoint,channel,1,systematics.at(i),name,2,systematics.at(i));
      }
      
      if (channel>=2 && channel<4)
      {
        createDatasetGeneral(tagPoint,channel,1,false,false,false,systematics.at(i),3);
        doGeneralFit(tagPoint,channel,1,systematics.at(i),name,3,systematics.at(i));
      }
    }
  }

  if (runFit)
  {


    createDatasetGeneral(tagPoint,channel,nCombinationsToUse,false,false,false,"",-1);
    doGeneralFit(tagPoint,channel,nCombinationsToUse,"MCNominal",name,-1);

    int size=systematics.size();
    for (int i=0;i<size;i++)
    {

      if (!(runItem==-1 || i==runItem-1)) continue;
//      if (systematics.at(i).Contains("Mistag")&&!tracks) continue;
      if (systematics.at(i).Contains("ttBB")) continue;


      doGeneralFit(tagPoint,channel,nCombinationsToUse,"MCNominal",name,-1,systematics.at(i));
    }

  }
  if (runFitData)
  {


    if (runItem<=0)
    {

      createDatasetGeneral(tagPoint,channel,nCombinationsToUse,false,false,true,"");
      doGeneralFit(tagPoint,channel,nCombinationsToUse,"DataNominal",name);
      
    }

    int size=systematics.size();
    for (int i=0;i<size;i++)
    {
      if (!(runItem==-1 || i==runItem-1)) continue;
      createDatasetGeneral(tagPoint,channel,nCombinationsToUse,false,false,true,systematics.at(i));
      doGeneralFit(tagPoint,channel,nCombinationsToUse,"DataNominal",name,-1,systematics.at(i));
    }


  }
  
  
}

