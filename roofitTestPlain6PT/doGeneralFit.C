#include "buildUpMultiSampleModel.h"
#include "RooDataSet.h"
#include "RooFitResult.h"
#include "TFile.h"
#include "RooFitResult.h"
#include "RooAbsPdf.h"
#include "RooRealVar.h"
#include "TIterator.h"
#include "RooAbsArg.h"
#include "RooGaussian.h"
#include "RooProdPdf.h"
#include "RooMsgService.h"
#include "RooGlobalFunc.h"
#include "RooCmdArg.h"
#include "RooPlot.h"
#include "RooBinning.h"
#include "TIterator.h"
#include "TCanvas.h"
#include "TRandom3.h"
#include "RooRandom.h"
#include "RooDataHist.h"
#include "TTree.h"

#include <iostream>

using namespace std;
using namespace RooFit;


namespace
{
#include "getCategory.icc"
#include "tracks.icc"
}


void setParameters(RooFitResult* result,RooArgSet* parameters,TString selection="")
{
  const RooArgList& listOfParsFixed=result->floatParsFinal();
  TIterator* iterator=listOfParsFixed.createIterator();  
  TObjString* arg=0;
     
  while( ( arg= (TObjString*)iterator->Next()) ) 
  {
    RooAbsArg&  varArg= (*parameters)[arg->GetName()];

    RooRealVar* var=dynamic_cast<RooRealVar*>(&varArg);

    if (var!=0) 
    {
      RooRealVar* var2=dynamic_cast<RooRealVar*>(listOfParsFixed.find(arg->GetName()));
      if (var2!=0)
      {


	TString varName=arg->GetName();
	if (selection!=0)
	{
	  if (!varName.Contains(selection))
	  {
	    cout << " Var " << varName << " doesn't contain " << selection << endl;
	    continue;
	  }
	}

	if (varName=="coef_all")
	{
	  cout << " variable coef_all was: " << 
	    var->getVal() << " adding " << var2->getVal() << endl;
	  var->setVal(var->getVal()+var2->getVal());
	}
	else
	{
	  cout << " setting fixed: " << arg->GetName() << " == " << var2->GetName() << " to " << var2->getVal() << endl;
	  var->setVal(var2->getVal());
	}
      }
    }
  }
}





