#ifndef R3BNEULANDWRREADER_H
#define R3BNEULANDWRREADER_H
#include "R3BReader.h"

struct EXT_STR_h101_nn_wr_t;
typedef struct EXT_STR_h101_nn_wr_t EXT_STR_h101_nn_wr;

class FairLogger;
class R3BEventHeader;

class R3BNeulandWRReader : public R3BReader
{
	public:
		R3BNeulandWRReader(EXT_STR_h101_nn_wr *, UInt_t,
		    UInt_t);
		~R3BNeulandWRReader();

		Bool_t Init(ext_data_struct_info *);
		Bool_t Read();
		void Reset();

	private:
		/* An event counter */
		UInt_t fNEvent;
		/* Reader specific data structure from ucesb */
		EXT_STR_h101_nn_wr* fData;
		/* Offset of detector specific data in full data structure */
		UInt_t fOffset;
		/* FairLogger */
		FairLogger* fLogger;
		/* The whiterabbit subsystem ID */
		UInt_t fNeulandWRId;
		/* A pointer to the R3BEventHeader structure */
		R3BEventHeader* fEventHeader;

	public:
		ClassDef(R3BNeulandWRReader, 0);
};
#endif
