#include "R3BNeulandOnlineSpectraTest.h"
#include "FairRunOnline.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "THttpServer.h"
#include <iostream>
#include <limits>

R3BNeulandOnlineSpectraTest::R3BNeulandOnlineSpectraTest()
    : FairTask("R3BNeulandOnlineSpectraTest", 0)
    , fNeulandMappedData("NeulandMappedData")
    , fNeulandCalData("NeulandCalData")
    , fNeulandHits("LandDigi")    // NeulandHits
{
}

InitStatus R3BNeulandOnlineSpectraTest::Init()
{   
    // Initialize random number:
    std::srand(std::time(0)); //use current time as seed for random generator

    hevents = 0;
    
    auto run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("/Tasks", this);
    run->GetHttpServer()->RegisterCommand("Reset_Neuland", Form("/Tasks/%s/->ResetHistos()", GetName()));

    fNeulandMappedData.Init();
    fNeulandCalData.Init();
    fNeulandHits.Init();

    auto canvasMapped = new TCanvas("NeulandMapped", "NeulandMapped", 10, 10, 850, 850);
    canvasMapped->Divide(1, 2);

    canvasMapped->cd(1);
    ahMappedBar1[0] = new TH1D("hMappedBar1fLE", "Mapped: Bars fine 1LE", fNBars, 0.5, fNBars + 0.5); 
    ahMappedBar1[0]->Draw();
    ahMappedBar1[1] = new TH1D("hMappedBar1fTE", "Mapped: Bars fine 1TE", fNBars, 0.5, fNBars + 0.5); 
    ahMappedBar1[1]->SetLineColor(1);
    ahMappedBar1[1]->Draw("same");
    ahMappedBar1[2] = new TH1D("hMappedBar1cLE", "Mapped: Bars coarse 1LE", fNBars, 0.5, fNBars + 0.5); 
    ahMappedBar1[2]->SetLineColor(2);
    ahMappedBar1[2]->Draw("same");
    ahMappedBar1[3] = new TH1D("hMappedBar1cTE", "Mapped: Bars coarse 1TE", fNBars, 0.5, fNBars + 0.5); 
    ahMappedBar1[3]->SetLineColor(6);
    ahMappedBar1[3]->Draw("same");

    canvasMapped->cd(2);
    ahMappedBar2[0] = new TH1D("hMappedBar2fLE", "Mapped: Bars fine 2LE", fNBars, 0.5, fNBars + 0.5); 
    ahMappedBar2[0]->Draw();
    ahMappedBar2[1] = new TH1D("hMappedBar2fTE", "Mapped: Bars fine 2TE", fNBars, 0.5, fNBars + 0.5); 
    ahMappedBar2[1]->SetLineColor(1);
    ahMappedBar2[1]->Draw("same");
    ahMappedBar2[2] = new TH1D("hMappedBar2cLE", "Mapped: Bars coarse 2LE", fNBars, 0.5, fNBars + 0.5); 
    ahMappedBar2[2]->SetLineColor(2);
    ahMappedBar2[2]->Draw("same");
    ahMappedBar2[3] = new TH1D("hMappedBar2cTE", "Mapped: Bars coarse 2TE", fNBars, 0.5, fNBars + 0.5); 
    ahMappedBar2[3]->SetLineColor(6);
    ahMappedBar2[3]->Draw("same");

    canvasMapped->cd(0);
    run->AddObject(canvasMapped);

    auto canvasEvents = new TCanvas("CalvsEvnt", "CalvsEvnt", 10, 10, 850, 850);
    canvasEvents->Divide(2, 3);

    hQdc11vsEvnt = new TH2D("hQdc11vsEvnt", "Bar1 Qdc1 vs Evnt", 1000, 0., 1000000., 600, 0., 600.);
    canvasEvents->cd(1);
    hQdc11vsEvnt->Draw("colz");

    hQdc12vsEvnt = new TH2D("hQdc12vsEvnt", "Bar1 Qdc2 vs Evnt", 1000, 0., 1000000., 600, 0., 600.);
    canvasEvents->cd(2);
    hQdc12vsEvnt->Draw("colz");
    
    hQdc21vsEvnt = new TH2D("hQdc21vsEvnt", "Bar2 Qdc1 vs Evnt", 1000, 0., 1000000., 600, 0., 600.);
    canvasEvents->cd(3);
    hQdc21vsEvnt->Draw("colz");

    hQdc22vsEvnt = new TH2D("hQdc22vsEvnt", "Bar2 Qdc2 vs Evnt", 1000, 0., 1000000., 600, 0., 600.);
    canvasEvents->cd(4);
    hQdc22vsEvnt->Draw("colz");
    
    ahTdiffvsEvnt[0] = new TH2D("hTdiffvsEvnt1", "Bar1 Tdiff vs Evnt", 1000, 0., 1000000., 200, 50., 150.);
    canvasEvents->cd(5);
    ahTdiffvsEvnt[0]->Draw("colz");

    ahTdiffvsEvnt[1] = new TH2D("hTdiffvsEvnt2", "Bar2 Tdiff vs Evnt", 1000, 0., 1000000., 200, 0., 100.);
    canvasEvents->cd(6);
    ahTdiffvsEvnt[1]->Draw("colz");
    
    canvasEvents->cd(0);
    run->AddObject(canvasEvents);

    auto canvasCal = new TCanvas("NeulandCal", "NeulandCal", 10, 10, 850, 850);
    canvasCal->Divide(2, 2);
    
    ahCalTvsBar[0] =
      new TH2D("hCalT1vsBar", "CalLevel: Time1 vs Bars  ", fNBars, 0.5, fNBars + 0.5, 2000, -5000, 15000);
    canvasCal->cd(1);
    ahCalTvsBar[0]->Draw("colz");

    ahCalTvsBar[1] =
        new TH2D("hCalT2vsBar", "CalLevel: Time2 vs Bars  ", fNBars, 0.5, fNBars + 0.5, 2000, -5000, 15000);
    canvasCal->cd(2);
    ahCalTvsBar[1]->Draw("colz");

    ahCalEvsBar[0] = new TH2D("hCalE1vsBar", "CalLevel: Energy1 vs Bars", fNBars, 0.5, fNBars + 0.5, 600, 0, 600);
    canvasCal->cd(3);
    ahCalEvsBar[0]->Draw("colz");

    ahCalEvsBar[1] = new TH2D("hCalE2vsBar", "CalLevel: Energy2 vs Bars", fNBars, 0.5, fNBars + 0.5, 600, 0, 600);
    canvasCal->cd(4);
    ahCalEvsBar[1]->Draw("colz");

    canvasCal->cd(0);
    run->AddObject(canvasCal);

    auto canvasHit = new TCanvas("NeulandHit", "NeulandHit", 10, 10, 850, 850);
    canvasHit->Divide(2, 3);

    ahQdcvsTdiff[0] = new TH2D("hQdcvsTdiff1", "Qdc vs Tdiff Bar1", 200, 50., 150., 600, 0., 600.);
    canvasHit->cd(1);
    ahQdcvsTdiff[0]->Draw("colz");

    ahQdcvsTdiff[1] = new TH2D("hQdcvsTdiff2", "Qdc vs Tdiff Bar2", 200, 0., 100., 600, 0., 600.);
    canvasHit->cd(2);
    ahQdcvsTdiff[1]->Draw("colz");

    ahQdcvsTdiff1[0] = new TH2D("hQdc1vsTdiff1", "Qdc1 vs Tdiff Bar1", 200, 50., 150., 600, 0., 600.);
    canvasHit->cd(3);
    ahQdcvsTdiff1[0]->Draw("colz");

    ahQdcvsTdiff1[1] = new TH2D("hQdc1vsTdiff2", "Qdc1 vs Tdiff Bar2", 200, 0., 100., 600, 0., 600.);
    canvasHit->cd(4);
    ahQdcvsTdiff1[1]->Draw("colz");

    ahQdcvsTdiff2[0] = new TH2D("hQdc2vsTdiff1", "Qdc2 vs Tdiff Bar1", 200, 50., 150., 600, 0., 600.);
    canvasHit->cd(5);
    ahQdcvsTdiff2[0]->Draw("colz");

    ahQdcvsTdiff2[1] = new TH2D("hQdc2vsTdiff2", "Qdc2 vs Tdiff Bar2", 200, 0., 100., 600, 0., 600.);
    canvasHit->cd(6);
    ahQdcvsTdiff2[1]->Draw("colz");

    canvasHit->cd(0);
    run->AddObject(canvasHit);
       
    return kSUCCESS;
}

