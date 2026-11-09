// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Fractal Generator.h"

#include "MainFrm.h"
#include ".\mainfrm.h"

#include "Fractal.h"
#include "HelpDialog.h"

#include "ProfileSample.h"
#include "ProfileLogHandler.h"
#include "Timer.h"

#include "D3DWrapper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Timer Timer::ms_GlobalTimer;
float Timer::ms_TimeScale = 1.0f;
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_UPDATE_COMMAND_UI(ID_VIEW_FRACTALPARAMETERS, OnUpdateViewFractalparameters)
	ON_COMMAND(ID_VIEW_FRACTALPARAMETERS, OnViewFractalparameters)
	ON_COMMAND(ID_HELP_HELP, OnHelpHelp)
	ON_COMMAND(ID_VIEW_IMAGEPROPERTIES, OnViewImageproperties)
	ON_UPDATE_COMMAND_UI(ID_VIEW_IMAGEPROPERTIES, OnUpdateViewImageproperties)
	ON_COMMAND(ID_VIEW_JULIAWINDOW, OnViewJuliawindow)
	ON_UPDATE_COMMAND_UI(ID_VIEW_JULIAWINDOW, OnUpdateViewJuliawindow)
	ON_COMMAND(ID_VIEW_PALETTEEDITOR, OnViewPaletteeditor)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PALETTEEDITOR, OnUpdateViewPaletteeditor)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	bReadyToGo = false;
	Timer::ms_GlobalTimer.Init();
	
	//CProfileSample::ms_bProfilerEnabled = true;
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
	theApp.m_bQuit = true;

	//CProfileSample::Output();
	//g_ProfileHandler->DeleteInstance();
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_FRACTAL))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	m_ParamsDlg.Create( IDD_FRACTAL_PARAMS, this );

	
	
	m_JuliaDlg.Create( IDD_JULIA_WINDOW, this );

	m_JuliaDlg.ShowWindow( SW_HIDE);
	m_wndView.m_bAutoJulia = false;

	m_ImgPropsDlg.Create( IDD_IMAGE_PROPS, this );

	m_PaletteDlg.Create( IDD_PALETTEDLG, this );

	//g_D3D->m_hWnd = this;

	bReadyToGo = true;

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | /*WS_MAXIMIZE |*/ WS_SYSMENU;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'b':
			if (CProfileSample::ms_bProfilerEnabled)
				CProfileSample::ms_bProfilerEnabled = false;
		}
		break;
	}
// 	if (KEYDOWN(VK_CONTROL) && KEYDOWN('B') && !CProfileSample::ms_bProfilerEnabled)
// 	{
// 		CProfileSample::ms_bProfilerEnabled = true;
// 		CProfileSample::ResetAll();
// 	}
// 	else if (KEYDOWN(VK_CONTROL) && KEYDOWN('N') && CProfileSample::ms_bProfilerEnabled)
// 	{
// 		CProfileSample::ms_bProfilerEnabled = false;
// 	}
// 	else if (KEYDOWN(VK_CONTROL) && KEYDOWN('M'))
// 	{
// 		CProfileSample::ResetAll();
// 	}

	return CFrameWnd::WindowProc(message, wParam, lParam);
}

void CMainFrame::OnUpdateViewFractalparameters(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_ParamsDlg.IsWindowVisible() );
}

void CMainFrame::OnViewFractalparameters()
{
	if (m_ParamsDlg.IsWindowVisible())
		m_ParamsDlg.ShowWindow( SW_HIDE );
	else
		m_ParamsDlg.ShowWindow( SW_SHOW );
}

void CMainFrame::OnHelpHelp()
{
	HelpDialog dlg;

	dlg.DoModal();
}

void CMainFrame::OnViewImageproperties()
{
	if (m_ImgPropsDlg.IsWindowVisible())
		m_ImgPropsDlg.ShowWindow(SW_HIDE);
	else
		m_ImgPropsDlg.ShowWindow(SW_SHOW);
}

void CMainFrame::OnUpdateViewImageproperties(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_ImgPropsDlg.IsWindowVisible() );
}

void CMainFrame::OnViewJuliawindow()
{
	if (m_JuliaDlg.IsWindowVisible())
		m_JuliaDlg.ShowWindow(SW_HIDE);
	else
		m_JuliaDlg.ShowWindow(SW_SHOW);
}

void CMainFrame::OnUpdateViewJuliawindow(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_JuliaDlg.IsWindowVisible() );
}

void CMainFrame::OnViewPaletteeditor()
{
	if (m_PaletteDlg.IsWindowVisible())
		m_PaletteDlg.ShowWindow(SW_HIDE);
	else
		m_PaletteDlg.ShowWindow(SW_SHOW);
}

void CMainFrame::OnUpdateViewPaletteeditor(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_PaletteDlg.IsWindowVisible() );
}

LRESULT CMainFrame::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	//CMainFrame* pMainFrm = ((CMainFrame*)(theApp.m_pMainWnd));

	//static Timer	renderTimer(true, false);

	//static char buf[128] = {'\0'};


	//if (pMainFrm->bReadyToGo && frac.m_State == eState_Processing || frac.m_State == eState_Rendering)
	//{
	//	sprintf_s(buf, "%f%% done...  (X, Y): (%d, %d)", frac.m_Processors[0].m_PercentComplete, frac.m_Processors[0].m_nX, frac.m_Processors[0].m_nY);
	//	pMainFrm->m_wndStatusBar.SetWindowText(buf);
	//}
	return CFrameWnd::DefWindowProc(message, wParam, lParam);
}

void CMainFrame::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	theApp.m_bQuit = true;

	CFrameWnd::OnClose();
}
