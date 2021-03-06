#include "FastCalibratorEB.h"
#include "GetHashedIndexEB.h"
#include <TH2.h>
#include <TF1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TRandom.h>
#include <TRandom3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>


///==== Default constructor Contructor 

FastCalibratorEB::FastCalibratorEB(TTree *tree, std::vector<TGraphErrors*> & inputMomentumScale, const std::string& typeEB, TString outEPDistribution):
outEPDistribution_p(outEPDistribution)
{
  // if parameter tree is not specified (or zero), connect the file
  // used to generate this class and read the Tree.
  if (tree == 0) {
    TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/data1/dimatteo/Calibration/Ntuples/Run2011A/WZAnalysisSingleXtal/WZAnalysis_SingleElectron_Run2011A-WElectron-May10ReReco-v1.root");
    if (!f) {
      f = new TFile("/data1/dimatteo/Calibration/Ntuples/Run2011A/WZAnalysisSingleXtal/WZAnalysis_SingleElectron_Run2011A-WElectron-May10ReReco-v1.root");
    }
    tree = (TTree*)gDirectory->Get("ntu");

  }
  Init(tree);

  // Set my momentum scale using the input graphs
  myMomentumScale = inputMomentumScale;
  myTypeEB = typeEB;
}

///==== deconstructor

FastCalibratorEB::~FastCalibratorEB()
{
  if (!fChain) return;
  delete fChain->GetCurrentFile();
}

///=== Acces to the entry information in the input tree or chain

Int_t FastCalibratorEB::GetEntry(Long64_t entry)
{
  if (!fChain) return 0;
  return fChain->GetEntry(entry);
}

///==== Load information of input Ntupla

Long64_t FastCalibratorEB::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
  if (!fChain) return -5;
  Long64_t centry = fChain->LoadTree(entry);
  if (centry < 0) return centry;
  if (!fChain->InheritsFrom(TChain::Class()))  return centry;

  TChain *chain = (TChain*)fChain;
  if (chain->GetTreeNumber() != fCurrent) {
    fCurrent = chain->GetTreeNumber();
    Notify();
  }
  return centry;
}

///==== Variables initialization

