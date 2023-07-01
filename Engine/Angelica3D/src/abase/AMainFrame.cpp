#include "AMainFrame.h"
#include "Adebug.h"
#include "assert.h"

AMainFrame::AMainFrame()
{
	m_x			= 0;
	m_y			= 0;
	m_hWnd		= NULL;
	m_nHeight	= 0;
	m_nWidth	= 0;
}

AMainFrame::~AMainFrame()
{
}

bool AMainFrame::PreCreateWindow(LPCREATEFRMSTRUCT pWc)
{
	return true;
}

bool AMainFrame::OnCreate(LPCREATEFRMSTRUCT pWc)
{
	return true;
}

bool AMainFrame::Create(AUString product, 
						GUID productid,
						int x,
						int y,
						int cx,
						int cy,
						HINSTANCE hInstance, 
						LPSTR lpCmdLine, 
                        int nCmdShow, 
						WNDPROC WndProc)
{
	strncpy(m_product.name, product, sizeof(m_product.name));
	m_product.guid	= productid;
	m_x				= x;
	m_y				= y;
	m_nWidth		= cx;
	m_nHeight		= cy;
	m_cmdLine		= lpCmdLine;
	
	return InitApplication(hInstance,lpCmdLine,nCmdShow,WndProc);
}


bool AMainFrame::InitApplication(HINSTANCE hInstance, LPSTR lpCmdLine, 
                           int nCmdShow, WNDPROC WndProc)
{
	WNDCLASS wc;
	memset(&wc,0,sizeof(wc));

	wc.style = CS_DBLCLKS;
	wc.cbWndExtra = sizeof(AMainFrame::PRODUCT);
	wc.lpfnWndProc	= WndProc;
	wc.hInstance	= hInstance;
	wc.hCursor		= LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW));
	wc.hIcon		= NULL;//Customize
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName =(LPCTSTR)m_product.name;

	if(!RegisterClass(&wc))
	{
		ADebug::Msg(0,"InitApplication error");
		return false;
	}

	return InitInstance(hInstance,lpCmdLine,nCmdShow,WndProc);
}

bool AMainFrame::InitInstance(HINSTANCE hInstance, LPSTR lpCmdLine, 
                        int nCmdShow, WNDPROC WndProc)
{
	CREATEFRMSTRUCT	createStruct;
	memset(&createStruct,0,sizeof(CREATEFRMSTRUCT));

	createStruct.dwExStyle		= 0;
	createStruct.lpszClass		= (LPCSTR)m_product.name;
	createStruct.lpszName		= (LPCSTR)m_product.name;
	createStruct.style			= WS_POPUP | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	createStruct.x				= m_x;
	createStruct.y				= m_y;
	createStruct.cx				= m_nWidth;
	createStruct.cy				= m_nHeight;
	createStruct.hwndParent		= NULL;
	createStruct.hMenu			= NULL;//Customize
	createStruct.hInstance		= hInstance;
	createStruct.lpCreateParams = NULL;

	if(!PreCreateWindow(&createStruct))
	{
		ADebug::Msg(0,"PreCreateWindow error");
		return false;
	}

	m_x = createStruct.x;
	m_y = createStruct.y;
	m_nWidth = createStruct.cx;
	m_nHeight = createStruct.cy;

	m_hWnd = CreateWindowEx(
		createStruct.dwExStyle,
		createStruct.lpszClass,
		createStruct.lpszName,
		createStruct.style,
		createStruct.x,
		createStruct.y,
		createStruct.cx,
		createStruct.cy,
		createStruct.hwndParent,
		createStruct.hMenu,
		createStruct.hInstance,
		createStruct.lpCreateParams);

	if( NULL == m_hWnd )
	{
		return false;
	}

	// 埋设产品标识
	for(int lc = 0;lc < sizeof(m_product.data) / sizeof(long);lc++)
	{
		SetWindowLong(m_hWnd, lc * 4, m_product.data[lc]);
	}

	if(!OnCreate(&createStruct))
	{
		return false;
	}

	return true;
}

void AMainFrame::ShowWindow(int __nStatus)
{
	assert(m_hWnd);
	::ShowWindow(m_hWnd,__nStatus);
	::UpdateWindow(m_hWnd);
}

bool AMainFrame::MoveWindow(int __x, int __y, int __cx, int __cy)
{
	assert(::IsWindow(m_hWnd));
	::MoveWindow(m_hWnd, __x, __y, __cx, __cy, TRUE);
	m_x				= __x;
	m_y				= __y;
	m_nWidth		= __cx;
	m_nHeight		= __cy;
	return true;
}
