 /*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitBabar                                                      *
 *    File: $Id: RhhBinnedPdfInCategories.cc,v 1.1.1.1 2008/01/15 15:36:53 mpierini Exp $
 * Authors:                                                                  *
 *    Aaron Roodman, Stanford Linear Accelerator Center, Stanford University *
 *    Adapted by Wouter                                                      *
 *                                                                           *
 * Copyright (c) 2004, Stanford University. All rights reserved.        *
 *           
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

// -- CLASS DESCRIPTION [PDF] --
//

// This is a reimplementation of the
// RooModels/RooParametricStepFunction. In the RooModels implementation
// the coefficient of bin n represents the density in bin n. In this
// implementation, the coefficient for bin n is the integral of the
// pdf in bin 'n', divided by the integral over bins [n,N]. The
// advantage of this approach is that as long as each coefficient is
// limited to values [0,1], there will never be a problem with the
// normalization.
//
// Analytical expressions for calculating each bin integral for a given set of
// parameters:
//    b1 = N p1
//    b2 = N p2 (1-p1)
//    b3 = N p3 (1-p1) (1-p2)
//    b4 = N p4 (1-p1) (1-p2) (1-p3)
//    ... 
//
// An example of usage is:
//
// Int_t nbins(10);
// TArrayD limits(nbins+1);
// limits[0] = 0.0; //etc...
// RooArgList* list = new RooArgList("list");
// RooRealVar* binHeight0 = new RooRealVar("binHeight0","bin 0 Value",0.1,0.0,1.0);
// list->add(binHeight0); // up to binHeight8, ie. 9 parameters
//
// RhhBinnedPdfInCategories  aPdf = ("aPdf","PSF",*x,*list,limits);
//

#include <math.h>

#include "RhhBinnedPdfInCategories.h"
#include "RooAbsReal.h"
#include "RooRealVar.h"
#include "RooArgList.h"

namespace 
{
#include "getCategory.icc"
}                        


ClassImp(RhhBinnedPdfInCategories);

RhhBinnedPdfInCategories::RhhBinnedPdfInCategories() {}

RhhBinnedPdfInCategories::RhhBinnedPdfInCategories(const char* name, const char* title, 
                                                   RooAbsReal& x, RooAbsReal& pt,
                                                   RooAbsReal& eta,
                                                   int nPtEtaBins,
                                                   const RooArgList& coefList, const TArrayD& limits,bool normalizeLastBin) :
  RooAbsPdf(name, title),
  _x("x", "Dependent", this, x),
  _pt("pt","Dependent2",this,pt),
  _eta("eta","Dependent3",this,eta),
  _nPtEtaBins(nPtEtaBins),
  _coefList("coefList","List of coefficients",this),
  _nBins(limits.GetSize()-1),
  _normalizeLastBin(normalizeLastBin)
{
  // Check lowest order
  if (_nBins<0) {
    cout << "RhhBinnedPdfInCategories::ctor(" << GetName() 
	 << ") WARNING: nBins must be >=0, setting value to 0" << endl ;
    _nBins=0 ;
  }

  TIterator* coefIter = coefList.createIterator() ;
  RooAbsArg* coef ;
  while(coef = (RooAbsArg*)coefIter->Next()) {
    if (!dynamic_cast<RooAbsReal*>(coef)) {
      cout << "RhhBinnedPdfInCategories::ctor(" << GetName() << ") ERROR: coefficient " << coef->GetName() 
	   << " is not of type RooAbsReal" << endl ;
      assert(0) ;
    }
    _coefList.add(*coef) ;
  }
  delete coefIter ;

  // Bin limits  
  limits.Copy(_limits);
}


RhhBinnedPdfInCategories::RhhBinnedPdfInCategories(const RhhBinnedPdfInCategories& other, const char* name) :
  RooAbsPdf(other, name), 
  _x("x", this, other._x), 
  _pt("pt",this,other._pt),
  _eta("eta",this,other._eta),
  _nPtEtaBins(other._nPtEtaBins),
  _coefList("coefList",this,other._coefList),
  _nBins(other._nBins),
  _normalizeLastBin(other._normalizeLastBin)
{
  // Copy constructor
  (other._limits).Copy(_limits);
}



RhhBinnedPdfInCategories::~RhhBinnedPdfInCategories()
{
}


Int_t RhhBinnedPdfInCategories::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName) const 
{
  if (allVars.getSize()==1 && allVars.contains(_x.arg()))
  {
    analVars.add(allVars);
    return 1;
  }
  return 0;
//  if (matchArgs(allVars, analVars, _x)) return 1;
}

Double_t RhhBinnedPdfInCategories::analyticalIntegral(Int_t code, const char* rangeName) const 
{
  assert(code==1) ;

  // Old method:
//   Double_t sum(1.0) ;
//   return sum;  

  //integral depends on _pt and _eta

  Double_t integral(0.0); Double_t width(0.0);
  Double_t average(0.0); 
  Double_t min(_x.min(rangeName)); Double_t max(_x.max(rangeName));
  Int_t first(0); Int_t last(_nBins-1);
  
  if (min >= _limits[0] && max <= _limits[_nBins]){



    // Get first and last bin
    int i=0 ;
    for (i=0; i<_nBins && min>=_limits[i];++i);
    {
      first = i-1; 
    }
    if(first < 0) first = 0;
    for (i=0; i<_nBins && max>=_limits[i];++i);
    {
      last = i-1;
    }
    if(last > _nBins-1) last = _nBins-1;
    
    if(first != last) {
      width = _limits[first+1] - _limits[first];
      average = _limits[first] + (width/2.0);
      width = _limits[first+1] - min;
      integral += (width * localEval(average));
      //      integral += (width * static_cast<RooRealVar*>(_coefList.at(first))->getVal());
      
//      std::cout << " first bin: " << width * localEval(average) << std::endl;

      width = _limits[last+1] - _limits[last];
      average = _limits[last] + (width/2.0);
      width = max - _limits[last];
      integral += (width * localEval(average));
      //      integral += (width * static_cast<RooRealVar*>(_coefList.at(last))->getVal());
      
      //    std::cout << " last bin: " << width * localEval(average) << std::endl;

      for(i=first+1; i < last; i++) {
        width = _limits[i+1] - _limits[i];
        average = _limits[i] + (width/2.0);
        integral += (width * localEval(average));
//        std::cout << " bin: " <<  i << " int: " << width * localEval(average) << std::endl;
        //	integral += (width * static_cast<RooRealVar*>(_coefList.at(i))->getVal());
      }
    }
    else {
      width = _limits[first+1] - _limits[first];
      average = _limits[first] + (width/2.0);
      width = max - min;
      integral += (width * localEval(average));
      //      integral += (width * static_cast<RooRealVar*>(_coefList.at(first))->getVal());
    }
  }
  else
  {
//    std::cout << " NOT IN THE LIMITS " << std::endl;
  }
  
  if (integral<=0)
  {
    std::cout<< "integral " << integral << " _limits[first] " << _limits[first] << " _limits[last] " << _limits[last] << std::endl;
    std::cout<< "nbins " << _nBins << std::endl;
    throw;
  }

//  std::cout << " integral: " << integral << std::endl;
  
  return integral;
}


Double_t RhhBinnedPdfInCategories::evaluate() const 
{

  //evaluates needs to depend on pt and eta

  const Double_t xval = _x;
  return localEval(xval);
}

Double_t RhhBinnedPdfInCategories::localEval(const Double_t xval) const
{

  int categoryNumber=getCategory(_pt,_eta,_nPtEtaBins);
  
  //periodicity is _nBins-1, so "+(_nBins-1)*categoryNumber"
  //when accessing coefList

  Double_t value(0);
  if (xval >= _limits[0] && xval < _limits[_nBins]){
    double sum(0),binval(0);
    int i=0 ;
    if (_normalizeLastBin)
    {
      for (i=0; i<_nBins-1 && xval>=_limits[i];++i) {
        binval = (1-sum)*static_cast<RooRealVar*>(_coefList.at(i+(_nBins-1)*categoryNumber))->getVal() ;
//        std::cout << " LASTBIN! coeff is: " << static_cast<RooRealVar*>(_coefList.at(i-2+(_nBins-1)*categoryNumber))->getVal() << std::endl;
        //binval = static_cast<RooRealVar*>(_coefList.at(i+(_nBins-1)*categoryNumber))->getVal() ;
        sum    += binval ;
      }
      if( xval>=_limits[_nBins-1] ) { // the last bin
        binval = 1-sum ;
        i = _nBins ;
      }
    }
    else//now go from right to left
    {
      for (i=_nBins; i>1 && xval<=_limits[i];--i) {
//        std::cout << " coeff is: " << static_cast<RooRealVar*>(_coefList.at(i-2))->getVal() << std::endl;
        binval = (1-sum)*static_cast<RooRealVar*>(_coefList.at(i-2+(_nBins-1)*categoryNumber))->getVal() ;
        //binval = static_cast<RooRealVar*>(_coefList.at(i))->getVal() ;
        sum    += binval ;
      }
      if( xval<=_limits[1] ) { // the first bin
        binval = 1-sum ;
        i = 0 ;
      }
    }
    
//    for (int u=0;u<_nBins+1;u++)
//    {
//      cout << " limit(" << u << ")" << " = " << _limits[u] << endl;
//    }

    double binwidth = 0;
    if (!_normalizeLastBin)
    {
      binwidth = _limits[i+1] - _limits[i];
    }
    else
    {
      binwidth=_limits[i] - _limits[i-1];
    }

    value = binval/binwidth ;
//    std::cout << " x: " << xval << " value : " << binval << " binwidth: " << binwidth << std::endl;
    if (value<=0){
      cout << "RhhBinnedPdfInCategories: sum of values gt 1.0 -- beware!!" 
	   << value << " " << binval << " " << sum << " " << i << " " << xval << endl;
      value = 0.000001;
    }
  }
  else
  {
    cout << " value: " << xval << 
        " outside limits: " << _limits[0] << 
        " and " << _limits[_nBins] << endl;
  }
        
  return value;
}

Int_t RhhBinnedPdfInCategories::getnBins(){
  return _nBins;
}

Double_t* RhhBinnedPdfInCategories::getLimits(){
  Double_t* limoutput = _limits.GetArray();
  return limoutput;
}
