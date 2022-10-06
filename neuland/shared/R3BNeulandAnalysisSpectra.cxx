#include "R3BNeulandAnalysisSpectra.h"
#include "FairRunAna.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "THttpServer.h"
#include <iostream>
#include <limits>

#define CLIGHT 29.979245
#define MNEUTRON 939.56563
#define AMU 931.4940954
#define TBEAM 720.0
#define MBEAM 111662.6316

R3BNeulandAnalysisSpectra::R3BNeulandAnalysisSpectra()
    : FairTask("R3BNeulandAnalysisSpectra", 0)
    , fNeulandMappedData("NeulandMappedData")
    , fNeulandCalData("NeulandCalData")
    , fNeulandHits("LandDigi")    // NeulandHits
    , fLosCalData("LosCal")
{
}

InitStatus R3BNeulandAnalysisSpectra::Init()
{   
    // Initialize random number:
    std::srand(std::time(0)); //use current time as seed for random generator

    auto run = FairRunAna::Instance();
    //run->GetHttpServer()->Register("/Tasks", this);
    //run->GetHttpServer()->RegisterCommand("Reset_Neuland", Form("/Tasks/%s/->ResetHistos()", GetName()));

    fNeulandMappedData.Init();
    fNeulandCalData.Init();
    fNeulandHits.Init();
    fLosCalData.Init();

    ahMappedBar1[0] = new TH1D("hMappedBar1fLE", "Mapped: Bars fine 1LE", fNBars, 0.5, fNBars + 0.5); 
    ahMappedBar1[1] = new TH1D("hMappedBar1fTE", "Mapped: Bars fine 1TE", fNBars, 0.5, fNBars + 0.5); 
    ahMappedBar1[2] = new TH1D("hMappedBar1cLE", "Mapped: Bars coarse 1LE", fNBars, 0.5, fNBars + 0.5); 
    ahMappedBar1[3] = new TH1D("hMappedBar1cTE", "Mapped: Bars coarse 1TE", fNBars, 0.5, fNBars + 0.5); 

    ahMappedBar2[0] = new TH1D("hMappedBar2fLE", "Mapped: Bars fine 2LE", fNBars, 0.5, fNBars + 0.5); 
    ahMappedBar2[1] = new TH1D("hMappedBar2fTE", "Mapped: Bars fine 2TE", fNBars, 0.5, fNBars + 0.5); 
    ahMappedBar2[2] = new TH1D("hMappedBar2cLE", "Mapped: Bars coarse 2LE", fNBars, 0.5, fNBars + 0.5); 
    ahMappedBar2[3] = new TH1D("hMappedBar2cTE", "Mapped: Bars coarse 2TE", fNBars, 0.5, fNBars + 0.5); 

    hTstart = new TH1D("hTstart", "Tstart", 1000, 0., 41000.);
    hNstart = new TH1D("hNstart", "Nstart", 20, 0.5, 20.5);

    ahCalTvsBar[0] =
        new TH2D("hCalT1vsBar", "CalLevel: Time1 vs Bars  ", fNBars, 0.5, fNBars + 0.5, 2000, -5000, 15000);
    ahCalTvsBar[1] =
        new TH2D("hCalT2vsBar", "CalLevel: Time2 vs Bars  ", fNBars, 0.5, fNBars + 0.5, 2000, -5000, 15000);

    ahCalEvsBar[0] = new TH2D("hCalE1vsBar", "CalLevel: Energy1 vs Bars", fNBars, 0.5, fNBars + 0.5, 600, 0, 600);
    ahCalEvsBar[1] = new TH2D("hCalE2vsBar", "CalLevel: Energy2 vs Bars", fNBars, 0.5, fNBars + 0.5, 600, 0, 600);

    hHitEvsBar = new TH2D("hHitEvsBar", "HitLevel: Energy vs Bars ", fNBars, 0.5, fNBars + 0.5, 1000, 0, 60);
    hTdiffvsBar = new TH2D("hTdiffvsBar", "Tdiff vs Bars", fNBars, 0.5, fNBars + 0.5, 10000, -100, 100);

    hToFvsBar = new TH2D("hTofvsBar", "Tof vs Bars", fNBars, 0.5, fNBars + 0.5, 10000, -500, 2500); //0,300
    hToFvsTdiff = new TH2D("hTofvsTdiff", "Tof vs Tdiff", 1000, -10300, 10300, 10000, -2400, 10000);
    hToFrawvsBar = new TH2D("hTofrawvsBar", "Tofraw vs Bars", fNBars, 0.5, fNBars + 0.5, 10000, -50000, 50000);
    hToFxvsBar = new TH2D("hTofxvsBar", "Tofx vs Bars", fNBars, 0.5, fNBars + 0.5, 10000, -3000, 3000);
    hToFxxvsBar = new TH2D("hTofxxvsBar", "Tofxx vs Bars", fNBars, 0.5, fNBars + 0.5, 10000, -3000, 3000);
    hToFcvsBar = new TH2D("hTofcvsBar", "Tofc vs Bars", fNBars, 0.5, fNBars + 0.5, 10000, 0, 300);
    hToFvsX = new TH2D("hTofvsX", "Tof vs X", 3000, -200., 200., 10000, 0, 300);
    hTofvsEhit = new TH2D("hTofvsEhit", "Tof vs Ehit", 1000, 0, 60, 5000, 0, 300);

    hTofvsPx = new TH2D("hTofvsPx", "Tof vs Px", 1000, -1000, 1000, 10000, -3000, 3000);
    hPyvsPx = new TH2D("hPyvsPx", "Py vs Px", 1000, -1000, 1000, 1000, -1000, 1000);
    hPxvsPzint = new TH2D("hPxvsPzint", "Px vs Pzint", 2000, -2400, 600, 1000, -1000, 1000);
    hPyvsPzint = new TH2D("hPyvsPzint", "Py vs Pzint", 2000, -2400, 600, 1000, -1000, 1000);
    hPxvsPz = new TH2D("hPxvsPz", "Px vs Pz", 2000, -400, 1600, 1000, -1000, 1000);
    hPyvsPz = new TH2D("hPyvsPz", "Py vs Pz", 2000, -400, 1600, 1000, -1000, 1000);

    hPxvsBar = new TH2D("hPxvsBar", "Px vs Bars", fNBars, 0.5, fNBars + 0.5, 5000, -1000, 1000);
    hPyvsBar = new TH2D("hPyvsBar", "Py vs Bars", fNBars, 0.5, fNBars + 0.5, 5000, -1000, 1000);
    hPzintvsBar = new TH2D("hPzintvsBar", "Pzint vs Bars", fNBars, 0.5, fNBars + 0.5, 5000, -1000, 1000);
    hVelvsBar = new TH2D("hVelvsBar", "Velocity vs Bars", fNBars, 0.5, fNBars + 0.5, 5000, -50, 50);
    hDV = new TH1D("hDV", "Vel - Vel_beam", 5000, -100, 100);

    hHitEvsBarCosmics = new TH2D("hHitEvsBarCosmics", "HitLevel: Energy vs Bars cosmics", fNBars, 0.5, fNBars + 0.5, 1000, 0, 60);
    hTdiffvsBarCosmics = new TH2D("hTdiffvsBarCosmics", "Tdiff vs Bars cosmics", fNBars, 0.5, fNBars + 0.5, 10000, -100, 100);
    hDT375 = new TH2D("hDT375", "Thit - Thit375 vs Bars cosmics", fNBars, 0.5, fNBars + 0.5, 1000, -20, 20);
    hDT425 = new TH2D("hDT425", "Thit - Thit425 vs Bars cosmics", fNBars, 0.5, fNBars + 0.5, 1000, -20, 20);

    for (unsigned int i = 0; i < fNPlanes; i++)
    {
        ahXYperPlane[i] = new TH2D("hHitXYPlane" + TString::Itoa(i, 10),
                                   "Hit XY Plane" + TString::Itoa(i, 10),
                                   150,
                                   -150,
                                   150,
                                   150,
                                   -150,
                                   150);
    }

    hXY = new TH2D("hHitXYall", "Hit XY all planes",  300, -150, 150, 300, -150, 150);
    hXYGamma = new TH2D("hHitXYallGamma", "Hit XY gamma all planes",  300, -150, 150, 300, -150, 150);
    hXYCut = new TH2D("hHitXYallCut", "Hit XY cut all planes",  300, -150, 150, 300, -150, 150);
 
    for (unsigned int i = 0; i < fNPlanes; i++)
    {
        ahXYperPlaneGamma[i] = new TH2D("hHitXYPlaneGamma" + TString::Itoa(i, 10),
                                   "Hit XY Plane gamma" + TString::Itoa(i, 10),
                                   150,
                                   -150,
                                   150,
                                   150,
                                   -150,
                                   150);
    }

    for (unsigned int i = 0; i < fNPlanes; i++)
    {
        ahXYperPlaneCut[i] = new TH2D("hHitXYPlaneCut" + TString::Itoa(i, 10),
                                   "Hit XY Plane cut" + TString::Itoa(i, 10),
                                   150,
                                   -150,
                                   150,
                                   150,
                                   -150,
                                   150);
    }

    return kSUCCESS;
}

