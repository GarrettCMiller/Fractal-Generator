// MainFrm.h : interface of the CMainFrame class
//


#pragma once

#include "PropertyDock.h"

#include "ParamsDlg.h"
#include "JuliaDlg.h"
#include "ImgPropsDlg.h"
#include "PaletteDlg.h"

#include "OutputDlg.h"

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

	CPropertyDock	m_PropsDock;
	CPropertyDock	m_UtilDock;

	JuliaDlg	m_JuliaDlg;
	ParamsDlg	m_ParamsDlg;
	ImgPropsDlg	m_ImgPropsDlg;
	PaletteDlg	m_PaletteDlg;

	COutputDlg	m_OutputDlg;

	bool bReadyToGo;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL DestroyWindow();

	afx_msg void OnUpdateViewFractalparameters(CCmdUI *pCmdUI);
	afx_msg void OnViewFractalparameters();
	afx_msg void OnHelpHelp();
	afx_msg void OnViewImageproperties();
	afx_msg void OnUpdateViewImageproperties(CCmdUI *pCmdUI);
	afx_msg void OnViewJuliawindow();
	afx_msg void OnUpdateViewJuliawindow(CCmdUI *pCmdUI);
	afx_msg void OnViewPaletteeditor();
	afx_msg void OnUpdateViewPaletteeditor(CCmdUI *pCmdUI);
};