void R3BNeulandOnlineSpectraTest::Exec(Option_t*)
{
  if (hevents<1000000)
    {
      hevents++;
    }
  else
    {
      hevents = 0;
      hQdc11vsEvnt->Reset();
      hQdc12vsEvnt->Reset();
      hQdc21vsEvnt->Reset();
      hQdc22vsEvnt->Reset();
      ahTdiffvsEvnt[0]->Reset();
      ahTdiffvsEvnt[1]->Reset();
   }
  
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

    float t[fNBars][2]={-1};
    float q[fNBars][2]={-1};
    
    for (const auto& data : calData)
      {
        const auto side = data->GetSide() - 1; // [1,2] -> [0,1]
        const auto bar = data->GetBarId();
        ahCalTvsBar[side]->Fill(bar, data->GetTime());
        ahCalEvsBar[side]->Fill(bar, data->GetQdc());
	
	t[bar-1][side] = data->GetTime();
	q[bar-1][side] = data->GetQdc();
	
	if (bar==1&&side==0)
	  hQdc11vsEvnt->Fill(hevents, data->GetQdc());
	if (bar==1&&side==1)
	  hQdc12vsEvnt->Fill(hevents, data->GetQdc());
	if (bar==2&&side==0)
	  hQdc21vsEvnt->Fill(hevents, data->GetQdc());
	if (bar==2&&side==1)
	  hQdc22vsEvnt->Fill(hevents, data->GetQdc());
      }

    for (int br=0; br<2; br++)
      {
	if (t[br][0]>0&&t[br][1]>0) {
	  ahQdcvsTdiff[br]->Fill(t[br][0]-t[br][1], sqrt(q[br][0]*q[br][1]));
	  ahQdcvsTdiff1[br]->Fill(t[br][0]-t[br][1], q[br][0]);
	  ahQdcvsTdiff2[br]->Fill(t[br][0]-t[br][1], q[br][1]);
	  ahTdiffvsEvnt[br]->Fill(hevents,t[br][0]-t[br][1]);
	}
      }
}

