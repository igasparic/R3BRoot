#include "R3BFi7Cal2Hit.h"

R3BFi7Cal2Hit::R3BFi7Cal2Hit(Int_t a_verbose)
  : R3BBunchedFiberCal2Hit("Fi7", a_verbose, VERTICAL, 2, 256, 1)
{
}

R3BFi7Cal2Hit::~R3BFi7Cal2Hit()
{
}

UInt_t R3BFi7Cal2Hit::FixMistake(UInt_t a_fiber_id)
{
//  if (201 == a_fiber_id) {
//    a_fiber_id = 200;
//  }
//  if (200 == a_fiber_id) {
//    a_fiber_id = 201;
//  }
  return a_fiber_id;
}

ClassImp(R3BFi7Cal2Hit)
