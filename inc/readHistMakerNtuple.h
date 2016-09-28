//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sun Jul 17 13:31:53 2016 by ROOT version 5.34/30
// from TTree tree_55/tree_55
// found on file: ./../../../PGS/outputGeneral_fit_all_rescaling_train.root
//////////////////////////////////////////////////////////

#ifndef readHistMakerNtuple_h
#define readHistMakerNtuple_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"

using namespace std;
// Fixed size dimensions of array or collections stored in the TTree if any.

class readHistMakerNtuple {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Float_t         mll;
   Float_t         ptll;
   Float_t         ptlep;
   Float_t         ptlep2;
   Float_t         etalep;
   Float_t         etalep2;
   Float_t         etmiss;
   Int_t           leptontype;
   Int_t           jetN;
   Double_t        mu;
   Double_t        PUweight;
   vector<int>     *flavours;
   vector<int>     *trueflavours;
   vector<float>   *bweight_mv2c20;
   vector<float>   *bweight_mv2c20_FLAT;
   Int_t           forwardjets;
   Float_t         pTextraforwardjet;
   vector<float>   *pT;
   vector<float>   *eta;
   vector<float>   *phi;
   Double_t        weight;
   Double_t        weight_track_70;
   Double_t        weight_track_77;
   Double_t        weight_MV2c2060;
   Double_t        weight_MV2c2070;
   Double_t        weight_MV2c2077;
   Double_t        weight_MV2c2085;
   Float_t         imbalance;
   Float_t         train_met;
   Float_t         mlj;
   Float_t         delta_phi;
   Float_t         leading_pt;
   Float_t         sub_leading_pt;
   Bool_t          subtraction;

   // List of branches
   TBranch        *b_mll;   //!
   TBranch        *b_ptll;   //!
   TBranch        *b_ptlep;   //!
   TBranch        *b_ptlep2;   //!
   TBranch        *b_etalep;   //!
   TBranch        *b_etalep2;   //!
   TBranch        *b_etmiss;   //!
   TBranch        *b_leptontype;   //!
   TBranch        *b_jetN;   //!
   TBranch        *b_mu;   //!
   TBranch        *b_PUweight;   //!
   TBranch        *b_flavours;   //!
   TBranch        *b_trueflavours;   //!
   TBranch        *b_bweight_mv2c20;   //!
   TBranch        *b_bweight_mv2c20_FLAT;   //!
   TBranch        *b_forwardjets;   //!
   TBranch        *b_pTextraforwardjet;   //!
   TBranch        *b_pT;   //!
   TBranch        *b_eta;   //!
   TBranch        *b_phi;   //!
   TBranch        *b_weight;   //!
   TBranch        *b_weight_track_70;   //!
   TBranch        *b_weight_track_77;   //!
   TBranch        *b_weight_MV2c2060;   //!
   TBranch        *b_weight_MV2c2070;   //!
   TBranch        *b_weight_MV2c2077;   //!
   TBranch        *b_weight_MV2c2085;   //!
   TBranch        *b_imbalance;   //!
   TBranch        *b_train_met;   //!
   TBranch        *b_mlj;   //!
   TBranch        *b_delta_phi;   //!
   TBranch        *b_leading_pt;   //!
   TBranch        *b_sub_leading_pt;   //!
   TBranch        *b_subtraction;   //!

   readHistMakerNtuple(TTree *tree=0);
   virtual ~readHistMakerNtuple();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef readHistMakerNtuple_cxx
readHistMakerNtuple::readHistMakerNtuple(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("./../../../PGS/outputGeneral_fit_all_rescaling_train.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("./../../../PGS/outputGeneral_fit_all_rescaling_train.root");
      }
      f->GetObject("tree_55",tree);

   }
   Init(tree);
}

readHistMakerNtuple::~readHistMakerNtuple()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t readHistMakerNtuple::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t readHistMakerNtuple::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void readHistMakerNtuple::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   flavours = 0;
   trueflavours = 0;
   bweight_mv2c20 = 0;
   bweight_mv2c20_FLAT = 0;
   pT = 0;
   eta = 0;
   phi = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("mll", &mll, &b_mll);
   fChain->SetBranchAddress("ptll", &ptll, &b_ptll);
   fChain->SetBranchAddress("ptlep", &ptlep, &b_ptlep);
   fChain->SetBranchAddress("ptlep2", &ptlep2, &b_ptlep2);
   fChain->SetBranchAddress("etalep", &etalep, &b_etalep);
   fChain->SetBranchAddress("etalep2", &etalep2, &b_etalep2);
   fChain->SetBranchAddress("etmiss", &etmiss, &b_etmiss);
   fChain->SetBranchAddress("leptontype", &leptontype, &b_leptontype);
   fChain->SetBranchAddress("jetN", &jetN, &b_jetN);
   fChain->SetBranchAddress("mu", &mu, &b_mu);
   fChain->SetBranchAddress("PUweight", &PUweight, &b_PUweight);
   fChain->SetBranchAddress("flavours", &flavours, &b_flavours);
   fChain->SetBranchAddress("trueflavours", &trueflavours, &b_trueflavours);
   fChain->SetBranchAddress("bweight_mv2c20", &bweight_mv2c20, &b_bweight_mv2c20);
   fChain->SetBranchAddress("bweight_mv2c20_FLAT", &bweight_mv2c20_FLAT, &b_bweight_mv2c20_FLAT);
   fChain->SetBranchAddress("forwardjets", &forwardjets, &b_forwardjets);
   fChain->SetBranchAddress("pTextraforwardjet", &pTextraforwardjet, &b_pTextraforwardjet);
   fChain->SetBranchAddress("pT", &pT, &b_pT);
   fChain->SetBranchAddress("eta", &eta, &b_eta);
   fChain->SetBranchAddress("phi", &phi, &b_phi);
   fChain->SetBranchAddress("weight", &weight, &b_weight);
   fChain->SetBranchAddress("weight_track_70", &weight_track_70, &b_weight_track_70);
   fChain->SetBranchAddress("weight_track_77", &weight_track_77, &b_weight_track_77);
   fChain->SetBranchAddress("weight_MV2c2060", &weight_MV2c2060, &b_weight_MV2c2060);
   fChain->SetBranchAddress("weight_MV2c2070", &weight_MV2c2070, &b_weight_MV2c2070);
   fChain->SetBranchAddress("weight_MV2c2077", &weight_MV2c2077, &b_weight_MV2c2077);
   fChain->SetBranchAddress("weight_MV2c2085", &weight_MV2c2085, &b_weight_MV2c2085);
   fChain->SetBranchAddress("imbalance", &imbalance, &b_imbalance);
   fChain->SetBranchAddress("train_met", &train_met, &b_train_met);
   fChain->SetBranchAddress("mlj", &mlj, &b_mlj);
   fChain->SetBranchAddress("delta_phi", &delta_phi, &b_delta_phi);
   fChain->SetBranchAddress("leading_pt", &leading_pt, &b_leading_pt);
   fChain->SetBranchAddress("sub_leading_pt", &sub_leading_pt, &b_sub_leading_pt);
   fChain->SetBranchAddress("subtraction", &subtraction, &b_subtraction);
   Notify();
}

Bool_t readHistMakerNtuple::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void readHistMakerNtuple::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t readHistMakerNtuple::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef readHistMakerNtuple_cxx