void R3BNeulandAnalysisSpectra::Exec(Option_t*)
{
    const double start = GetTstart();

    const auto mappedData = fNeulandMappedData.Retrieve();
    const auto calData = fNeulandCalData.Retrieve();
    const auto hits = fNeulandHits.Retrieve();

    for (const auto& mapped : mappedData)
    {
        const auto plane = mapped->GetPlaneId();
        const auto barp = mapped->GetBarId();
	const auto bar = (plane-1)*50 + barp;
	
	if (mapped->GetFineTime1LE()>0) ahMappedBar1[0]->Fill(bar);
	if (mapped->GetFineTime1TE()>0) ahMappedBar1[1]->Fill(bar);
	if (mapped->GetCoarseTime1LE()>0) ahMappedBar1[2]->Fill(bar);
	if (mapped->GetCoarseTime1TE()>0) ahMappedBar1[3]->Fill(bar);
	if (mapped->GetFineTime2LE()>0) ahMappedBar2[0]->Fill(bar);
	if (mapped->GetFineTime2TE()>0) ahMappedBar2[1]->Fill(bar);
	if (mapped->GetCoarseTime2LE()>0) ahMappedBar2[2]->Fill(bar);
	if (mapped->GetCoarseTime2TE()>0) ahMappedBar2[3]->Fill(bar);
    }

    for (const auto& data : calData)
    {
        const auto side = data->GetSide() - 1; // [1,2] -> [0,1]
        const auto bar = data->GetBarId();
        ahCalTvsBar[side]->Fill(bar, data->GetTime());
        ahCalEvsBar[side]->Fill(bar, data->GetQdc());
    }

    Double_t randx;

    Double_t ebeam, pbeam, vbeam, beta, gamma;
    Double_t px, py, pz, pzint;

    ebeam = 120*TBEAM+MBEAM;
    
    pbeam = sqrt(ebeam*ebeam-MBEAM*MBEAM);
    beta = pbeam/ebeam;
    gamma = 1./sqrt(1-beta*beta);
    vbeam = beta*CLIGHT;
    
    for (const auto& hit : hits)
    {
        const auto bar = hit->GetPaddleNr();
	if (IsBeam())
	  {
	    hHitEvsBar->Fill(bar, hit->GetQdc());
	    hTdiffvsBar->Fill(bar, hit->GetTdcL() - hit->GetTdcR());

	    //if (fabs(hit->GetTdcL() - hit->GetTdcR())>20.) continue;
	    
	    Double_t s = sqrt(hit->GetXX()*hit->GetXX()+
			      hit->GetYY()*hit->GetYY()+
			      hit->GetZZ()*hit->GetZZ());
	    
	    Double_t tof_corr = (s - 1520.)/CLIGHT;

	    randx = (std::rand() / (float)RAND_MAX);
	    const int plane = static_cast<const int>(std::floor((hit->GetPaddleNr()-1) / 50));

	    Double_t tof = remainder(hit->GetTdc() - start+5*8192+2000,5*2048);
	    Double_t v = s/tof;
	    Double_t betan = v/CLIGHT;
	    Double_t gamman = 1./sqrt(1-betan*betan);
	    Double_t en = MNEUTRON*gamman;
	    
	    //if (fabs(remainder(hit->GetTdc() - start+5*8192+2000,5*2048) - tof_corr-140.)<40.) {
	    //if (remainder(hit->GetTdc() - start+5*8192+2000,5*2048) - tof_corr>180.) {

	    //randx=0.5;
	    //px = MNEUTRON*betan*gamman*(hit->GetXX()+(plane%2)*5.*(randx-0.5))/s;
	    px = MNEUTRON/CLIGHT*gamman*(hit->GetXX()+(plane%2)*5.*(randx-0.5))/tof;
	    randx = (std::rand() / (float)RAND_MAX);
	    //randx=0.5;
	    py = MNEUTRON*betan*gamman*(hit->GetYY()+((plane+1)%2)*5.*(randx-0.5))/s;
	    randx = (std::rand() / (float)RAND_MAX);
	    //randx=0.5;
	    pz = MNEUTRON*betan*gamman*(hit->GetZZ()+2.5+5.*(randx-0.5))/s;
	    pzint = gamma*(pz - beta*en);
	    
	    hTofvsPx->Fill(px, tof);
	    hPyvsPx->Fill(px, py);
	    hPxvsPzint->Fill(pzint, px);
	    hPyvsPzint->Fill(pzint, py);

	    hPxvsPz->Fill(pz, px);
	    hPyvsPz->Fill(pz, py);

	    hPxvsBar->Fill(bar, px);
	    hPyvsBar->Fill(bar, py);
	    hPzintvsBar->Fill(bar, pzint);
	    //}

	    hVelvsBar->Fill(bar, v);
	    hDV->Fill(v-vbeam);
	    
	    randx = (std::rand() / (float)RAND_MAX);
	    //randx=0.5;
	    if (hit->GetQdc()>7.)
	      {
		hToFvsX->Fill(hit->GetXX()+(plane%2)*5.*(randx-0.5), remainder(hit->GetTdc() - start+5*8192+2000,5*2048));
		hToFrawvsBar->Fill(bar, hit->GetTdc() - start);
		hToFvsBar->Fill(bar, remainder(hit->GetTdc() - start+5*8192+2000,5*2048));
		hToFvsTdiff->Fill(hit->GetTdcL() - hit->GetTdcR(), remainder(hit->GetTdc() - start+5*8192+2000,5*2048));
		hToFxvsBar->Fill(bar, remainder(hit->GetTdc() - start+5*8192+2500,5*2048));
		hToFxxvsBar->Fill(bar, remainder(hit->GetTdc() - start+5*8192+5000,5*2048));
		hToFcvsBar->Fill(bar, remainder(hit->GetTdc() - start+5*8192+2000,5*2048) - tof_corr);
		//hToFvsBar->Fill(bar, 1520./s*remainder(hit->GetTdc() - start+5*8192+2000,5*2048));
	      }
	    
	    hTofvsEhit->Fill(hit->GetQdc(), remainder(hit->GetTdc() - start+5*8192+2000,5*2048) - tof_corr);
	    //hTofvsEhit->Fill(hit->GetQdc(), 1520./s*remainder(hit->GetTdc() - start+5*8192+2000,5*2048));
	    
	    ahXYperPlane[plane]->Fill(hit->GetXX()+(plane%2)*5.*(randx-0.5),
				      hit->GetYY()+((plane+1)%2)*5.*(randx-0.5));

	    hXY->Fill(hit->GetXX()+(plane%2)*5.*(randx-0.5),
				      hit->GetYY()+((plane+1)%2)*5.*(randx-0.5));

	    if (fabs(remainder(hit->GetTdc() - start+5*8192+2000,5*2048) - tof_corr-50.7)<0.3) {
	      ahXYperPlaneGamma[plane]->Fill(hit->GetXX()+(plane%2)*5.*(randx-0.5),
					     hit->GetYY()+((plane+1)%2)*5.*(randx-0.5));

	      if (hit->GetQdc()>7.)
		hXYGamma->Fill(hit->GetXX()+(plane%2)*5.*(randx-0.5),
			       hit->GetYY()+((plane+1)%2)*5.*(randx-0.5));
	    }	      
	    
	    if (fabs(remainder(hit->GetTdc() - start+5*8192+2000,5*2048) - tof_corr-65.)<10.) {
	      ahXYperPlaneCut[plane]->Fill(hit->GetXX()+(plane%2)*5.*(randx-0.5),
					   hit->GetYY()+((plane+1)%2)*5.*(randx-0.5));
	      hXYCut->Fill(hit->GetXX()+(plane%2)*5.*(randx-0.5),
			   hit->GetYY()+((plane+1)%2)*5.*(randx-0.5));
	    }
	  }
	else
	  {
	    hHitEvsBarCosmics->Fill(bar, hit->GetQdc());
	    hTdiffvsBarCosmics->Fill(bar, hit->GetTdcL() - hit->GetTdcR());

	    for (const auto& hitref : hits)
	      {
		if ((hitref->GetPaddleNr() == 375) && (bar != 375))
		  hDT375->Fill(bar, hit->GetTdc() - hitref->GetTdc());
		if ((hitref->GetPaddleNr() == 425) && (bar != 425))
		  hDT425->Fill(bar, hit->GetTdc() - hitref->GetTdc());
	      }
	  }
    }
}

