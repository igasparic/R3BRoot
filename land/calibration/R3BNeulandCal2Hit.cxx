// -----------------------------------------------------------------------------
// -----                          R3BNeulandCal2Hit                        -----
// -----                   Created 07-05-2014 by D.Kresan                  -----
// -----------------------------------------------------------------------------

#include "TClonesArray.h"
#include "TMath.h"

#include "FairRootManager.h"
#include "FairLogger.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "R3BNeulandCalData.h"
#include "R3BLandDigi.h"
#include "R3BNeulandCal2Hit.h"
#include "R3BNeulandHitPar.h"
#include "R3BTCalPar.h"

#include "TH1F.h"
#include "TGraph2D.h"
#include "TCanvas.h"
#include "TH2F.h"

using std::ifstream;

R3BNeulandCal2Hit::R3BNeulandCal2Hit()
   : fLandPmt (NULL)
   , fLandDigi (new TClonesArray ("R3BLandDigi"))
   , fNDigi (0)
   , fFirstPlaneHorizontal (kTRUE) {
}

R3BNeulandCal2Hit::R3BNeulandCal2Hit (const char* name, Int_t iVerbose)
   : FairTask (name, iVerbose)
   , fLandPmt (NULL)
   , fLandDigi (new TClonesArray ("R3BLandDigi"))
   , fNDigi (0)
   , fFirstPlaneHorizontal (kTRUE) {
}

R3BNeulandCal2Hit::~R3BNeulandCal2Hit() {
}

InitStatus R3BNeulandCal2Hit::Init() {
   FairRootManager* fMan = FairRootManager::Instance();
   if (!fMan) {
     //FairLogger::GetLogger()->Fatal (MESSAGE_ORIGIN, "FairRootManager not found");
     LOG(ERROR) << "FairRootManager not found" << FairLogger::endl;
   }

   fLandPmt = (TClonesArray*) fMan->GetObject ("NeulandCalData");
   if (!fLandPmt) {
     //FairLogger::GetLogger()->Fatal (MESSAGE_ORIGIN, "NeulandCalData not found");
      LOG(ERROR) << "NeulandCalData not found" << FairLogger::endl;
   }

   fMan->Register ("LandDigi", "Land", fLandDigi, kTRUE);
   
   fNEvent = 0;
    
   SetParameter();
   return kSUCCESS;
}

void R3BNeulandCal2Hit::SetParContainers() {
  //FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  //fPar = (R3BNeulandHitPar*) rtdb->getContainer ("NeulandHitPar");
  
   fPar = (R3BNeulandHitPar*)FairRuntimeDb::instance()->getContainer ("NeulandHitPar");
}



void R3BNeulandCal2Hit::SetParameter(){
   std::map<Int_t, Bool_t> tempMapIsSet;
   std::map<Int_t, Double_t> tempMapVeff;
   std::map<Int_t, Double_t> tempMapTSync;
   std::map<Int_t, Double_t> tempMapEGain;
   
   for (Int_t i = 0; i < fPar->GetNumModulePar(); i++) {
      R3BNeulandHitModulePar* fModulePar = fPar->GetModuleParAt(i);
      Int_t id = fModulePar->GetModuleId() * 2 + fModulePar->GetSide() - 3;
      tempMapIsSet[id] = kTRUE;
      tempMapVeff[id] = fModulePar->GetEffectiveSpeed();
      tempMapTSync[id] = fModulePar->GetTimeOffset() + fPar->GetGlobalTimeOffset();
      tempMapEGain[id] = fModulePar->GetEnergieGain();
   }
     
   LOG(INFO) << "R3BNeulandCal2Hit::SetParameter : Number of Parameters: " << fPar->GetNumModulePar() << FairLogger::endl;
   
   fMapIsSet = tempMapIsSet;
   fMapVeff = tempMapVeff;
   fMapTSync = tempMapTSync;
   fMapEGain = tempMapEGain;
}

