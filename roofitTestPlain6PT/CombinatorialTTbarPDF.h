// -( Atlas Collaboration (C) )-
// New b-tagging ttbar calibration method
// Author: G. Piacquadio (SLAC - Stanford University)
// Date: 15.12.2012

#ifndef CombinatorialTTbarPDF_h
#define CombinatorialTTbarPDF_h

#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "RooListProxy.h"
#include "RooCategoryProxy.h"
#include "RooAbsReal.h"
#include "RooAbsCategory.h"
#include "TObject.h"
#include <vector>

using namespace std;
 
class DataObject;

class CombinatorialTTbarPDF: public RooAbsPdf {
public:
  CombinatorialTTbarPDF() {} ; 
  CombinatorialTTbarPDF(const char *name, const char *title,
                        RooArgList& xvec,//must be of size 10
                        int nCombination=0,
			int iterationNumber=4,
			TString directory="jetnet",
			double normalization=1.,
                        bool addLeptonicTopMassVars=true,
                        bool useBDT=false,
                        int symmetrytype=-1);
  CombinatorialTTbarPDF(const CombinatorialTTbarPDF& other, const char* name=0) ;
  virtual TObject* clone(const char* newname) const { return new CombinatorialTTbarPDF(*this,newname); }
  virtual ~CombinatorialTTbarPDF();
  

  virtual Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName=0) const ;
  virtual Double_t analyticalIntegral(Int_t code, const char* rangeName=0) const ;

protected:
  
  virtual Double_t evaluate() const ;

//  virtual void	generateEvent(Int_t code);

  virtual const DataObject & getMyPDFclass() const;

private:

  void initializeSymmetryCombinations();

  RooListProxy _x ;
  DataObject* myPDF;
  vector<vector<int> > combinationsOfm12Vector;
  vector<vector<int> > combinationsOfm4lnuVector;
  int m_nCombination;
  int m_nIteration;
  TString m_directory;
  double m_normalization;
  bool m_addLeptonicTopMassVars;
  bool m_useBDT;
  int m_symmetrytype;
  vector<int> m_symmetryCombinations;
  
  void fixCombinations(  vector<vector<int> > & combinationsOfm12Vector,
                         vector<vector<int> > & combinationsOfm4lnuVector);

  ClassDef(CombinatorialTTbarPDF,1) // Your description goes here...
};
 

inline  const DataObject & CombinatorialTTbarPDF::getMyPDFclass() const
{
  return *myPDF;
}




#endif
