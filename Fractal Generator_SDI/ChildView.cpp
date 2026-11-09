// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "Fractal Generator.h"
#include "ChildView.h"
#include "MainFrm.h"

#include "Fractal.h"
#include ".\childview.h"

#include "PlasmaFractal.h"

#include <atltime.h>

#include "ProfileSample.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMidpointDisplacement	md;
CPlasmaFractal			pf;

// CChildView

CChildView::CChildView()
{
	m_bAutoJulia = true;
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_FRACTAL_BEGIN, OnFractalBegin)
	ON_COMMAND(ID_FRACTAL_TIME, OnFractalTime)
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveas)
	ON_UPDATE_COMMAND_UI(ID_FRACTAL_BEGIN, OnUpdateFractalBegin)
	ON_WM_MBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_FRACTAL_AUTOJULIA, OnFractalAutojulia)
	ON_UPDATE_COMMAND_UI(ID_FRACTAL_AUTOJULIA, OnUpdateFractalAutojulia)
	ON_COMMAND(ID_SAVE_SAVESECONDARYAS, OnSaveSavesecondaryas)
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), (HBRUSH)GetStockObject(BLACK_BRUSH), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	frac.Render(dc/*, 0, 0, 1024, 1024*/);
	//md.Render(dc);
	//pf.Render(dc);
}

void CChildView::BeginFractal()
{
	frac.Init(this);
	
	//md.Init(this, 1);//Vec2(0.0f, 512.0f), Vec2(1024.0f, 512.0f), 0);
	//md.Process(12);

//  	pf.Init(k_vZero,				Vec2(512.0f, 0.0f), 
//  			Vec2(512.0f, 512.0f),	Vec2(0.0f, 512.0f),
//  			513, 513, 0);
// 	pf.Process(9);

	//CProfileSample::ResetAll();
}

BOOL CChildView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class

	//BeginFractal();
	
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect fr;

	fr.SetRect(0, 0, frac.m_vImageSize.X, frac.m_vImageSize.Y);

	if (!fr.PtInRect(point))
	{
		CWnd::OnLButtonDown(nFlags, point);
		return;
	}

	if ( nFlags & MK_CONTROL)
		frac.SetCenterPoint( point.x, point.y );
	else
		frac.SetCenterPoint( point.x, point.y, eZoom_In, ( nFlags & MK_SHIFT ? eZoomSpec_Double : eZoomSpec_None ) );
	
	BeginFractal();

	Invalidate(false);

	CWnd::OnLButtonDown(nFlags, point);
}

void CChildView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CRect fr;

	fr.SetRect(0, 0, frac.m_vImageSize.X, frac.m_vImageSize.Y);

	if (!fr.PtInRect(point))
	{
		CWnd::OnRButtonDown(nFlags, point);
		return;
	}

	frac.SetCenterPoint( point.x, point.y, eZoom_Out, ( nFlags & MK_SHIFT ? eZoomSpec_Double : eZoomSpec_None ) );

	BeginFractal();

	CWnd::OnRButtonDown(nFlags, point);
}

void CChildView::OnFractalBegin()
{
	if (frac.m_State != eState_Processing)
		BeginFractal();
}

void CChildView::OnFractalTime()
{
}

void CChildView::OnFileSaveas()
{
	CTime t = CTime::GetCurrentTime();
	
	CString ctime	= t.Format("%m%d%y_%H%M%S"),
			fname	= "Mand_" + ctime;

	CFileDialog dlg(false,
					"jpg", fname,
					OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_PATHMUSTEXIST,
					"JPEG Files",
					this->GetParent());

	if (dlg.DoModal() == IDOK)
	{
		frac.m_Image.Save( dlg.GetPathName() );
	}
}

void CChildView::OnUpdateFractalBegin(CCmdUI *pCmdUI)
{
	if (frac.m_State == eState_Processing)
		pCmdUI->SetText("Stop");
	else
		pCmdUI->SetText("Begin");
}

void CChildView::OnMButtonDown(UINT nFlags, CPoint point)
{
	CRect fr;

	fr.SetRect(0, 0, frac.m_vImageSize.X, frac.m_vImageSize.Y);

	if (!fr.PtInRect(point))
	{
		CWnd::OnRButtonDown(nFlags, point);
		return;
	}

	//((CMainFrame*)(theApp.m_pMainWnd))->m_wndStatusBar->SetWindowText( "Please wait, rendering in progress..." );

	julia.SetupJulia(point.x, point.y);
	julia.Init(this);
	//julia.Update();

	//( (CMainFrame*) theApp.m_pMainWnd )->m_wndStatusBar->SetWindowText( "Done!" );

	Invalidate(false);

	CWnd::OnMButtonDown(nFlags, point);
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bAutoJulia)
	{
		CRect fr;

		fr.SetRect(0, 0, frac.m_vImageSize.X, frac.m_vImageSize.Y);

		if (!fr.PtInRect(point))
		{
			CWnd::OnRButtonDown(nFlags, point);
			return;
		}

		if (julia.m_State != eState_Processing)
		{
			julia.SetupJulia(point.x, point.y);
			julia.Init(this);
			//julia.Update();

			((CMainFrame *) GetParent())->m_JuliaDlg.Invalidate(NULL);

			//Invalidate(false);
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CChildView::OnFractalAutojulia()
{
	m_bAutoJulia = !m_bAutoJulia;
}

void CChildView::OnUpdateFractalAutojulia(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bAutoJulia);
}

void CChildView::OnSaveSavesecondaryas()
{
	CTime t = CTime::GetCurrentTime();

	CString ctime	= t.Format("%m%d%y_%H%M%S"),
		fname	= "Julia_" + ctime;

	CFileDialog dlg(false,
		"jpg", fname,
		OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_PATHMUSTEXIST,
		"JPEG Files",
		this->GetParent());

	if (dlg.DoModal() == IDOK)
	{
		julia.m_Image.Save( dlg.GetPathName() );
	}
}

LRESULT CChildView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return CWnd::WindowProc(message, wParam, lParam);
}