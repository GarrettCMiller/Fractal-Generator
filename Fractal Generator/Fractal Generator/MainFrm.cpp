// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Fractal Generator.h"

#include "Globals.h"

#include "MainFrm.h"
#include "HelpDialog.h"

#include "ThreadMgr.h"

#include "D3DWrapper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern String DefTexturePath;
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_VIEW_FRACTALPARAMETERS, OnUpdateViewFractalparameters)
	ON_COMMAND(ID_VIEW_FRACTALPARAMETERS, OnViewFractalparameters)
	ON_COMMAND(ID_HELP_HELP, OnHelpHelp)
	ON_COMMAND(ID_VIEW_IMAGEPROPERTIES, OnViewImageproperties)
	ON_UPDATE_COMMAND_UI(ID_VIEW_IMAGEPROPERTIES, OnUpdateViewImageproperties)
	ON_COMMAND(ID_VIEW_JULIAWINDOW, OnViewJuliawindow)
	ON_UPDATE_COMMAND_UI(ID_VIEW_JULIAWINDOW, OnUpdateViewJuliawindow)
	ON_COMMAND(ID_VIEW_PALETTEEDITOR, OnViewPaletteeditor)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PALETTEEDITOR, OnUpdateViewPaletteeditor)
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
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
	g_ThreadMgr->DeleteInstance();
	g_D3D->DeleteInstance();
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
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

// 	m_UtilDock.Construct("Utility Dock Window", this);
// 
// 	m_UtilDock.AddPage(&m_OutputDlg);
// 
// 	m_UtilDock.Create(this,	WS_POPUP | WS_BORDER | WS_CAPTION | WS_VISIBLE | WS_SYSMENU, 
// 							WS_EX_ACCEPTFILES | WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW);
// 
// 	m_UtilDock.ShowWindow(SW_SHOW);
// 	m_UtilDock.SetWindowPos(&wndTopMost, 50, 50, 0, 0, SWP_NOSIZE);

	//BOOL ret = m_OutputDlg.Create(IDD_OUTPUT_WINDOW, this);
	//m_OutputDlg.ShowWindow(SW_SHOW);

	/*m_PropsDock.Construct("Property Dock", this);

	m_PropsDock.AddPage(&m_PaletteDlg);
	m_PropsDock.AddPage(&m_ImgPropsDlg);
	m_PropsDock.AddPage(&m_ParamsDlg);

	m_PropsDock.Create(this,	WS_POPUP | WS_BORDER | WS_CAPTION | WS_VISIBLE | WS_SYSMENU, 
	 							WS_EX_ACCEPTFILES | WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW);
	
	m_PropsDock.ShowWindow(SW_SHOW);*/




//   	m_ParamsDlg.Create(IDD_FRACTAL_PARAMS, this);
//  	m_ParamsDlg.ShowWindow(SW_SHOW);




// 
 	//m_JuliaDlg.Create( IDD_JULIA_WINDOW, this );
// 
 	//m_JuliaDlg.ShowWindow( SW_SHOW);
// 	//m_wndView.m_bAutoJulia = false;
// 
// 	m_ImgPropsDlg.Create( IDD_IMAGE_PROPS, this );
// 
// 	m_PaletteDlg.Create( IDD_PALETTEDLG, this );

	//g_D3D->m_hWnd = this;

	bReadyToGo = true;

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers




BOOL CMainFrame::DestroyWindow()
{
	Timer threadTimeout(true);

	theApp.m_bQuit = true;

	while (!g_ThreadMgr->AllThreadsKilled())
	{
		//g_Output << "Waiting for particle thread to exit...\n";
		Sleep(30);

		if (threadTimeout > 1.0f)
			break;
	}

	//theApp.m_Emitters.FreePointers();

	//g_Log.DetachMFC();
	//g_ResLog.DetachMFC();
	//g_Output.DetachMFC();

	return CMDIFrameWnd::DestroyWindow();
}

void CMainFrame::OnUpdateViewFractalparameters(CCmdUI *pCmdUI)
{
	//pCmdUI->SetCheck( m_ParamsDlg.IsWindowVisible() );
}

void CMainFrame::OnViewFractalparameters()
{
// 	if (m_ParamsDlg.IsWindowVisible())
// 		m_ParamsDlg.ShowWindow( SW_HIDE );
// 	else
// 		m_ParamsDlg.ShowWindow( SW_SHOW );
}

void CMainFrame::OnHelpHelp()
{
	HelpDialog dlg;

	dlg.DoModal();
}

void CMainFrame::OnViewImageproperties()
{
// 	if (m_ImgPropsDlg.IsWindowVisible())
// 		m_ImgPropsDlg.ShowWindow(SW_HIDE);
// 	else
// 		m_ImgPropsDlg.ShowWindow(SW_SHOW);
}

void CMainFrame::OnUpdateViewImageproperties(CCmdUI *pCmdUI)
{
	//pCmdUI->SetCheck( m_ImgPropsDlg.IsWindowVisible() );
}

void CMainFrame::OnViewJuliawindow()
{
// 	if (m_JuliaDlg.IsWindowVisible())
// 		m_JuliaDlg.ShowWindow(SW_HIDE);
// 	else
// 		m_JuliaDlg.ShowWindow(SW_SHOW);
}

void CMainFrame::OnUpdateViewJuliawindow(CCmdUI *pCmdUI)
{
	//pCmdUI->SetCheck( m_JuliaDlg.IsWindowVisible() );
}

void CMainFrame::OnViewPaletteeditor()
{
// 	if (m_PaletteDlg.IsWindowVisible())
// 		m_PaletteDlg.ShowWindow(SW_HIDE);
// 	else
// 		m_PaletteDlg.ShowWindow(SW_SHOW);
}

void CMainFrame::OnUpdateViewPaletteeditor(CCmdUI *pCmdUI)
{
	//pCmdUI->SetCheck( m_PaletteDlg.IsWindowVisible() );
}
