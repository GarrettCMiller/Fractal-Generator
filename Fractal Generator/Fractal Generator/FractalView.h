//FractalView.h : interface of the CFractalView class
//

#pragma once

#include "Thread.h"

#include "FractalGlobals.h"

#include "CameraView.h"

class CFractal;
// CFractalView window

class CFractalView : public CWnd
{
// Construction
public:
	CFractalView();

// Attributes
public:
	bool m_bAutoJulia;

	CFractal*	m_Fractal;
	CThread*	m_Thread;

	static CCameraView cam;
	
	static Float orthoWidth;
	static Float orthoHeight;

// Operations
public:
	void BeginFractal(EFractalType eType, UInt32 subType);

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CFractalView();

	static DWORD ProcessingThread(LPVOID param);

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

public:
	//virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
protected:
public:
	afx_msg void OnFractalBegin();
	afx_msg void OnFractalTime();
	afx_msg void OnFileSaveas();
	afx_msg void OnUpdateFractalBegin(CCmdUI *pCmdUI);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnFractalAutojulia();
	afx_msg void OnUpdateFractalAutojulia(CCmdUI *pCmdUI);
	afx_msg void OnSaveSavesecondaryas();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

