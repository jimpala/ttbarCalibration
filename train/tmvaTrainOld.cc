#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <TFile.h>
#include <TChain.h>
#include <TString.h>
#include <TMVA/Factory.h>

int main(int argc, char * argv[]) {  

// Create ouput file, factory object and open the input file

  int jetnum = std::atoi(argv[1]);

  bool lowpt = ( std::atoi(argv[2]) == 1 );

  bool useMET = false;

  std::cout << "Low pt " << lowpt << std::endl;
  std::cout << "Jetnum " << jetnum << std::endl;

  for(int i=0;i<2;i++){

    TString output_file = (jetnum==2) ? "_2jet" : "_3jet";
    TString output_file_pt = (lowpt) ? "_lowpt" : "";
    TString output_file_met = (useMET) ? "_MET" : "";
    TString kFold = (i==0) ? "_even" : "_odd";

    TFile* outputFile = TFile::Open( "TMVA"+output_file+output_file_pt+output_file_met+kFold+".root", "RECREATE" );
    TMVA::Factory* factory = new TMVA::Factory("tmvaTest"+output_file+output_file_pt+output_file_met+kFold, outputFile, "");


    TString signal = "tree_55";
    TString background_A = "tree_51";
    TString background_B = "tree_11";

    if(jetnum==3){
      signal = "tree_551";
      background_A = "tree_511";
      background_B = "tree_111";
    }

    TChain* sig = new TChain(signal);

    std::vector<std::string> signalFiles;
    signalFiles.push_back("");
    signalFiles.push_back("SysMCAtNLO");
    signalFiles.push_back("SysPowhegHerwig");
    signalFiles.push_back("SysttbarRadHi");
    signalFiles.push_back("SysttbarRadLo");

    for(int r=0;r<signalFiles.size();r++){
      int entriesToGet = 30000;
      sig->Add(("/unix/atlasvhbb/abell/mc15data7/PGS/outputGeneral_fit_top"+signalFiles.at(r)+"_rescaling.root").c_str());
//      sig->Add(("/unix/atlasvhbb/abell/mc15data7/PGS/outputGeneral_fit_top"+signalFiles.at(r)+"_rescaling.root").c_str());
    }
//    TFile* inputFileSignal = new TFile("/unix/atlasvhbb/abell/mc15data7/PGS/outputGeneral_fit_top_all_rescaling.root");
    TFile* inputFile = new TFile("/unix/atlasvhbb/abell/mc15data7/PGS/outputGeneral_fit_all_rescaling.root");
//    TFile* inputFile = new TFile("/unix/atlasvhbb/abell/mc15data7/PGS/outputGeneral_fit_all_rescaling.root");

// get the TTree objects from the input files

//    TTree* sig = (TTree*)inputFileSignal->Get(signal);
    TTree* bkg_A = (TTree*)inputFile->Get(background_A);
    TTree* bkg_B = (TTree*)inputFile->Get(background_B);

    outputFile->cd();
// global event weights (see below for setting event-wise weights)

    double sigWeight = 1.0;
    double bkgWeight = 1.0;

    sig->SetupAddresses();

//  factory->AddSpectator("weight", 'F');

    TCut mycut_train = (i==0) ? "isEven" : "!isEven";
    TCut mycut_test = (i!=0) ? "isEven" : "!isEven";

    factory->AddTree(sig, "Signal", sigWeight, mycut_train, "train");
    factory->AddTree(sig, "Signal", sigWeight, mycut_test, "test");

    factory->AddTree(bkg_A, "Background", bkgWeight, mycut_train, "train");
    factory->AddTree(bkg_A, "Background", bkgWeight, mycut_test, "test");
    factory->AddTree(bkg_B, "Background", bkgWeight, mycut_train, "train");
    factory->AddTree(bkg_B, "Background", bkgWeight, mycut_test, "test");

// Define the input variables that shall be used for the MVA training
// (the variables used in the expression must exist in the original TTree).

    factory->SetSignalWeightExpression( "weight" );
    factory->SetBackgroundWeightExpression( "weight" );

    factory->AddVariable("imbalance", 'F');
    factory->AddVariable("leading_pt", 'F');
    factory->AddVariable("sub_leading_pt", 'F');
//     factory->AddVariable("forwardjets", 'I');
    if(jetnum==3) factory->AddVariable("sub_sub_leading_pt", 'F');
    factory->AddVariable("delta_phi", 'F');
    factory->AddVariable("delta_r", 'F');
    factory->AddVariable("mlj", 'F');
    if(useMET) factory->AddVariable("train_met", 'F');

    factory->AddVariable("minl1j", 'F');
    factory->AddVariable("minl2j", 'F');

//  factory->AddVariable("train_met", 'F');

    TCut mycut = "";

    if(lowpt){
      mycut = "sub_leading_pt < 0.03  && (leptontype==0 || (etmiss>60e3 && (mll<80e3 || mll>100e3) && mll>50e3))";
      if(jetnum==3) mycut = "sub_leading_pt < 0.03 || sub_sub_leading_pt < 0.03 && (leptontype==0 || (etmiss>60e3 && (mll<80e3 || mll>100e3) && mll>50e3))";
    }
    else {
      mycut = "sub_leading_pt > 0.03 && (leptontype==0 || (etmiss>60e3 && (mll<80e3 || mll>100e3) && mll>50e3))";
      if(jetnum==3) mycut = "sub_leading_pt > 0.03 && sub_sub_leading_pt > 0.03  && (leptontype==0 || (etmiss>60e3 && (mll<80e3 || mll>100e3) && mll>50e3))";
    }
  
// Tell the factory how to use the training and testing events
    factory->PrepareTrainingAndTestTree( mycut, "NormMode=None:!V" );
//    factory->PrepareTrainingAndTestTree( mycut && mycut2, "SplitMode=random:!V" );

// Book MVA methods (see TMVA manual).  

//  factory->BookMethod(TMVA::Types::kFisher, "Fisher", "H:!V:Fisher");  
//  factory->BookMethod(TMVA::Types::kBDT, "BDT", "NTrees=800");
//  factory->BookMethod(TMVA::Types::kBDT, "BDT_Advanced","IgnoreNegWeightsInTraining:!H:!V:NTrees=800:MaxDepth=4:BoostType=AdaBoost:AdaBoostBeta=0.15:SeparationType=GiniIndex:nCuts=100:PruneMethod=NoPruning");
//    factory->BookMethod(TMVA::Types::kBDT, "BDT_Overtrain_reference","IgnoreNegWeightsInTraining:!H:!V:NTrees=200:MaxDepth=4:BoostType=AdaBoost:AdaBoostBeta=0.15:SeparationType=GiniIndex:nCuts=100:PruneMethod=NoPruning");

    factory->BookMethod(TMVA::Types::kBDT, "BDT_Overtrain_reference","IgnoreNegWeightsInTraining:!H:!V:NTrees=200:MaxDepth=4:BoostType=Grad:Shrinkage=0.10:SeparationType=GiniIndex:nCuts=100:PruneMethod=NoPruning");

//    factory->BookMethod( TMVA::Types::kBDT, "BDTG","!H:!V:NTrees=1000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=2" );

//  factory->BookMethod(TMVA::Types::kBDT, "BDT_Overtrain_safe","IgnoreNegWeightsInTraining:!H:!V:NTrees=200:MaxDepth=4:BoostType=AdaBoost:AdaBoostBeta=0.15:SeparationType=GiniIndex:nCuts=100:PruneMethod=NoPruning");
//  factory->BookMethod(TMVA::Types::kBDT, "BDT_Overtrain_mega_safe","IgnoreNegWeightsInTraining:!H:!V:NTrees=100:MaxDepth=4:BoostType=AdaBoost:AdaBoostBeta=0.15:SeparationType=GiniIndex:nCuts=100:PruneMethod=NoPruning");


//  factory->BookMethod(TMVA::Types::kBDT, "BDT_Advanced_NoIgnore","!H:!V:NTrees=800:MaxDepth=4:BoostType=AdaBoost:AdaBoostBeta=0.15:SeparationType=GiniIndex:nCuts=100:PruneMethod=NoPruning");

//  factory->BookMethod(TMVA::Types::kBDT, "BDT_Advanced_Deep","!H:!V:NTrees=800:MaxDepth=100:BoostType=AdaBoost:AdaBoostBeta=0.15:SeparationType=GiniIndex:nCuts=100:PruneMethod=NoPruning");
//  factory->BookMethod(TMVA::Types::kBDT, "BDT_Advanced_original","!H:!V:NTrees=200:MaxDepth=4:BoostType=AdaBoost:AdaBoostBeta=0.15:SeparationType=GiniIndex:nCuts=100:NEventsMin=0.1:PruneMethod=NoPruning");

// Train, test and evaluate all methods

    factory->TrainAllMethods();
    factory->TestAllMethods();
    factory->EvaluateAllMethods();    

// Save the output and finish up

    outputFile->Close();
    std::cout << "==> wrote root file TMVA.root" << std::endl;
    std::cout << "==> TMVAnalysis is done!" << std::endl; 

    delete factory;
  }
  return 0;

}
