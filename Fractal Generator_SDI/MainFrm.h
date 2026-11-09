// MainFrm.h : interface of the CMainFrame class
//


#pragma once

#include "ChildView.h"
#include "ParamsDlg.h"
#include "JuliaDlg.h"
#include "ImgPropsDlg.h"
#include "PaletteDlg.h"

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	JuliaDlg	m_JuliaDlg;
	CChildView  m_wndView;


public:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

	ParamsDlg	m_ParamsDlg;
	ImgPropsDlg	m_ImgPropsDlg;
	PaletteDlg	m_PaletteDlg;

	bool bReadyToGo;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnUpdateViewFractalparameters(CCmdUI *pCmdUI);
	afx_msg void OnViewFractalparameters();
	afx_msg void OnHelpHelp();
	afx_msg void OnViewImageproperties();
	afx_msg void OnUpdateViewImageproperties(CCmdUI *pCmdUI);
	afx_msg void OnViewJuliawindow();
	afx_msg void OnUpdateViewJuliawindow(CCmdUI *pCmdUI);
	afx_msg void OnViewPaletteeditor();
	afx_msg void OnUpdateViewPaletteeditor(CCmdUI *pCmdUI);
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnClose();
};