void FastCalibratorEB::Init(TTree *tree)
{
  /// Set object pointer
  
  ele1_recHit_E = 0;
  ele1_recHit_hashedIndex = 0;
  ele1_recHit_iphiORiy = 0;
  ele1_recHit_ietaORix = 0;
  ele1_recHit_flag = 0;

  ele2_recHit_E = 0;
  ele2_recHit_hashedIndex = 0;
  ele2_recHit_iphiORiy = 0;
  ele2_recHit_ietaORix = 0;
  ele2_recHit_flag = 0;
  
  /// Set branch addresses and branch pointers
  
  if (!tree) return;
  fChain = tree;
  fCurrent = -1;
  fChain->SetMakeClass(1);
  
  fChain->SetBranchStatus("*", 0);
  
  fChain->SetBranchStatus("runId", 1);   fChain->SetBranchAddress("runId", &runId, &b_runId);
  fChain->SetBranchStatus("lumiId", 1);  fChain->SetBranchAddress("lumiId", &lumiId, &b_lumiId);
  fChain->SetBranchStatus("eventId", 1); fChain->SetBranchAddress("eventId", &eventId, &b_eventId);
  fChain->SetBranchStatus("isW", 1);     fChain->SetBranchAddress("isW", &isW, &b_isW);
  fChain->SetBranchStatus("isZ", 1);     fChain->SetBranchAddress("isZ", &isZ, &b_isZ);
  
  // ele1
  fChain->SetBranchStatus("ele1_recHit_E", 1);           fChain->SetBranchAddress("ele1_recHit_E", &ele1_recHit_E, &b_ele1_recHit_E);
  fChain->SetBranchStatus("ele1_recHit_hashedIndex", 1); fChain->SetBranchAddress("ele1_recHit_hashedIndex", &ele1_recHit_hashedIndex, &b_ele1_recHit_hashedIndex);
  fChain->SetBranchStatus("ele1_recHit_ietaORix", 1);    fChain->SetBranchAddress("ele1_recHit_ietaORix", &ele1_recHit_ietaORix, &b_ele1_recHit_ietaORix);
  fChain->SetBranchStatus("ele1_recHit_iphiORiy", 1);    fChain->SetBranchAddress("ele1_recHit_iphiORiy", &ele1_recHit_iphiORiy, &b_ele1_recHit_iphiORiy);
  fChain->SetBranchStatus("ele1_recHit_flag", 1);        fChain->SetBranchAddress("ele1_recHit_flag", &ele1_recHit_flag, &b_ele1_recHit_flag);
  //fChain->SetBranchStatus("ele1_E_true", 1);             fChain->SetBranchAddress("ele1_E_true", &ele1_E_true, &b_ele1_E_true);
  //fChain->SetBranchStatus("ele1_DR", 1);                 fChain->SetBranchAddress("ele1_DR", &ele1_DR, &b_ele1_DR);
  //fChain->SetBranchStatus("ele1_charge", 1);             fChain->SetBranchAddress("ele1_charge", &ele1_charge, &b_ele1_charge);

  fChain->SetBranchStatus("ele1_charge", 1);         fChain->SetBranchAddress("ele1_charge", &ele1_charge, &b_ele1_charge);
  fChain->SetBranchStatus("ele1_eta", 1);            fChain->SetBranchAddress("ele1_eta", &ele1_eta, &b_ele1_eta);
  fChain->SetBranchStatus("ele1_phi", 1);            fChain->SetBranchAddress("ele1_phi", &ele1_phi, &b_ele1_phi);
  fChain->SetBranchStatus("ele1_scERaw", 1);         fChain->SetBranchAddress("ele1_scERaw", &ele1_scERaw, &b_ele1_scERaw);
  fChain->SetBranchStatus("ele1_scE", 1);            fChain->SetBranchAddress("ele1_scE", &ele1_scE, &b_ele1_scE);
  fChain->SetBranchStatus("ele1_scE_regression", 1); fChain->SetBranchAddress("ele1_scE_regression", &ele1_scE_regression, &b_ele1_scE_regression);
  
  fChain->SetBranchStatus("ele1_es", 1);          fChain->SetBranchAddress("ele1_es", &ele1_es, &b_ele1_es);
  fChain->SetBranchStatus("ele1_e3x3", 1);        fChain->SetBranchAddress("ele1_e3x3", &ele1_e3x3, &b_ele1_e3x3);
  fChain->SetBranchStatus("ele1_tkP", 1);         fChain->SetBranchAddress("ele1_tkP", &ele1_tkP, &b_ele1_tkP);
  fChain->SetBranchStatus("ele1_fbrem", 1);       fChain->SetBranchAddress("ele1_fbrem", &ele1_fbrem, &b_ele1_fbrem);
  fChain->SetBranchStatus("ele1_EOverP", 1);      fChain->SetBranchAddress("ele1_EOverP", &ele1_EOverP, &b_ele1_EOverP);
  fChain->SetBranchStatus("ele1_isEB", 1);        fChain->SetBranchAddress("ele1_isEB", &ele1_isEB, &b_ele1_isEB);
  fChain->SetBranchStatus("ele1_isEBEEGap", 1);   fChain->SetBranchAddress("ele1_isEBEEGap", &ele1_isEBEEGap, &b_ele1_isEBEEGap);
  fChain->SetBranchStatus("ele1_isEBEtaGap", 1);  fChain->SetBranchAddress("ele1_isEBEtaGap", &ele1_isEBEtaGap, &b_ele1_isEBEtaGap);
  fChain->SetBranchStatus("ele1_isEBPhiGap", 1);  fChain->SetBranchAddress("ele1_isEBPhiGap", &ele1_isEBPhiGap, &b_ele1_isEBPhiGap);
  fChain->SetBranchStatus("ele1_isEEDeeGap", 1);  fChain->SetBranchAddress("ele1_isEEDeeGap", &ele1_isEEDeeGap, &b_ele1_isEEDeeGap);
  fChain->SetBranchStatus("ele1_isEERingGap", 1); fChain->SetBranchAddress("ele1_isEERingGap", &ele1_isEERingGap, &b_ele1_isEERingGap);
  
  // ele2
  fChain->SetBranchStatus("ele2_recHit_E", 1);           fChain->SetBranchAddress("ele2_recHit_E", &ele2_recHit_E, &b_ele2_recHit_E);
  fChain->SetBranchStatus("ele2_recHit_hashedIndex", 1); fChain->SetBranchAddress("ele2_recHit_hashedIndex", &ele2_recHit_hashedIndex, &b_ele2_recHit_hashedIndex);
  fChain->SetBranchStatus("ele2_recHit_iphiORiy", 1);    fChain->SetBranchAddress("ele2_recHit_iphiORiy", &ele2_recHit_iphiORiy, &b_ele2_recHit_iphiORiy);
  fChain->SetBranchStatus("ele2_recHit_ietaORix", 1);    fChain->SetBranchAddress("ele2_recHit_ietaORix", &ele2_recHit_ietaORix, &b_ele2_recHit_ietaORix);
  fChain->SetBranchStatus("ele2_recHit_flag", 1);        fChain->SetBranchAddress("ele2_recHit_flag", &ele2_recHit_flag, &b_ele2_recHit_flag);
  //fChain->SetBranchStatus("ele2_E_true", 1);             fChain->SetBranchAddress("ele2_E_true", &ele2_E_true, &b_ele2_E_true);
  //fChain->SetBranchStatus("ele2_DR", 1);                 fChain->SetBranchAddress("ele2_DR", &ele2_DR, &b_ele2_DR);
  //fChain->SetBranchStatus("ele2_charge", 1);             fChain->SetBranchAddress("ele2_charge", &ele2_charge, &b_ele2_charge);
  
  fChain->SetBranchStatus("ele2_charge", 1);         fChain->SetBranchAddress("ele2_charge", &ele2_charge, &b_ele2_charge);
  fChain->SetBranchStatus("ele2_eta", 1);            fChain->SetBranchAddress("ele2_eta", &ele2_eta, &b_ele2_eta);
  fChain->SetBranchStatus("ele2_phi", 1);            fChain->SetBranchAddress("ele2_phi", &ele2_phi, &b_ele2_phi);
  fChain->SetBranchStatus("ele2_scERaw", 1);         fChain->SetBranchAddress("ele2_scERaw", &ele2_scERaw, &b_ele2_scERaw);
  fChain->SetBranchStatus("ele2_scE", 1);            fChain->SetBranchAddress("ele2_scE", &ele2_scE, &b_ele2_scE);
  fChain->SetBranchStatus("ele1_scE_regression", 1); fChain->SetBranchAddress("ele1_scE_regression", &ele1_scE_regression, &b_ele1_scE_regression);
  
  fChain->SetBranchStatus("ele2_e3x3", 1);        fChain->SetBranchAddress("ele2_e3x3", &ele2_e3x3, &b_ele2_e3x3);
  fChain->SetBranchStatus("ele2_tkP", 1);         fChain->SetBranchAddress("ele2_tkP", &ele2_tkP, &b_ele2_tkP);
  fChain->SetBranchStatus("ele2_fbrem", 1);       fChain->SetBranchAddress("ele2_fbrem", &ele2_fbrem, &b_ele2_fbrem);
  fChain->SetBranchStatus("ele2_EOverP", 1);      fChain->SetBranchAddress("ele2_EOverP", &ele2_EOverP, &b_ele2_EOverP);
  fChain->SetBranchStatus("ele2_isEB", 1);        fChain->SetBranchAddress("ele2_isEB", &ele2_isEB, &b_ele2_isEB);
  fChain->SetBranchStatus("ele2_isEBEEGap", 1);   fChain->SetBranchAddress("ele2_isEBEEGap", &ele2_isEBEEGap, &b_ele2_isEBEEGap);
  fChain->SetBranchStatus("ele2_isEBEtaGap", 1);  fChain->SetBranchAddress("ele2_isEBEtaGap", &ele2_isEBEtaGap, &b_ele2_isEBEtaGap);
  fChain->SetBranchStatus("ele2_isEBPhiGap", 1);  fChain->SetBranchAddress("ele2_isEBPhiGap", &ele2_isEBPhiGap, &b_ele2_isEBPhiGap);
  fChain->SetBranchStatus("ele2_isEEDeeGap", 1);  fChain->SetBranchAddress("ele2_isEEDeeGap", &ele2_isEEDeeGap, &b_ele2_isEEDeeGap);
  fChain->SetBranchStatus("ele2_isEERingGap", 1); fChain->SetBranchAddress("ele2_isEERingGap", &ele2_isEERingGap, &b_ele2_isEERingGap);
  Notify();
}

