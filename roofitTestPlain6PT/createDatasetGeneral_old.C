#include "RooRealVar.h"
#include <vector>
#include "RooArgSet.h"
#include "TFile.h"
#include "TChain.h"
#include "TString.h"
#include "RooDataSet.h"
#include "TRandom3.h"
#include "TH1F.h"

using namespace std;

#include "getCategory.icc"

double getWeight(TH1F * bHisto,
		 TH1F * cHisto,
		 TH1F * lHisto,
		 int flavour)
{
  if (flavour==5) return bHisto->GetRandom();
  if (flavour==4) return cHisto->GetRandom();
  if (flavour==0) return lHisto->GetRandom();
  cout << " Didn't recognize flavour " << flavour << endl;
  throw;
}


void createDatasetGeneral(TString tagPoint, int channel=0,int combinations=2, bool fakeBWeights=false,
			  bool genEvents=false,bool data=false,TString systematics="",
                          int optionSample=-1)
{

  #include "rescaling.icc"

  #include "nbins.icc"

  if(gRandom) delete gRandom;
  gRandom = new TRandom3(0);

  bool addLeptonicTopVariables=false;
  bool tracks = (tagPoint.Index("tracks")!=-1);
  double ptmin=20e3;
  if(tracks)ptmin=10e3;
  //fakeBweights basically removes any correlation with pT and 
  //sample dependency of the bweights!
  
  TH1F* bHisto=0;
  TH1F* cHisto=0;
  TH1F* lHisto=0;
  if (fakeBWeights)
  {
    TFile* fileWeights=new TFile("histoWeights.root");
    bHisto=(TH1F*)fileWeights->Get("h1");
    cHisto=(TH1F*)fileWeights->Get("h2");
    lHisto=(TH1F*)fileWeights->Get("h3");
  }

  std::vector<TString> samples;


  if (channel<=1)//up to 2 use 2 jets only, from 3 use 3 jets
  {
    if (!data)
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
    else
    {
      samples.push_back("all_99");
    }
  }
  else if (channel<=3)
  {
    if (!data)
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
      samples.push_back("all_999");
    }
  }
  else
  {
    if (!data)
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
        samples.push_back("all_51");
      }
      else if (optionSample==1)
      {
        samples.push_back("all_51");
        samples.push_back("all_11");
      }
    }
    else
    {
      samples.push_back("all_99");
    }
  }

  //INCLUDE FIX COMBINATIONS
  #include "../../ntupProduction/fixCombinations.icc"
  #include "../../ntupProduction/fixCombinationsThree.icc"

  RooRealVar* eventWeight=new RooRealVar("eventWeight","eventWeight",1,-1e10,1e10);

  RooRealVar* weight1=new RooRealVar("bweight_0","b-tagging weight 1",0.5,0.,1.);
  RooRealVar* weight2=new RooRealVar("bweight_1","b-tagging weight 2",0.5,0.,1.);
  RooRealVar* weight3=new RooRealVar("bweight_2","b-tagging weight 2",0.5,0.,1.);

  RooRealVar* pt1=new RooRealVar("pt_0","pt 1",30e3,ptmin,400e3);
  RooRealVar* pt2=new RooRealVar("pt_1","pt 2",30e3,ptmin,400e3);
  RooRealVar* pt3=new RooRealVar("pt_2","pt 3",30e3,ptmin,400e3);
  
  RooRealVar* pt1copy=new RooRealVar("ptcopy_0","ptcopy 1",30e3,ptmin,400e3);
  RooRealVar* pt2copy=new RooRealVar("ptcopy_1","ptcopy 2",30e3,ptmin,400e3);
  RooRealVar* pt3copy=new RooRealVar("ptcopy_2","ptcopy 3",30e3,ptmin,400e3);

  RooRealVar* pTcategory=new RooRealVar("ptcategory","ptcategory",5,-0.5,(double)(nbins*nbins)-0.5);
  RooRealVar* pTcategory2=new RooRealVar("ptcategory2","ptcategory2",5,-0.5,(double)(nbins*nbins)-0.5);
  RooRealVar* pTcategory3=new RooRealVar("ptcategory3","ptcategory3",5,-0.5,(double)(nbins*nbins)-0.5);
  
  RooRealVar* pTcategoryJet0=new RooRealVar("ptcategoryJet0","ptcategoryJet0",5,-0.5,(double)(nbins)-0.5);
  RooRealVar* pTcategoryJet1=new RooRealVar("ptcategoryJet1","ptcategoryJet1",5,-0.5,(double)(nbins)-0.5);
  RooRealVar* pTcategoryJet2=new RooRealVar("ptcategoryJet2","ptcategoryJet2",5,-0.5,(double)(nbins)-0.5);

  RooRealVar* eta1=new RooRealVar("eta_0","eta 1",0,-2.5,2.5);
  RooRealVar* eta2=new RooRealVar("eta_1","eta 2",0,-2.5,2.5);
  RooRealVar* eta3=new RooRealVar("eta_2","eta 3",0,-2.5,2.5);

  RooRealVar* eta1copy=new RooRealVar("etacopy_0","eta 1",0,-2.5,2.5);
  RooRealVar* eta2copy=new RooRealVar("etacopy_1","eta 2",0,-2.5,2.5);
  RooRealVar* eta3copy=new RooRealVar("etacopy_2","eta 3",0,-2.5,2.5);

  RooArgSet* newSet=new RooArgSet();

  newSet->addOwned(*weight1);
  newSet->addOwned(*weight2);
  newSet->addOwned(*weight3);

  newSet->addOwned(*pt1);
  newSet->addOwned(*pt2);
  newSet->addOwned(*pt3);

  newSet->addOwned(*pt1copy);
  newSet->addOwned(*pt2copy);
  newSet->addOwned(*pt3copy);

  newSet->addOwned(*pTcategory);
  newSet->addOwned(*pTcategory2);
  newSet->addOwned(*pTcategory3);

  newSet->addOwned(*pTcategoryJet0);
  newSet->addOwned(*pTcategoryJet1);
  newSet->addOwned(*pTcategoryJet2);
  
  newSet->addOwned(*eta1);
  newSet->addOwned(*eta2);
  newSet->addOwned(*eta3);

  newSet->addOwned(*eta1copy);
  newSet->addOwned(*eta2copy);
  newSet->addOwned(*eta3copy);
  
  newSet->addOwned(*eventWeight);
  
  //open input file
  TChain *simu = new TChain("newTree");
  for (unsigned int i=0;i<samples.size();i++)
  {
   
    int position=samples.at(i).First("_");
    TString sample=samples.at(i).operator()(0,position);
    TString type=samples.at(i).operator()(position+1,100);
    cout << " sample is: " << sample << " type is: " << type << endl;

    TString directory;
    directory = (tracks) ? "./../../PGS_tracks/outputGeneral_fit_" : "./../../PGS/outputGeneral_fit_";
    directory+=sample;
    if (systematics!="")
    {
      directory+="Sys";
      directory+=systematics;
    }
    if (rescaling)
    {
      directory+="_rescaling";
    }
    directory+=".root/";
    directory+="tree_";
    directory+=type;

    if (genEvents)
    {
      //NEED TO REWRITE
    }
    if (data)
    {
      directory = (tracks) ? "./../../PGS_tracks/outputGeneral_fit_data.root/tree_" : "./../../PGS/outputGeneral_fit_data.root/tree_";
      directory+=type;
    }
    
    cout << " ADDING: " << directory << endl;
    simu->Add(directory);
  }


  float mll=0;
  float ptll=0;
  float etmiss=0;
  double weight=0;
  int leptontype=0;

  int ptcategory=0;

  vector<float>* bweight_mv2c20=0;
  vector<float>* pT=0;
  vector<float>* eta=0;
  vector<int>* flavours=0;

  //  int type;

  simu->SetBranchAddress("mll",&mll);
  simu->SetBranchAddress("ptll",&ptll);
  simu->SetBranchAddress("etmiss",&etmiss);
  simu->SetBranchAddress("ptcategory",&ptcategory);
  simu->SetBranchAddress("leptontype",&leptontype);
  simu->SetBranchAddress("weight",&weight);

  //  simu->SetBranchAddress("type",&type);
  simu->SetBranchAddress("flavours",&flavours);
  simu->SetBranchAddress("bweight_mv2c20",&bweight_mv2c20);
  simu->SetBranchAddress("pT",&pT);
  simu->SetBranchAddress("eta",&eta);

  //now roofit

  TString name("fileRoofitGeneral");
  name+=channel;
  if (systematics!="")
  {
    name+="_";
    name+=systematics;
  }
  if (data)
  {
    name+="_data";
  }
  name+=".root";

  TFile* fileRooFit=new TFile(name,"recreate");
  RooDataSet* newDataSet=new RooDataSet("dataset","dataset",*newSet,"eventWeight");
