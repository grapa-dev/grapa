// GrapaCompress.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaCompress_
#define _GrapaCompress_

#include "GrapaValue.h"

class GrapaCompress
{
public:
	static bool Compress(GrapaBYTE& pData);
	static bool Expand(GrapaBYTE& pData);
	static bool Compress(GrapaBYTE& pData, GrapaBYTE& pEnc);
	static bool Expand(GrapaBYTE& pEnc, GrapaBYTE& pData);
};


#endif // _GrapaCompress_

////////////////////////////////////////////////////////////////////////////////