Bool_t FastCalibratorEB::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

  return kTRUE;
}

void FastCalibratorEB::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
  if (!fChain) return;
  fChain->Show(entry);
}

Int_t FastCalibratorEB::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
  return 1;
}


//! Declaration of the objects that are save in the output file

void FastCalibratorEB::bookHistos(int nLoops)
{
  hC_IntercalibValues = new hChain ("IntercalibValues", "IntercalibValues", 2000,0.5,1.5, nLoops);
  
  hC_PullFromScalib = new hChain ("hC_PullFromScalib", "hC_PullFromScalib", 2000,-0.5,0.5, nLoops);
  
  hC_EoP = new hChain ("EoP", "EoP", 100,0.2,1.9, nLoops);
  
  hC_scale_EB = new h2Chain("hC_scale_EB", "hC_scale_EB", 360,1, 361, 171, -85, 86, nLoops );
  
  h_scalib_EB = new TH2F("h_scalib_EB", "h_scalib_EB", 360,1, 361, 171, -85, 86 );
  
  h_Occupancy_hashedIndex = new TH1F ("h_Occupancy_hashedIndex", "h_Occupancy_hashedIndex", 61201,-0.5,61199.5);
  
  p_IntercalibValues_iEta = new TProfile ("p_IntercalibValues_iEta","p_IntercalibValues_iEta", 171, -85, 86, -0.1, 2.1);
  
  h_IntercalibSpread_iEta = new TH1F ("h_IntercalibSpread_iEta", "h_IntercalibSpread_iEta", 171, -85, 86);
  
  h_IntercalibValues_test = new TH1F ("h_IntercalibValues_test", "h_IntercalibValues_test", 400, -1, 1);
  
  h_scale_EB_hashedIndex = new TH1F("h_scale_EB_hashedIndex", "h_scale_EB_hashedIndex", 61201,-0.5,61999.5 );
  
  h_Init_IntercalibValues = new TH1F("h_Init_IntercalibValues","h_Init_IntercalibValues",2000,0.5,1.5);
  
  h_map_Dead_Channels = new TH2F("h_map_Dead_Channels","h_map_Dead_Channels",360,1,361,171,-85,86);

  g_ICmeanVsLoop = new TGraphErrors();
  g_ICmeanVsLoop -> SetName("g_ICmeanVsLoop");
  g_ICmeanVsLoop -> SetTitle("g_ICmeanVsLoop");

  g_ICrmsVsLoop  = new TGraphErrors();
  g_ICrmsVsLoop -> SetName("g_ICrmsVsLoop");
  g_ICrmsVsLoop ->SetTitle("g_ICrmsVsLoop");

  h_scale_EB = new TH2F("h_scale_EB", "h_scale_EB", 360,1, 361, 171, -85, 86 );
  
  h_scale_EB_meanOnPhi = new TH2F("h_scale_EB_meanOnPhi", "h_scale_EB_meanOnPhi", 360,1, 361, 171, -85, 86 );
  
  h_occupancy = new TH2F("h_occupancy", "h_occupancy", 360,1, 361, 171, -85, 86 );
  
  return;
}

//! Build E/p distribution for both ele1 and ele2 

