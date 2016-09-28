#include "RooRealVar.h"
#include "RooArgList.h"
#include "TMatrixTSym.h"
#include "TFile.h"
#include "RooDataSet.h"
#include "TCanvas.h"
#include "RooPlot.h"
#include "RooArgSet.h"
#include "TH1F.h"
#include "TFile.h"
#include "TRandom.h"
#include "RooGlobalFunc.h"
#include "TF1.h"
#include "RooFitResult.h"
#include "RhhBinnedPdf.h"
#include "RhhBinnedPdfSymmetricMatrix.h"
#include "RhhBinnedPdfInCategories.h"
#include "RhhBinnedPdfInPtCategories.h"
#include "RooProdPdf.h"
#include "RooCategory.h"
#include "RooSimultaneous.h"
#include "RooAddPdf.h"
#include "TH2F.h"
#include "RooDataHist.h"
#include "RooHistPdf.h"
#include "TTree.h"
#include "TRandom.h"
#include "buildUpMultiSampleModel.h"
#include "RooFormulaVar.h"
#include "RhhBinnedPdfSymmetricMatrix.h"
#include <TMVA/Reader.h>

#include "getCategory.icc"


float get_sub_sub(vector<float>* pT){

  float pt_0 = pT->at(0);
  float pt_1 = pT->at(1);
  float pt_2 = pT->at(2);

  float ptmin = (pt_0 < pt_1) ? pt_0 : pt_1;
  ptmin = (ptmin < pt_2) ? ptmin : pt_2;

//  std::cout << "pt_0: " << pt_0 << " pt_1: " << pt_1 << " pt_2: " << pt_2 << " ptmin: " << ptmin << std::endl;

  return (ptmin / 1e6);

}


