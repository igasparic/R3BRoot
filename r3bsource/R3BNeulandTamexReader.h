#ifndef R3BNEULANDTAMEXREADER_H
#define R3BNEULANDTAMEXREADER_H

#include "R3BReader.h"

class TClonesArray;
class FairLogger;

struct EXT_STR_h101_raw_nnp_t;
typedef struct EXT_STR_h101_raw_nnp_t EXT_STR_h101_raw_nnp;

class R3BNeulandTamexReader : public R3BReader
{
	public:
		R3BNeulandTamexReader(EXT_STR_h101_raw_nnp *, UInt_t);
		~R3BNeulandTamexReader();

		Bool_t Init(ext_data_struct_info *);
		Bool_t Read();
		void Reset();
		void SetNofPlanes(UInt_t nplanes) {fNofPlanes = nplanes;}

	private:
		/* An event counter */
		unsigned int fNEvent;
		/* Reader specific data structure from ucesb */
		EXT_STR_h101_raw_nnp* fData;
		/* Data offset */
		UInt_t fOffset;
		/* FairLogger */
		FairLogger*	fLogger;
	        TClonesArray* fArray; /**< Output array. */
		UInt_t fNofPlanes;
		
	public:
		ClassDef(R3BNeulandTamexReader, 1);
};

#endif