void FastCalibratorEB::BuildEoPeta_ele(int iLoop, int nentries , int useW, int useZ, std::vector<float> theScalibration, bool isSaveEPDistribution, bool isR9selection, float R9Min, bool isMCTruth){

  if(iLoop ==0)  
  {
   TString name = Form ("hC_EoP_eta_%d",iLoop);
   hC_EoP_eta_ele = new hChain (name,name, 100,0.2,1.9,171);
  }
  else{
          hC_EoP_eta_ele -> Reset();
          TString name = Form ("hC_EoP_eta_%d",iLoop);
          hC_EoP_eta_ele = new hChain (name,name, 100,0.2,1.9,171);
      }

  
  Long64_t nbytes = 0, nb = 0;
  
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
   
   Long64_t ientry = LoadTree(jentry);
   if (ientry < 0) break;
  
   nb = fChain->GetEntry(jentry);   
   nbytes += nb;
   if (!(jentry%1000000))std::cerr<<"building E/p distribution ----> "<<jentry<<" vs "<<nentries<<std::endl;

   float pIn, pSub, FdiEta;

   ///! Tight electron from W or Z only barrel
   
   if ( ele1_isEB == 1 && (( useW == 1 && isW == 1 ) ||  ( useZ== 1 && isZ == 1 ))) {

    FdiEta = ele1_scE/ele1_scERaw; /// FEta approximation
    
    float thisE = 0;
    int   iseed = 0 ;
    int seed_hashedIndex = 0;
    float E_seed = 0;
    float thisE3x3 = 0;
    
    /// Cycle on the all the recHits of the Event: to get the old IC and the corrected SC energy
    
    for (unsigned int iRecHit = 0; iRecHit < ele1_recHit_E->size(); iRecHit++ ) {
            
            float thisIC = 1.;
            int thisIndex = ele1_recHit_hashedIndex -> at(iRecHit);
           
            //if(ele1_recHit_E -> at(iRecHit) > E_seed &&  ele1_recHit_flag->at(iRecHit) < 4 ) /// control if this recHit is good
            if(ele1_recHit_E -> at(iRecHit) > E_seed) /// control if this recHit is good
            {
              seed_hashedIndex=ele1_recHit_hashedIndex -> at(iRecHit);
              iseed=iRecHit;
              E_seed=ele1_recHit_E -> at(iRecHit);  ///! Seed search
            }
    
            if (iLoop > 0) thisIC = h_scale_EB_hashedIndex -> GetBinContent(thisIndex+1);
            
            //if(ele1_recHit_flag->at(iRecHit) < 4) ///! SC energy taking only good channels
            thisE += theScalibration[thisIndex]*ele1_recHit_E -> at(iRecHit)*FdiEta*thisIC;
             
     }

    for (unsigned int iRecHit = 0; iRecHit < ele1_recHit_E->size(); iRecHit++ ) {
            
            float thisIC = 1.;
            int thisIndex = ele1_recHit_hashedIndex -> at(iRecHit);
           
            ///! 3x3 matrix informations in order to apply R9 selection
	    
            if (iLoop > 0) thisIC = h_scale_EB_hashedIndex -> GetBinContent(thisIndex+1);
         
            //if(fabs(ele1_recHit_ietaORix->at(iRecHit)-ele1_recHit_ietaORix->at(iseed))<=1 && 
            //   fabs(ele1_recHit_iphiORiy->at(iRecHit)-ele1_recHit_iphiORiy->at(iseed))<=1 && ele1_recHit_flag->at(iRecHit) < 4
            if(fabs(ele1_recHit_ietaORix->at(iRecHit)-ele1_recHit_ietaORix->at(iseed))<=1 && 
               fabs(ele1_recHit_iphiORiy->at(iRecHit)-ele1_recHit_iphiORiy->at(iseed))<=1
           )
              thisE3x3+=theScalibration[thisIndex]*ele1_recHit_E -> at(iRecHit)*FdiEta*thisIC;
           }

     ///! Eta seed from hashed index
     int eta_seed = GetIetaFromHashedIndex(seed_hashedIndex);
  
     pSub = 0.; //NOTALEO : test dummy
     bool skipElectron = false;
   
     ///! different E/p if I am using MCThruth informations or not
     if(!isMCTruth)  
     {
       pIn = ele1_tkP;
       int regionId = templIndexEB(myTypeEB,ele1_eta,ele1_charge,thisE3x3/thisE);
       pIn /= myMomentumScale[regionId] -> Eval( ele1_phi );
     }
     else{
           pIn = ele1_E_true;
           if(fabs(ele1_DR)>0.1) skipElectron = true; /// No macthing beetween gen ele and reco ele
         }
         
     /// R9 Selection    
     if( fabs(thisE3x3/thisE) < R9Min && isR9selection == true ) skipElectron = true;
     
     /// Save electron E/p in a chain of histogramm each for eta bin
     if(!skipElectron)    hC_EoP_eta_ele -> Fill(eta_seed+85,thisE/pIn);
     
  
  }
  ///=== Second medium electron from Z
  
  if ( ele2_isEB == 1 && (( useW == 1 && isW == 1 ) || ( useZ == 1 && isZ == 1 )) ){

    FdiEta = ele2_scE/ele2_scERaw; /// FEta approximation
 
    float thisE = 0;
    int   iseed = 0 ;
    int seed_hashedIndex = 0;
    float E_seed = 0;
    float thisE3x3 = 0;
  
    /// Cycle on the all the recHits of the Event: to get the old IC and the corrected SC energy
    
    for (unsigned int iRecHit = 0; iRecHit < ele2_recHit_E->size(); iRecHit++ ) {
            
            float thisIC = 1.;
            int thisIndex = ele2_recHit_hashedIndex -> at(iRecHit);
            
            //if(ele2_recHit_E -> at(iRecHit) > E_seed && ele2_recHit_flag->at(iRecHit) < 4)
            if(ele2_recHit_E -> at(iRecHit) > E_seed)
            {
              seed_hashedIndex=ele2_recHit_hashedIndex -> at(iRecHit);
              iseed=iRecHit;
              E_seed=ele2_recHit_E -> at(iRecHit); ///Seed informations
            }
    
            
            if (iLoop > 0) thisIC = h_scale_EB_hashedIndex -> GetBinContent(thisIndex+1);
            
            //if (ele2_recHit_flag->at(iRecHit) < 4 ) /// SC Energy only for good channels
            thisE += theScalibration[thisIndex]*ele2_recHit_E -> at(iRecHit)*FdiEta*thisIC;
             
     }

     for (unsigned int iRecHit = 0; iRecHit < ele2_recHit_E->size(); iRecHit++ ) {
            
            float thisIC = 1.;
            int thisIndex = ele2_recHit_hashedIndex -> at(iRecHit);
           
            if (iLoop > 0) thisIC = h_scale_EB_hashedIndex -> GetBinContent(thisIndex+1);
         
            //if(fabs(ele2_recHit_ietaORix->at(iRecHit)-ele2_recHit_ietaORix->at(iseed))<=1 && 
            //   fabs(ele2_recHit_iphiORiy->at(iRecHit)-ele2_recHit_iphiORiy->at(iseed))<=1 &&
            //   ele2_recHit_flag->at(iRecHit) < 4)
            if(fabs(ele2_recHit_ietaORix->at(iRecHit)-ele2_recHit_ietaORix->at(iseed))<=1 && 
               fabs(ele2_recHit_iphiORiy->at(iRecHit)-ele2_recHit_iphiORiy->at(iseed))<=1)
              thisE3x3+=theScalibration[thisIndex]*ele2_recHit_E -> at(iRecHit)*FdiEta*thisIC;
           }

  
     /// Eta seed info from hashed index          
     int eta_seed = GetIetaFromHashedIndex(seed_hashedIndex);
  
     pSub = 0.; //NOTALEO : test dummy
     bool skipElectron = false;
   
     /// MCTruth analysis option
     if(!isMCTruth)  
       {
         pIn = ele2_tkP;
	 int regionId = templIndexEB(myTypeEB,ele2_eta,ele2_charge,thisE3x3/thisE);
         pIn /= myMomentumScale[regionId] -> Eval( ele2_phi );
       }
     else{
       pIn = ele2_E_true;
       if(fabs(ele2_DR)>0.1) skipElectron = true; /// No macthing beetween gen ele and reco ele
     }
     
     ///R9 Selection
     if( fabs(thisE3x3/thisE) < R9Min && isR9selection==true ) skipElectron = true;
     /// Save E/p electron information
     if(!skipElectron) hC_EoP_eta_ele -> Fill(eta_seed+85,thisE/pIn);
  
  }
  
  
 }
 
 /// Histogramm Normalization
 for(unsigned int ieta=0 ; ieta < hC_EoP_eta_ele->Size() ; ieta++)
 {
     hC_EoP_eta_ele->Normalize(ieta);
 }
 
 /// Save E/p pdf if it is required
 if(isSaveEPDistribution == true) 
 {
   TFile *f2 = new TFile(outEPDistribution_p.Data(),"UPDATE");
   saveEoPeta(f2);
 }
   
}


/// Calibration Loop over the ntu events

