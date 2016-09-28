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
#ifndef RHH_BINNEDPDFINPTCATEGORIES_HH
#define RHH_BINNEDPDFINPTCATEGORIES_HH

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

class RhhBinnedPdfInPtCategories : public RooAbsPdf {
private:
  Double_t localEval(const Double_t) const;

public:

  RhhBinnedPdfInPtCategories(const char *name, const char *title,
                           RooAbsReal& x, RooAbsReal& pt,
                           int nPtBins,const RooArgList& coefList, 
                           const TArrayD & limits,bool normalizaLastBin=true) ;

  RhhBinnedPdfInPtCategories();

  RhhBinnedPdfInPtCategories(const RhhBinnedPdfInPtCategories& other, const char* name = 0);
  virtual TObject* clone(const char* newname) const { return new RhhBinnedPdfInPtCategories(*this, newname); }
  virtual ~RhhBinnedPdfInPtCategories() ;

  Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName=0) const ;
  Double_t analyticalIntegral(Int_t code, const char* rangeName=0) const ;
  Int_t getnBins();
  Double_t* getLimits();
  bool isNormalizeLastBin();
  
protected:

  RooRealProxy _x;
  RooRealProxy _pt;
  int _nPtBins;
  RooListProxy _coefList ;
  TArrayD _limits;
  Int_t _nBins ;
  bool _normalizeLastBin;
  Double_t evaluate() const;

  ClassDef(RhhBinnedPdfInPtCategories,1) // Parametric Step Function Pdf
};

inline bool RhhBinnedPdfInPtCategories::isNormalizeLastBin()
{
  return _normalizeLastBin;
}


#endif
