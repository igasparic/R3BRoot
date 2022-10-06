#include "FairLogger.h"
#include "FairRootManager.h"
#include "R3BEventHeader.h"
#include "R3BNeulandWRReader.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_nn_wr.h"
}

R3BNeulandWRReader::R3BNeulandWRReader(EXT_STR_h101_nn_wr *data,
    UInt_t offset, UInt_t nn_wr_id)
	: R3BReader("R3BNeulandWRReader")
	, fNEvent(0)
	, fData(data)
	, fOffset(offset)
	, fLogger(FairLogger::GetLogger())
	, fNeulandWRId(nn_wr_id)
	, fEventHeader(nullptr)
{
}

R3BNeulandWRReader::~R3BNeulandWRReader()
{
}

Bool_t R3BNeulandWRReader::Init(ext_data_struct_info *a_struct_info)
{
	int ok;

	EXT_STR_h101_nn_wr_ITEMS_INFO(ok, *a_struct_info, fOffset,
	    EXT_STR_h101_nn_wr, 0);

	if (!ok) {
		perror("ext_data_struct_info_item");
		fLogger->Error(MESSAGE_ORIGIN,
		    "Failed to setup structure information.");
		return kFALSE;
	}

	FairRootManager* mgr = FairRootManager::Instance();
	fEventHeader = (R3BEventHeader *)mgr->GetObject("R3BEventHeader");

	return kTRUE;
}

Bool_t R3BNeulandWRReader::Read()
{
	fLogger->Info(MESSAGE_ORIGIN, "NeulandWRReader::Read BEGIN");
std::cout
	<< fData->NN_WR_ID << ' '
	<< fData->NN_WR_T1 << ' '
	<< fData->NN_WR_T2 << ' '
	<< fData->NN_WR_T3 << ' '
	<< fData->NN_WR_T4 << '\n';
	if (fData->NN_WR_ID != 0
	    && fNeulandWRId != fData->NN_WR_ID) {
		fLogger->Error(MESSAGE_ORIGIN,
		    "Event %u: NeulandWR ID mismatch: expect %u, got %u\n",
		    fEventHeader->GetEventno(),
		    fNeulandWRId, fData->NN_WR_ID);
	}

	if (fEventHeader != nullptr) {
		uint64_t timestamp =
		    ((uint64_t) fData->NN_WR_T4 << 48)
		    | ((uint64_t) fData->NN_WR_T3 << 32)
		    | ((uint64_t) fData->NN_WR_T2 << 16)
		    | (uint64_t) fData->NN_WR_T1;

		fEventHeader->SetTimeStamp(timestamp);
		fNEvent = fEventHeader->GetEventno();
	} else {
		fNEvent++;
	}

	if (0 == (fNEvent % 1000)) {
		LOG(DEBUG1) << "R3BNeulandWRReader : event : " << fNEvent
		    << FairLogger::endl;
	}

	/* Display data */
	fLogger->Info(MESSAGE_ORIGIN, "  NeulandWR timestamp (id=%u):",
	    fData->NN_WR_ID);
	fLogger->Info(MESSAGE_ORIGIN, "  e%04x h%04x m%04x l%04x",
		fData->NN_WR_T4, fData->NN_WR_T3,
		fData->NN_WR_T2, fData->NN_WR_T1);

	fLogger->Info(MESSAGE_ORIGIN, "NeulandWRReader::Read END");
    return kTRUE;
}

void R3BNeulandWRReader::Reset()
{
	fNEvent = 0;
}

ClassImp(R3BNeulandWRReader)

