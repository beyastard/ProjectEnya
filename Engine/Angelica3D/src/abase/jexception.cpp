#include <stdio.h>
#include "windows.h"
#include "jexception.h"

static __exception_stack *ex_stack = NULL;
int	glb_exception_flag	= 0;
int g_jexception_line	= 0;;

int init_exception_stack(int number)
{
	if(number <=0) return -1;
	ex_stack = (__exception_stack *) malloc(sizeof(__exception_stack) * number);
	if(!ex_stack) return -2;
	memset(ex_stack,0,sizeof(__exception_stack) * number);
	return 0;
}

int finalize_exception_stack()
{
	free(ex_stack);
	return 0;
}

static void ex_push(__exception_stack * __es,const char * filename, int linenumber, DWORD eip)
{

	if(__es->sp >= J_EXCEPTION_MAX_STACK_SIZE){
		// raise a Exception
		RaiseException(0xAABBCCDD,0,NULL,NULL);
	}
	else
	{
		int tmp = __es->sp;
		__es->stack[tmp].filename	= filename;
		__es->stack[tmp].iaddr		= eip;
		__es->stack[tmp].linenumber	= linenumber;
		__es->sp ++;
	}
}

static void ex_pop(__exception_stack * __es)
{
	if(__es->sp <= 0){
		// raise a Exception
		RaiseException(0xAABBCCDE,0,NULL,NULL);
	}
	else
	{
		__es->sp --;
	}
}

__exception_stack * get_exception_stack(int index)
{
	return ex_stack + index;
}



JException::JException()
{
	m_index = -1;
}

void JException::Push(int index,const char * filename, int linenumber, DWORD eip)
{
	m_index = index;
	ex_push(ex_stack + index,filename,linenumber,eip);
}

JException::~JException()
{
	if(m_index >=0) ex_pop(ex_stack + m_index);
}

