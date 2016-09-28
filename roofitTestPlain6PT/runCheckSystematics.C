
void runCheckSystematics(TString tagPoint="MV2c2077",int stage=0) {
  gROOT->ProcessLine(".L /unix/atlasvhbb/abell/mc15data7/scripts/atlasstyle/AtlasStyle.C");
  gROOT->ProcessLine(".L checkSystematicsAndCombine_new.C++g");
//  gROOT->ProcessLine(".L checkSystematicsAndCombine_new_noseparate.C++g");
  SetAtlasStyle();
  
//  checkSystematicsAndCombine("MV2c2080");
//  checkSystematicsAndCombine("MV2c2060");



// checkSystematicsAndCombine("MV2c2070",true);  
// checkSystematicsAndCombine("MV2c2070");  
//checkSystematicsAndCombine("MV2c2080",true);  
//checkSystematicsAndCombine("MV2c2080");  
//checkSystematicsAndCombine("MV2c2090",true);  
//checkSystematicsAndCombine("MV2c2090");  

//  checkSystematicsAndCombine(tagPoint);
//checkSystematicsAndCombine("MV2c2060");  

  checkSystematicsAndCombine(tagPoint,false,true,0,false,true,false);
  checkSystematicsAndCombine(tagPoint,false,true,1,false,true,false);  
  checkSystematicsAndCombine(tagPoint,false,true,2,false,true,false);  
  checkSystematicsAndCombine(tagPoint,false,true,3,false,true,false); 
  
  checkSystematicsAndCombine(tagPoint,true,false,0,false,true,false);
//  checkSystematicsAndCombine(tagPoint,false,false,0,false,true,false);    
/**/
//   checkSystematicsAndCombine(tagPoint,false,true,0,false,false,true);  
//   checkSystematicsAndCombine(tagPoint,false,true,1,false,false,true);  
//   checkSystematicsAndCombine(tagPoint,false,true,2,false,false,true);  
//   checkSystematicsAndCombine(tagPoint,false,true,3,false,false,true);  

//   checkSystematicsAndCombine(tagPoint,true,false,0,false,false,true);  


// checkSystematicsAndCombine(tagPoint);  

//  checkSystematicsAndCombine("MV2c2070",false,true,0);  
//  checkSystematicsAndCombine("MV2c2070",false,true,1);  
//  checkSystematicsAndCombine("MV2c2070",false,true,2);  
//  checkSystematicsAndCombine("MV2c2070",false,true,3);  

//  checkSystematicsAndCombine("MV2c2080",false,true,0);  
//  checkSystematicsAndCombine("MV2c2080",false,true,1);  
//  checkSystematicsAndCombine("MV2c2080",false,true,2);  
//  checkSystematicsAndCombine("MV2c2080",false,true,3);  
}

