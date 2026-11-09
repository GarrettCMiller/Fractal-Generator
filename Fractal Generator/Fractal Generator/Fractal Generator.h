// Fractal Generator.h : main header file for the Fractal Generator application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CFractalGeneratorApp:
// See Fractal Generator.cpp for the implementation of this class
//
class CFractal;
class CChildFrame;

class CFractalGeneratorApp : public CWinApp
{
public:
	CFractalGeneratorApp();
	~CFractalGeneratorApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
protected:
	HMENU m_hMDIMenu;
	HACCEL m_hMDIAccel;

	CChildFrame* CreateFractalWindow(Bool bBlueprintWindow = false);

public:
	Bool	m_bQuit;
	Bool	m_bCloseWindow;

	CFractal* GetActiveFractal();

	afx_msg void OnAppAbout();
	
	DECLARE_MESSAGE_MAP()

	afx_msg void OnFileNew();
	
	afx_msg void OnNewMandelbrot();
	
	afx_msg void OnNewGasket();
	afx_msg void OnNewTwinXmasTree();
	afx_msg void OnNew5ptsnowflake();
	afx_msg void OnNewFern();
	afx_msg void OnNewKochcurve();

	afx_msg void OnNewLSystem1();
	afx_msg void OnNewLSystem2();
	afx_msg void OnNewLSystem3();
	
	afx_msg void OnNewRtShader();
};

extern CFractalGeneratorApp theApp;