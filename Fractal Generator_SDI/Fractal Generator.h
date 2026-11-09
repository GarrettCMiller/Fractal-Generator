// Fractal Generator.h : main header file for the Fractal Generator application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CFractalGeneratorApp:
// See Fractal Generator.cpp for the implementation of this class
//

class CMainFrame;

class CFractalGeneratorApp : public CWinApp
{
public:
	CFractalGeneratorApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	static bool m_bQuit;
	static bool m_bActive;

	static CMainFrame*	pMainFrame;

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

// 	virtual int Run();
// 	virtual BOOL OnIdle(LONG lCount);

protected:
	static DWORD Update(LPVOID param);
};

extern CFractalGeneratorApp theApp;