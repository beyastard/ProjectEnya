
#ifndef __JEXCEPTION_H__
#define __JEXCEPTION_H__

#define J_EXCEPTION_MAX_STACK_SIZE	4096

typedef struct __exception_stack_node_t
{
	const char *	filename;
	int		linenumber;
	DWORD		iaddr;
} __exception_stack_node;

typedef struct __exception_stack_t
{
	__exception_stack_node	stack[J_EXCEPTION_MAX_STACK_SIZE];
	int			sp;
}__exception_stack;

class JException
{
public:
	JException();
	void Push(int index, const char * filename, int linenumber, DWORD eip);
	~JException();
private:
	int m_index;
};

int init_exception_stack(int number);
int finalize_exception_stack();
__exception_stack * get_exception_stack(int index);

#ifndef _DEBUG
#define J_EXCEPTION_ENABLED
#define J_EXCEPTION_LINE_ENABLED
#endif

#ifdef J_EXCEPTION_ENABLED
	#define J_EXCEPTION_RECORD(thread_id) \
		DWORD exception_tmp; \
		JException MyException; \
		extern int glb_exception_flag;	\
		if(glb_exception_flag & 0x01) goto TMP_EXCEPTION_LABEL;	\
		__asm { lea eax, TMP_EXCEPTION_LABEL } \
		__asm { nop }	\
		__asm { mov exception_tmp, eax } \
		MyException.Push(thread_id, __FILE__, __LINE__, exception_tmp);\
TMP_EXCEPTION_LABEL: 
#else
	#define J_EXCEPTION_RECORD(thread_id)	NULL;
#endif

#ifdef J_EXCEPTION_LINE_ENABLED
	#define J_EXCEPTION_LINE()	{g_jexception_line = __LINE__;}
#else
	#define J_EXCEPTION_LINE()				NULL;
#endif

int jexception_handle(const char * logfile, int idStack, DWORD error_code, LPEXCEPTION_POINTERS lpex);
extern int g_jexception_line;
#endif