void R3BNeulandAnalysisSpectra::FinishTask()
{
    TDirectory* tmp = gDirectory;
    FairRootManager::Instance()->GetOutFile()->cd();

    gDirectory->mkdir("R3BNeulandAnalysisSpectra");
    gDirectory->cd("R3BNeulandAnalysisSpectra");

    ahMappedBar1[0]->Write();
    ahMappedBar1[1]->Write();
    ahMappedBar1[2]->Write();
    ahMappedBar1[3]->Write();
    ahMappedBar2[0]->Write();
    ahMappedBar2[1]->Write();
    ahMappedBar2[2]->Write();
    ahMappedBar2[3]->Write();
  
    hTstart->Write();
    hNstart->Write();

    ahCalTvsBar[0]->Write();
    ahCalTvsBar[1]->Write();
    ahCalEvsBar[0]->Write();
    ahCalEvsBar[1]->Write();

    hHitEvsBar->Write();

    hHitEvsBarCosmics->Write();

    hTdiffvsBar->Write();
    hToFvsBar->Write();
    hToFvsTdiff->Write();
    hToFrawvsBar->Write();
    hToFxvsBar->Write();
    hToFxxvsBar->Write();
    hToFcvsBar->Write();
    hToFvsX->Write();
    hTofvsEhit->Write();

    hTofvsPx->Write();
    hPyvsPx->Write();
    hPxvsPz->Write();
    hPyvsPz->Write();
    hPxvsPzint->Write();
    hPyvsPzint->Write();
    
    hPxvsBar->Write();
    hPyvsBar->Write();
    hPzintvsBar->Write();
    hVelvsBar->Write();
    hDV->Write();
    
    hTdiffvsBarCosmics->Write();

    hXY->Write();
    hXYGamma->Write();
    hXYCut->Write();
    
    for (unsigned int i = 0; i < fNPlanes; i++)
    {
        ahXYperPlane[i]->Write();
        ahXYperPlaneGamma[i]->Write();
        ahXYperPlaneCut[i]->Write();
    }

    gDirectory = tmp;
}

double R3BNeulandAnalysisSpectra::GetTstart() const
{
    const auto losCalData = fLosCalData.Retrieve();

    if (losCalData.empty())
    {
        return std::numeric_limits<double>::quiet_NaN();
    }

    hNstart->Fill(losCalData.size());
    for (const auto& los : fLosCalData.Retrieve())
    {
        hTstart->Fill(los->GetMeanTimeVFTX());
    }

    return losCalData.back()->GetMeanTimeVFTX();
}

bool R3BNeulandAnalysisSpectra::IsBeam() const { return !fLosCalData.Retrieve().empty(); }

ClassImp(R3BNeulandAnalysisSpectra)