int jexception_handle(const char * logfile, int idStack, DWORD error_code, LPEXCEPTION_POINTERS lpex)
{
	int i;
	SYSTEMTIME st;
	FILE *file = NULL;
	char szTitle[MAX_PATH], szMsg[1024];
	__exception_stack * es = get_exception_stack(idStack);
	
	file = fopen(logfile, "a");
	if( file )
	{
		GetLocalTime(&st);
		fprintf(file, "%04d.%02d.%02d %02d:%02d:%02d.%03d\n\n", st.wYear, st.wMonth,
			st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

		if(es->sp > 0 && es->sp < 1024)
		{
			for( i = 0; i < es->sp; i++ )
			{
				strcpy(szTitle, es->stack[i].filename);
				fprintf(file, "\tFile: %s Line: %d EIP: 0x%08X\n", szTitle,
					es->stack[i].linenumber, es->stack[i].iaddr);
			}
			fprintf(file, "\n");
		}
		fprintf(file, "\tRealLine:%d\n", g_jexception_line);

		fprintf(file, "\tExceptionRecord->ExceptionCode = 0x%08X\n", lpex->ExceptionRecord->ExceptionCode);
		fprintf(file, "\tExceptionRecord->ExceptionFlags = 0x%08X\n", lpex->ExceptionRecord->ExceptionFlags);
		fprintf(file, "\tExceptionRecord->ExceptionAddress = 0x%08X\n", lpex->ExceptionRecord->ExceptionAddress);
		fprintf(file, "\tExceptionRecord->NumberParameters = %d\n\n", lpex->ExceptionRecord->NumberParameters);

		fprintf(file, "\tContextRecord->ContextFlags = 0x%08X\n", lpex->ContextRecord->ContextFlags);
		fprintf(file, "\tContextRecord->Dr0 = 0x%08X\n", lpex->ContextRecord->Dr0);
		fprintf(file, "\tContextRecord->Dr1 = 0x%08X\n", lpex->ContextRecord->Dr1);
		fprintf(file, "\tContextRecord->Dr2 = 0x%08X\n", lpex->ContextRecord->Dr2);
		fprintf(file, "\tContextRecord->Dr3 = 0x%08X\n", lpex->ContextRecord->Dr3);
		fprintf(file, "\tContextRecord->Dr6 = 0x%08X\n", lpex->ContextRecord->Dr6);
		fprintf(file, "\tContextRecord->Dr7 = 0x%08X\n", lpex->ContextRecord->Dr7);
		fprintf(file, "\tContextRecord->SegGs = 0x%08X\n", lpex->ContextRecord->SegGs);
		fprintf(file, "\tContextRecord->SegFs = 0x%08X\n", lpex->ContextRecord->SegFs);
		fprintf(file, "\tContextRecord->SegEs = 0x%08X\n", lpex->ContextRecord->SegEs);
		fprintf(file, "\tContextRecord->SegDs = 0x%08X\n", lpex->ContextRecord->SegDs);
		fprintf(file, "\tContextRecord->Edi = 0x%08X\n", lpex->ContextRecord->Edi);
		fprintf(file, "\tContextRecord->Esi = 0x%08X\n", lpex->ContextRecord->Esi);
		fprintf(file, "\tContextRecord->Ebx = 0x%08X\n", lpex->ContextRecord->Ebx);
		fprintf(file, "\tContextRecord->Edx = 0x%08X\n", lpex->ContextRecord->Edx);
		fprintf(file, "\tContextRecord->Ecx = 0x%08X\n", lpex->ContextRecord->Ecx);
		fprintf(file, "\tContextRecord->Eax = 0x%08X\n", lpex->ContextRecord->Eax);
		fprintf(file, "\tContextRecord->Ebp = 0x%08X\n", lpex->ContextRecord->Ebp);
		fprintf(file, "\tContextRecord->Eip = 0x%08X\n", lpex->ContextRecord->Eip);
		fprintf(file, "\tContextRecord->SegCs = 0x%08X\n", lpex->ContextRecord->SegCs);
		fprintf(file, "\tContextRecord->EFlags = 0x%08X\n", lpex->ContextRecord->EFlags);
		fprintf(file, "\tContextRecord->Esp = 0x%08X\n", lpex->ContextRecord->Esp);
		fprintf(file, "\tContextRecord->SegSs = 0x%08X\n\n", lpex->ContextRecord->SegSs);

		fflush(file);

		fprintf(file, "Data Near EIP:\n");
		unsigned char * eip ;
		for(i = 0; i < 64;i++)
		{
			int j ;
			eip = (unsigned char *)(lpex->ContextRecord->Eip+ i * 32 - 32 * 32);
			fprintf(file,"%08X  ",eip);
			for(j = 0;j<32;j++)
			{
				fprintf(file,"%02X ",*eip);
				eip ++;
			}
			fprintf(file,"\n");
		}
		fprintf(file,"\n\n");
		fflush(file);

		fprintf(file,"Stack:\n");
		unsigned char * sp ;
		for(i = 0; i < 256;i++)
		{
			int j ;
			sp = (unsigned char *)(lpex->ContextRecord->Esp + i * 16);
			fprintf(file,"%08X  ",sp);
			for(j = 0;j<16;j++)
			{
				fprintf(file,"%02X ",*sp);
				sp ++;
			}
			fprintf(file," ");

			sp = (unsigned char *)(lpex->ContextRecord->Esp + i * 16);
			for(j = 0;j<16;j++)
			{
				fprintf(file,"%c",(*sp<32 || *sp>127)?'.':*sp);
				sp ++;
			}
			fprintf(file,"\n");
		}

		fprintf(file, "------------------------------------------------------------\n\n");
		fclose(file);
	}
	/*
	AGetFileTitle(es->stack[es->sp - 1].filename, szTitle, MAX_PATH);
	*/

	// 如果是纯服务器出错就直接退出
	//if( !g_vw_data.Network.IsPureServer )
	{
		sprintf(szMsg, "Fatal error occurs at\n\nFile: %s\nLine: %d\nAddr: 0x%08X ReaLine:%d\n\nPlease contact the software vendor.",
			es->stack[es->sp - 1].filename, es->stack[es->sp - 1].linenumber, es->stack[es->sp - 1].iaddr, g_jexception_line);
		MessageBox(NULL, szMsg, "Exception", MB_OK | MB_SYSTEMMODAL);
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

