/*
 * FILE: AMainFrame.h
 *
 * DESCRIPTION: 游戏的框架窗口类
 *
 * CREATED BY: 王远明, 2002/8/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */

#ifndef __AMAINFRAME__H__
#define __AMAINFRAME__H__

#include "windows.h"
#include "AUString.h"

typedef struct tagCREATEFRMSTRUCT {
    LPVOID      lpCreateParams;
    HINSTANCE   hInstance;
	//HCURSOR		hCursor;
    HMENU       hMenu;
	//HICON		hIcon;
    HWND        hwndParent;
    int         cy;
    int         cx;
    int         y;
    int         x;
    LONG        style;
    LPCSTR     lpszName;
    LPCSTR     lpszClass;
    DWORD       dwExStyle;
} CREATEFRMSTRUCT, *LPCREATEFRMSTRUCT;

class AMainFrame  
{
public:

	struct PRODUCT
	{
		union
		{
			struct
			{
				GUID guid;
				char name[sizeof(GUID)];
			};
			long data[(2 * sizeof(GUID)) / sizeof(long)];
		};
	};

	HWND		m_hWnd;

protected:

	PRODUCT		m_product;

	int			m_x;
	int			m_y;
	int			m_nWidth;
	int			m_nHeight;
	AUString	m_wndclassname;
	AUString	m_cmdLine;

public:

	//__cx and __cy stand for client width and height
	bool MoveWindow(int __x,int __y,int __cx,int __cy);
	inline int GetClientWidth() { return m_nWidth;}
	inline int GetClientHeight() { return m_nHeight;}
	void ShowWindow(int __nStatus);
	//cx and cy stand for client width and height
	bool Create(AUString product, 
				GUID productid, 
				int x,
				int y,
				int cx,
				int cy,
				HINSTANCE hInstance, 
				LPSTR lpCmdLine, 
                int nCmdShow, 
				WNDPROC WndProc);

	virtual bool OnCreate(LPCREATEFRMSTRUCT pWc);
	virtual bool PreCreateWindow(LPCREATEFRMSTRUCT pWc);
	AMainFrame();
	virtual ~AMainFrame();

protected:
	virtual bool InitInstance(HINSTANCE hInstance, LPSTR lpCmdLine, int nCmdShow, WNDPROC WndProc);
	virtual bool InitApplication(HINSTANCE hInstance, LPSTR lpCmdLine, int nCmdShow, WNDPROC WndProc);
};

#endif // __AMAINFRAME__H__