void doGeneralFit(TString tagPoint="MV2c1070",
                  int channel=0,
                  int nCombinationsFit=24,
                  TString systematics="Nominal",
		  TString inputFile="fileRoofitGeneral.root",
                  int optionSample=-1,
                  TString systematicsToLoad="",
                  bool runToy=false,
                  int randomNumber=0)
{

  bool useoldmistag=false;
  
#include "nbins.icc"
#include "btagBinning.icc"
#include "/unix/atlasvhbb/abell/mc15data7/roofitTestPlain/roofitTestPlain6PTFLAT/mistagRates.icc"
#include "tracks.icc"

  //INCLUDE FIX COMBINATIONS
#include "/unix/atlasvhbb/abell/mc15data7/ntupProduction/fixCombinations.icc"
#include "/unix/atlasvhbb/abell/mc15data7/ntupProduction/fixCombinationsThree.icc"


  TString systematicsToAddToOutputFile=systematicsToLoad;

  double varMistag=1;
  if (systematicsToLoad=="MistagUp"&&useoldmistag&&!tracks)
  {
    varMistag=2.00;
    systematicsToLoad="";
  }
  else if (systematicsToLoad=="MistagDo"&&useoldmistag&&!tracks)
  {
    varMistag=0.5;
    systematicsToLoad="";
  }
  else if (systematicsToLoad=="MistagCUp"&&useoldmistag&&!tracks)
  {
    varMistag=1.5;
    systematicsToLoad="";
  }
  else if (systematicsToLoad=="MistagCDo"&&useoldmistag&&!tracks)
  {
    varMistag=0.75;
    systematicsToLoad="";
  }

//  RooMsgService::instance().addStream(DEBUG,RooFit::Topic(RooFit::Generation));
//  RooMsgService::instance().addStream(DEBUG,RooFit::Topic(RooFit::Minimization));
//  RooMsgService::instance().getStream(1).removeTopic(RooFit::Tracing);
//  RooMsgService::instance().getStream(1).removeTopic(RooFit::InputArguments);
//  RooMsgService::instance().getStream(1).removeTopic(RooFit::ObjectHandling);
//  RooMsgService::instance().addStream(DEBUG,RooFit::Topic(RooFit::InputArguments));
//  RooMsgService::instance().addStream(DEBUG,RooFit::Topic(RooFit::ObjectHandling));
//  RooMsgService::instance().addStream(DEBUG,RooFit::Topic(RooFit::Tracing));
//  RooMsgService::instance().addStream(DEBUG,RooFit::Topic(RooFit::Fitting));
//  RooMsgService::instance().addStream(DEBUG,RooFit::Topic(RooFit::Contents));
//  RooMsgService::instance().addStream(DEBUG,RooFit::Topic(RooFit::DataHandling));

// Print streams configuration
  RooMsgService::instance().Print() ;
  cout << endl ;

//  throw;
  
  TFile* fileToFit=0;

  inputFile="fileRoofitGeneral";
  inputFile+=channel;
  if (systematics!="MCNominal" && systematics!="DataNominal")
  {
    inputFile+="_";
    inputFile+=systematics;
  }
  if (systematics=="DataNominal" && systematicsToAddToOutputFile!="")
  {
    inputFile+="_";
    inputFile+=systematicsToAddToOutputFile;
  }
  if (systematics=="DataNominal")
  {
    inputFile+="_data";
  }
  inputFile+=".root";

  if (!runToy) fileToFit=new TFile(inputFile);
  cout << " name of File " << inputFile << endl;

  RooDataSet* dataset=0;
  if (!runToy)
  {
    dataset=(RooDataSet*)fileToFit->Get("dataset");
  }

  const RooArgSet* set=0;

  cout << " BEFORE " << endl;

  RooAbsPdf* totalPDF=buildUpMultiSampleModel(tagPoint,channel,nCombinationsFit,true,optionSample,systematicsToLoad);//nCombinationsFit

  cout << " AFTER " << endl;

  RooAbsPdf* totalPDFtoy=0;
  if (runToy)
  {
    //one only combination (keep order of flavours)
    totalPDFtoy=buildUpMultiSampleModel(tagPoint,channel,1,
                                        true,optionSample,
					systematicsToLoad);//nCombinationsFit
  }

  if (!runToy)
  {
    set=dataset->get(0);
  }
  else
  {
    set=totalPDF->getVariables();
  }

  //  set->Print("v");

  RooRealVar* pt1=(RooRealVar*)set->find("pt_0");
  RooRealVar* pt2=(RooRealVar*)set->find("pt_1");
  RooRealVar* pt3=(RooRealVar*)set->find("pt_2");
  RooRealVar* eta1=(RooRealVar*)set->find("eta_0");
  RooRealVar* eta2=(RooRealVar*)set->find("eta_1");
  RooRealVar* eta3=(RooRealVar*)set->find("eta_2");

  RooRealVar* bweight1=(RooRealVar*)set->find("bweight_0");
  RooRealVar* bweight2=(RooRealVar*)set->find("bweight_1");
  RooRealVar* bweight3=(RooRealVar*)set->find("bweight_2");

  RooRealVar* pTcategory=(RooRealVar*)set->find("ptcategory");
  RooRealVar* pTcategory2=(RooRealVar*)set->find("ptcategory2");

  RooRealVar* pTcategoryJet0=(RooRealVar*)set->find("ptcategoryJet0");
  RooRealVar* pTcategoryJet1=(RooRealVar*)set->find("ptcategoryJet1");
  RooRealVar* pTcategoryJet2=(RooRealVar*)set->find("ptcategoryJet2");

  // do onnnnnnnnnly if not calibrating
  double totalExpN=0;
  if (runToy)
  {
    totalExpN=((RooRealVar*)set->find("coef_all"))->getVal();
  }

  RooArgList condObs;
  condObs.add(*pt1);
  if (channel<4)
  {
    condObs.add(*pt2);
  }

  condObs.add(*eta1);
  if (channel<4)
  {
    condObs.add(*eta2);
  }
  if (channel>=2 && channel<4)
  {
    condObs.add(*pt3);
    condObs.add(*eta3);
  }

  //  condObs.add(*eta1);
  //  condObs.add(*eta2);



  if (totalPDF==0) return;

  if (systematicsToLoad=="ttBBUp" || systematicsToLoad=="ttBBDo" )
  {
    systematicsToLoad="";
  }

  RooArgSet* parameters=(RooArgSet*)totalPDF->getVariables();
  RooArgSet* parametersToy=0;
  if (runToy)
  {
    parametersToy=(RooArgSet*)totalPDF->getVariables();
  }

  if (optionSample==-1)
  {
    TString base="results";
    base+="_";
    base+=tagPoint;
    base+="/MCNominal";
    if (systematicsToLoad!="")
    {
      base="results";
      base+="_";
      base+=tagPoint;
      base+="/";
      base+=systematicsToLoad;
    }
    if (channel>=2 && channel<4)
    {
      base+="_3jets";
    }
    if (channel==1 || channel==3 || channel==5 || channel==7)
    {
      base+="_sameflav";
    }
    if (channel>=4 && channel<6)
    {
      base+="_1dtagprobe";
    }
    if (channel>=6)
    {
      base+="_1dinclusive";
    }
    TString base0=base;
    base0+="_0.root";
    TString base1=base;
    base1+="_1.root";
    TString base2=base;
    base2+="_2.root";
    TString base3=base;
    base3+="_3.root";


    cout << " Loading files: " << base0 << " " << base1 << " " << base2 << endl;

    TFile* file0=new TFile(base0);
    TFile* file1=new TFile(base1);
    TFile* file2=new TFile(base2);

    ((RooRealVar*)totalPDF->getVariables()->find("coef_all"))->setVal(0.);
    if (runToy)
    {
      ((RooRealVar*)totalPDFtoy->getVariables()->find("coef_all"))->setVal(0.);
    }


    RooFitResult* result0=(RooFitResult*)file0->Get("fitresult_PDFtot_dataset");
    RooFitResult* result1=(RooFitResult*)file1->Get("fitresult_PDFtot_dataset");
    RooFitResult* result2=0;
    if (channel<4)
    {
      result2=(RooFitResult*)file2->Get("fitresult_PDFtot_dataset");
    }
    RooFitResult* result3=0;
    if (channel>=2 && channel<4)
    {
      TFile* file3=new TFile(base3);
      result3=(RooFitResult*)file3->Get("fitresult_PDFtot_dataset");
      setParameters(result3,(RooArgSet*)totalPDF->getVariables());
      if (runToy)
      {
	setParameters(result3,(RooArgSet*)totalPDFtoy->getVariables());
      }
    }

    if (channel<4)
    {
      setParameters(result2,(RooArgSet*)totalPDF->getVariables());
      if (runToy)
      {
	setParameters(result2,(RooArgSet*)totalPDFtoy->getVariables());
      }
    }

    if (channel>=2 && channel<4)
    {
      setParameters(result0,(RooArgSet*)totalPDF->getVariables());    
      setParameters(result1,(RooArgSet*)totalPDF->getVariables());
      if (runToy)
      {
	setParameters(result0,(RooArgSet*)totalPDFtoy->getVariables());
	setParameters(result1,(RooArgSet*)totalPDFtoy->getVariables());
      }
    }
    else
    {
      setParameters(result1,(RooArgSet*)totalPDF->getVariables());
      setParameters(result0,(RooArgSet*)totalPDF->getVariables());    
      if (runToy)
      {
	setParameters(result1,(RooArgSet*)totalPDFtoy->getVariables());
	setParameters(result0,(RooArgSet*)totalPDFtoy->getVariables());
      }
    }


    parameters->Print("v");

    if (channel<2)
    {
      parameters->selectByName("*BWeight*all_51*")->setAttribAll("Constant", kTRUE);
      parameters->selectByName("*BWeight*_all_1*")->setAttribAll("Constant", kTRUE);
      //  parameters->selectByName("*BWeight*")->setAttribAll("Constant", kTRUE);
      parameters->selectByName("coef_all_1")->setAttribAll("Constant", kTRUE);
      parameters->selectByName("coef_all_55")->setAttribAll("Constant", kTRUE);
      parameters->selectByName("coef_all_51")->setAttribAll("Constant", kTRUE);
      parameters->selectByName("coefPT*")->setAttribAll("Constant", kTRUE);
      // ((RooRealVar*)totalPDF->getVariables()->find("coef_all_55"))->setVal(0.8);
      //  ((RooRealVar*)totalPDF->getVariables()->find("coef_all_0"))->setConstant(kTRUE);
    }
    else if (channel<4)
    {
      parameters->selectByName("*BWeight*all_551*")->setAttribAll("Constant", kTRUE);
      parameters->selectByName("*BWeight*all_5*")->setAttribAll("Constant", kTRUE);
      parameters->selectByName("*BWeight*_all_1*")->setAttribAll("Constant", kTRUE);
      //  parameters->selectByName("*BWeight*")->setAttribAll("Constant", kTRUE);
      parameters->selectByName("coef_all_5*")->setAttribAll("Constant", kTRUE);
      parameters->selectByName("coef_all_1*")->setAttribAll("Constant", kTRUE);
      parameters->selectByName("coefPT_*")->setAttribAll("Constant", kTRUE);
    }
    else
    {
      parameters->selectByName("*BWeight*_all_1*")->setAttribAll("Constant", kTRUE);
      parameters->selectByName("coef_all_5")->setAttribAll("Constant", kTRUE);
      parameters->selectByName("coef_all_1")->setAttribAll("Constant", kTRUE);
      parameters->selectByName("coefPT_*")->setAttribAll("Constant", kTRUE);
    }
  }




  if (runToy)
  {

    if (channel<2)
    {
      parametersToy->selectByName("*BWeight*all_51*")->setAttribAll("Constant", kTRUE);
      parametersToy->selectByName("*BWeight*_all_1*")->setAttribAll("Constant", kTRUE);
      //  parametersToy->selectByName("*BWeight*")->setAttribAll("Constant", kTRUE);
      parametersToy->selectByName("coef_all_1")->setAttribAll("Constant", kTRUE);
      parametersToy->selectByName("coef_all_55")->setAttribAll("Constant", kTRUE);
      parametersToy->selectByName("coef_all_51")->setAttribAll("Constant", kTRUE);
      parametersToy->selectByName("coefPT*")->setAttribAll("Constant", kTRUE);
      // ((RooRealVar*)totalPDF->getVariables()->find("coef_all_55"))->setVal(0.8);
      //  ((RooRealVar*)totalPDF->getVariables()->find("coef_all_0"))->setConstant(kTRUE);
    }
    else if (channel<4)
    {
      parametersToy->selectByName("*BWeight*all_551*")->setAttribAll("Constant", kTRUE);
      parametersToy->selectByName("*BWeight*all_5*")->setAttribAll("Constant", kTRUE);
      parametersToy->selectByName("*BWeight*_all_1*")->setAttribAll("Constant", kTRUE);
      //  parametersToy->selectByName("*BWeight*")->setAttribAll("Constant", kTRUE);
      parametersToy->selectByName("coef_all_5*")->setAttribAll("Constant", kTRUE);
      parametersToy->selectByName("coef_all_1*")->setAttribAll("Constant", kTRUE);
      parametersToy->selectByName("coefPT_*")->setAttribAll("Constant", kTRUE);
    }
    else
    {
      parametersToy->selectByName("*BWeight*_all_1*")->setAttribAll("Constant", kTRUE);
      parametersToy->selectByName("coef_all_5")->setAttribAll("Constant", kTRUE);
      parametersToy->selectByName("coef_all_1")->setAttribAll("Constant", kTRUE);
      parametersToy->selectByName("coefPT_*")->setAttribAll("Constant", kTRUE);
    }
  }

  if (channel<2)
  {
    if (totalPDF->getVariables()->find("coefBWeight_lctau_all_11_pTetabin0_weightbin0")!=0)
    {
      for (unsigned int r=0;r<nbins;r++)
      {

	TString condition("coefBWeight_lctau_all_11_pTetabin");
	condition+=r;
	condition+="_weightbin0";

	TString condition2("coefBWeight_lctau_all_51_pTetabin");
	condition2+=r;
	condition2+="_weightbin0";

	double value=1.5e-2;
	if (optionSample==-1)
	{
	  value=((RooRealVar*)totalPDF->getVariables()->find(condition2))->getVal();
	}
	((RooRealVar*)totalPDF->getVariables()->find(condition))->setVal(value);
	((RooRealVar*)totalPDF->getVariables()->find(condition))->setConstant(kTRUE);
	if (runToy)
	{
	  ((RooRealVar*)totalPDFtoy->getVariables()->find(condition))->setVal(value);
	  ((RooRealVar*)totalPDFtoy->getVariables()->find(condition))->setConstant(kTRUE);
	}
      }
    }
  }
  else if (channel<4)
  {
    if (totalPDF->getVariables()->find("coefBWeight_lctau_all_551_pTetabin0_weightbin0")!=0)
    {
      //      ((RooRealVar*)totalPDF->getVariables()->find("coefBWeight_lctau_all_551_pTetabin0_weightbin0"))->setVal(1.1e-2);
      //      ((RooRealVar*)totalPDF->getVariables()->find("coefBWeight_lctau_all_551_pTetabin0_weightbin0"))->setConstant(kTRUE);
      if (runToy)
      {
	//        ((RooRealVar*)totalPDFtoy->getVariables()->find("coefBWeight_lctau_all_551_pTetabin0_weightbin0"))->setVal(1.1e-2);
	//        ((RooRealVar*)totalPDFtoy->getVariables()->find("coefBWeight_lctau_all_551_pTetabin0_weightbin0"))->setConstant(kTRUE);
      }
    }



    if (totalPDF->getVariables()->find("coefBWeight_lctau_all_111_pTetabin0_weightbin0")!=0)
    {

      for (unsigned int r=0;r<nbins;r++)
      {

	TString condition("coefBWeight_lctau_all_111_pTetabin");
	condition+=r;
	condition+="_weightbin0";

	TString condition2("coefBWeight_lctau_all_511_pTetabin");
	condition2+=r;
	condition2+="_weightbin0";

	double value=1.5e-2;
	if (optionSample==-1)
	{
	  value=((RooRealVar*)totalPDF->getVariables()->find(condition2))->getVal();
	}
	((RooRealVar*)totalPDF->getVariables()->find(condition))->setVal(value);
	((RooRealVar*)totalPDF->getVariables()->find(condition))->setConstant(kTRUE);
	if (runToy)
	{
	  ((RooRealVar*)totalPDFtoy->getVariables()->find(condition))->setVal(value);
	  ((RooRealVar*)totalPDFtoy->getVariables()->find(condition))->setConstant(kTRUE);
	}
      }
    }
  }


  //Setting up mistag SFs


  {
    RooArgSet* listOfParsFixed=totalPDF->getVariables();
    TIterator* iterator=listOfParsFixed->createIterator();  
    TObjString* arg=0;
    while( ( arg= (TObjString*)iterator->Next())&&useoldmistag&&!tracks) 
    {
      RooAbsArg&  varArg= (*listOfParsFixed)[arg->GetName()];
      RooRealVar* var=dynamic_cast<RooRealVar*>(&varArg);
      if (TString(arg->GetName()).Contains("coefBWeight_lctau_all_11") ||
	  TString(arg->GetName()).Contains("coefBWeight_lctau_all_51") ||
	  TString(arg->GetName()).Contains("coefBWeight_lctau_all_111") ||
	  TString(arg->GetName()).Contains("coefBWeight_lctau_all_511") ||
	  TString(arg->GetName()).Contains("coefBWeight_lctau_all_551") ||
	  TString(arg->GetName()).Contains("coefBWeight_lctau_all_1"))
      {
	cout << " Setting mistag " << arg->GetName() << " from " << var->getVal() << " to " << var->getVal()*varMistag << endl;


	TString pTs("pTetabin");
	for (int r=0;r<7;r++)
	{
	  TString pTthis=pTs;
	  pTthis+=r;
	  pTthis+="_";

	  if (nbins!=7)
	  {
	    cout << " Can't run with nbins != 7. Mistag rates hard-coded..." << endl;
	    throw;
	  }

	  if (TString(arg->GetName()).Contains(pTthis))
	  {
	    cout << " was " << var->getVal() << " will be " << var->getVal()*mistagBin[r] << endl;

	    var->setVal(var->getVal()*mistagBin[r]);
	    //	    var->setVal(var->getVal());
	    if (varMistag>1.001&&varMistag<2.0)
	    {
	      var->setVal(var->getVal()/mistagBin[r]*(mistagBin[r]+mistagBinErr_c[r]));
	    }
	    else if (varMistag<0.999&&varMistag>0.6)
	    {
	      var->setVal(var->getVal()/mistagBin[r]*(mistagBin[r]-mistagBinErr_c[r]));
	    }
	    else if (varMistag>1.001)
	    {
	      var->setVal(var->getVal()/mistagBin[r]*(mistagBin[r]+mistagBinErr[r]));
	    }
	    else if (varMistag<0.999)
	    {
	      var->setVal(var->getVal()/mistagBin[r]*(mistagBin[r]-mistagBinErr[r]));
	    }
	    std::cout<<"********************HERE ARE THE VALUES FOR MISTAG SFs************************"<<std::endl;
	    std::cout<<"                                                                              "<<std::endl;
	    std::cout<<"                                                                              "<<std::endl;
	    std::cout<<"                                                                              "<<std::endl;
	    std::cout<<"                                                                              "<<std::endl;
	    std::cout<<"                                                                              "<<std::endl;
	    std::cout<<" r= "<<r<< " SF= "<< mistagBin[r]<< "  SFerr= "<< mistagBinErr[r] <<std::endl;
	    std::cout<<"                                                                              "<<std::endl;
	    std::cout<<"                                                                              "<<std::endl;
	    std::cout<<"                                                                              "<<std::endl;
	    std::cout<<"                                                                              "<<std::endl;
	    std::cout<<"                                                                              "<<std::endl;
	    std::cout<<"******************************************************************************"<<std::endl;
	  }
	}
      }
    }
  }

  RooAbsPdf* myConstraintFitFinal=0;

  bool applyConstraint=false;
  if (optionSample==-1)
  {
    applyConstraint=false;
  }


  if (applyConstraint)
  {


    double uncertainty0=0.15;
    double uncertainty55=0.1;


    //  RooRealVar* coef_all_50=(RooRealVar*)totalPDF->getVariables()->find("coef_all_50");
    RooRealVar* coef_all_0=(RooRealVar*)totalPDF->getVariables()->find("coef_all_0");
    RooRealVar* coef_all_55=(RooRealVar*)totalPDF->getVariables()->find("coef_all_55");

    //  RooRealVar* coef_all_50_medium=new RooRealVar("coef_all_50_medium","coef_all_50_medium",coef_all_50->getVal(),0,1e10);
    //  RooRealVar* coef_all_50_unc=new RooRealVar("coef_all_50_unc","coef_all_50_unc",uncertainty50*coef_all_50->getVal(),0,1e10);

    RooRealVar* coef_all_0_medium=new RooRealVar("coef_all_0_medium","coef_all_0_medium",coef_all_0->getVal(),0,1e10);
    RooRealVar* coef_all_0_unc=new RooRealVar("coef_all_0_unc","coef_all_0_unc",uncertainty0*coef_all_0->getVal(),0,1e10);

    RooRealVar* coef_all_55_medium=new RooRealVar("coef_all_55_medium","coef_all_55_medium",coef_all_55->getVal(),0,1e10);
    RooRealVar* coef_all_55_unc=new RooRealVar("coef_all_55_unc","coef_all_55_unc",uncertainty55*coef_all_55->getVal(),0,1e10);

    //  coef_all_50_unc->setConstant(kTRUE);
    coef_all_55_unc->setConstant(kTRUE);

    coef_all_0_unc->setConstant(kTRUE);
    coef_all_0_medium->setConstant(kTRUE);

    //  coef_all_50_medium->setConstant(kTRUE);
    coef_all_55_medium->setConstant(kTRUE);

    RooGaussian* constraint_55=new RooGaussian("yield55UncPDF",
					       "yield55UncPDF",
					       *coef_all_55,
					       *coef_all_55_medium,
					       *coef_all_55_unc);
    //
    //  RooGaussian* constraint_50=new RooGaussian("yield50UncPDF",
    //                                             "yield50UncPDF",
    //                                             *coef_all_50,
    //                                             *coef_all_50_medium,
    //                                             *coef_all_50_unc);

    RooGaussian* constraint_0=new RooGaussian("yield0UncPDF",
					      "yield0UncPDF",
					      *coef_all_0,
					      *coef_all_0_medium,
					      *coef_all_0_unc);



    myConstraintFitFinal=new RooProdPdf("myConstrainPDFFinal",
					"myConstrainPDFFinal",
					RooArgList(*constraint_55,
						   //*constraint_50,
						   *constraint_0));

  }



  //    ((RooRealVar*)totalPDF->getVariables()->find("coefPT_all_55_pTetabin0"))->setConstant(kTRUE);

  TIterator* iter=totalPDF->getVariables()->createIterator();

  bool weighted=true;
  bool hessian=false;
  if (optionSample==-1) 
  {
    weighted=false;//SHOULD BE AUTOMATIC IN FUTURE!!
    hessian=true;
  }


  totalPDF->Print("v");
  totalPDF->Print("all");


  //here in the case of toys you need to build your own dataset, no doubt!

  TTree* treeToy=0;

  vector<float> toy_truebeff;
  vector<float> toy_fitbeff;
  vector<float> toy_fitbeff_err;


  if (runToy)
  {
    RooRandom::randomGenerator()->SetSeed(randomNumber);

    treeToy=new TTree("treeToy","treeToy");

    treeToy->Branch("toy_truebeff",&toy_truebeff,32000,0);
    treeToy->Branch("toy_fitbeff",&toy_fitbeff,32000,0);
    treeToy->Branch("toy_fitbeff_err",&toy_fitbeff_err,32000,0);
  }



  int niterations=1;
  if (!runToy) niterations=1;

  for (unsigned int s=0;s<niterations;s++)
  {



    if (runToy)
    {

      toy_truebeff.clear();
      toy_fitbeff.clear();
      toy_fitbeff_err.clear();


      for (unsigned int r=0;r<nbins;r++)
      {
	TString name("coefBWeight_b__pTetabin");
	name+=r;
	name+="_weightbin0";

	double origEff=    ((RooRealVar*)totalPDFtoy->getVariables()->find(name))->getVal();

	double newEff=origEff*      RooRandom::randomGenerator()->Uniform(0.7,1.1);

	toy_truebeff.push_back(newEff);

	((RooRealVar*)totalPDFtoy->getVariables()->find(name))->setVal(newEff);
      }

      //    int Nevents=RooRandom::randomGenerator().Poisson(totalExpN);
      //    cout << " N events exp " << totalExpN << " generating " << Nevents << endl;

      //totalExpN

      std::vector<TString> samples;
      if (channel<=1)
      {
	samples.push_back("all_55");
	samples.push_back("all_51");
	samples.push_back("all_11");
      }
      else
      {
	samples.push_back("all_555");
	samples.push_back("all_551");
	samples.push_back("all_511");
	samples.push_back("all_111");
      }


      std::vector<double> coefficients;

      double add=0;
      for (unsigned int u=0;u<samples.size()-1;u++)
      {
	TString namevar("coef_");
	namevar+=samples.at(u);
	double coeff=((RooRealVar*)set->find(namevar))->getVal();
	add+=coeff;
	cout << samples.at(u) << " setting coeff to: " << coeff << endl;
	coefficients.push_back(coeff);
      }
      cout << " last coeff is " << 1.-add << endl;
      coefficients.push_back(1.-add);



      RooArgSet varsGen;
      if (channel<=1)
      {
	varsGen.add(*pTcategory);
	varsGen.add(*pTcategory2);
	varsGen.add(*pt1);
	varsGen.add(*pt2);
	varsGen.add(*eta1);
	varsGen.add(*eta2);
	varsGen.add(*bweight1);
	varsGen.add(*bweight2);
      }
      else 
      {
	varsGen.add(*pTcategoryJet0);
	varsGen.add(*pTcategoryJet1);
	varsGen.add(*pTcategoryJet2);
	varsGen.add(*pt1);
	varsGen.add(*pt2);
	varsGen.add(*pt3);
	varsGen.add(*eta1);
	varsGen.add(*eta2);
	varsGen.add(*eta3);
	varsGen.add(*bweight1);
	varsGen.add(*bweight2);
	varsGen.add(*bweight3);
      }

      dataset=new RooDataSet("genDS","genDS",varsGen);
      //the above will be the final generated dataset 

      for (unsigned int u=0;u<samples.size();u++)
      {
	cout << " starting generation for " << samples.at(u) << endl;


	for (unsigned int l=0;l<samples.size()-1;l++)
	{
	  TString namevar("coef_");
	  namevar+=samples.at(l);

	  if (l==u)
	  {
	    ((RooRealVar*)totalPDFtoy->getVariables()->find(namevar))->setVal(0.99999999);
	  }
	  else
	  {
	    ((RooRealVar*)totalPDFtoy->getVariables()->find(namevar))->setVal(0.00000001);
	  }
	}

	//now all are set to zero except for the one of interest

	//1. first need to generate the PTs

	RooArgSet varsToGen;
	if (channel<=1)
	{
	  varsToGen.add(*pTcategory);
	}
	else 
	{
	  varsToGen.add(*pTcategoryJet0);
	  varsToGen.add(*pTcategoryJet1);
	  varsToGen.add(*pTcategoryJet2);
	}

	double NeventExp= totalExpN * coefficients.at(u);
	int Ngen=RooRandom::randomGenerator()->Poisson(NeventExp);

	cout << " coeff " << coefficients.at(u) << " x Ntotal " << totalExpN << " exp: " << NeventExp << " gen: " << Ngen << endl;


	//      RooDataSet* tempDS=totalPDFtoy->generate(varsToGen,Ngen,RooFit::Verbose(kTRUE));
	if (channel<=1)
	{
	  pTcategory->setBins(nbins*nbins);
	}
	else
	{
	  pTcategoryJet0->setBins(nbins);
	  pTcategoryJet1->setBins(nbins);
	  pTcategoryJet2->setBins(nbins);
	}

	RooDataSet* tempDSbinned=totalPDFtoy->generate(varsToGen,Ngen);

	tempDSbinned->Print("v");


	RooDataSet* tempDS=new RooDataSet("tempDS","tempDS",varsToGen);

	for (unsigned int i =0;i<tempDSbinned->numEntries();i++)
	{

	  const RooArgSet* temp =tempDSbinned->get(i);

	  int multiplicity=(int)(tempDSbinned->weight()+0.5);

	  if (channel<=1)
	  {
	    pTcategory->setVal(((RooRealVar*)temp->find("ptcategory"))->getVal());
	  }
	  else
	  {
	    pTcategoryJet0->setVal(((RooRealVar*)temp->find("ptcategoryJet0"))->getVal());
	    pTcategoryJet1->setVal(((RooRealVar*)temp->find("ptcategoryJet1"))->getVal());
	    pTcategoryJet2->setVal(((RooRealVar*)temp->find("ptcategoryJet2"))->getVal());
	  }


	  //        cout << " adding bin " << i << " n. times " << multiplicity << endl;
	  for (unsigned int r = 0; r<multiplicity;r++)
	  {
	    tempDS->add(varsToGen);
	  }
	}

	tempDS->Print("v");

	//now generated the PTs, now need to translate that into real pTs
	RooArgSet varsForProtoDS;
	if (channel<=1)
	{
	  //        varsForProtoDS.add(*pTcategory);
	  //        varsForProtoDS.add(*pTcategory2);
	  varsForProtoDS.add(*pt1);
	  varsForProtoDS.add(*pt2);
	  varsForProtoDS.add(*eta1);
	  varsForProtoDS.add(*eta2);
	}
	else 
	{
	  //        varsForProtoDS.add(*pTcategoryJet0);
	  //        varsForProtoDS.add(*pTcategoryJet1);
	  //        varsForProtoDS.add(*pTcategoryJet2);
	  varsForProtoDS.add(*pt1);
	  varsForProtoDS.add(*pt2);
	  varsForProtoDS.add(*pt3);
	  varsForProtoDS.add(*eta1);
	  varsForProtoDS.add(*eta2);
	  varsForProtoDS.add(*eta3);
	}


	RooDataSet* protoDS=new RooDataSet("protoDS","protoDS",varsForProtoDS);

	for (unsigned int i=0;i<Ngen;i++)
	{
	  const RooArgSet* temp =tempDS->get(i);

	  if (channel<=1)
	  {
	    int ptcat=((RooRealVar*)temp->find("ptcategory"))->getVal();
	    pTcategory->setVal(ptcat);
	    int ptcat2=ptcat/nbins+(ptcat%nbins)*nbins;
	    pTcategory2->setVal(ptcat2);

	    //now set pt1,pt2,eta1,eta2
	    int ptcatIdx1=ptcat/nbins;
	    int ptcatIdx2=ptcat%nbins;

	    std::pair<double,double> pteta1=getCentralPTETA(ptcatIdx1,nbins);
	    std::pair<double,double> pteta2=getCentralPTETA(ptcatIdx2,nbins);

	    pt1->setVal(pteta1.first);
	    pt2->setVal(pteta2.first);
	    eta1->setVal(pteta1.second);
	    eta2->setVal(pteta2.second);
	  }
	  else
	  {
	    int ptcat1=((RooRealVar*)temp->find("ptcategoryJet0"))->getVal();
	    int ptcat2=((RooRealVar*)temp->find("ptcategoryJet1"))->getVal();
	    int ptcat3=((RooRealVar*)temp->find("ptcategoryJet2"))->getVal();

	    std::pair<double,double> pteta1=getCentralPTETA(ptcat1,nbins);
	    std::pair<double,double> pteta2=getCentralPTETA(ptcat2,nbins);
	    std::pair<double,double> pteta3=getCentralPTETA(ptcat3,nbins);

	    pt1->setVal(pteta1.first);
	    pt2->setVal(pteta2.first);
	    pt3->setVal(pteta3.first);

	    eta1->setVal(pteta1.second);
	    eta2->setVal(pteta2.second);
	    eta3->setVal(pteta3.second);
	  }

	  protoDS->add(varsForProtoDS);

	}


	//now you have the protodataset, generate the weight distribution


	RooArgSet varsWithBWeight;

	if (channel<=1)
	{
	  //        varsWithBWeight.add(*pTcategory);
	  //        varsWithBWeight.add(*pTcategory2);
	  //        varsWithBWeight.add(*pt1);
	  //        varsWithBWeight.add(*pt2);
	  //        varsWithBWeight.add(*eta1);
	  //        varsWithBWeight.add(*eta2);
	  varsWithBWeight.add(*bweight1);
	  varsWithBWeight.add(*bweight2);
	}
	else 
	{
	  //        varsWithBWeight.add(*pTcategoryJet0);
	  //        varsWithBWeight.add(*pTcategoryJet1);
	  //        varsWithBWeight.add(*pTcategoryJet2);
	  //        varsWithBWeight.add(*pt1);
	  //        varsWithBWeight.add(*pt2);
	  //        varsWithBWeight.add(*pt3);
	  //        varsWithBWeight.add(*eta1);
	  //        varsWithBWeight.add(*eta2);
	  //        varsWithBWeight.add(*eta3);
	  varsWithBWeight.add(*bweight1);
	  varsWithBWeight.add(*bweight2);
	  varsWithBWeight.add(*bweight3);
	}



	RooDataSet* genBweightDS=totalPDFtoy->generate(varsWithBWeight,
						       RooFit::ProtoData(*protoDS));

	//now go over it and create final dataset
	RooArgSet varsFinal;

	if (channel<=1)
	{
	  varsFinal.add(*pTcategory);
	  varsFinal.add(*pTcategory2);
	  varsFinal.add(*pt1);
	  varsFinal.add(*pt2);
	  varsFinal.add(*eta1);
	  varsFinal.add(*eta2);
	  varsFinal.add(*bweight1);
	  varsFinal.add(*bweight2);
	}
	else 
	{
	  varsFinal.add(*pTcategoryJet0);
	  varsFinal.add(*pTcategoryJet1);
	  varsFinal.add(*pTcategoryJet2);
	  varsFinal.add(*pt1);
	  varsFinal.add(*pt2);
	  varsFinal.add(*pt3);
	  varsFinal.add(*eta1);
	  varsFinal.add(*eta2);
	  varsFinal.add(*eta3);
	  varsFinal.add(*bweight1);
	  varsFinal.add(*bweight2);
	  varsFinal.add(*bweight3);
	}



	RooDataSet* almostFinal=new RooDataSet("almostFinal","almostFinal",varsFinal);

	//  RooRealVar* pTcategory=(RooRealVar*)set->find("ptcategory");
	//    RooRealVar* pTcategory2=(RooRealVar*)set->find("ptcategory2");

	//  RooRealVar* pTcategoryJet0=(RooRealVar*)set->find("ptcategoryJet0");
	//  RooRealVar* pTcategoryJet1=(RooRealVar*)set->find("ptcategoryJet1");
	//  RooRealVar* pTcategoryJet2=(RooRealVar*)set->find("ptcategoryJet2");


	for (unsigned int i=0;i<Ngen;i++)
	{
	  const RooArgSet* temp =tempDS->get(i);
	  const RooArgSet* temp2 =genBweightDS->get(i);


	  int nCombination=(int)(RooRandom::randomGenerator()->Uniform(0,(double)nCombinationsFit));

	  vector<vector<int> >* combinationsOfm4lnuVectorToUse=&combinationsOfm4lnuVector;
	  if (channel>=2)
	  {
	    combinationsOfm4lnuVectorToUse=&tree_combinationsOfm4lnuVector;
	  }

	  if (channel<=1)
	  {
	    int ptcat=((RooRealVar*)temp->find("ptcategory"))->getVal();
	    int ptcat2=ptcat/nbins+(ptcat%nbins)*nbins;

	    double pt1val=((RooRealVar*)temp2->find("pt_0"))->getVal();
	    double pt2val=((RooRealVar*)temp2->find("pt_1"))->getVal();

	    double eta1val=((RooRealVar*)temp2->find("eta_0"))->getVal();
	    double eta2val=((RooRealVar*)temp2->find("eta_1"))->getVal();

	    double bweight1val=((RooRealVar*)temp2->find("bweight_0"))->getVal();
	    double bweight2val=((RooRealVar*)temp2->find("bweight_1"))->getVal();

	    if (nCombination==1)//you need to exchange the pT of the jets if you exchange the b-weights!!!
	    {
	      double oldptcat=ptcat;
	      ptcat=ptcat2;
	      ptcat2=oldptcat;

	      double oldpt=pt1val;
	      pt1val=pt2val;
	      pt2val=oldpt;

	      double oldeta=eta1val;
	      eta1val=eta2val;
	      eta2val=oldeta;

	      double oldbweight=bweight1val;
	      bweight1val=bweight2val;
	      bweight2val=oldbweight;
	    }

	    pTcategory->setVal(ptcat);
	    pTcategory2->setVal(ptcat2);
	    pt1->setVal(pt1val);
	    pt2->setVal(pt2val);
	    eta1->setVal(eta1val);
	    eta2->setVal(eta2val);
	    bweight1->setVal(bweight1val);
	    bweight2->setVal(bweight2val);
	  }
	  else
	  {

	    int ptcat[3];
	    double ptval[3];
	    double etaval[3];
	    double bweightval[3];


	    ptcat[0]=((RooRealVar*)temp->find("ptcategoryJet0"))->getVal();
	    ptcat[1]=((RooRealVar*)temp->find("ptcategoryJet1"))->getVal();
	    ptcat[2]=((RooRealVar*)temp->find("ptcategoryJet2"))->getVal();

	    ptval[0]=((RooRealVar*)temp2->find("pt_0"))->getVal();
	    ptval[1]=((RooRealVar*)temp2->find("pt_1"))->getVal();
	    ptval[2]=((RooRealVar*)temp2->find("pt_2"))->getVal();

	    etaval[0]=((RooRealVar*)temp2->find("eta_0"))->getVal();
	    etaval[1]=((RooRealVar*)temp2->find("eta_1"))->getVal();
	    etaval[2]=((RooRealVar*)temp2->find("eta_2"))->getVal();

	    bweightval[0]=((RooRealVar*)temp2->find("bweight_0"))->getVal();
	    bweightval[1]=((RooRealVar*)temp2->find("bweight_1"))->getVal();
	    bweightval[2]=((RooRealVar*)temp2->find("bweight_2"))->getVal();

	    pTcategoryJet0->setVal(ptcat[combinationsOfm4lnuVectorToUse->at(nCombination).at(0)]);
	    pTcategoryJet1->setVal(ptcat[combinationsOfm4lnuVectorToUse->at(nCombination).at(1)]);
	    pTcategoryJet2->setVal(ptcat[combinationsOfm4lnuVectorToUse->at(nCombination).at(2)]);

	    pt1->setVal(ptval[combinationsOfm4lnuVectorToUse->at(nCombination).at(0)]);
	    pt2->setVal(ptval[combinationsOfm4lnuVectorToUse->at(nCombination).at(1)]);
	    pt3->setVal(ptval[combinationsOfm4lnuVectorToUse->at(nCombination).at(2)]);

	    eta1->setVal(etaval[combinationsOfm4lnuVectorToUse->at(nCombination).at(0)]);
	    eta2->setVal(etaval[combinationsOfm4lnuVectorToUse->at(nCombination).at(1)]);
	    eta3->setVal(etaval[combinationsOfm4lnuVectorToUse->at(nCombination).at(2)]);

	    bweight1->setVal(bweightval[combinationsOfm4lnuVectorToUse->at(nCombination).at(0)]);
	    bweight2->setVal(bweightval[combinationsOfm4lnuVectorToUse->at(nCombination).at(1)]);
	    bweight3->setVal(bweightval[combinationsOfm4lnuVectorToUse->at(nCombination).at(2)]);

	  }

	  almostFinal->add(varsFinal);
	}//end iteration

	//adding to total dataset

	dataset->append(*almostFinal);
      }//iterating over all subcategories

    }


    cout << " BEFORE RUNNING FIT " << endl;

    dataset->Print("v");

    RooArgSet* parameters=(RooArgSet*)totalPDF->getVariables();
    parameters->Print("v");

    RooFitResult* result=totalPDF->fitTo(*dataset,
					 RooFit::NumCPU(8),
					 RooFit::Save(kTRUE),
					 RooFit::Hesse(hessian),
//                                       RooFit::Minos(kTRUE),
//                                       RooFit::Verbose(kTRUE),
					 RooFit::Extended(kTRUE),
					 RooFit::SumW2Error(weighted),
					 RooFit::ConditionalObservables(condObs),
					 RooFit::ExternalConstraints(*myConstraintFitFinal));
    cout << "BEFORE FIT 3 " << endl;

    if (runToy)
    {
      for (unsigned int r=0;r<nbins;r++)
      {
	TString name("coefBWeight_b__pTetabin");
	name+=r;
	name+="_weightbin0";
    
	RooRealVar* myVar=(RooRealVar*)result->floatParsFinal().find(name);
	double val=myVar->getVal();
	double err=myVar->getError();
	toy_fitbeff.push_back(val);
	toy_fitbeff_err.push_back(err);
      }
      treeToy->Fill();
    }
  

    TString outputFileName("results");
    outputFileName+="_";
    outputFileName+=tagPoint;
    if (runToy)
    {
      outputFileName+="_toy";
    }
    outputFileName+="/";
    outputFileName+=systematics;
    if (channel>=2 && channel<4)
    {
      outputFileName+="_3jets";
    }
    if (channel==1 || channel==3  || channel==5 || channel==7)
    {
      outputFileName+="_sameflav";
    }
    if (channel>=4 && channel<6)
    {
      outputFileName+="_1dtagprobe";
    }
    if (channel>=6)
    {
      outputFileName+="_1dinclusive";
    }
    outputFileName+="_";
    if (optionSample>=0)
    {
      outputFileName+=optionSample;
    }
    if (systematicsToAddToOutputFile!="" && optionSample==-1)
    {
      outputFileName+="_Sys";
      outputFileName+=systematicsToAddToOutputFile;
    }
    if (runToy)
    {
      outputFileName+="_seed";
      outputFileName+=randomNumber;
    }
    outputFileName+=".root";

    cout << " Writing file " << outputFileName << endl;

    cout << "BEFORE FIT 5 " << endl;

    TFile* outputResult=new TFile(outputFileName,"recreate");
    result->Write();
    if (runToy)
    {
      treeToy->Write(0);
    }

    outputResult->Write();


    RooAbsArg* mynext=0;
  
    while ( ( mynext=(RooAbsArg*)iter->Next() ) )
    {
      RooRealVar* thisVar=(RooRealVar*)mynext;
      if (!thisVar) continue;
      cout << " ((RooRealVar*)totalPDF->getVariables()->find(\"" << 
        thisVar->GetName() << "\"))->setVal(" << thisVar->getVal() << ");" << endl;


    
    }

    iter->Reset();
  
    while ( ( mynext=(RooAbsArg*)iter->Next() ) )
    {
      RooRealVar* thisVar=(RooRealVar*)mynext;
      if (!thisVar) continue;

      cout << " ((RooRealVar*)totalPDF->getVariables()->find(\"" << 
        thisVar->GetName() << "\"))->setConstant(kTRUE);" << endl;

    }

    result->Print("v");

    Int_t ptAdditionalJet_nbins(11);
    double ptAdditionalJet_limits[]= {25e3,30e3,40e3,50e3,60e3,75e3,90e3,110e3,140e3,200e3,300e3,400e3};

    if(tracks)
    {
      Double_t ptAdditionalJet_limits_tracks[1000] = {10e3,20e3,30e3,50e3,60e3,80e3,100e3,140e3,200e3,250e3,300e3,400e3};
      for(int i=0;i<20;i++)
      {
	ptAdditionalJet_limits[i]=ptAdditionalJet_limits_tracks[i];
	//    std::cout << "Filling.... " << limitsPtEta1d[i] << std::endl;                                                                                                            
      }
    }
      
    RooBinning*  binning=new RooBinning(ptAdditionalJet_nbins,ptAdditionalJet_limits,
					"binning");
    pt1->setBinning(*binning);
    pt1->setMin(25e3);
    pt1->setMax(400e3);
      
    pt2->setBinning(*binning);
    pt2->setMin(25e3);
    pt2->setMax(400e3);

    RooBinning*  binning2=new RooBinning(nLimitsBtag,limitsBWeight,
					 "binning2");
    bweight1->setBinning(*binning2);
    bweight1->setMin(0.);
    bweight1->setMax(1.);
    bweight2->setBinning(*binning2);
 
    result->floatParsFinal().selectByName("coefBWeight_b*")->Print("v");


  }//end niterations
  
}
