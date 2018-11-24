// ------------------------------------------------------------
// -----               R3BNeulandMapped2Cal               -----
// -----          Created 22-04-2014 by D.Kresan          -----
// ------------------------------------------------------------

#include "R3BNeulandMapped2Cal.h"

#include "R3BTCalEngine.h"
#include "R3BPaddleTamexMappedData.h"
#include "R3BNeulandCalData.h"
#include "R3BTCalPar.h"
#include "R3BEventHeader.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TH2F.h"

R3BNeulandMapped2Cal::R3BNeulandMapped2Cal()
    : FairTask("NeulandMapped2Cal", 1)
    , fNEvents(0)
    , fPulserMode(kFALSE)
    , fWalkEnabled(kTRUE)
    , fMapped(NULL)
    , fPmt(new TClonesArray("R3BNeulandCalData"))
    , fNPmt(0)
    , fTcalPar(NULL)
    , fTrigger(-1)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
}

R3BNeulandMapped2Cal::R3BNeulandMapped2Cal(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fNEvents(0)
    , fPulserMode(kFALSE)
    , fWalkEnabled(kTRUE)
    , fMapped(NULL)
    , fPmt(new TClonesArray("R3BNeulandCalData"))
    , fNPmt(0)
    , fTcalPar(NULL)
    , fTrigger(-1)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
}

R3BNeulandMapped2Cal::~R3BNeulandMapped2Cal()
{
    if (fPmt)
    {
        delete fPmt;
        fPmt = NULL;
        fNPmt = 0;
    }
}

InitStatus R3BNeulandMapped2Cal::Init()
{
  fNofTcalPars = fTcalPar->GetNumModulePar();

  if (fNofTcalPars==0)
    {
      LOG(ERROR) << "There are no TCal parameters in container LandTCalPar" << FairLogger::endl;
      return kFATAL;
    }

  LOG(INFO) << "R3BNeulandMapped2Cal::Init : read " << fNofTcalPars << " calibrated modules"
	    << FairLogger::endl;
  
  FairRootManager* mgr = FairRootManager::Instance();
  if (NULL == mgr)
    {
      FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
    }
  
  header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
  if (NULL == header)
    {
      FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Branch R3BEventHeader not found");
    }
  
  fMapped = (TClonesArray*)mgr->GetObject("NeulandMappedData");
  if (NULL == fMapped)
    {
      FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Branch NeulandMapped not found");
    }
  
  mgr->Register("NeulandCalData", "Neuland", fPmt, kTRUE);

  htcal1 = new TH2F("htcal1", "htcal1", 700, 0.5, 700.5, 500, -1., 6.);
  htcal2 = new TH2F("htcal2", "htcal2", 700, 0.5, 700.5, 500, -1., 6.);
  htcal3 = new TH2F("htcal3", "htcal3", 700, 0.5, 700.5, 500, -1., 6.);
  htcal4 = new TH2F("htcal4", "htcal4", 700, 0.5, 700.5, 500, -1., 6.);
  
  return kSUCCESS;
}

void R3BNeulandMapped2Cal::SetParContainers()
{
  fTcalPar = (R3BTCalPar*)FairRuntimeDb::instance()->getContainer("LandTCalPar");
  
  if (!fTcalPar)
    {
      LOG(ERROR) << "Could not get access to LandTCalPar-Container." << FairLogger::endl;
      fNofTcalPars=0;
      return;
    }
}

InitStatus R3BNeulandMapped2Cal::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

void R3BNeulandMapped2Cal::Exec(Option_t* option)
{
    if (fTrigger >= 0)
    {
        if (header->GetTrigger() != fTrigger)
        {
            return;
        }
    }

    Int_t nHits = fMapped->GetEntriesFast();
    if (fPulserMode)
    {
        if (nHits < fNofPMTs)
        {
            return;
        }
    }
    else
    {
        if (nHits > (fNofPMTs / 2))
        {
            return;
        }
    }

    if(nHits >= fNhitmin)  // ig  0
    {
        MakeCal();
    }

}

