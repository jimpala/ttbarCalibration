/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 *    File: $Id: RhhBinnedPdfInCategories.h 30136 2011-03-09 18:36:59Z giacinto $
 * Authors:                                                                  *
 *    Aaron Roodman, Stanford Linear Accelerator Center, Stanford University *
 *    Modified by Wouter                                                     *
 *                                                                           *
 * Copyright (c) 2000-2004, Stanford University. All rights reserved.        *
 *           
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/
#ifndef RHH_BINNEDPDFINCATEGORIES_HH
#define RHH_BINNEDPDFINCATEGORIES_HH

#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "RooListProxy.h"
//
#include "RooAbsReal.h"
//
#include "TArrayD.h"
//
#include <iostream>
using namespace std;


class RooRealVar;
class RooArgList ;
class RooAbsCategory;

class RhhBinnedPdfInCategories : public RooAbsPdf {
private:
  Double_t localEval(const Double_t) const;

public:

  RhhBinnedPdfInCategories(const char *name, const char *title,
                           RooAbsReal& x, RooAbsReal& pt,RooAbsReal& eta,
                           int nPtEtaBins,const RooArgList& coefList, 
                           const TArrayD & limits,bool normalizaLastBin=true) ;

  RhhBinnedPdfInCategories();

  RhhBinnedPdfInCategories(const RhhBinnedPdfInCategories& other, const char* name = 0);
  virtual TObject* clone(const char* newname) const { return new RhhBinnedPdfInCategories(*this, newname); }
  virtual ~RhhBinnedPdfInCategories() ;

  Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName=0) const ;
  Double_t analyticalIntegral(Int_t code, const char* rangeName=0) const ;
  Int_t getnBins();
  Double_t* getLimits();
  bool isNormalizeLastBin();
  
protected:

  RooRealProxy _x;
  RooRealProxy _pt;
  RooRealProxy _eta;
  int _nPtEtaBins;
  RooListProxy _coefList ;
  TArrayD _limits;
  Int_t _nBins ;
  bool _normalizeLastBin;
  Double_t evaluate() const;

  ClassDef(RhhBinnedPdfInCategories,1) // Parametric Step Function Pdf
};

inline bool RhhBinnedPdfInCategories::isNormalizeLastBin()
{
  return _normalizeLastBin;
}


#endif