void R3BNeulandOnlineSpectraTest::FinishTask()
{
    TDirectory* tmp = gDirectory;
    FairRootManager::Instance()->GetOutFile()->cd();

    gDirectory->mkdir("R3BNeulandOnlineSpectraTest");
    gDirectory->cd("R3BNeulandOnlineSpectraTest");

    ahMappedBar1[0]->Write();
    ahMappedBar1[1]->Write();
    ahMappedBar1[2]->Write();
    ahMappedBar1[3]->Write();
    ahMappedBar2[0]->Write();
    ahMappedBar2[1]->Write();
    ahMappedBar2[2]->Write();
    ahMappedBar2[3]->Write();
  
    hQdc11vsEvnt->Write();
    hQdc12vsEvnt->Write();
    hQdc21vsEvnt->Write();
    hQdc22vsEvnt->Write();

    ahTdiffvsEvnt[0]->Write();
    ahTdiffvsEvnt[1]->Write();

    ahCalTvsBar[0]->Write();
    ahCalTvsBar[1]->Write();
    ahCalEvsBar[0]->Write();
    ahCalEvsBar[1]->Write();

    ahQdcvsTdiff[0]->Write();
    ahQdcvsTdiff[1]->Write();
    ahQdcvsTdiff1[0]->Write();
    ahQdcvsTdiff1[1]->Write();
    ahQdcvsTdiff2[0]->Write();
    ahQdcvsTdiff2[1]->Write();

    gDirectory = tmp;
}

void R3BNeulandOnlineSpectraTest::ResetHistos()
{
    ahMappedBar1[0]->Reset();
    ahMappedBar1[1]->Reset();
    ahMappedBar1[2]->Reset();
    ahMappedBar1[3]->Reset();
    ahMappedBar2[0]->Reset();
    ahMappedBar2[1]->Reset();
    ahMappedBar2[2]->Reset();
    ahMappedBar2[3]->Reset();

    hQdc11vsEvnt->Reset();
    hQdc12vsEvnt->Reset();
    hQdc21vsEvnt->Reset();
    hQdc22vsEvnt->Reset();
    ahTdiffvsEvnt[0]->Reset();
    ahTdiffvsEvnt[1]->Reset();
    hevents = 0;
    
    ahCalTvsBar[0]->Reset();
    ahCalTvsBar[1]->Reset();
    ahCalEvsBar[0]->Reset();
    ahCalEvsBar[1]->Reset();

    ahQdcvsTdiff[0]->Reset();
    ahQdcvsTdiff[1]->Reset();
    ahQdcvsTdiff1[0]->Reset();
    ahQdcvsTdiff1[1]->Reset();
    ahQdcvsTdiff2[0]->Reset();
    ahQdcvsTdiff2[1]->Reset();

}

ClassImp(R3BNeulandOnlineSpectraTest)