InitStatus R3BNeulandCal2Hit::ReInit() {
   SetParContainers();
   SetParameter();
   return kSUCCESS;
}

void R3BNeulandCal2Hit::Exec (Option_t* option) {

  if(fVerbose != 0 && ++fNEvent % 100000 == 0)
    //FairLogger::GetLogger()->Info(MESSAGE_ORIGIN, "R3BNeulandCal2Hit::Exec : Event: %8d", fNEvent);
    LOG(ERROR) << "R3BNeulandCal2Hit::Exec : Event: " << fNEvent << FairLogger::endl;

     
   Int_t nLandPmt = fLandPmt->GetEntriesFast();
  
   R3BNeulandCalData* pmt1 = NULL;
   R3BNeulandCalData* pmt2 = NULL;
   Int_t barId;
   Int_t plane;
   Double_t veff;
   Double_t tdcL, tdcR, tdc;
   Double_t qdcL, qdcR, qdc;
   Double_t x, y, z;
   Int_t id = 0;

   Int_t bar_fired[801] = {0};
   
   if (fFirstPlaneHorizontal) {
      id = 1;
   }
   for (Int_t i1 = 0; i1 < nLandPmt; i1++) {
      pmt1 = (R3BNeulandCalData*) fLandPmt->At (i1);
      if (1 != pmt1->GetSide()) {
         continue;
      }
      barId = pmt1->GetBarId();
      if (! fMapIsSet[(barId-1)*2]) {
         continue;
      }

      Int_t skip = 0;
      
      for (Int_t i2 = 0; i2 < nLandPmt; i2++) {
         if (i1 == i2) {
            continue;
         }
         pmt2 = (R3BNeulandCalData*) fLandPmt->At (i2);
         if (pmt2->GetBarId() != barId) {
            continue;
         }
         if (2 != pmt2->GetSide()) {
            continue;
         }
	 if (skip < bar_fired[barId]) {
	   skip++;
	   continue;
	 }
	 
	 bar_fired[barId]++;
	 
         qdcL = pmt1->GetQdc() * fMapEGain[pmt1->GetBarId() * 2 - 2];
         qdcR = pmt2->GetQdc() * fMapEGain[pmt1->GetBarId() * 2 - 1];
	 		 
         veff = fMapVeff[(barId-1)*2];
         tdcL = pmt1->GetTime() + fMapTSync[pmt1->GetBarId() * 2 - 2];
         tdcR = pmt2->GetTime() + fMapTSync[pmt2->GetBarId() * 2 - 1];

	 //cout << skip << "    " << pmt1->GetBarId() << "    " << fMapTSync[barId * 2 - 2] << endl;

	 if (tdcL - tdcR < -0.5*5.*2048) tdcR = tdcR - 5.*2048; 
	 if (tdcL - tdcR >  0.5*5.*2048) tdcL = tdcL - 5.*2048; 

	 tdc = (tdcL + tdcR) / 2. - fGlobalTimeOffset;
         qdc = TMath::Sqrt (qdcL * qdcR);
         plane = ((barId - 1) / 50) + 1;
         if (id == plane % 2) {
            x = veff * (tdcR - tdcL);
            y = (barId - 0.5 - (plane - 1) * 50) * 5. - 125.;
         }
         else {
            x = (barId - 0.5 - (plane - 1) * 50) * 5. - 125.;
            y = veff * (tdcR - tdcL);
         }
         z = (plane - 0.5) * 5. + fDistanceToTarget;
	 
	 new ( (*fLandDigi) [fNDigi]) R3BLandDigi (barId, tdcL, tdcR, tdc, qdcL, qdcR, qdc, x, y, z);
	 fNDigi += 1;	 
      }
   }
}

void R3BNeulandCal2Hit::FinishEvent() {
   fLandDigi->Clear();
   fNDigi = 0;
}

ClassImp (R3BNeulandCal2Hit)