void FastCalibratorEB::Loop(int nentries, int useZ, int useW, int splitStat, int nLoops, bool isMiscalib,bool isSaveEPDistribution,
                            bool isEPselection,bool isR9selection, float R9Min, bool isMCTruth, std::map<int, std::vector<std::pair<int, int> > > jsonMap){
   if (fChain == 0) return;
   
   /// Define the number of crystal you want to calibrate
   int m_regions = 0;
 

   /// Define useful numbers
   static const int MIN_IETA = 1;
   static const int MIN_IPHI = 1;
   static const int MAX_IETA = 85;
   static const int MAX_IPHI = 360;
  
   for ( int iabseta = MIN_IETA; iabseta <= MAX_IETA; iabseta++ ){
     for ( int iphi = MIN_IPHI; iphi <= MAX_IPHI; iphi++ ){
       for ( int theZside = -1; theZside < 2; theZside = theZside+2 ){
         
         m_regions++;
         
       }
     }
   }
   
  /// Barrel region = Barrel xtal
  std::cout << "m_regions " << m_regions << std::endl;
   
  //Prepare the calibration blocks
       
  /// Build the scalibration Map for MC Analysis
  
  std::vector<float> theScalibration(m_regions, 0.);
  TRandom3 genRand;
  for ( int iIndex = 0; iIndex < m_regions; iIndex++ )  {

     bool isDeadXtal = false ;
     
     /// Save Map of DeadXtal and put the scalibration value = 0 in order to skip them in the calibration procedure -> Fake dead list given by user
     
     if(DeadXtal_HashedIndex.at(0)!=-9999) isDeadXtal = CheckDeadXtal(GetIetaFromHashedIndex(iIndex), GetIphiFromHashedIndex(iIndex));
     if(isDeadXtal == true ) {
     theScalibration[iIndex]=0;
     h_map_Dead_Channels->Fill(GetIphiFromHashedIndex(iIndex),GetIetaFromHashedIndex(iIndex));
     }
     else{
       
       if(isMiscalib == true) theScalibration[iIndex] = genRand.Gaus(1.,0.01); ///! 5% of Miscalibration fixed
       if(isMiscalib == false) theScalibration[iIndex] = 1.;
       h_scalib_EB -> Fill ( GetIphiFromHashedIndex(iIndex), GetIetaFromHashedIndex(iIndex), theScalibration[iIndex] ); ///! Scalib map
     }
   }

   /// ----------------- Calibration Loops -----------------------------//
   
   for ( int iLoop = 0; iLoop < nLoops; iLoop++ ) {

    std::cout << "Starting iteration " << iLoop + 1 << std::endl;
    
    /// prepare the numerator and denominator for each Xtal
   
    std::vector<float> theNumerator(m_regions, 0.);
    std::vector<float> theDenominator(m_regions, 0.);
     
    std::cout << "Number of analyzed events = " << nentries << std::endl;
    
    ///==== build E/p distribution ele 1 and 2
    BuildEoPeta_ele(iLoop,nentries,useW,useZ,theScalibration,isSaveEPDistribution,isR9selection,R9Min,isMCTruth); 
    
    // define map with events
    std::map<std::pair<int,std::pair<int,int> >,int> eventsMap;
    
    /// Loop on each entry 
    Long64_t nbytes = 0, nb = 0;
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
      
        if (!(jentry%10000))std::cerr<<jentry;
        if (!(jentry%1000)) std::cerr<<".";
      
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);   
        nbytes += nb;
        
        
        
	//*********************************
	// JSON FILE AND DUPLIACTES IN DATA
        
	bool skipEvent = false;
	if( isMCTruth == 0 )
        {
          if(AcceptEventByRunAndLumiSection(runId,lumiId,jsonMap) == false) skipEvent = true;
          
          std::pair<int,Long64_t> eventLSandID(lumiId,eventId);
          std::pair<int,std::pair<int,Long64_t> > eventRUNandLSandID(runId,eventLSandID);
          if( eventsMap[eventRUNandLSandID] == 1 ) skipEvent = true;
          else eventsMap[eventRUNandLSandID] = 1;
        }
        
        if( skipEvent == true ) continue;
        
        
        
        float pIn, pSub, FdiEta;
        
        std::map<int,double> map;
        bool skipElectron=false;
       
        /// Tight electron information from W and Z, it depends on the flag variable isW, isZ
	
        if ( ele1_isEB == 1 && (( useW == 1 && isW == 1 ) || ( useZ == 1 && isZ == 1 )) ) {
                  
         /// SCL energy containment correction
          FdiEta = ele1_scE/ele1_scERaw;
         
	  float thisE = 0;
          int iseed = 0 ;
          float E_seed = 0;
          int seed_hashedIndex = 0; 
          float thisE3x3 = 0 ;
         
          /// Cycle on the all the recHits of the Event: to get the old IC and the corrected SC energy
          for (unsigned int iRecHit = 0; iRecHit < ele1_recHit_E->size(); iRecHit++ ) {
            
            float thisIC = 1.;
            int thisIndex = ele1_recHit_hashedIndex -> at(iRecHit);
           
            if (iLoop > 0 ) thisIC = h_scale_EB_hashedIndex -> GetBinContent(thisIndex+1);
            
            //if (ele1_recHit_flag->at(iRecHit) < 4) ///! SC Energy
            thisE += theScalibration[thisIndex]*ele1_recHit_E -> at(iRecHit)*FdiEta*thisIC;

              
            //if(ele1_recHit_E -> at(iRecHit) > E_seed && ele1_recHit_flag->at(iRecHit)<4)
            if(ele1_recHit_E -> at(iRecHit) > E_seed)
             {
              E_seed=ele1_recHit_E -> at(iRecHit);
              iseed=iRecHit;
              seed_hashedIndex=ele1_recHit_hashedIndex -> at(iRecHit); //! Seed Infos
             }
          
              
          }
          
         for (unsigned int iRecHit = 0; iRecHit < ele1_recHit_E->size(); iRecHit++ ) {
            
            float thisIC = 1.;
            int thisIndex = ele1_recHit_hashedIndex -> at(iRecHit);
     
            if (iLoop > 0) thisIC = h_scale_EB_hashedIndex -> GetBinContent(thisIndex+1);
         
            //if(fabs(ele1_recHit_ietaORix->at(iRecHit)-ele1_recHit_ietaORix->at(iseed))<=1 && 
            //   fabs(ele1_recHit_iphiORiy->at(iRecHit)-ele1_recHit_iphiORiy->at(iseed))<=1 &&
            //   ele1_recHit_flag->at(iRecHit) < 4 )
            if(fabs(ele1_recHit_ietaORix->at(iRecHit)-ele1_recHit_ietaORix->at(iseed))<=1 && 
               fabs(ele1_recHit_iphiORiy->at(iRecHit)-ele1_recHit_iphiORiy->at(iseed))<=1)
              thisE3x3+=theScalibration[thisIndex]*ele1_recHit_E -> at(iRecHit)*FdiEta*thisIC;
           }
 
          pSub = 0.; //NOTALEO : test dummy
          bool skipElectron = false;
   
	  ///! if MCTruth Analysis
          if(!isMCTruth)  
	  {
            pIn = ele1_tkP;
	    int regionId = templIndexEB(myTypeEB,ele1_eta,ele1_charge,thisE3x3/thisE);
            pIn /= myMomentumScale[regionId] -> Eval( ele1_phi );
          }
          else{
           pIn = ele1_E_true;
           if(fabs(ele1_DR)>0.1) skipElectron = true; /// No macthing beetween gen ele and reco ele
          }
          
          /// Take the correct pdf for the ring in order to reweight the events in L3
          int eta_seed = GetIetaFromHashedIndex(seed_hashedIndex);
          TH1F* EoPHisto = hC_EoP_eta_ele->GetHisto(eta_seed+85);
          
	  /// Basic selection on E/p or R9 if you want to apply
          if( fabs(thisE/pIn  - 1) > 0.3 && isEPselection == true ) skipElectron = true;
          if( fabs(thisE3x3/thisE) < R9Min && isR9selection == true ) skipElectron = true;
          if( thisE/pIn < EoPHisto->GetXaxis()->GetXmin() || thisE/pIn > EoPHisto->GetXaxis()->GetXmax()) skipElectron = true;
	  if( !skipElectron) {
          
            /// Now cycle on the all the recHits and update the numerator and denominator
            for ( unsigned int iRecHit = 0; iRecHit < ele1_recHit_E->size(); iRecHit++ ) {
            
              //if (ele1_recHit_flag->at(iRecHit) >= 4) continue ;
           
              int thisIndex = ele1_recHit_hashedIndex -> at(iRecHit);
              float thisIC = 1.;

              if (iLoop > 0) thisIC = h_scale_EB_hashedIndex -> GetBinContent(thisIndex+1);
  
              /// Fill the occupancy map JUST for the first Loop
              if ( iLoop == 0 ) {
                h_Occupancy_hashedIndex -> Fill(thisIndex);  
                h_occupancy -> Fill(GetIphiFromHashedIndex(thisIndex), GetIetaFromHashedIndex(thisIndex));
              }
  
              /// use full statistics
              if ( splitStat == 0 ) {
                
                int EoPbin = EoPHisto->FindBin(thisE/(pIn-pSub)); /// factor use to reweight the evemts
                theNumerator[thisIndex] += theScalibration[thisIndex]*ele1_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*(pIn-pSub)/thisE*EoPHisto->GetBinContent(EoPbin);
                theDenominator[thisIndex] += theScalibration[thisIndex]*ele1_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*EoPHisto->GetBinContent(EoPbin);
                
              }
              /// Use Half Statistic only even   
              else if ( splitStat == 1 && jentry%2 == 0 ) {
                int EoPbin = EoPHisto->FindBin(thisE/(pIn-pSub));
                theNumerator[thisIndex] += theScalibration[thisIndex]*ele1_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*(pIn-pSub)/thisE*EoPHisto->GetBinContent(EoPbin);
                theDenominator[thisIndex] += theScalibration[thisIndex]*ele1_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*EoPHisto->GetBinContent(EoPbin);
              }  
              /// use odd event
              else if ( splitStat == -1 && jentry%2 != 0 ) {
                int EoPbin = EoPHisto->FindBin(thisE/(pIn-pSub));
                theNumerator[thisIndex] += theScalibration[thisIndex]*ele1_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*(pIn-pSub)/thisE*EoPHisto->GetBinContent(EoPbin);
                theDenominator[thisIndex] += theScalibration[thisIndex]*ele1_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*EoPHisto->GetBinContent(EoPbin);
              }
            
            }
          
          }

          
          //Fill EoP
          hC_EoP -> Fill(iLoop, thisE/pIn);
        
        }  
              
        skipElectron = false;
      
        /// Ele2 medium from Z only Barrel
        if ( ele2_isEB == 1 && (( useW == 1 && isW == 1 ) || ( useZ == 1 && isZ == 1 )) ) {
  
          FdiEta = ele2_scE/ele2_scERaw;
          // Electron energy
          float thisE = 0;
          int iseed = 0 ;
          float E_seed = 0;
          int seed_hashedIndex = 0; 
          float thisE3x3 = 0;
         
          /// Cycle on the all the recHits of the Event: to get the old IC and the corrected SC energy
          for (unsigned int iRecHit = 0; iRecHit < ele2_recHit_E->size(); iRecHit++ ) {
            
            float thisIC = 1.;
            int thisIndex = ele2_recHit_hashedIndex -> at(iRecHit);
            if (iLoop > 0) thisIC = h_scale_EB_hashedIndex -> GetBinContent(thisIndex+1);
            
            //if(ele2_recHit_flag->at(iRecHit) < 4)
            thisE += theScalibration[thisIndex]*ele2_recHit_E -> at(iRecHit)*FdiEta*thisIC;

              
            //if(ele2_recHit_E -> at(iRecHit) > E_seed && ele2_recHit_flag->at(iRecHit) < 4)
            if(ele2_recHit_E -> at(iRecHit) > E_seed)
             {
              E_seed=ele2_recHit_E -> at(iRecHit);
              iseed=iRecHit;
              seed_hashedIndex=ele2_recHit_hashedIndex -> at(iRecHit); /// Seed information
             }
          
              
          }
          
          for (unsigned int iRecHit = 0; iRecHit < ele2_recHit_E->size(); iRecHit++ ) {
            
            float thisIC = 1.;
            int thisIndex = ele2_recHit_hashedIndex -> at(iRecHit);
            // IC obtained from previous Loops
            if (iLoop > 0) thisIC = h_scale_EB_hashedIndex -> GetBinContent(thisIndex+1);

       
	    //if(fabs(ele2_recHit_ietaORix->at(iRecHit)-ele2_recHit_ietaORix->at(iseed))<=1 && 
            //  fabs(ele2_recHit_iphiORiy->at(iRecHit)-ele2_recHit_iphiORiy->at(iseed))<=1 &&
            //  ele2_recHit_flag->at(iRecHit) < 4)
           if(fabs(ele2_recHit_ietaORix->at(iRecHit)-ele2_recHit_ietaORix->at(iseed))<=1 && 
              fabs(ele2_recHit_iphiORiy->at(iRecHit)-ele2_recHit_iphiORiy->at(iseed))<=1)
             thisE3x3+=theScalibration[thisIndex]*ele2_recHit_E -> at(iRecHit)*FdiEta*thisIC;
             
 
          }

         
          pSub = 0.; //NOTALEO : test dummy
          
          ///! Option for MCTruth analysis 
          if(!isMCTruth)  
          {
            pIn = ele2_tkP;
	    int regionId = templIndexEB(myTypeEB,ele2_eta,ele2_charge,thisE3x3/thisE);
            pIn /= myMomentumScale[regionId] -> Eval( ele2_phi );
          }
          else{
           pIn = ele2_E_true;
           if(fabs(ele2_DR)>0.1) skipElectron = true; /// No macthing beetween gen ele and reco ele
          }
          int eta_seed = GetIetaFromHashedIndex(seed_hashedIndex);
          TH1F* EoPHisto = hC_EoP_eta_ele->GetHisto(eta_seed+85);
          
          /// discard electrons with bad E/P or R9
          if( thisE/pIn  < EoPHisto->GetXaxis()->GetXmin() || thisE/pIn  > EoPHisto->GetXaxis()->GetXmax()) skipElectron=true;
          if( fabs(thisE/pIn  - 1) > 0.3 && isEPselection == true ) skipElectron = true;
          if( fabs(thisE3x3/thisE) < R9Min && isR9selection == true ) skipElectron = true;
          
          if( !skipElectron ){
          
            /// Now cycle on the all the recHits and update the numerator and denominator
            for ( unsigned int iRecHit = 0; iRecHit < ele2_recHit_E->size(); iRecHit++ ) {

              //if (ele2_recHit_flag->at(iRecHit) >= 4) continue ;
  
              int thisIndex = ele2_recHit_hashedIndex -> at(iRecHit);
              float thisIC = 1.;
           
              if (iLoop > 0) thisIC = h_scale_EB_hashedIndex -> GetBinContent(thisIndex+1);
  
              /// Fill the occupancy map JUST for the first Loop
              if ( iLoop == 0 ) {
                h_Occupancy_hashedIndex -> Fill(thisIndex);  
                h_occupancy -> Fill(GetIphiFromHashedIndex(thisIndex), GetIetaFromHashedIndex(thisIndex));
              }
  
              /// use full statistics
              if ( splitStat == 0 ) {
                
                int EoPbin = EoPHisto->FindBin(thisE/(pIn-pSub));
                theNumerator[thisIndex] += theScalibration[thisIndex]*ele2_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*(pIn-pSub)/thisE*EoPHisto->GetBinContent(EoPbin);
                theDenominator[thisIndex] += theScalibration[thisIndex]*ele2_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*EoPHisto->GetBinContent(EoPbin);
              }
              /// use evens    
              else if ( splitStat == 1 && jentry%2 == 0 ) {
                int EoPbin = EoPHisto->FindBin(thisE/(pIn-pSub));
                theNumerator[thisIndex] += theScalibration[thisIndex]*ele2_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*(pIn-pSub)/thisE*EoPHisto->GetBinContent(EoPbin);
                theDenominator[thisIndex] += theScalibration[thisIndex]*ele2_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*EoPHisto->GetBinContent(EoPbin);
              }  
             /// use odds
              else if ( splitStat == -1 && jentry%2 != 0 ) {
                int EoPbin = EoPHisto->FindBin(thisE/(pIn-pSub));
                theNumerator[thisIndex] += theScalibration[thisIndex]*ele2_recHit_E -> at(iRecHit)*FdiEta*(thisIC/thisE)*(pIn-pSub)/thisE*EoPHisto->GetBinContent(EoPbin);
                theDenominator[thisIndex] += theScalibration[thisIndex]*ele2_recHit_E -> at(iRecHit)*FdiEta*thisIC/thisE*EoPHisto->GetBinContent(EoPbin);
              }
            
            }
          
          }

          
          //Fill EoP
          hC_EoP -> Fill(iLoop, thisE/pIn);
        
       
        }
          
    }
    ///! End Cycle on the events
       
   ///New Loop cycle + Save info
   std::cout << ">>>>> [L3][endOfLoop] entering..." << std::endl;

   TH1F auxiliary_IC("auxiliary_IC","auxiliary_IC",50,0.2,1.9);

   ///Fill the histo of IntercalibValues before the solve
    for ( int iIndex = 0; iIndex < 61200; iIndex++ ){
      
      if ( h_Occupancy_hashedIndex -> GetBinContent(iIndex+1) > 0 ){
	
        float thisIntercalibConstant = 1.;
        /// Solve the cases where the recHit energy is always 0 (dead Xtal?)
        bool isDeadXtal = false ;
        if(DeadXtal_HashedIndex.at(0)!=-9999) isDeadXtal = CheckDeadXtal(GetIetaFromHashedIndex(iIndex),GetIphiFromHashedIndex(iIndex));
        if(isDeadXtal == true ) continue;
          
        
        if ( theDenominator[iIndex] != 0. ) thisIntercalibConstant = theNumerator[iIndex]/theDenominator[iIndex];
        float oldIntercalibConstant = 1.;
        if ( iLoop > 0 ) oldIntercalibConstant = h_scale_EB_hashedIndex -> GetBinContent (iIndex+1);
        
	h_scale_EB_hashedIndex -> SetBinContent(iIndex+1, thisIntercalibConstant*oldIntercalibConstant); /// IC product useful for L3 methods
	hC_IntercalibValues -> Fill(iLoop, thisIntercalibConstant); /// IC distribution at each loop
        hC_PullFromScalib -> Fill(iLoop,(thisIntercalibConstant*oldIntercalibConstant-1./theScalibration[iIndex]));
        hC_scale_EB -> Fill(iLoop, GetIphiFromHashedIndex(iIndex), GetIetaFromHashedIndex(iIndex),thisIntercalibConstant*oldIntercalibConstant); ///IC Map
        
        ///Save the new IC coefficient
        auxiliary_IC.Fill(thisIntercalibConstant);

      }

    }
    /// Info in order to test convergence
    g_ICmeanVsLoop -> SetPoint(iLoop, iLoop, auxiliary_IC . GetMean());
    g_ICmeanVsLoop -> SetPointError(iLoop, 0., auxiliary_IC . GetMeanError());
    
    g_ICrmsVsLoop -> SetPoint(iLoop, iLoop, auxiliary_IC . GetRMS());
    g_ICrmsVsLoop -> SetPointError(iLoop, 0., auxiliary_IC . GetRMSError());
   }/// end calibration loop
      
   
   int myPhiIndex = 0;

   for ( int iabseta = MIN_IETA; iabseta < MAX_IETA + 1; iabseta++ ){
     for ( int theZside = -1; theZside < 2; theZside = theZside+2 ){

     ///Setup the histo for fit
     TH1F histoAuxiliary("histoAuxiliary","histoAuxiliary",400, 0.2, 1.9);
     TF1 f1("f1","gaus",0.2,1.9);
     
     int totIphi = 0;
     float meanICforPhiRing = 0.;

       for ( int iphi = MIN_IPHI; iphi <= MAX_IPHI; iphi++ ){
         

         int thisHashedIndex = GetHashedIndexEB(iabseta*theZside, iphi, theZside);
	 if ( h_Occupancy_hashedIndex -> GetBinContent(thisHashedIndex+1) == 0 ) continue;
	 float thisIntercalibConstant = h_scale_EB_hashedIndex -> GetBinContent (thisHashedIndex+1);
         h_scale_EB -> Fill(iphi, iabseta*theZside, thisIntercalibConstant); ///Fill with Last IC Value
         
         if (GetIetaFromHashedIndex(thisHashedIndex) == 85) h_IntercalibValues_test -> Fill (thisIntercalibConstant);
         p_IntercalibValues_iEta -> Fill(GetIetaFromHashedIndex(thisHashedIndex), thisIntercalibConstant);
         
         histoAuxiliary . Fill (thisIntercalibConstant);
         
         ///Vectors
         IetaValues.push_back(iabseta*theZside);
         IphiValues.push_back(iphi);
         ICValues.push_back(thisIntercalibConstant);
         
         meanICforPhiRing += thisIntercalibConstant;
         totIphi++;

       }
       
       for ( int uu = 0; uu < totIphi; uu++ )
         meanICforPhiRingValues.push_back(meanICforPhiRing/totIphi); 
       /// Note this info are not used furhter because channels near to the dead ones are not skipped 
	                                               

       for ( int iphi = MIN_IPHI; iphi <= MAX_IPHI; iphi++ ){

         int thisHashedIndex = GetHashedIndexEB(iabseta*theZside, iphi, theZside);
	 if ( h_Occupancy_hashedIndex -> GetBinContent(thisHashedIndex+1) == 0 ) continue;

         h_scale_EB_meanOnPhi -> Fill(iphi, iabseta*theZside, ICValues.at(myPhiIndex)/meanICforPhiRingValues.at(myPhiIndex));
         myPhiIndex++; /// Normalization IC with the mean of each ring

       }
       f1.SetParameters(histoAuxiliary.GetEntries(),histoAuxiliary.GetMean(),histoAuxiliary.GetRMS());
       f1.SetRange(1-5*histoAuxiliary.GetRMS(), 1+5*histoAuxiliary.GetRMS());
       histoAuxiliary . Fit("f1","QR");
       
       if ( f1.GetParError(2) > 0.5 ) continue;
       h_IntercalibSpread_iEta -> SetBinContent( iabseta*theZside + 85 + 1, f1.GetParameter(2) );
       h_IntercalibSpread_iEta -> SetBinError( iabseta*theZside + 85 + 1, f1.GetParError(2) );

     }
 
   }

}
/// Save infos in the output
void FastCalibratorEB::saveHistos(TFile * f1){

  f1->cd();
  hC_IntercalibValues -> Write(*f1);
  hC_PullFromScalib -> Write(*f1);
  hC_EoP -> Write(*f1);
  hC_scale_EB -> Write("",*f1);
  h_scalib_EB -> Write();

  h_IntercalibValues_test -> Write();
  h_Occupancy_hashedIndex -> Write();
  p_IntercalibValues_iEta -> Write();
  h_Init_IntercalibValues -> Write(); 

  h_IntercalibSpread_iEta -> Write();
  h_scale_EB -> Write();
  h_scale_EB_meanOnPhi -> Write("h_scale_map");
  h_scale_EB_hashedIndex -> Write(); 
  
  h_occupancy -> Write();
  
  g_ICmeanVsLoop -> Write();
  g_ICrmsVsLoop -> Write();

  h_map_Dead_Channels -> Write() ;


  f1->Close();

  return;
}
/// Save E/0 distribution
void FastCalibratorEB::saveEoPeta(TFile * f2){
 f2->cd();
 hC_EoP_eta_ele ->Write(*f2);
 f2->Close(); 
}

