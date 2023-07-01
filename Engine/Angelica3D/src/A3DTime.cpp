#include "A3DTime.h"
#include "A3DCounter.h"

#pragma comment(lib, "winmm.lib")

void A3DGetSystemTime(WORD * pYear, WORD * pMonth, WORD * pDay, WORD * pDate, WORD * pHour, WORD * pMinute, WORD * pSecond)
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	if( pYear ) *pYear = st.wYear;
	if( pMonth ) *pMonth = st.wMonth;
	if( pDay ) *pDay = st.wDay;
	if( pDate ) *pDate = st.wDayOfWeek;
	if( pHour ) *pHour = st.wHour;
	if( pMinute ) *pMinute = st.wMinute;
	if( pSecond ) *pSecond = st.wSecond;
}

DWORD A3DGetTime()
{
	return (DWORD)(A3DCounter::GetMicroSecondNow() / 1000);
}