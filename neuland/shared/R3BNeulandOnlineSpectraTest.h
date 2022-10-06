#ifndef R3BROOT_R3BNEULANDONLINESPECTRATEST_H
#define R3BROOT_R3BNEULANDONLINESPECTRATEST_H

#include "FairTask.h"
#include "R3BPaddleTamexMappedData.h"
#include "R3BLosCalData.h"
#include "R3BNeulandCalData.h"
//#include "R3BNeulandHit.h"
#include "R3BLandDigi.h"
#include "TCAConnector.h"

class TCanvas;
class TH1D;
class TH2D;

class R3BNeulandOnlineSpectraTest : public FairTask
{

  public:
    R3BNeulandOnlineSpectraTest();

    InitStatus Init() override;
    void Exec(Option_t*) override;
    void FinishTask() override;

    void ResetHistos();

  private:
    static const unsigned int fNPlanes = 1;
    static const unsigned int fNBars = fNPlanes * 4;

    int hevents = 0;
    
    TCAInputConnector<R3BPaddleTamexMappedData> fNeulandMappedData;
    TCAInputConnector<R3BNeulandCalData> fNeulandCalData;
    TCAInputConnector<R3BLandDigi> fNeulandHits;
 
    std::array<TH1D*, 4> ahMappedBar1;
    std::array<TH1D*, 4> ahMappedBar2;

    std::array<TH2D*, 2> ahCalTvsBar;
    std::array<TH2D*, 2> ahCalEvsBar;

    TH2D* hQdc11vsEvnt;
    TH2D* hQdc12vsEvnt;
    TH2D* hQdc21vsEvnt;
    TH2D* hQdc22vsEvnt;

    std::array<TH2D*, 2> ahTdiffvsEvnt;
    
    std::array<TH2D*, 2> ahQdcvsTdiff;
    std::array<TH2D*, 2> ahQdcvsTdiff1;
    std::array<TH2D*, 2> ahQdcvsTdiff2;

    ClassDefOverride(R3BNeulandOnlineSpectraTest, 0)
};

#endif // R3BROOT_R3BNEULANDONLINESPECTRATEST_H