///! Acquire fake dead channel list on order to evaluate the effected of IC near to them
void FastCalibratorEB::AcquireDeadXtal(TString inputDeadXtal){

  if(inputDeadXtal!="NULL")
  {
   std::ifstream DeadXtal (inputDeadXtal.Data(),std::ios::binary);
   
   std::string buffer;
   int iEta, iPhi ;
  

   while(!DeadXtal.eof())
   {
    getline(DeadXtal,buffer);
    std::stringstream line( buffer );
    line >> iEta >> iPhi ;

    if(iEta >=0) DeadXtal_HashedIndex.push_back(GetHashedIndexEB(iEta,iPhi,1)) ;
    else DeadXtal_HashedIndex.push_back(GetHashedIndexEB(iEta,iPhi,-1)) ;
  
   
   }

  sort(DeadXtal_HashedIndex.begin(), DeadXtal_HashedIndex.end());
  }
  else{
       DeadXtal_HashedIndex.push_back(-9999);
      }

}
///! Check if the channel is dead or not
bool FastCalibratorEB::CheckDeadXtal(const int & iEta, const int & iPhi){
  int hashed_Index;
  if(iEta>=0) hashed_Index = GetHashedIndexEB(iEta,iPhi,1);
  else hashed_Index = GetHashedIndexEB(iEta,iPhi,-1);

  std::vector<int>::iterator iter = find(DeadXtal_HashedIndex.begin(),DeadXtal_HashedIndex.end(),hashed_Index);

  if(iter !=DeadXtal_HashedIndex.end())
     return true;
  else return false;
}