//  RooDataSet* newDataSet=new RooDataSet("dataset","dataset",*newSet);

  int nEntries=simu->GetEntries();

  cout << " Entries in sample: " << nEntries << endl;

  TRandom3 randomGen(213123);

  double weightTotal=0;

  int reduceFactor=1;

  for (int v=0;v<nEntries;v++)
  {

    simu->GetEntry(v);
    //    if(abs(weight<0.5)) continue;
    if (channel==0 || channel==2 || channel==4 || channel ==6)
    {
      if (leptontype!=0) continue;
    }
    else if (channel==1 || channel==3 || channel==5 || channel==7)
    {
      if (leptontype!=1 && leptontype!=2) continue;
      if (etmiss<60e3) continue;
      if (mll<50e3) continue;
      if (mll>80e3 && mll<100e3) continue;
    }

    double SF=1;
    if ((!data) && optionSample==-1)
    {
      SF=16;
    }

    eventWeight->setVal(SF*weight*reduceFactor);
    if (v % 10000 == 0)
    {
      cout << " Iterating over element: " << v << endl;
    }

    if (channel<=3)
    {

      int nCombination=(int)(randomGen.Uniform(0,(double)combinations));
      
      vector<vector<int> >* combinationsOfm4lnuVectorToUse=&combinationsOfm4lnuVector;
      if (pT->size()>2)
      {
        combinationsOfm4lnuVectorToUse=&tree_combinationsOfm4lnuVector;
      }
      
      
      if (!fakeBWeights)
      {
	{weight1->setVal((bweight_mv2c20->at(combinationsOfm4lnuVectorToUse->at(nCombination).at(0))));}
        {weight2->setVal((bweight_mv2c20->at(combinationsOfm4lnuVectorToUse->at(nCombination).at(1))));}
        if (pT->size()>2)
        {
	  {weight3->setVal((bweight_mv2c20->at(combinationsOfm4lnuVectorToUse->at(nCombination).at(2))));}
        }
      }
      else
      {
        weight1->setVal(getWeight(bHisto,cHisto,lHisto,flavours->at(combinationsOfm4lnuVectorToUse->at(nCombination).at(0))));
        weight2->setVal(getWeight(bHisto,cHisto,lHisto,flavours->at(combinationsOfm4lnuVectorToUse->at(nCombination).at(1))));
        if (pT->size()>2)
        {
          weight3->setVal(getWeight(bHisto,cHisto,lHisto,flavours->at(combinationsOfm4lnuVectorToUse->at(nCombination).at(2))));
        }
        
      }
      
      if (!genEvents)
      {
        eta1->setVal(eta->at(combinationsOfm4lnuVectorToUse->at(nCombination).at(0)));
        eta2->setVal(eta->at(combinationsOfm4lnuVectorToUse->at(nCombination).at(1)));
        if (pT->size()>2)
        {
          eta3->setVal(eta->at(combinationsOfm4lnuVectorToUse->at(nCombination).at(2)));
        }
        
        eta1copy->setVal(eta->at(combinationsOfm4lnuVectorToUse->at(nCombination).at(0)));
        eta2copy->setVal(eta->at(combinationsOfm4lnuVectorToUse->at(nCombination).at(1)));
        if (pT->size()>2)
        {
          eta3copy->setVal(eta->at(combinationsOfm4lnuVectorToUse->at(nCombination).at(2)));
        }
      }
      
      
      pt1->setVal(pT->at(combinationsOfm4lnuVectorToUse->at(nCombination).at(0)));
      pt2->setVal(pT->at(combinationsOfm4lnuVectorToUse->at(nCombination).at(1)));
      if (pT->size()>2)
      {
        pt3->setVal(pT->at(combinationsOfm4lnuVectorToUse->at(nCombination).at(2)));
      }
      
      //cut on leading pT
      if (pT->at(0)<30e3 && pT->at(1)<30e3 && pT->size()==2) 
      {
        cout << " CUTTING! " << endl;
        throw;
//      continue;
      }
      
      
      //reobtain ptcategory on the fly
      ptcategory=
          getCategory(pT->at(0),
                      eta->at(0),nbins)*nbins+
          getCategory(pT->at(1),
                      eta->at(1),nbins);
      
      
      if (nCombination==0)//you need to exchange the pT of the jets if you exchange the b-weights!!!
      {
        pTcategory->setVal(ptcategory);
        pTcategory2->setVal(ptcategory/nbins+(ptcategory%nbins)*nbins);
      }
      else
      {
        pTcategory->setVal(ptcategory/nbins+(ptcategory%nbins)*nbins);
        pTcategory2->setVal(ptcategory);
      }
      
      pTcategoryJet0->setVal(getCategory(pT->at(combinationsOfm4lnuVectorToUse->at(nCombination).at(0)),
                                         eta->at(combinationsOfm4lnuVectorToUse->at(nCombination).at(0)),nbins));
      pTcategoryJet1->setVal(getCategory(pT->at(combinationsOfm4lnuVectorToUse->at(nCombination).at(1)),
                                         eta->at(combinationsOfm4lnuVectorToUse->at(nCombination).at(1)),nbins));
      if (pT->size()>2)
      {
        pTcategoryJet2->setVal(getCategory(pT->at(combinationsOfm4lnuVectorToUse->at(nCombination).at(2)),
                                           eta->at(combinationsOfm4lnuVectorToUse->at(nCombination).at(2)),nbins));
      }
      
      pt1copy->setVal(pT->at(combinationsOfm4lnuVectorToUse->at(nCombination).at(0)));
      pt2copy->setVal(pT->at(combinationsOfm4lnuVectorToUse->at(nCombination).at(1)));
      if (pT->size()>2)
      {
        pt3copy->setVal(pT->at(combinationsOfm4lnuVectorToUse->at(nCombination).at(2)));
      }
///XXXXXXXXXXXX CHANGES HERE
      if (abs(weight)<1e-8) continue;
      
      if (!genEvents)
      {
        newDataSet->add(*newSet,weight*reduceFactor*SF);
        weightTotal+=weight*reduceFactor*SF;
//      newDataSet->add(*newSet,1);
//     weightTotal+=1;
      }else{
        newDataSet->add(*newSet);
        weightTotal+=1.;
      }
      
    } 
    else//new case with 1d fit...
    {
      ///BEGIN

      for (unsigned int p=0;p<2;p++)
      {
        
        if (optionSample==0 && !data)
        {
          if (abs(flavours->at(p))!=5)
          {
            continue;
          }
        }
        if (optionSample==1 && !data)
        {
          if (abs(flavours->at(p))==5)
          {
            continue;
          }
        }

        if (channel==4 || channel==5)
        {
          //tag and probe method --> flavour is irrelevant
          if ((bweight_mv2c20->at(1-p))<0.8119) continue;
        }
      

        if (!fakeBWeights)
        {
          weight1->setVal((bweight_mv2c20->at(p)));
        }
        else
        {
          weight1->setVal(getWeight(bHisto,cHisto,lHisto,flavours->at(p)));
        }
        
        if (!genEvents)
        {
          eta1->setVal(eta->at(p));
          eta1copy->setVal(eta->at(p));
        }
        
        
        pt1->setVal(pT->at(p));
        
        //cut on leading pT
        if (pT->at(0)<30e3 && pT->at(1)<30e3 && pT->size()==2) 
        {
          cout << " CUTTING! " << endl;
          throw;
//      continue;
        }
      
        pTcategoryJet0->setVal(getCategory(pT->at(p),
                                           eta->at(p),nbins));
        
        pt1copy->setVal(pT->at(p));
        
///XXXXXXXXXXXXCHANGES HERE
        if (abs(weight)<1e-8) continue;
        
        if (!genEvents)
        {
          newDataSet->add(*newSet,weight*reduceFactor*SF);
          weightTotal+=weight*reduceFactor*SF;
//      newDataSet->add(*newSet,1);
//     weightTotal+=1;
        }else{
          newDataSet->add(*newSet);
          weightTotal+=1.;
        }
        
        
      }//end iterating on two jets
//END     

    }//end new case with 1d fit
    
  }
    
    
  fileRooFit->cd();

  cout << " Total weight is: " << weightTotal << endl;

  newDataSet->Write();

  cout <<  "Total entries: " << newDataSet->numEntries() << endl;

  fileRooFit->Write();
  fileRooFit->Close();


}


      
      


  
  


