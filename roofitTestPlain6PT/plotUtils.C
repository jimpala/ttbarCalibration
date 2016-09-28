#include "TString.h"
#include "TFile.h"
#include <vector>
#include <map>
#include "TH1F.h"
#include <iostream>
#include <fstream>
#include "TROOT.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"
#include <TLatex.h>
#include <TMarker.h>
#include <TLine.h>
#include "TIterator.h"
#include "THStack.h"
#include "TSystem.h"

class sample {
public:
  sample(){}

  sample(TString t_sampleName): sampleName(t_sampleName){
    lineColour=kBlack;
    markerColour=kBlack;
    fillColour=kWhite;

    lineStyle=kSolid;
    markerStyle=20;
    fillStyle=1001;
    lineWidth = 1;
    hist = 0;
    histUp.clear();
    histDo.clear();
    nEvents = 0;
    totFracUp=0;
    totFracDo=0;
    FracUp.clear();
    FracDo.clear();
    crossSection = 0;
    selected = 0;
    selectedEventsMCErr = 0;  
    norm = 0;
  }

  void add(TString t_histName){
    histName.push_back(t_histName);
  }
  
sample(TString t_sampleName, TString t_histName,int t_fillColour=kWhite, int t_markerColour=kBlack, int t_lineColour=kBlack):
    sampleName(t_sampleName),
    lineColour(t_lineColour),
    markerColour(t_markerColour),
    fillColour(t_fillColour){
    histName.push_back(t_histName);
    lineStyle=kSolid;
    markerStyle=20;
    fillStyle=1001;
    lineWidth = 1;
    hist = 0;
    histUp.clear();
    histDo.clear();
    nEvents = 0;
    totFracUp=0;
    totFracDo=0;
    FracUp.clear();
    FracDo.clear();
    crossSection = 0;
    selected = 0;
    selectedEventsMCErr = 0;  
    norm = 0;
  }
  
  void clear(){
    hist = 0;
    histUp.clear();
    histDo.clear();
    histName.clear();
    splitHists.clear();
    events.clear();
    eventsErr.clear();
    nEvents = 0;
    totFracUp=0;
    totFracDo=0;
    FracUp.clear();
    FracDo.clear();
    crossSection = 0;
    selected = 0;
    selectedEventsMCErr = 0;
    norm = 0;
  }

  void formatHist(TH1F *h1, int ls=kSolid, bool fill=true){
    if(fill){
      h1->SetLineColor(lineColour);
      h1->SetFillColor(fillColour);
      h1->SetFillStyle(fillStyle);
    }
    else{
      h1->SetLineColor(kBlack);
      h1->SetFillStyle(4000);
    }
//    h1->SetMarkerSize(2);
    h1->SetMarkerColor(markerColour);
    h1->SetLineStyle(ls);
    h1->SetMarkerStyle(markerStyle);
    h1->SetLineWidth(lineWidth);
  }

  void format(int rebin=1){
    if(hist) formatHist(hist, lineStyle, true);
    else return;
    for(vector<TH1F*>::iterator histIter = splitHists.begin(); histIter != splitHists.end(); ++histIter) formatHist((*histIter),  lineStyle, true);    
    //for(vector<TH1F*>::iterator histIter = histUp.begin(); histIter != histUp.end(); ++histIter) formatHist((*histIter),  kDashed, false);
    //for(vector<TH1F*>::iterator histIter = histDo.begin(); histIter != histDo.end(); ++histIter) formatHist((*histIter),  kDashed, false);
  }
  
  void split(int nBlind = -1, bool doSplit=true, int nBin=5, bool skipFirst = false, int nRebin=1){
    //cout << "in split " << endl;
    if(!hist) return;
    if(nBin == 1){
      //if(hist->GetNbinsX() > 23 && nBin) hist->Rebin(23);
      splitHists.push_back(hist);
      return;
    }
    else if(!doSplit){
      if(nRebin > 1) hist->Rebin(nRebin);
      for(int i=0; i<nBin; ++i){
        TString newname = hist->GetName();
        splitHists.push_back(new TH1F(newname + "base" + Form("%d",i), ";p_{T} W;Events / 10 GeV", hist->GetNbinsX()/nBin, 0, 5));
        for(int j=1; j <= (hist->GetNbinsX()/nBin+1); j++){
         // cout << "setting bin " << j << " from " << i* nBin +j << endl;
          splitHists.back()->SetBinContent(j, hist->GetBinContent(i* nBin +j));
          splitHists.back()->SetBinError(j, hist->GetBinError(i*nBin + j));
        }  
      }
      return;
    }

    int nbins = hist->GetNbinsX()/23;
    for(int i=0; i <= nbins; i++){
      TString newname = hist->GetName();
      if(i==0) splitHists.push_back(new TH1F(newname + "base", ";m_{b#bar{b}} [GeV];Events / 10 GeV", 23, 20, 250));
      if(i==nBin) splitHists.push_back(new TH1F(newname + "base5", ";m_{b#bar{b}} [GeV];Events / 10 GeV", 23, 20, 250)); 
      newname+=Form("%d",i);
      splitHists.push_back(new TH1F(newname, ";m_{b#bar{b}} [GeV];Events / 10 GeV", 23, 20, 250));

      for(int j=1; j<=23; ++j){
        splitHists.back()->SetBinContent(j, hist->GetBinContent(j + i*23));
        splitHists.back()->SetBinError(j, hist->GetBinError(j + i*23));
        /*if(i < nBlind && j > 6 && j < 14){
          if(hist->GetBinContent(j + i*23) > 0) cout << "Error! Data histogram is not blinded ! for hist " << i << " in bin " << j << " value " << splitHists.back()->GetBinCenter(j) << endl;
          splitHists.back()->SetBinContent(j, 0);
          splitHists.back()->SetBinError(j, 0);     
        }*/
      }

      //cout << "count events" << endl;
      events.push_back(splitHists.back()->Integral(splitHists.back()->FindBin(85), splitHists.back()->FindBin(145)));

      if(skipFirst && (i==0 || i==nBin) ) continue;
      if(i < nBin) splitHists[0]->Add(splitHists.back());
      else splitHists[nBin+1]->Add(splitHists.back());
      //cout<< "done split!" << endl;
      //events.push_back(splitHists.back()->Integral());
    }
  }

