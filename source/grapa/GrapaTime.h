// GrapaTime.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GrapaTime_
#define _GrapaTime_

#include "GrapaInt.h"

class GrapaTime : public GrapaValue
{
public:
	GrapaInt mSec;
public:
	GrapaTime();
	GrapaTime(const GrapaCHAR& pChar);
public:
	static GrapaTime Now();
	virtual void SetNow();
	virtual void ToBytes(GrapaCHAR& pChar);
	virtual void FromBytes(const GrapaBYTE& pChar);
	virtual void ToString(GrapaCHAR& pChar);
	virtual void FromString(const GrapaCHAR& pChar);
	virtual s64 LocalOffset();
	static bool isLeap(s64 y);
	static s64 offsetDays(s64 d, s64 m, s64 y);
	static void revoffsetDays(s64 offset, s64 y, s64* d, s64* m);
	static void addDays(s64 d1, s64 m1, s64 y1, s64 x, s64& d2, s64& m2, s64& y2, s64& x2);
	GrapaCHAR getBytes();
	GrapaCHAR getString();
public:
	virtual void* GetPtr();
	virtual u64 GetLength();
};

#endif //_GrapaTime_

////////////////////////////////////////////////////////////////////////////////