void R3BNeulandMapped2Cal::MakeCal()
{
    Int_t nHits = fMapped->GetEntriesFast();

    R3BTCalModulePar* par;

    Int_t tdc;
    Double_t timeLE;
    Double_t timeTE;

    for (Int_t ihit = 0; ihit < nHits; ihit++)
    {
      R3BPaddleTamexMappedData* hit = (R3BPaddleTamexMappedData*)fMapped->At(ihit);
        if (NULL == hit)
        {
            continue;
        }

        Int_t iPlane = hit->GetPlaneId();
        Int_t iBar = hit->GetBarId();
        Int_t iSide = -1 == hit->fCoarseTime1LE ? 2 : 1;

	if (hit->Is17())
	  {
	    // 17-th channel
	    continue;
	  }

	if ((iPlane<1) || (iPlane>fNofPlanes))
	  {
	    LOG(INFO) << "R3BNeulandMapped2TCal::Exec : Plane number out of range: " << 
	      iPlane << FairLogger::endl;
	    continue;
	  }       
	if ((iBar<1) || (iBar>fNofBarsPerPlane))
	  {
	    LOG(INFO) << "R3BNeulandMapped2TCal::Exec : Bar number out of range: " << 
	      iBar << FairLogger::endl;
	    continue;
	  }

	int edge = 2 * iSide - 1;
	
	// Convert TDC to [ns] leading
	if (! (par = fTcalPar->GetModuleParAt(iPlane, iBar, edge)))
	  {
	    LOG(DEBUG) << "R3BNeulandTcal::Exec : Tcal par not found, barId: " << iBar << ", side: " << iSide
		       << FairLogger::endl;
	    continue;
	  }
	
	tdc = 1 == iSide ? hit->fFineTime1LE : hit->fFineTime2LE;
	timeLE = par->GetTimeVFTX(tdc);

	// Convert TDC to [ns] trailing
	if (! (par = fTcalPar->GetModuleParAt(iPlane, iBar, edge + 1)))
	  {
	    LOG(DEBUG) << "R3BNeulandTcal::Exec : Tcal par not found, barId: " << iBar << ", side: " << iSide
                      << FairLogger::endl;
	    continue;
	  }
	
	tdc = 1 == iSide ? hit->fFineTime1TE : hit->fFineTime2TE;
	timeTE = par->GetTimeVFTX(tdc);
	
	if (timeLE < 0. || timeLE > fClockFreq || timeTE < 0. || timeTE > fClockFreq)
	  {
	    LOG(ERROR) << "R3BNeulandMapped2Tcal::Exec : error in time calibration: ch= "
		       << iPlane << iBar << iSide  << ", tdc= " << tdc
		       << ", time leading edge = " << timeLE << ", time trailing edge = " << timeTE << FairLogger::endl;
	    continue;
	  }

	if (1 == iSide) {
	  htcal1->Fill((iPlane-1)*50+iBar, timeLE);
	  htcal3->Fill((iPlane-1)*50+iBar, timeTE);
	}
	if (2 == iSide) {
	  htcal2->Fill((iPlane-1)*50+iBar, timeLE);
	  htcal4->Fill((iPlane-1)*50+iBar, timeTE);
	}

	auto coarse = 1 == iSide ? hit->fCoarseTime1LE : hit->fCoarseTime2LE;
	timeLE = fClockFreq-timeLE + coarse * fClockFreq;
	coarse = 1 == iSide ? hit->fCoarseTime1TE : hit->fCoarseTime2TE;
	timeTE = fClockFreq-timeTE + coarse * fClockFreq;
	
	if (timeTE-timeLE < 0)
	  {
	    new ((*fPmt)[fNPmt]) R3BNeulandCalData((iPlane-1)*50+iBar, iSide, timeLE,
						   2048*fClockFreq + timeTE-timeLE);
	    fNPmt += 1;
	  }
	else
	  {
	    new ((*fPmt)[fNPmt]) R3BNeulandCalData((iPlane-1)*50+iBar, iSide, timeLE,
						   timeTE-timeLE);
	    fNPmt += 1;
	  }	    
    }
}

void R3BNeulandMapped2Cal::FinishEvent()
{
    if (fVerbose && 0 == (fNEvents % 1000))
    {
        LOG(INFO) << "R3BNeulandMapped2Cal::Exec : event=" << fNEvents << " nPMTs=" << fNPmt << FairLogger::endl;
    }

    if (fPmt)
    {
        fPmt->Clear();
        fNPmt = 0;
    }

    fNEvents += 1;
}

void R3BNeulandMapped2Cal::FinishTask()
{

  htcal1->Write();
  htcal2->Write();
  htcal3->Write();
  htcal4->Write();
  
}

ClassImp(R3BNeulandMapped2Cal)
