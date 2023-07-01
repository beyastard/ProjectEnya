#include "A3DErrLog.h"
#include "A3DTime.h"

A3DErrLog::A3DErrLog()
{
	m_pFile = NULL;
}

A3DErrLog::~A3DErrLog()
{
}

bool A3DErrLog::Init(char * szLogFile, bool bAppend)
{
	WORD nYear, nMonth, nDay, nDate, nHour, nMinute, nSecond;
	char szLogPath[MAX_PATH];

	sprintf(szLogPath, "%s\\%s", ALog::GetLogDir(), szLogFile);

	if( bAppend )
		m_pFile = fopen(szLogPath, "at");
	else
		m_pFile = fopen(szLogPath, "wt");
	if( NULL == m_pFile ) 
		return true; 

	A3DGetSystemTime(&nYear, &nMonth, &nDay, &nDate, &nHour, &nMinute, &nSecond);
	fprintf(m_pFile, "Angelica 3D Error Log\nCreated(or opened) on %02d/%02d/%04d %02d:%02d:%02d\n\n", nDay, nMonth, nYear,
		nHour, nMinute, nSecond);
	return true;
}

bool A3DErrLog::Release()
{
	if( m_pFile )
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}

	return true;
}

bool A3DErrLog::ErrLog(const char * fmt, ...)
{
	char szErrorMsg[2048];
	va_list args_list;


    va_start(args_list, fmt);
    vsprintf(szErrorMsg, fmt, args_list);
	va_end(args_list);

	return ErrString(szErrorMsg);
}

bool A3DErrLog::ErrString(char * szString)
{
	if( !m_pFile ) return true;

	WORD nYear, nMonth, nDay, nDate, nHour, nMinute, nSecond;
    A3DGetSystemTime(&nYear, &nMonth, &nDay, &nDate, &nHour, &nMinute, &nSecond);

    fprintf(m_pFile, "[%02d:%02d:%02d] %s\n", nHour, nMinute, nSecond, szString);
	fflush(m_pFile);
	return true;
}

A3DErrLog * g_pA3DErrLog;