  TString sampleName;
  vector<TString> histName;


  // formatting styles
  int lineColour;
  int markerColour;
  int fillColour;
  int lineStyle;
  int markerStyle;
  int fillStyle;
  int lineWidth;
  float nEvents;
  float totFracUp;
  float totFracDo;
  vector<float> FracUp;
  vector<float> FracDo;
  float crossSection;
  float selected;
  float selectedEventsMCErr;
  float norm;

  // list of sub-plots etc

  TH1F*   hist;
  TH1F*   histTotUp;
  TH1F*   histTotDo;
  vector<TH1F*>   histUp;
  vector<TH1F*>   histDo; 
  
  vector<TH1F*> splitHists; 
  vector<float> events;
  vector<float> eventsErr;
};

class plot {
public:
  plot() {}
  plot(TString t_xaxis, TString t_yaxis, int t_rebin, float t_xmin, float t_xmax, float t_ymin, float t_ymax, bool t_log=false){
    xaxis = t_xaxis;
    yaxis = t_yaxis;
    rebin = t_rebin;
    xmin = t_xmin;
    xmax = t_xmax;
    ymin = t_ymin;
    ymax = t_ymax;
    log = t_log;
  }
  TString xaxis;
  TString yaxis;
  int rebin;
  float xmin;
  float xmax;
  float ymin;
  float ymax;
  bool log;
};

void ATLASLabel(Double_t x,Double_t y, bool Preliminary) 
{
  float fTextScaleFactor=0.8;

  TLatex l; //l.SetTextAlign(12); l.SetTextSize(tsize); 
  float siz=l.GetTextSize();
  l.SetTextSize(siz*fTextScaleFactor); // set the size here
  l.SetNDC();
  l.SetTextFont(72);
  l.SetTextColor(1);

  double delx = 0.115*696*gPad->GetWh()/(472*gPad->GetWw());
  l.DrawLatex(x,y,"ATLAS");
  if (Preliminary) {
    TLatex p; 
    siz=p.GetTextSize();
    p.SetTextSize(siz*fTextScaleFactor); // set the size here
    p.SetNDC();
    p.SetTextFont(42);
    p.SetTextColor(1);
    //  p.DrawLatex(x+delx-.03,y,"Preliminary");
    p.DrawLatex(x+delx,y,"Internal");
  }
}


void ATLASLumi(Double_t x,Double_t y, float datlum, int energy=7) 
{
// add the Atlas Lumi and centre of mass

  //float datlum=8.2;

//    if (!fIsPseudoData) {
//  TString tit="#sqrt{s} = 7 TeV, #int L dt=";
  TString tit;
  if(energy == 7) tit +="#sqrt{s} = 7 TeV";   
  if(energy == 8) tit +="#sqrt{s} = 8 TeV";   
  if(energy == 13) tit +="#sqrt{s} = 13 TeV";
  tit +=Form(", %.1f",datlum);  
  tit +=" fb^{-1}";

//

  float fTextScaleFactor=0.8;

  TLatex l; //l.SetTextAlign(12); 
  l.SetNDC();
//l.SetTextFont(42);// standard
  l.SetTextFont(72); // italics 
  float siz=l.GetTextSize();
  l.SetTextSize(siz*fTextScaleFactor); 
  //l.SetTextSize(siz*0.7); // set the size here
  l.SetTextColor(1);
  l.DrawLatex(x,y,tit);
}


void ATLASChannel(Double_t x,Double_t y, TString label) 
{
// add the Atlas Lumi and centre of mass


//    if (!fIsPseudoData) {
//  TString tit="#sqrt{s} = 7 TeV, #int L dt=";
  
  float fTextScaleFactor=0.8;


//
  TLatex l; //l.SetTextAlign(12); l.SetTextSize(tsize); 
  l.SetNDC();
//l.SetTextFont(42);// standard
  l.SetTextFont(72); // italics 
  float siz=l.GetTextSize();
  l.SetTextSize(siz*fTextScaleFactor); 

  //l.SetTextSize(siz*0.7); // set the size here
  l.SetTextColor(1);
  l.DrawLatex(x,y,label);
}
