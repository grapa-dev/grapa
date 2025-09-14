// GrapaTime.cpp
/*
Copyright 2025 Chris Ernest Matichuk

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
////////////////////////////////////////////////////////////////////////////////

#include "GrapaTime.h"

#include "GrapaSystem.h"

extern GrapaSystem* gSystem;

////////////////////////////////////////////////////////////////////////////////

GrapaTime::GrapaTime() { mSec = 0; }
GrapaTime::GrapaTime(const GrapaCHAR& pChar) { FromBytes(pChar); }
GrapaCHAR GrapaTime::getBytes() { GrapaCHAR s; ToBytes(s); return(s); }
GrapaCHAR GrapaTime::getString() { GrapaCHAR s; ToString(s); return(s); }
void* GrapaTime::GetPtr() { return(&mSec); }
u64 GrapaTime::GetLength() { return(12); }

GrapaTime GrapaTime::Now()
{
	GrapaTime r;
	r.SetNow();
	return(r);
}

void GrapaTime::SetNow()
{
	gSystem->mTimeLock.WaitCritical();
#if defined(__MINGW32__) || defined(__GNUC__)
	timeval tv;
	gettimeofday(&tv, NULL);
	mSec = ((u64)tv.tv_sec * (u64)1000000000) + ((u64)tv.tv_usec * (u64)1000);
#else
#ifdef _WIN32
	bool pass = false;
	FILETIME ft;
	memset(&ft, 0, sizeof(FILETIME));
	GetSystemTimeAsFileTime(&ft);
	//GetSystemTimePreciseAsFileTime(&ft);
	u64 qwResult = (((ULONGLONG)ft.dwHighDateTime) << 32) + ft.dwLowDateTime;
	time_t t = ((u64)(qwResult / 10000000));
	if (tm* st = gmtime(&t))
	{
		st->tm_year -= 369;
		st->tm_isdst = 0;
		t = mktime(st);
		if (t >= 0)
		{
			u64 sec = t;
			if (st = gmtime(&t))
			{
				mSec = (sec + (sec - mktime(st))) * 1000000000 + (qwResult % 10000000) * 100;
				pass = true;
			}
		}
	}
	if (!pass)
	{
		time(&t);
		mSec = t * 1000000000;
	}
#endif
#endif
	gSystem->mTimeLock.LeaveCritical();
}

void GrapaTime::ToBytes(GrapaCHAR& pChar)
{
	pChar = mSec.getBytes();
	pChar.mToken = GrapaTokenType::TIME;
}

void GrapaTime::FromBytes(const GrapaBYTE& pValue)
{
	mSec.FromBytes(pValue);
}

void GrapaTime::FromString(const GrapaCHAR& pChar)
{
	if (pChar.mLength)
	{
		mSec = 0;
		tm st;
		memset(&st, 0, sizeof(tm));
		char mst[32];
		char tz[16];
		int len = 0;
		s64 timezoneOffset = 0;
		
		// Parse incrementally: start with basic date-time, then add optional components
		
		// Step 1: Parse basic ISO 8601 date-time: "2025-09-14T20:15:17"
		len = sscanf((char*)pChar.mBytes, "%d-%02d-%02dT%02d:%02d:%02d", &st.tm_year, &st.tm_mon, &st.tm_mday, &st.tm_hour, &st.tm_min, &st.tm_sec);
		if (len < 6)
		{
			// Try space-separated format: "2025-09-14 20:15:17"
			len = sscanf((char*)pChar.mBytes, "%d-%02d-%02d %02d:%02d:%02d", &st.tm_year, &st.tm_mon, &st.tm_mday, &st.tm_hour, &st.tm_min, &st.tm_sec);
		}
		if (len < 6)
		{
			// Try US date format: "09/14/2025 20:15:17"
			len = sscanf((char*)pChar.mBytes, "%02d/%02d/%d %02d:%02d:%02d", &st.tm_mon, &st.tm_mday, &st.tm_year, &st.tm_hour, &st.tm_min, &st.tm_sec);
		}
		if (len < 6)
		{
			// Try European date format: "14/09/2025 20:15:17"
			len = sscanf((char*)pChar.mBytes, "%02d/%02d/%d %02d:%02d:%02d", &st.tm_mday, &st.tm_mon, &st.tm_year, &st.tm_hour, &st.tm_min, &st.tm_sec);
		}
		if (len < 6)
		{
			// Try date only: "2025-09-14"
			len = sscanf((char*)pChar.mBytes, "%d-%02d-%02d", &st.tm_year, &st.tm_mon, &st.tm_mday);
			if (len >= 3)
			{
				st.tm_hour = 0; st.tm_min = 0; st.tm_sec = 0;
			}
		}
		if (len < 3)
		{
			// Try time only: "20:15:17"
			len = sscanf((char*)pChar.mBytes, "%02d:%02d:%02d", &st.tm_hour, &st.tm_min, &st.tm_sec);
			if (len >= 3)
			{
				// Use current date for time-only strings
				time_t now = time(NULL);
				struct tm* now_tm = gmtime(&now);
				st.tm_year = now_tm->tm_year;
				st.tm_mon = now_tm->tm_mon;
				st.tm_mday = now_tm->tm_mday;
			}
		}
		
		// Step 2: If we have basic date-time, try to add microseconds
		if (len >= 6)
		{
			// Look for microseconds after the seconds: ".8017135"
			char* dotPos = strchr((char*)pChar.mBytes, '.');
			if (dotPos && dotPos > (char*)pChar.mBytes)
			{
				// Check if this dot is after seconds (not in date part)
				char* tPos = strchr((char*)pChar.mBytes, 'T');
				char* spacePos = strchr((char*)pChar.mBytes, ' ');
				if ((tPos && dotPos > tPos) || (spacePos && dotPos > spacePos))
				{
					// This dot is after time, try to parse microseconds
					int microLen = sscanf(dotPos, ".%s", mst);
					if (microLen == 1)
					{
						// Successfully parsed microseconds
						// mst now contains the microseconds string
					}
				}
			}
		}
		
		// Step 3: If we have basic date-time, try to add timezone
		if (len >= 6)
		{
			// Look for timezone indicators: +05:00, -08:00, Z
			char* tzPos = NULL;
			char* plusPos = strrchr((char*)pChar.mBytes, '+');
			char* minusPos = strrchr((char*)pChar.mBytes, '-');
			char* zPos = strrchr((char*)pChar.mBytes, 'Z');
			
			// Find the last timezone indicator (most likely to be the actual timezone)
			if (zPos && (!tzPos || zPos > tzPos)) tzPos = zPos;
			if (plusPos && (!tzPos || plusPos > tzPos)) tzPos = plusPos;
			if (minusPos && (!tzPos || minusPos > tzPos)) tzPos = minusPos;
			
			if (tzPos)
			{
				// Parse timezone
				int tzHours = 0, tzMinutes = 0;
				char tzSign;
				if (*tzPos == 'Z')
				{
					timezoneOffset = 0;
				}
				else if (*tzPos == '+' || *tzPos == '-')
				{
					// Try colon format first: +05:00 or -08:00
					int tzLen = sscanf(tzPos, "%c%02d:%02d", &tzSign, &tzHours, &tzMinutes);
					if (tzLen >= 3)
					{
						timezoneOffset = (tzSign == '+') ? 1 : -1;
						timezoneOffset *= (tzHours * 3600 + tzMinutes * 60);
					}
					else
					{
						// Try 4-digit format: +0500 or -0800
						tzLen = sscanf(tzPos, "%c%04d", &tzSign, &tzHours);
						if (tzLen >= 2)
						{
							timezoneOffset = (tzSign == '+') ? 1 : -1;
							tzMinutes = tzHours % 100;
							tzHours = tzHours / 100;
							timezoneOffset *= (tzHours * 3600 + tzMinutes * 60);
						}
					}
				}
			}
		}
		
		// Initialize microseconds to empty if not parsed
		if (len < 6)
		{
			mst[0] = 0; // No microseconds
		}
		
		if (len > 0)
		{
			// "1969-04-23T12:33:33.921638".time() + 24*60*60*1000000000;
			s64 year = 1970;
			s64 sec = 0;
			if (st.tm_year >= year)
			{
				while (st.tm_year > year)
				{
					sec += isLeap(year) ? 31622400 : 31536000;
					year++;
				}
			}
			else
			{
				while (st.tm_year < year)
				{
					year--;
					sec -= isLeap(year) ? 31622400 : 31536000;
				}
			}
			if (st.tm_mday)
				st.tm_mday--;
			s64 offset1 = offsetDays(st.tm_mday, st.tm_mon, st.tm_year);
			sec += offset1 * 86400 + (((st.tm_hour * 60) + st.tm_min) * 60) + st.tm_sec;
			
			// Apply timezone offset (convert to UTC)
			sec -= timezoneOffset;
			
			u32 ms = 0;
			u64 p = 1;
			GrapaCHAR x(mst);
			x.RTrim('0');
			if (x.mLength)
			{
				if (x.mLength > 9) x.SetLength(9);
				sscanf((char*)x.mBytes, "%d", &ms);
				p = (u64)pow(10, (9 - x.mLength));
			}
			//if (sec < 0 && ms)
			//	sec--;
			mSec = sec; 
			mSec = mSec * GrapaInt(1000000000);
			//if (sec>0)
				mSec = mSec + GrapaInt(ms * p);
			//else
				//mSec = mSec + GrapaInt(ms * p);

			//"2020-07-09T03:54:43.123456789123".time()
			// (-1).time();
			// (-2).time();

		}
	}
}

s64 GrapaTime::LocalOffset()
{
	gSystem->mTimeLock.WaitCritical();
	time_t t = 0;
	time(&t);
	s64 returnTime = 0;
	if (tm* st = gmtime(&t))
	{
		st->tm_isdst = -1;
		time_t t2 = mktime(st);
		if (t2 >= 0)
			returnTime = ((s64)1000000000 * (s64)((t - t2)));
	}
	gSystem->mTimeLock.LeaveCritical();
	return(returnTime);
}

bool GrapaTime::isLeap(s64 y)
{
	if ((y % 100 != 0 && y % 4 == 0) || y % 400 == 0)
		return true;
	return false;
}

// Given a date, returns number of days elapsed 
// from the  beginning of the current year (1st 
// jan). 
s64 GrapaTime::offsetDays(s64 d, s64 m, s64 y)
{
	s64 offset = d;
	switch (m - 1)
	{
	case 11:
		offset += 30;
	case 10:
		offset += 31;
	case 9:
		offset += 30;
	case 8:
		offset += 31;
	case 7:
		offset += 31;
	case 6:
		offset += 30;
	case 5:
		offset += 31;
	case 4:
		offset += 30;
	case 3:
		offset += 31;
	case 2:
		offset += 28;
	case 1:
		offset += 31;
	}
	if (isLeap(y) && m > 2)
		offset += 1;
	return offset;
}

// Given a year and days elapsed in it, finds 
// date by storing results in d and m. 
void GrapaTime::revoffsetDays(s64 offset, s64 y, s64* d, s64* m)
{
	s64 month[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	if (isLeap(y))
		month[2] = 29;
	s64 i;
	for (i = 1; i <= 12; i++)
	{
		if (offset <= month[i])
			break;
		offset = offset - month[i];
	}
	*d = offset;
	*m = i;
}

// Add x days to the given date. 
void GrapaTime::addDays(s64 d1, s64 m1, s64 y1, s64 x, s64& d2, s64& m2, s64& y2, s64& x2)
{
	s64 offset1 = offsetDays(d1, m1, y1);
	s64 remDays = isLeap(y1) ? (366 - offset1) : (365 - offset1);
	// y2 is going to store result year and 
	// offset2 is going to store offset days 
	// in result year. 
	if (x <= remDays)
	{
		y2 = y1;
		x2 = offset1 + x;
	}
	else
	{
		// x may store thousands of days. 
		// We find correct year and offset 
		// in the year. 
		x -= remDays;
		y2 = y1 + 1;
		s64 y2days = isLeap(y2) ? 366 : 365;
		while (x >= y2days)
		{
			x -= y2days;
			y2++;
			y2days = isLeap(y2) ? 366 : 365;
		}
		x2 = x;
	}
	// Find values of day and month from 
	// offset of result year. 
	revoffsetDays(x2, y2, &d2, &m2);
}

//(-100000000)/1000000000
// 100000000/1000000000
// (-1)/10

void GrapaTime::ToString(GrapaCHAR& pChar)
{
	s64 d2, m2, y2, x2;
	s64 year = 1970;
	GrapaInt n1000000000(1000000000);
	s64 sdif = (mSec / n1000000000).LongValue();
	s64 mic = (mSec % n1000000000).LongValue();
	if ((sdif < 0 || mic < 0) && mic)
	{
		if (mic < 0)
			mic = -mic;
		mic = (1000000000-mic);
		sdif -= 1;
	}
	if (sdif < 0)
	{
		while (sdif < 0)
		{
			year--;
			sdif += isLeap(year) ? 31622400 : 31536000;
		}
	}
	else
	{
		while ((sdif - (isLeap(year) ? 31622400 : 31536000)) > 0)
		{
			sdif -= isLeap(year) ? 31622400 : 31536000;
			year++;
		}
	}
	addDays(1, 1, year, sdif / 86400, d2, m2, y2, x2);
	s64 sec = sdif % 86400;
	s64 hr = 0;
	s64 min = 0;
	if (sec)
	{
		min = sec / 60;
		sec = sec % 60;
		hr = min / 60;
		min = min % 60;
	}
	pChar.SetLength(128);
	        int len = snprintf((char*)pChar.mBytes, pChar.mSize, "%d-%02d-%02dT%02d:%02d:%02d.%09d", (int)y2, (int)m2, (int)d2, (int)hr, (int)min, (int)sec, (int)mic);
	if (len > 0) pChar.SetLength(len);
	else pChar.SetLength(0);
	pChar.RTrim('0');
	if (pChar.mBytes[pChar.mLength - 1] == '.')
		pChar.Append('0');
}


// -62167219200
// 1000000000


// "1969-04-23T12:33:33.921638".time() + 24*60*60*1000000000;

////////////////////////////////////////////////////////////////////////////////
