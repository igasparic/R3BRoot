#include "R3BNeulandTacquilaMappedData.h"

R3BNeulandTacquilaMappedData::R3BNeulandTacquilaMappedData()
  : fClock(0)
  , fTacData(0)
  , fStopT(0)
  , fQdcData(0)
  , fPlane(0)
  , fPaddle(0)
  , fSide(0)
{
}

R3BNeulandTacquilaMappedData::R3BNeulandTacquilaMappedData(Int_t clock,
							   Int_t tacData,
							   Int_t stopT,
							   Int_t qdcData,
							   Int_t plane,
							   Int_t paddle,
							   Int_t side)
  : fClock(clock)
  , fTacData(tacData)
  , fStopT(stopT)
  , fQdcData(qdcData)
  , fPlane(plane)
  , fPaddle(paddle)
  , fSide(side)
{
}

ClassImp(R3BNeulandTacquilaMappedData)