RooAbsPdf* buildUpMultiSampleModel(TString tagPoint,int channel,int nCombinationsModel,bool addWeight,int optionSample,TString systematicsToLoad)
{

#include "rescaling.icc"
#include "nbins.icc"

  TMVA::Reader* reader = new TMVA::Reader();
  float imbalance, leading_pt, delta_phi, train_met, mlj, sub_leading_pt, sub_sub_leading_pt, delta_r, minl1j, minl2j;
  double weightSingle=0;
//  reader->AddSpectator("weightSingle", &weightSingle);

  reader->AddVariable("imbalance", &imbalance);
  reader->AddVariable("leading_pt", &leading_pt);
  reader->AddVariable("sub_leading_pt", &sub_leading_pt);
  if(channel>1)  reader->AddVariable("sub_sub_leading_pt", &sub_sub_leading_pt);
  reader->AddVariable("delta_phi", &delta_phi);
  reader->AddVariable("delta_r", &delta_r);
  reader->AddVariable("mlj", &mlj);
//  reader->AddVariable("minl1j", &minl1j);
//  reader->AddVariable("minl2j", &minl2j);
//  reader->AddVariable("train_met", &train_met);                                                                                                                                                       
  std::string dir    = "/unix/atlasvhbb/abell/mc15data7/scripts/tmvaExamples/train/weights/";
  std::string prefix = "tmvaTest";
  if(channel<=1){
    prefix+="_2jet";
  }
  else{
    prefix+="_3jet";
  }

  reader->BookMVA("BDT_even", dir + prefix + "_even_BDT_Advanced.weights.xml"); //Trained on even events -> apply to odd!!!
  reader->BookMVA("BDT_lowpt_even", dir + prefix + "_lowpt_even_BDT_Advanced.weights.xml"); //Trained on even events -> apply to odd!!!
  reader->BookMVA("BDT_odd", dir + prefix + "_odd_BDT_Advanced.weights.xml"); //Trained on off events -> apply to even!!!
  reader->BookMVA("BDT_lowpt_odd", dir + prefix + "_lowpt_odd_BDT_Advanced.weights.xml"); //Trained on odd events -> apply to even!!!

/*

  reader->BookMVA("BDT_even", dir + prefix + "_even_BDT_Overtrain_reference.weights.xml"); //Trained on even events -> apply to odd!!!
  reader->BookMVA("BDT_lowpt_even", dir + prefix + "_lowpt_even_BDT_Overtrain_reference.weights.xml"); //Trained on even events -> apply to odd!!!
  reader->BookMVA("BDT_odd", dir + prefix + "_odd_BDT_Overtrain_reference.weights.xml"); //Trained on off events -> apply to even!!!
  reader->BookMVA("BDT_lowpt_odd", dir + prefix + "_lowpt_odd_BDT_Overtrain_reference.weights.xml"); //Trained on odd events -> apply to even!!!   
*/
  bool optimisation=true;

  //INCLUDE FIX COMBINATIONS
#include "/unix/atlasvhbb/abell/mc15data7/ntupProduction/fixCombinations.icc"
#include "/unix/atlasvhbb/abell/mc15data7/ntupProduction/fixCombinationsThree.icc"

  bool tracks = (tagPoint.Index("tracks")!=-1);
  double ptmin=20e3;
  if(tracks) ptmin=10e3;
//  bool SFRecalculate=tracks;
  bool SFRecalculate=true;

#include "btagcut.icc"

//  float bdt_cut = -0.;
//  if(channel>1) bdt_cut = -0.;

  vector<vector<int> >* combinationsOfm4lnuVectorToUse=&combinationsOfm4lnuVector;
  if (channel>=2)
  {
    combinationsOfm4lnuVectorToUse=&tree_combinationsOfm4lnuVector;
  }

  vector<vector<int> > four_combinationsOfm4lnuVector;
  vector<int> temp;
  temp.push_back(0);
  four_combinationsOfm4lnuVector.push_back(temp);
  
  if (channel>=4)
  {
    combinationsOfm4lnuVectorToUse=&four_combinationsOfm4lnuVector;
  }


  int nCombinationsToUse=nCombinations;
  int nJets=2;
  if (channel>=2)
  {
    nCombinationsToUse=tree_nCombinations;
    nJets=3;
  }
  if (channel>=4)
  {
    nJets=1;
  }
  
  
  bool readHistosFromFile=true;
//  bool readHistosFromFileBWeight=true;

  bool singleVariables=false;
  if (channel>=2) singleVariables=true;

  int numPtEtaBins=nbins;
  int numPtBins=nbins;
  bool useEta=true;
//false;
//  if (nbins==14 || nbins==10 || nbins==5 || nbins==3 || nbins==15) useEta=true;

  bool splitIntoCombinations=false;
 
  std::vector<std::vector<int> > sample_combinations;

  std::vector<int> combAll;
  for (int u=0;u<nCombinationsToUse;u++)
  {
    combAll.push_back(u);
  }

  int nIs=3;
  if (channel>=2) nIs=4;
  if (channel>=4) nIs=2;
  

  for (int u=0;u<nIs;u++)
  {
    sample_combinations.push_back(combAll);
  }

  std::vector<TString> samples;
  if (channel<=1)
  {
    if (optionSample==-1)
    {
      samples.push_back("all_55");
      samples.push_back("all_51");
      samples.push_back("all_11");
    }
    else if (optionSample==0)
    {
      samples.push_back("all_55");
    }
    else if (optionSample==1)
    {
      samples.push_back("all_51");
    }
    else if (optionSample==2)
    {
      samples.push_back("all_11");
    }
  }
  else if (channel<=3)
  {
    if (optionSample==-1)
    {
      samples.push_back("all_555");
      samples.push_back("all_551");
      samples.push_back("all_511");
      samples.push_back("all_111");
    }
    else if (optionSample==0)
    {
      samples.push_back("all_555");
    }
    else if (optionSample==1)
    {
      samples.push_back("all_551");
    }
    else if (optionSample==2)
    {
      samples.push_back("all_511");
    }
    else if (optionSample==3)
    {
      samples.push_back("all_111");
    }
  }
  else
  {
    if (optionSample==-1)
    {
      samples.push_back("all_5");
      samples.push_back("all_1");
    }
    else if (optionSample==0)
    {
      samples.push_back("all_5");
    }
    else if (optionSample==1)
    {
      samples.push_back("all_1");
    }
  }

  std::vector<std::vector<TString> > samplesToCreatePDFsfrom;
  if (channel<=3)
  {
    for (unsigned int q=0;q<samples.size();q++)
    {
      std::vector<TString> samplesTemp;
      samplesTemp.push_back(samples.at(q));
      samplesToCreatePDFsfrom.push_back(samplesTemp);
    }
  }
  else
  {
    if (optionSample==-1)
    {
      std::vector<TString> samplesTemp;
      samplesTemp.push_back("all_55");
      samplesTemp.push_back("all_51");
      samplesToCreatePDFsfrom.push_back(samplesTemp);
      samplesTemp.clear();
      samplesTemp.push_back("all_51");
      samplesTemp.push_back("all_11");
      samplesToCreatePDFsfrom.push_back(samplesTemp);
    }
    else if (optionSample==0)
    {
      std::vector<TString> samplesTemp;
      samplesTemp.push_back("all_55");
      samplesTemp.push_back("all_51");
      samplesToCreatePDFsfrom.push_back(samplesTemp);
    }
    else if (optionSample==1)
    {
      std::vector<TString> samplesTemp;
      samplesTemp.push_back("all_51");
      samplesTemp.push_back("all_11");
      samplesToCreatePDFsfrom.push_back(samplesTemp);
    }
  }

  RooRealVar* pTcategory=new RooRealVar("ptcategory","ptcategory",5,-0.5,(double)(nbins*nbins)-0.5);
  RooRealVar* pTcategory2=new RooRealVar("ptcategory2","ptcategory2",5,-0.5,(double)(nbins*nbins)-0.5);

  RooRealVar* pTcategoryJet0=new RooRealVar("ptcategoryJet0","ptcategoryJet0",5,-0.5,(double)(nbins)-0.5);
  RooRealVar* pTcategoryJet1=new RooRealVar("ptcategoryJet1","ptcategoryJet1",5,-0.5,(double)(nbins)-0.5);
  RooRealVar* pTcategoryJet2=new RooRealVar("ptcategoryJet2","ptcategoryJet2",5,-0.5,(double)(nbins)-0.5);
  
  RooArgSet* newSet=new RooArgSet();
  newSet->addOwned(*pTcategory);
  newSet->addOwned(*pTcategory2);

  newSet->addOwned(*pTcategoryJet0);
  newSet->addOwned(*pTcategoryJet1);
  newSet->addOwned(*pTcategoryJet2);

  vector<RooRealVar*> bweightVars;
  vector<RooRealVar*> ptVars;
  vector<RooRealVar*> ptCopyVars;
  vector<RooRealVar*> ptcategoryJetVars;
  vector<RooRealVar*> etaVars;

  ptcategoryJetVars.push_back(pTcategoryJet0);
  ptcategoryJetVars.push_back(pTcategoryJet1);
  if (nJets>2)
  {
    ptcategoryJetVars.push_back(pTcategoryJet2);
  }

  for (int i=0;i<nJets;i++)
  {
    TString name="bweight_";
    name+=i;
    
    RooRealVar* weight=new RooRealVar(name,name,0.2,-1.,1.); //////////CHANGES
    bweightVars.push_back(weight);
    newSet->addOwned(*weight);

    name="pt_";
    name+=i;
    
    RooRealVar* pt=new RooRealVar(name,name,30e3,ptmin,400e3);
    newSet->addOwned(*pt);
    ptVars.push_back(pt);

    name="ptcopy_";
    name+=i;
    
    RooRealVar* ptcopy=new RooRealVar(name,name,30e3,ptmin,400e3);
    newSet->addOwned(*ptcopy);
    ptCopyVars.push_back(ptcopy);
//    ptCopyVars.push_back(pt);

    name="eta_";
    name+=i;
    
    RooRealVar* eta=new RooRealVar(name,name,0,-2.5,2.5);
    newSet->addOwned(*eta);
    etaVars.push_back(eta);

  }

#include "btagBinning.icc"
  TArrayD limitsBWeighta(nLimitsBtag,limitsBWeight);

  int nLimitsPtEta=nbins*nbins+1;
  Double_t limitsPtEta[1000];
  for (int i=0;i<nbins*nbins+1;i++)
  {
    limitsPtEta[i]=-0.5+i;
  }
  TArrayD limitsArrayPtEta(nLimitsPtEta,limitsPtEta);


  int nLimitsPtEta1d=nbins+1;
  Double_t limitsPtEta1d[1000] = {ptmin,30e3,40e3,50e3,60e3,75e3,90e3,110e3,140e3,200e3,300e3,400e3};

  if(tracks)
  {
    Double_t limitsPtEta1d_tracks[1000] = {ptmin,20e3,30e3,50e3,60e3,80e3,100e3,140e3,200e3,250e3,300e3,400e3};
    for(int i=0;i<20;i++)
    {
      limitsPtEta1d[i]=limitsPtEta1d_tracks[i];
//    std::cout << "Filling.... " << limitsPtEta1d[i] << std::endl;
    }
  }

  if (nbins==1)
  {
    limitsPtEta1d[1]=400e3;
  }
  
  TArrayD limitsArrayPtEta1d(nLimitsPtEta1d,limitsPtEta1d);

  RooArgList*  coefficientsBWL[3];
  RooArgList*  coefficientsPTL[3][100];
  for (int j=0;j<3;j++)
  {
    coefficientsBWL[j]=0;
    for (int s=0;s<100;s++)
    {
      coefficientsPTL[j][s]=0;
    }
  }


  {
    for (int j=0;j<3;j++)
    {
      
      TString name="coefBWeight_";
      TString namePT="coefPT_";
      
      if (j==0)
      {
        name+="b_";
        namePT+="b_";
      }
      else if (j==1)
      {
        name+="c_";
        namePT+="c_";
      }
      else if (j==2)
      {
        name+="l_";
        namePT+="l_";
      }
      
      coefficientsBWL[j]=new RooArgList();
      
      for (int r=0;r<samples.size();r++)
      {
        
        coefficientsPTL[j][r]=new RooArgList();
        
        for (int s=0;s<numPtEtaBins-1;s++)
        {
          TString nameCoef=namePT;
          nameCoef+="_pTetabin";
          nameCoef+=s;
          nameCoef+="_";
          nameCoef+=samples.at(r);
          
          RooRealVar * var=new RooRealVar(nameCoef,nameCoef,0.2,0.,1); ////////CHANGES
          
          coefficientsPTL[j][r]->addOwned(*var);
        }
      }
      
      for (int s=0;s<numPtEtaBins;s++)
      {
        
        for (int i=0;i<nLimitsBtag-2;i++)
        {
          TString nameCoef=name;
          nameCoef+="_pTetabin";
          nameCoef+=s;
          nameCoef+="_weightbin";
          nameCoef+=i;
          
          RooRealVar * var=new RooRealVar(nameCoef,nameCoef,0.2,0,1);
          
          coefficientsBWL[j]->addOwned(*var);
        }
      }//end n btag weight bins
    } //end num pT eta bins
  }//end flavours

  RooArgList* coefficientsBWeff[20];
  for (int j=0;j<20;j++)
  {
    coefficientsBWeff[j]=0;
  }


  {
    for (int j=0;j<samples.size();j++)
    {
      
      TString name="coefBWeight_";
      
      name+="lctau_";
      name+=samples.at(j);
      
      coefficientsBWeff[j]=new RooArgList();

      for (int s=0;s<numPtEtaBins;s++)
      {
        
        for (int i=0;i<nLimitsBtag-2;i++)
        {
          TString nameCoef=name;
          nameCoef+="_pTetabin";
          nameCoef+=s;
          nameCoef+="_weightbin";
          nameCoef+=i;
          
          RooRealVar * var=new RooRealVar(nameCoef,nameCoef,0.2,0,1);
          
          coefficientsBWeff[j]->addOwned(*var);
        }
      }//end n btag weight bins
    } //end num pT eta bins
  }//end flavours

  //now care about pT/eta bins!!!

  RooArgList* coefficientsPTeff[1000][10];
  for (int u=0;u<10;u++)
  {
    for (int j=0;j<1000;j++)
    {
      coefficientsPTeff[j][u]=0;
    }
  }

  {
    for (int j=0;j<samples.size();j++)
    {
      
      for (int s=0;s<nJets;s++)
      {
        
	TString name="coefPT";
	name+=s;
	name+="_";
      
	coefficientsPTeff[j][s]=new RooArgList();

	int nLimitsCoef=nbins*nbins+1;
	if (samples.at(j)=="all_55" || samples.at(j)=="all_11")
	{
	  cout << " Consider symmetry for sample n. " << j << endl;
	  nLimitsCoef=nbins*(nbins+1)/2+1;
	}
	if (singleVariables)
	{
	  nLimitsCoef=nbins+1;
	}
      

	for (int i=0;i<nLimitsCoef-2;i++)
	{
	  TString nameCoef=name;
	  nameCoef+="_pTetabin";
	  nameCoef+=i;
	  nameCoef+="_";
	  nameCoef+=samples.at(j);
        
	  RooRealVar * var=new RooRealVar(nameCoef,nameCoef,0.01,0.,1);
          
	  coefficientsPTeff[j][s]->addOwned(*var);
	}
      } //end num pT eta bins
    }//end njets
  }//end flavour combinations
  
//end care of pT bins!!!
  
  cout << " G " << endl;
///XXXX finished with the coefficnets start ntuple stuff

  //now count number of events and set accordingly
  std::vector<double> neventspersample;
  RooDataHist* histoOfPTEtaJet[3][3][30];//first is var, second is real jet,third is sample
//unfortunately you need one per variable  

  std::vector<RooDataHist*> histoOfPTEta;
  std::vector<RooDataHist*> histoOfPTEta2;
//OLD  std::vector<RooDataHist*> histoOfPTEtaJet;

//  RooDataHist* histoOfBWeightPDFl[3][30];

  float etmiss=0;
  float mll=0;
  int leptontype=0;
  int ptcategory=0;
  int ptcategory0=0;
  int ptcategory1=0;
  int ptcategory2=0;
  vector<float>* pT=0;
  vector<float>* eta=0;
  vector<int>* flavours=0;
  vector<float>* bweight_MV2c10=0;

  double weight_track_70=1;
  double weight_track_77=1;

  double weight_MV2c1060=1;
  double weight_MV2c1070=1;
  double weight_MV2c1077=1;
  double weight_MV2c1085=1;
  bool isEven;

/*

  vector<double>* weight_track_70=0;
  vector<double>* weight_track_77=0;

  vector<double>* weight_MV2c1060=0;
  vector<double>* weight_MV2c1070=0;
  vector<double>* weight_MV2c1077=0;
  vector<double>* weight_MV2c1085=0;
*/

  vector<TString> sampleTypes;
  vector<int> sampleTypeStart;
  vector<int> sampleTypeEnd;
  vector<double> SampleTypeNormalization;

  int start=-1;
  int end=-1;
  TString sampleNow("");

  for (int i=0;i<samples.size();i++)
  {
    int position=samples.at(i).First("_");
    TString sample=samples.at(i).operator()(0,position);
    //initialization
    if (start==-1) start=i;
    if (end==-1) end=i;
    if (sampleNow=="") {
      sampleNow=sample;
    }
    
    //now has it changed?
    
    if (i==samples.size()-1) end=i;
    
    if (sample!=sampleNow || i==samples.size()-1)
    {
      sampleTypes.push_back(sampleNow);
      sampleTypeStart.push_back(start);
      sampleTypeEnd.push_back(end);
      start=end+1;
      end=end+1;
      sampleNow=sample;
      cout << " Adding sample: " << sample << " start " << start << " end " << end << endl;
    }
    else
    {
      end=i;
    }
  }

  for (int i=0;i<sampleTypes.size();i++)
  {
    cout << "Sample type: " << sampleTypes.at(i) << " start " << sampleTypeStart.at(i) << " end " << sampleTypeEnd.at(i) << endl;
  }
  

  for (int u=0;u<sampleTypes.size();u++)
  {
    
    TString directory;
    directory = (tracks) ? "/unix/atlasvhbb/abell/mc15data7/PGS_tracks/outputGeneral_fit_" : "/unix/atlasvhbb/abell/mc15data7/PGS/outputGeneral_fit_";
//    directory = (tracks) ? "./../../PGS_tracks/outputGeneral_fit_" : "/unix/atlas4/abell/FirstxAOD/abell/ttbarPDFCalibration/mc15data5/PGS/outputGeneral_fit_";
    directory+=sampleTypes.at(u);
    if (systematicsToLoad!="" && !systematicsToLoad.Contains("ttBB"))//for ttHF use std One
    {
      directory+="Sys";
      directory+=systematicsToLoad;
    }
    if (rescaling)
    {
      directory+="_rescaling";
    }    
    directory+=".root";

    SampleTypeNormalization.push_back(0);

    
    std::cout << " Opening: " << directory << endl;
    TFile* originalFile=TFile::Open(directory);

    for (int i=sampleTypeStart.at(u);i<sampleTypeEnd.at(u)+1;i++)
    {

      TString histoName(samples.at(i));
      histoName+="_histoPtEta";
      
      TString histoName2(samples.at(i));
      histoName2+="_histoPtEtaJet0";
      
      TString histoName3(samples.at(i));
      histoName3+="_histoPtEtaJet1";
      
      TString histoName4(samples.at(i));
      histoName4+="_histoPtEtaJet2";
      
      TH1F* histoPtEta=new TH1F(histoName,histoName,nbins*nbins,-0.5,nbins*nbins-0.5);
      TH1F* histoPtEtaJet0=new TH1F(histoName2,histoName2,nbins,-0.5,nbins-0.5);
      TH1F* histoPtEtaJet1=new TH1F(histoName3,histoName3,nbins,-0.5,nbins-0.5);
      TH1F* histoPtEtaJet2=new TH1F(histoName4,histoName4,nbins,-0.5,nbins-0.5);
      
      TString histoName5(samples.at(i));
      histoName5+="_histoLight";
      
      int nBinsBTag=nLimitsBtag-1;
//      TH1F* histoBWeightLight=new TH1F(histoName5,histoName5,nbinsBTag,-0.5,nbinsNTag-0.5);

      double weightTotal=0;

      for (int s=0;s<samplesToCreatePDFsfrom.at(i).size();s++)
      {

        int position=samplesToCreatePDFsfrom.at(i).at(s).First("_");
        TString sample=samplesToCreatePDFsfrom.at(i).at(s).operator()(0,position);
        TString type=samplesToCreatePDFsfrom.at(i).at(s).operator()(position+1,100);
        cout << " sample is: " << sample << " type is: " << type << endl;
        
        TString treeName="tree_";
        treeName+=type;
        
        cout << " Running over tree " << treeName  << endl;

        weightSingle=0;
        etmiss=0;
        mll=0;
/*
  weight_track_70->clear();
  weight_track_77->clear();

  weight_MV2c1060->clear();
  weight_MV2c1070->clear();
  weight_MV2c1077->clear();
  weight_MV2c1085->clear();
*/

/*
  weight_MV2c1060=1;
  weight_MV2c1070=1;
  weight_MV2c1077=1;
  weight_MV2c1085=1;
*/

	leading_pt=0;
	sub_leading_pt=0;
	sub_sub_leading_pt=0;
	imbalance=0;
	delta_phi=0;
	delta_r=0;
	mlj=0;
	minl1j=0;
	minl2j=0;
//	isEven=false;

        TString first=samplesToCreatePDFsfrom.at(i).at(s).operator()(position+1,1);
        TString second=samplesToCreatePDFsfrom.at(i).at(s).operator()(position+2,1);
        TString third("");
        if (channel>=2 && channel<4)
        {
          third=samplesToCreatePDFsfrom.at(i).at(s).operator()(position+3,1);;
        }
        
        TTree* tree=(TTree*)originalFile->Get(treeName);
        if (tree==0) cout << " Could not find tree " << treeName << " in file " << directory << endl;
        tree->SetBranchAddress("mll",&mll);
        tree->SetBranchAddress("etmiss",&etmiss);
        tree->SetBranchAddress("weight",&weightSingle);
        tree->SetBranchAddress("leptontype",&leptontype);
	//	tree->SetBranchAddress("ptcategory",&ptcategory);
        tree->SetBranchAddress("pT",&pT);
        tree->SetBranchAddress("eta",&eta);
        tree->SetBranchAddress("bweight_MV2c10",&bweight_MV2c10);
	tree->SetBranchAddress("weight_track_70",&weight_track_70);
	tree->SetBranchAddress("weight_track_77",&weight_track_77);
	tree->SetBranchAddress("weight_MV2c1060",&weight_MV2c1060);
	tree->SetBranchAddress("weight_MV2c1070",&weight_MV2c1070);
	tree->SetBranchAddress("weight_MV2c1077",&weight_MV2c1077);
	tree->SetBranchAddress("weight_MV2c1085",&weight_MV2c1085);
        tree->SetBranchAddress("flavours",&flavours);

	tree->SetBranchAddress("imbalance",&imbalance);
	tree->SetBranchAddress("delta_phi",&delta_phi);
	tree->SetBranchAddress("leading_pt",&leading_pt);
	tree->SetBranchAddress("sub_leading_pt",&sub_leading_pt);
	tree->SetBranchAddress("mlj",&mlj);
	tree->SetBranchAddress("minl1j",&minl1j);
	tree->SetBranchAddress("minl2j",&minl2j);
	tree->SetBranchAddress("delta_r",&delta_r);
	tree->SetBranchAddress("isEven",&isEven);


        for(int r=0;r<tree->GetEntries();r++)
        {
/*
  weight_MV2c1060->clear();
  weight_MV2c1070->clear();
  weight_MV2c1077->clear();
  weight_MV2c1085->clear();
  weight_track_70->clear();
  weight_track_77->clear();
*/
          if (r % 10000 == 0) cout << " Sample: " << sample << " == " << sampleTypes.at(u) << " tree " << treeName << " event n. " << r << endl;
          tree->GetEntry(r);
//	  std::cout << "Is even " << isEven << std::endl;

	  if(channel>1) sub_sub_leading_pt = get_sub_sub(pT);

	  if(SFRecalculate)
	  {

	    if(tagPoint=="MV2c1060") weightSingle*=weight_MV2c1060;
	    else if(tagPoint=="MV2c1070") weightSingle*=weight_MV2c1070;
	    else if(tagPoint=="MV2c1077") weightSingle*=weight_MV2c1077*weight_MV2c1077;
	    else if(tagPoint=="MV2c1085") weightSingle*=weight_MV2c1085;
	    else if(tagPoint=="MV2c1070_tracks") weightSingle*=weight_track_70;
	    else if(tagPoint=="MV2c1077_tracks") weightSingle*=weight_track_77;

	  }

	  //          if((weightSingle)<0. && systematicsToLoad=="MCAtNLO") continue;

          if (systematicsToLoad.Contains("ttBBUp") && samplesToCreatePDFsfrom.at(i).at(s)=="all_555") weightSingle*=2.0;
          if (systematicsToLoad.Contains("ttBBDo") && samplesToCreatePDFsfrom.at(i).at(s)=="all_555") weightSingle*=0.5;
  
	  double weight0=1;
          double weight1=1;
          double weight2=1;
/*
  if(SFRecalculate)
  {
  if(tagPoint=="MV2c1060") weight0=weight_MV2c1060->at(0);
  else if(tagPoint=="MV2c1070") weight0=weight_MV2c1070->at(0);
  else if(tagPoint=="MV2c1077") weight0=weight_MV2c1077->at(0);
  else if(tagPoint=="MV2c1085") weight0=weight_MV2c1085->at(0);
  else if(tagPoint=="MV2c1070_tracks") weight0=weight_track_70->at(0);
  else if(tagPoint=="MV2c1077_tracks") weight0=weight_track_77->at(0);  
  }
*/
        
          ptcategory0=getCategory(pT->at(0),
                                  eta->at(0),nbins);
          if (pT->size()>1)
          {
            ptcategory1=getCategory(pT->at(1),
                                    eta->at(1),nbins);
/*
  if(SFRecalculate)
  {
  if(tagPoint=="MV2c1060") weight1=weight_MV2c1060->at(1);
  else if(tagPoint=="MV2c1070") weight1=weight_MV2c1070->at(1);
  else if(tagPoint=="MV2c1077") weight1=weight_MV2c1077->at(1);
  else if(tagPoint=="MV2c1085") weight1=weight_MV2c1085->at(1);
  else if(tagPoint=="MV2c1070_tracks") weight1=weight_track_70->at(1);
  else if(tagPoint=="MV2c1077_tracks") weight1=weight_track_77->at(1);  
  }
*/
          }
        
          if (pT->size()>2)
          {
            ptcategory2=getCategory(pT->at(2),
                                    eta->at(2),nbins);
/*
  if(SFRecalculate)
  {
  if(tagPoint=="MV2c1060") weight2=weight_MV2c1060->at(2);
  else if(tagPoint=="MV2c1070") weight2=weight_MV2c1070->at(2);
  else if(tagPoint=="MV2c1077") weight2=weight_MV2c1077->at(2);
  else if(tagPoint=="MV2c1085") weight2=weight_MV2c1085->at(2);
  else if(tagPoint=="MV2c1070_tracks") weight2=weight_track_70->at(2);
  else if(tagPoint=="MV2c1077_tracks") weight2=weight_track_77->at(2);  
  }
*/
          }

	  if(abs(flavours->at(0))!=5 && weight0!=1){
//	    std::cout << "HODODS pT " << pT->at(0) << " flav " << flavours->at(0) << " bweight " << bweight_MV2c10->at(0) << " weight0 " << weight0 << std::endl;
	  }

	  double weightcombined = weight0*weight1*weight2;
	  double weightcombined1_2 = weight0*weight1;


          ptcategory=ptcategory0*nbins+ptcategory1;

          bool takejet[2];
          
          takejet[0]=true;
          takejet[1]=false;

          if (channel>=4)
          {
            takejet[1]=true;

            for (unsigned int p=0;p<2;p++)//need to consider both candidates
            {
              if (samples.at(i)=="all_5")
              {
                if (abs(flavours->at(p))!=5)
                {
                  takejet[p]=false;
                }
              }
              if (samples.at(i)=="all_1")
              {
                if (abs(flavours->at(p))==5)
                {
                  takejet[p]=false;
                }
              }
              
              if (channel==4 || channel==5)
              {
                //tag and probe method --> flavour is irrelevant
                if ((bweight_MV2c10->at(1-p))<0.8119) 
                {
                  takejet[p]=false;
                }
              }
            }
          }

          if(optimisation)
	  {

/*
  if(isEven){
  double tBDT = reader->EvaluateMVA("BDT_odd");
  if(tBDT < bdt_cut){
  if(pT->size()==2 && pT->at(0) > 30e3 && pT->at(1) > 30e3) continue;
  else if(pT->size()==3 && pT->at(0) > 30e3 && pT->at(1) > 30e3 && pT->at(2) > 30e3) continue;
  else {
  double tBDT_lowpt = reader->EvaluateMVA("BDT_lowpt_odd");
  if ( tBDT_lowpt < low_bdt_cut ) continue;
  }

  }
  }else{
  double tBDT = reader->EvaluateMVA("BDT_even");
  if(tBDT < bdt_cut){
  if(pT->size()==2 && pT->at(0) > 30e3 && pT->at(1) > 30e3) continue;
  else if(pT->size()==3 && pT->at(0) > 30e3 && pT->at(1) > 30e3 && pT->at(2) > 30e3) continue;
  else {
  double tBDT_lowpt = reader->EvaluateMVA("BDT_lowpt_even");
  if ( tBDT_lowpt < low_bdt_cut ) continue;
  }
  }
  } 

*/
	    if(isEven){
	      double tBDT = reader->EvaluateMVA("BDT_odd");
	      if(tBDT < bdt_cut){
		continue;
	      }
	    }else{
	      double tBDT = reader->EvaluateMVA("BDT_even");
	      if(tBDT < bdt_cut){
		continue;
	      } 
	    }



          }
          
          if (channel==0 || channel==2 || channel==4 || channel==6)
          {
            if (leptontype!=0) continue;

            if (channel==0)
            {
              if (first==second)
              {
                histoPtEta->Fill(ptcategory,weightcombined1_2*weightSingle/2.);
                histoPtEta->Fill(ptcategory/nbins+(ptcategory%nbins)*nbins,weightcombined1_2*weightSingle/2.);
                
                histoPtEtaJet0->Fill(ptcategory0,weight0*weightSingle/2);
                histoPtEtaJet0->Fill(ptcategory1,weight1*weightSingle/2);
                
                histoPtEtaJet1->Fill(ptcategory0,weight0*weightSingle/2);
                histoPtEtaJet1->Fill(ptcategory1,weight1*weightSingle/2);
/*
  histoPtEtaJet0->Fill(ptcategory0,weightSingle/2);
  histoPtEtaJet0->Fill(ptcategory1,weightSingle/2);
                
  histoPtEtaJet1->Fill(ptcategory0,weightSingle/2);
  histoPtEtaJet1->Fill(ptcategory1,weightSingle/2);
*/

              }
              else
              {
                histoPtEta->Fill(ptcategory,weightcombined1_2*weightSingle);
                
                histoPtEtaJet0->Fill(ptcategory0,weight0*weightSingle);
                histoPtEtaJet1->Fill(ptcategory1,weight1*weightSingle);
              }
            }
            if (channel==2)
            {
              histoPtEtaJet0->Fill(ptcategory0,weight0*weightSingle);
              histoPtEtaJet1->Fill(ptcategory1,weight1*weightSingle);
              histoPtEtaJet2->Fill(ptcategory2,weight2*weightSingle);
              if (first==second)
              {
                histoPtEtaJet0->Fill(ptcategory1,weight1*weightSingle);
                histoPtEtaJet1->Fill(ptcategory0,weight0*weightSingle);
              }
              if (second==third)
              {
                histoPtEtaJet1->Fill(ptcategory2,weight2*weightSingle);
                histoPtEtaJet2->Fill(ptcategory1,weight1*weightSingle);
              }
              if (first==third)
              {
                histoPtEtaJet0->Fill(ptcategory2,weight2*weightSingle);
                histoPtEtaJet2->Fill(ptcategory0,weight0*weightSingle);
              }
            }
            if (channel==4 || channel==6)
            {

              if (takejet[0])
              {
                histoPtEtaJet0->Fill(ptcategory0,weight0*weightSingle);
              }
              if (takejet[1])
              {
                histoPtEtaJet0->Fill(ptcategory1,weight1*weightSingle);
              }
            }
          }
          else if (channel==1||channel==3||channel==5||channel==7)
          {
            if (leptontype!=1 && leptontype!=2) continue;
            if (etmiss<60e3) continue;
            if (mll<50e3) continue;
            if (mll>80e3 && mll<100e3) continue;
            
            if (channel==1)
            {
              if (first==second)
              {
                histoPtEta->Fill(ptcategory,weightcombined1_2*weightSingle/2.);
                histoPtEta->Fill(ptcategory/nbins+(ptcategory%nbins)*nbins,weightcombined1_2*weightSingle/2.);
                
                histoPtEtaJet0->Fill(ptcategory0,weight0*weightSingle/2);
                histoPtEtaJet0->Fill(ptcategory1,weight1*weightSingle/2);
                
                histoPtEtaJet1->Fill(ptcategory0,weight0*weightSingle/2);
                histoPtEtaJet1->Fill(ptcategory1,weight1*weightSingle/2);
              }
              else
              {
                histoPtEta->Fill(ptcategory,weightcombined1_2*weightSingle);
                
                histoPtEtaJet0->Fill(ptcategory0,weight0*weightSingle);
                histoPtEtaJet1->Fill(ptcategory1,weight1*weightSingle);
              }
            }
            if (channel==3)
            {
              histoPtEtaJet0->Fill(ptcategory0,weight0*weightSingle);
              histoPtEtaJet1->Fill(ptcategory1,weight1*weightSingle);
              histoPtEtaJet2->Fill(ptcategory2,weight2*weightSingle);
              if (first==second)
              {
                histoPtEtaJet0->Fill(ptcategory1,weight1*weightSingle);
                histoPtEtaJet1->Fill(ptcategory0,weight0*weightSingle);
	      }
              if (second==third)
              {
                histoPtEtaJet1->Fill(ptcategory2,weight2*weightSingle);
                histoPtEtaJet2->Fill(ptcategory1,weight1*weightSingle);
              }
              if (first==third)
              {
                histoPtEtaJet0->Fill(ptcategory2,weight2*weightSingle);
                histoPtEtaJet2->Fill(ptcategory0,weight0*weightSingle);
              }
            }
            if (channel==5 || channel==7)
            {
              
              if (takejet[0])
              {
                histoPtEtaJet0->Fill(ptcategory0,weight0*weightSingle);
              }
              if (takejet[1])
              {
                histoPtEtaJet0->Fill(ptcategory1,weight1*weightSingle);
              }
            }
          }
        
          if (takejet[0])
          {
            weightTotal+=weightSingle*weightcombined;
          }
          if (takejet[1])
          {
            weightTotal+=weightSingle*weightcombined;
          }
        }//iterate over sample trees within a sample name (new)
        cout << " now weightTotal is : " << weightTotal << endl;
      }
//Fix for aMC@NLO sample which sometimes gives negative bin entry -> sets value to be small and positive 1e-10

      for(int i=0;i<=histoPtEtaJet0->GetNbinsX();i++)
      {
	if(histoPtEtaJet0->GetBinContent(i)<0) histoPtEtaJet0->SetBinContent(i,1e-10);
	if(histoPtEtaJet1->GetBinContent(i)<0) histoPtEtaJet1->SetBinContent(i,1e-10);
	if(histoPtEtaJet2->GetBinContent(i)<0) histoPtEtaJet2->SetBinContent(i,1e-10);
//          cout << " histoPtEtaJet0 : " << histoPtEtaJet0->GetBinContent(i) << endl;
//          cout << " histoPtEtaJet1 : " << histoPtEtaJet1->GetBinContent(i) << endl;
//          cout << " histoPtEtaJet2 : " << histoPtEtaJet2->GetBinContent(i) << endl;
//          cout << " histoPtEta : " << histoPtEta->GetBinContent(i) << endl;
      }
      
      for(int i=0;i<=histoPtEta->GetNbinsX();i++)
      {
	if(histoPtEta->GetBinContent(i)<0)
	{
	  //	      std::cout << "Negative bin content found: " << histoPtEta->GetBinContent(i) << std::endl;
	  histoPtEta->SetBinContent(i,1e-10);
	}
	//	  std::cout << " histoPtEta : " << histoPtEta->GetBinContent(i) << std::endl;
      }
      
      
      histoName+="_roodatahist";
      
      histoPtEta->Draw();
      
      RooDataHist* hist = new RooDataHist(histoName,histoName,RooArgList(*pTcategory),histoPtEta);
      
      histoName+="2";
      
      RooDataHist* hist2 = new RooDataHist(histoName,histoName,RooArgList(*pTcategory2),histoPtEta);
        
      histoOfPTEta.push_back(hist);
      histoOfPTEta2.push_back(hist2);
      
      histoName.ReplaceAll("2","_jet0");
      RooDataHist* hist3 = new RooDataHist(histoName,histoName,RooArgList(*pTcategoryJet0),histoPtEtaJet0);
      histoName.ReplaceAll("_jet0","_jet0b");
      RooDataHist* hist3b = new RooDataHist(histoName,histoName,RooArgList(*pTcategoryJet1),histoPtEtaJet0);
      histoName.ReplaceAll("_jet0b","_jet0c");
      RooDataHist* hist3c = new RooDataHist(histoName,histoName,RooArgList(*pTcategoryJet2),histoPtEtaJet0);
      
      histoName.ReplaceAll("_jet0c","_jet1");
      RooDataHist* hist4 = new RooDataHist(histoName,histoName,RooArgList(*pTcategoryJet0),histoPtEtaJet1);
      histoName.ReplaceAll("_jet1","_jet1b");
      RooDataHist* hist4b = new RooDataHist(histoName,histoName,RooArgList(*pTcategoryJet1),histoPtEtaJet1);
      histoName.ReplaceAll("_jet1b","_jet1c");
      RooDataHist* hist4c = new RooDataHist(histoName,histoName,RooArgList(*pTcategoryJet2),histoPtEtaJet1);
      
      histoName.ReplaceAll("_jet1c","_jet2");
      RooDataHist* hist5 = new RooDataHist(histoName,histoName,RooArgList(*pTcategoryJet0),histoPtEtaJet2);
      histoName.ReplaceAll("_jet2","_jet2b");
      RooDataHist* hist5b = new RooDataHist(histoName,histoName,RooArgList(*pTcategoryJet1),histoPtEtaJet2);
      histoName.ReplaceAll("_jet2b","_jet2c");
      RooDataHist* hist5c = new RooDataHist(histoName,histoName,RooArgList(*pTcategoryJet2),histoPtEtaJet2);      
      
      histoOfPTEtaJet[0][0][i]=hist3;
      histoOfPTEtaJet[1][0][i]=hist3b;
      histoOfPTEtaJet[2][0][i]=hist3c;//var position, n. of jet in input, subsample
      histoOfPTEtaJet[0][1][i]=hist4;
      histoOfPTEtaJet[1][1][i]=hist4b;
      histoOfPTEtaJet[2][1][i]=hist4c;
      histoOfPTEtaJet[0][2][i]=hist5;
      histoOfPTEtaJet[1][2][i]=hist5b;
      histoOfPTEtaJet[2][2][i]=hist5c;
           
      
      TCanvas * c1 = new TCanvas("c1","c1",400,400);
      histoPtEta->Draw();
      
      neventspersample.push_back(weightTotal);
      SampleTypeNormalization.back()+=weightTotal;
      cout << " Sample: " <<samples.at(i) << " == " << sampleTypes.at(u) << " Tot events "  << weightTotal << endl;
    }
    
    originalFile->Close();
  }

  
  


///XXXX finished with ntuple stufff


  RooAbsPdf* listOfPDFsWeight[4][3][20];
  RooAbsPdf* listOfPDFsPT[4][3][20];//numJets,flavours,samples

  for (int u=0;u<samples.size();u++)
  {

    TString namePT="PDF_PT_";

    for (int j=0;j<3;j++)
    {
      TString name="PDF_BWeight_";
      namePT="PDF_PT_";

      if (j==0)
      {
        name+="b_";
        namePT+="b_";
      }
      else if (j==1)
      {
        name+="c_";
        namePT+="c_";
      }
      else if (j==2)
      {
//        name+="l_";
        name+="lctau_";
        namePT+="lctau_";
      }
      
      //create one for each variable??? quite strange...
      for (int i=0;i<nJets;i++)
      {
        TString namePDF=name;
        namePDF+="_var";
        namePDF+=i;

        TString namePDFpt=namePT;
        namePDFpt+="_var";
        namePDFpt+=i;
        
        RooArgList* pointerCoefPT=coefficientsPTL[j][u];
        namePDFpt+="_";
        namePDFpt+=samples.at(u);
        
        if (!readHistosFromFile)
        {
          RooAbsPdf* tmpPointPT=new RhhBinnedPdf(namePDFpt,namePDFpt,
                                                 *ptCopyVars.at(i),
//                                             numPtBins,
                                                 *pointerCoefPT,//j refers to the flavour
                                                 limitsArrayPtEta1d,
                                                 false);     

          listOfPDFsPT[i][j][u]=tmpPointPT;
      
        }
        else
        {
          namePDFpt.ReplaceAll("b_","_jet0");
          namePDFpt.ReplaceAll("c_","_jet1");
          namePDFpt.ReplaceAll("lctau_","_jet2");
          
          RooAbsPdf* tmpPointPT=new  RooHistPdf(namePDFpt,
                                                namePDFpt,
                                                *ptcategoryJetVars.at(i),
                                                *histoOfPTEtaJet[i][j][u]);
          
          listOfPDFsPT[i][j][u]=tmpPointPT;
          //now here j is the original position and flavour before 
          //permutating combinations
          
        }

        

//RooHistPdf
//histoPtEtaJet0
//histoPtEtaJet1


        RooArgList* pointerCoef=coefficientsBWL[j];
        if (j==2)
        {
          pointerCoef=coefficientsBWeff[u];
          namePDF+="_";
          namePDF+=samples.at(u);
        }
        else if (u!=0)
        {
          continue;
        }


        RooAbsPdf* tmpPoint=0;
        if (useEta)
        {
          tmpPoint=new RhhBinnedPdfInCategories(namePDF,namePDF,
                                                *bweightVars.at(i),//i refers to the variable
                                                *ptVars.at(i),
                                                *etaVars.at(i),
                                                numPtEtaBins,
                                                *pointerCoef,//j refers to the flavour
                                                limitsBWeighta,
                                                false);     
        }
        else
        {
          tmpPoint=new RhhBinnedPdfInPtCategories(namePDF,namePDF,
                                                  *bweightVars.at(i),//i refers to the variable
						  *ptVars.at(i),
                                                  numPtBins,
                                                  *pointerCoef,//j refers to the flavour
                                                  limitsBWeighta,
                                                  false);     
        } 
        
        listOfPDFsWeight[i][j][u]=tmpPoint;
      }
    }
  }//iterate on samples
  
  cout << " H " << endl;
  

  RooAbsPdf* listOfPDFsCombinatorics[20][24];
  RooAbsPdf* listOfPDFsCombinatorics2[20][24];
 
//now for the general PDF

  
   

  vector<vector<int> > flavoursToUse;
  vector<int> flavoursToUseSubCase;
  

  for (int r=0;r<samples.size();r++)
  {

    flavoursToUseSubCase.clear();

    int position=samples.at(r).First("_");
    TString first=samples.at(r).operator()(position+1,1);
    TString second("");
    if (nJets>1)
    {
      second=samples.at(r).operator()(position+2,1);
    }
    TString third("");
    if (nJets>2)
    {
      third=samples.at(r).operator()(position+3,1);
    }
    
    int flav1=first.Atoi();
    int flav2=-1;
    if (nJets>1)
    {
      flav2=second.Atoi();
    }
    int flav3=-1;
    if (nJets>2)
    {
      flav3=third.Atoi();
    }


    cout << " sample " << samples.at(r) << " flavs " << flav1;
    if (nJets>1)
    {
      cout << flav2;
    }
    
    if (nJets>2)
    {
      cout << flav3;
    }
    cout << endl;
    

    flavoursToUseSubCase.push_back(flav1);
    if (nJets>1)
    {
      flavoursToUseSubCase.push_back(flav2);
    }
    if (nJets>2)
    {
      flavoursToUseSubCase.push_back(flav3);
    }
    flavoursToUse.push_back(flavoursToUseSubCase);
  }

  cout << " HH1 " << endl;
  

  for (int e=0;e<samples.size();e++)
  {
    TString directoryToUse;
//    directoryToUse = (tracks) ? "./../../PGS_tracks/" : "/unix/atlas4/abell/FirstxAOD/abell/ttbarPDFCalibration/mc15data5/PGS/";
    directoryToUse = (tracks) ? "/unix/atlasvhbb/abell/mc15data7/PGS_tracks/" : "/unix/atlasvhbb/abell/mc15data7/PGS/";

    directoryToUse+=samples.at(e);

//    for (int j=0;j<nCombinationsModel;j++)

    int nCombinationsThisModel=sample_combinations.at(e).size();
    if (nCombinationsModel==1) nCombinationsThisModel=1;

    for (int j=0;j<nCombinationsThisModel;j++)
    {

      TString namePDF=samples.at(e);
      namePDF+="_PDF_comb";
      namePDF+=sample_combinations.at(e).at(j);

      TString namePDF2=samples.at(e);
      namePDF2+="_PDF2_comb";
      namePDF2+=sample_combinations.at(e).at(j);

      RooAbsReal* var=0;
      int nPDF=0;
      if (j==0)
      {
        var=pTcategory;
        nPDF=0;
      }
      else
      {
        var=pTcategory2;
        nPDF=1;
      }

      RooAbsPdf* combinatorialTTbarPDF=0;
      RooAbsPdf* combinatorialTTbarPDF2=0;

      if (channel<=1)
      {

        RooAbsReal* pt1=ptCopyVars.at(0);
        RooAbsReal* pt2=ptCopyVars.at(1);
        if (j==1)
        {
          pt1=ptCopyVars.at(1);
          pt2=ptCopyVars.at(0);
        }
        

        if (!singleVariables)
        {
          if (!readHistosFromFile)
          {
            if (!(samples.at(e)=="all_55" || samples.at(e)=="all_11"))
            {
              combinatorialTTbarPDF=new RhhBinnedPdf(namePDF,namePDF,*var,
                                                     *(coefficientsPTeff[e][0]),limitsArrayPtEta,true);
            }
            else
            {
              combinatorialTTbarPDF=new RhhBinnedPdfSymmetricMatrix(namePDF,namePDF,*var,
                                                                    *(coefficientsPTeff[e][0]),limitsArrayPtEta,true);
            }
          }
          else
          {
            if (nPDF==0)
            {
              combinatorialTTbarPDF=new  RooHistPdf(namePDF,namePDF,*var,*histoOfPTEta.at(e));            
            }
            else
            {
              combinatorialTTbarPDF=new  RooHistPdf(namePDF,namePDF,*var,*histoOfPTEta2.at(e));
            }
          }
        }
        else
        {
          combinatorialTTbarPDF=new RhhBinnedPdf(namePDF,namePDF,*pt1,
                                                 *(coefficientsPTeff[e][0]),limitsArrayPtEta1d,false);
          if (!(samples.at(e)=="all_55" || samples.at(e)=="all_11"))
          {
            combinatorialTTbarPDF2=new RhhBinnedPdf(namePDF2,namePDF2,*pt2,
                                                    *(coefficientsPTeff[e][1]),limitsArrayPtEta1d,false);
          }
          else
          {
            combinatorialTTbarPDF2=new RhhBinnedPdf(namePDF2,namePDF2,*pt2,
                                                    *(coefficientsPTeff[e][0]),limitsArrayPtEta1d,false);
          }
        }
      }
      
//      cout << " H3 " << endl;
      listOfPDFsCombinatorics[e][j]=combinatorialTTbarPDF;
      listOfPDFsCombinatorics2[e][j]=combinatorialTTbarPDF2;
//      cout << " H4 " << endl;
    }
  }
  
  cout << " HH2 " << endl;

  //ok now you have all PDFs.. do you want to put them together???

  vector<RooProdPdf*> prodPdfs;

  cout << " I " << endl;

  RooArgList*  coefficientsFlavourClass=new RooArgList();
  RooArgList*  pdfsFlavourClass=new RooArgList();
  RooAbsPdf*  pdfs[20][24];
  RooArgList* coefFlavClas=new RooArgList();//smaller dimensionality
  RooArgList* coefFlavClasDiv=new RooArgList();

  vector<RooRealVar*> coeffSampleType;
  vector<RooAbsReal*> coeffSubSampleType;
  vector<RooFormulaVar*> coefficientsToUseInCombinations;

  for (int u=0;u<sampleTypes.size();u++)
  {
    
    TString nameFlavClass("coef_");
    nameFlavClass+=sampleTypes.at(u);
    RooRealVar* coef=new RooRealVar(nameFlavClass,nameFlavClass,1000,0,1000000);
    coeffSampleType.push_back(coef);
    coefFlavClas->addOwned(*coef);

    //set it here because it's easier
    coef->setVal(SampleTypeNormalization.at(u));
//    if (sampleTypes.at(u)!="top")
//    {
    coef->setConstant(kFALSE);
//    }

    cout << " sampleType : " << sampleTypes.at(u) << " start " << sampleTypeStart.at(u) << " end " << sampleTypeEnd.at(u) << endl;
    
    vector<RooRealVar*> othercoefficients;

    for (int i=sampleTypeStart.at(u);i<sampleTypeEnd.at(u)+1;i++)
    {
      int position=samples.at(i).First("_");
      TString type=samples.at(i).operator()(position+1,100);

      double fraction=neventspersample.at(i)/SampleTypeNormalization.at(u);
      cout << " Sample " << sampleTypes.at(u) << 
	" Type " << type << " fraction " << fraction << endl;
      
      TString nameSubSample=nameFlavClass;
      nameSubSample+="_";
      nameSubSample+=type;

      RooAbsReal* coefToUse=0;
      if (i!=sampleTypeEnd.at(u))
      {
        RooRealVar* coef=new RooRealVar(nameSubSample,nameSubSample,0.2,0,1);
        coef->setVal(fraction);
//        coef->setConstant(kTRUE);
        coeffSubSampleType.push_back(coef);
        othercoefficients.push_back(coef);
        coefToUse=coef;
      }
      else
      {
        coeffSampleType.push_back(0);
        TString computation("1.");
        RooArgList listOther;
        for (int y=0;y<othercoefficients.size();y++)
        {
          listOther.add(*othercoefficients.at(y));
          computation+="-@";
          computation+=y;
        }
        cout << " Using " << computation << endl;
        RooFormulaVar* coefLast=new RooFormulaVar(nameSubSample,computation,listOther);
        coeffSubSampleType.push_back(coefLast);
        coefToUse=coefLast;
      }
      
      nameSubSample+="_div";

      int nCombinationsThisModel=sample_combinations.at(i).size();
      if (nCombinationsModel==1) nCombinationsThisModel=1;

      TString division("@0*@1/");
      division+=nCombinationsThisModel;

      RooFormulaVar* coefDivided=new RooFormulaVar(nameSubSample,division,RooArgList(*coef,*coefToUse));
      coefficientsToUseInCombinations.push_back(coefDivided);
    }
  }
  
  cout << " HH3 " << endl;


  for (int r=0;r<flavoursToUse.size();r++)
  {
    cout << " Constructing PDF for case: " << r 
         << " flav1: " << flavoursToUse.at(r).at(0)  << endl;
    
    if (nJets>1)
    {
      cout << " flav2: " << flavoursToUse.at(r).at(1) << endl;
    }
    if (nJets>2)
    {
      cout << " flav3: " << flavoursToUse.at(r).at(2) << endl;
    }
    
    RooFormulaVar* coefDivided=coefficientsToUseInCombinations.at(r);
    coefFlavClasDiv->addOwned(*coefDivided);

//    vector<RooRealVar*> allPreviousRealVars;
    
    int nCombinationsThisModel=sample_combinations.at(r).size();
    if (nCombinationsModel==1) nCombinationsThisModel=1;

//nCombinationsModel
    for (int j=0;j<nCombinationsThisModel;j++)
    {

      RooRealVar* singleCoef=0;
      RooFormulaVar* singleCoefTot=0;
      if (splitIntoCombinations) 
      {      

        TString nameSingleCoeff("coef");
        nameSingleCoeff+=samples.at(r);
        nameSingleCoeff+="_comb";
        nameSingleCoeff+=j;
        
        singleCoef=new RooRealVar(nameSingleCoeff,nameSingleCoeff,
                                  1000,0,100000);

//        singleCoefTot=new RooFormulaVar(nameSingleCoeff,"@0*@1",RooArgList(*coef,*singleCoef));
        singleCoefTot=new RooFormulaVar(nameSingleCoeff,"@0",RooArgList(*singleCoef));

      }
      else
      {
        singleCoefTot=coefDivided;
      }


      TString nameProdPdf("PDFprod_");
      nameProdPdf+=samples.at(r);
      nameProdPdf+="_comb";
      nameProdPdf+=sample_combinations.at(r).at(j);
            
      RooArgList* listPdfToProd=new RooArgList();
      RooArgList* listPdfCondToProd=new RooArgList();
      RooArgList* listBWeightVarsToProd=new RooArgList();
      
      //XXX
      if (channel<=1)//don't use 2d anymore... too complicated
      {
        listPdfToProd->add(*listOfPDFsCombinatorics[r][j]);
        if (singleVariables)
        {
          listPdfToProd->add(*listOfPDFsCombinatorics2[r][j]);
        }
      }
      else
      {
        for (int s=0;s<nJets;s++)
        {
          //usual problem --> we're going in the wrong direction!
          int ohere=0;
          if (combinationsOfm4lnuVectorToUse->at(sample_combinations.at(r).at(j)).at(0)==s) ohere=0;
          if (nJets>1)
          {
            if (combinationsOfm4lnuVectorToUse->at(sample_combinations.at(r).at(j)).at(1)==s) ohere=1;
          }
          if (nJets>2)
          {
            if (combinationsOfm4lnuVectorToUse->at(sample_combinations.at(r).at(j)).at(2)==s) ohere=2;
          }
          
          int whichVar=ohere;
          int flavToUse=flavoursToUse.at(r).at(s);

          int nPdfFlav=0;
          int nSampleToUse=0;
          if (flavToUse==5) 
          {
            nPdfFlav=0;
          }
          else if (flavToUse==4)
          {
            nPdfFlav=1;
          }
          else if (flavToUse==0||flavToUse==1)
          {
            nPdfFlav=2;
          }
          nSampleToUse=r;

          
          if (!readHistosFromFile)
          {
            listPdfToProd->add(*listOfPDFsPT[whichVar][nPdfFlav][nSampleToUse]);
          }
          else
          {
            listPdfToProd->add(*listOfPDFsPT[whichVar][s][nSampleToUse]);
          }
          
        }
      }
      
      cout << " L " << endl;

      if (addWeight)
      {
        for (int s=0;s<nJets;s++)
        {
          //usual problem --> we're going in the wrong direction!
          int ohere=0;
          if (combinationsOfm4lnuVectorToUse->at(sample_combinations.at(r).at(j)).at(0)==s) ohere=0;
          if (nJets>1)
          {
            if (combinationsOfm4lnuVectorToUse->at(sample_combinations.at(r).at(j)).at(1)==s) ohere=1;
          }
          if (nJets>2)
          {
            if (combinationsOfm4lnuVectorToUse->at(sample_combinations.at(r).at(j)).at(2)==s) ohere=2;
          }

          int whichVar=ohere;
          int flavToUse=flavoursToUse.at(r).at(s);

          int nPdfFlav=0;
          int nSampleToUse=0;
          if (flavToUse==5) 
          {
            nPdfFlav=0;
          }
          else if (flavToUse==4)
          {
            nPdfFlav=1;
          }
          else if (flavToUse==0||flavToUse==1)
          {
            nPdfFlav=2;
            nSampleToUse=r;
          }
          

	  listPdfToProd->add(*listOfPDFsWeight[whichVar][nPdfFlav][nSampleToUse]);
          
        }
      }
      
      cout << " M " << endl;

      TString nameFlavClass("coefFlavClass_");
      nameFlavClass+=samples.at(r);
      nameFlavClass+="_comb";
      nameFlavClass+=j;
      
      coefficientsFlavourClass->add(*singleCoefTot);
      
      RooProdPdf* prodPdf=0;

      prodPdf=new RooProdPdf(nameProdPdf,nameProdPdf,*listPdfToProd);

      
      pdfsFlavourClass->addOwned(*prodPdf);
      prodPdf->Print("all");
      
    }//nCombinationsModel (of the same flavour)

    cout << " N " << endl;

  }//end flavours (samples) to use
  
  
  cout << " O " << endl;

  RooAddPdf* PDFtot=new RooAddPdf("PDFtot",
                                  "PDFtot",
                                  *pdfsFlavourClass,
//                                  RooArgList(*test1,*test2),
                                  *coefficientsFlavourClass);
  
//                                  RooArgList(*test1),
//                                  RooArgList(*coefficientsFlavourClass->at(0)));


  PDFtot->Print("v");
  
  return PDFtot;
  
  
}
  


  
