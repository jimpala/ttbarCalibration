#ifndef DataObject_h
#define DataObject_h

#include "TRandom3.h"
#include "TTree.h"
#include <iostream>
#include <cmath>
#include <vector>
#include "TString.h"
#include "TH1F.h"
#include <map>
#include "jetnet/TTrainedNetwork.h"
#include "jetnet/doNormalization.C"
#include "TFile.h"
#include "TCanvas.h"
#include <cmath>
#include "TFoam.h"
#include "TFoamIntegrand.h"
//add TMVA
#include "TMVA/Reader.h"

class DataObject : public TFoamIntegrand
{
 public:
  DataObject(int iterationNumber,TString directory="jetnet",int nInputs=10,bool useBDT=false);
  ~DataObject() { delete m_reader; };

  ClassDef(DataObject,1.0);

  Double_t Density(Int_t nDim, Double_t *Xarg);
  Double_t EvaluateFinal(vector<double> inputsOrig);
  double getXvalue(double inputY, TH1F* histogramToUse);
  double getYvalue(double inputX, TH1F* histogramToUse);

  vector<TH1F*> histos;
  vector<TH1F*> histoCumulatives;
  vector<TH1F*> histoInvertedCumulatives;

private:
  
  bool m_useBDT;

  int m_nInputs;

  int m_nIteration;

  TString m_directory;

  //nice thing is that you need only one here
  TMVA::Reader* m_reader;
  float m_tempValues[20];

  vector<TTrainedNetwork*> trainedNetworksToUse;
  //use this for both NN and BDT...
  vector<TH1F*> calibrationNN;

  double minimumAllowed;
  double maximumAllowed;

  double minimumAllowed_startSmoothing;
  double maximumAllowed_startSmoothing;



};

#endif
