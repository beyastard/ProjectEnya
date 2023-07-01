#include <Windows.h>

#include "Render.h"
#include "ALog.h"
#include "resource.h"

// Locals & Globals
static HINSTANCE l_hInstance = nullptr;
static HWND	l_hMainWnd = nullptr;
static const char* l_szClassName = "Angelica v1.0 Demo Test Class";

static int l_iRenderWidth  = 800;
static int l_iRenderHeight = 600;
static bool l_bFullScreen  = false;

static ALog	l_Log;

// Local Functions
static bool _InitGameApp();
static void _FinalizeGameApp();
static ATOM _RegisterWndClass(HINSTANCE hInstance);
static bool _CreateMainWnd(HINSTANCE hInstance, int nCmdShow);
static LRESULT CALLBACK _WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

static bool _Tick(DWORD dwTickTime);
static bool _Render();

static void _LogOutput(const char* szMsg)
{
	l_Log.Log(szMsg);
}

// WinMain
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* lpCmdLine, int nCmdShow)
{
	// Initialize application
	if (!_InitGameApp())
	{
		a_LogOutput(1, "WinMain, Failed to initialize application");
		return -1;
	}

	// Register window class
	if (!_RegisterWndClass(hInstance))
	{
		a_LogOutput(1, "WinMain, Failed to register window class");
		return -1;
	}

	// Create main window
	if (!_CreateMainWnd(hInstance, nCmdShow))
	{
		a_LogOutput(1, "WinMain, Failed to create main window");
		return -1;
	}

	// Initialize render
	if (!g_Render.Init(l_hInstance, l_hMainWnd, l_bFullScreen))
	{
		a_LogOutput(1, "WinMain: Failed to initialize render");
		return -1;
	}

	static DWORD dwLastFrame = ATime_GetTime();
	MSG	msg;

	while(true)
	{
		DWORD dwTickTime = ATime_GetTime() - dwLastFrame;

		if (dwTickTime)
		{
			dwLastFrame = ATime_GetTime();

			// Limit time of a logic tick
			CLAMPMAXVALUE(dwTickTime, 100);
				
			//	Do logic tick
			_Tick(dwTickTime);

			//	Render scene
			_Render();
		}

		// Message tick which will prevent the application hanging up;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE))
		{
			GetMessage(&msg, nullptr, 0, 0); 
			if (msg.message == WM_QUIT)
			{
				goto EXIT;
			}
	
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Sleep a little time
		Sleep(5);
	}

EXIT:
	g_Render.Release();  // Release render	
	_FinalizeGameApp();  // Finalize game
	
	return 0;
}

static bool _InitGameApp()
{
	// Initialize 
	AFileMod_Initialize();

	// Get current directory as angelica's work path
	char szCurPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurPath);
	AFileMod_SetBaseDir(szCurPath);

	//	Create log file and redirect log output
	if (!l_Log.Init("Demo00.log", "Angelica Demo00 log file"))
		return false;

	a_RedirectDefLogOutput(_LogOutput);

	return true;
}

static void _FinalizeGameApp()
{
	AFileMod_Finalize();
}

static ATOM _RegisterWndClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof (WNDCLASSEX); 

	wcex.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc   = _WndProc;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = 0;
	wcex.hInstance     = hInstance;
	wcex.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_DEMO));
	wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wcex.lpszMenuName  = nullptr;
	wcex.lpszClassName = l_szClassName;
	wcex.hIconSm       = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_DEMO));

	return RegisterClassEx(&wcex);
}

static bool _CreateMainWnd(HINSTANCE hInstance, int nCmdShow)
{
	l_hInstance = hInstance;

	DWORD dwStyles = WS_POPUP;
	if (!l_bFullScreen)
		dwStyles |= WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME;

	DWORD dwExStyle = 0;
	const char* l_szWndName = "Angelica v1.0 Demo Test Window";

	HWND hWnd = CreateWindowExA(dwExStyle, l_szClassName, l_szWndName, dwStyles, 0, 0,
		l_iRenderWidth, l_iRenderHeight, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd)
		return false;

	l_hMainWnd = hWnd;

	// Adjust window position and size in non-fullscreen mode
	if (!l_bFullScreen)
	{
		RECT rcWnd = {0, 0, l_iRenderWidth, l_iRenderHeight};
		AdjustWindowRect(&rcWnd, dwStyles, FALSE);

		const int w = rcWnd.right - rcWnd.left;
		const int h = rcWnd.bottom - rcWnd.top;
		const int x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
		const int y = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;

		MoveWindow(l_hMainWnd, x, y, w, h, FALSE);
	}

	// Show main window
	ShowWindow(l_hMainWnd, nCmdShow);
	UpdateWindow(l_hMainWnd);

	// we force set foreground window to ensure our main window is activated
	SetForegroundWindow(l_hMainWnd);
	
	return true;
}

static LRESULT CALLBACK _WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_SETCURSOR:
		SetCursor(nullptr);
		break;

	case WM_KEYDOWN:
		{
			switch (wParam)
			{
			case VK_ESCAPE:
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				return 0;
			default: ;
			}
			break;
		}

		case WM_SIZE:
		{
			if (wParam == SIZE_MINIMIZED || wParam == SIZE_MAXHIDE || wParam == SIZE_MAXSHOW)
				break;

			const int cx = (lParam & 0x0000ffff);
			const int cy = (lParam & 0xffff0000) >> 16;

			if (!cx || !cy)
				return 0;

			g_Render.ResizeDevice(cx, cy);
			break;
		}
		
	default: ;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

static bool _Tick(DWORD dwTickTime)
{
	// TODO: do logic tick here...
	return true;
}

static bool _Render()
{
	if (!g_Render.BeginRender())
		return false;

	g_Render.ClearViewport();

	// TODO: render scene and objects here...

	g_Render.EndRender();
	
	return true;
}
