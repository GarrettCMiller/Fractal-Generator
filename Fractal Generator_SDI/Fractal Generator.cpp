// Fractal Generator.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Fractal Generator.h"
#include "MainFrm.h"

#include "Fractal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CFractal frac;

bool CFractalGeneratorApp::m_bActive = true;
bool CFractalGeneratorApp::m_bQuit = false;
CMainFrame* CFractalGeneratorApp::pMainFrame = NULL;

// CFractalGeneratorApp

BEGIN_MESSAGE_MAP(CFractalGeneratorApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
END_MESSAGE_MAP()


// CFractalGeneratorApp construction

CFractalGeneratorApp::CFractalGeneratorApp()
{
	// TODO: add construction code here,
	
	pMainFrame = NULL;
}


// The one and only CFractalGeneratorApp object

CFractalGeneratorApp theApp;

// CFractalGeneratorApp initialization

BOOL CFractalGeneratorApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("GWare"));
	
	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	ASSERT(!pMainFrame);

	pMainFrame = new CMainFrame;
	
	if (!pMainFrame)
		return FALSE;

	m_pMainWnd = pMainFrame;
	// create and load the frame with its resources
	
	pMainFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);
	
	// The one and only window has been initialized, so show and update it
	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pMainFrame->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand

	::CreateThread(NULL, 0,
		(LPTHREAD_START_ROUTINE) &CFractalGeneratorApp::Update,
		NULL,
		0,
		NULL);

	return TRUE;
}


// CFractalGeneratorApp message handlers



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CFractalGeneratorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


//////////////////////////////////////////////////////////////////////////
DWORD CFractalGeneratorApp::Update(LPVOID param)
{
	static const float k_TimeSlice = 1.0f / 8.0f;

	Timer renderTimer(true);

	m_bQuit = false;
	m_bActive = true;

	while (!m_bQuit)
	{
		frac.Update();

		if (frac.m_State != eState_Done && m_bActive && renderTimer >= k_TimeSlice)
		{
			pMainFrame->m_wndView.Invalidate(false);
			renderTimer.Reset();
		}
	}

	return 1;
}
//////////////////////////////////////////////////////////////////////////

// CFractalGeneratorApp message handlers

#if 0
int CFractalGeneratorApp::Run()
{
	ASSERT_VALID(this);

	// for tracking the idle time state
	BOOL bIdle = TRUE;
	LONG lIdleCount = 0;

	// acquire and dispatch messages until a WM_QUIT message is received.
	for (;;)
	{
		// phase1: check to see if we can do idle work
		while (bIdle &&
			!::PeekMessage(&m_msgCur, NULL, NULL, NULL, PM_NOREMOVE))
		{
			// call OnIdle while in bIdle state
			if (!OnIdle(lIdleCount++))
				bIdle = FALSE; // assume "no idle" state
		}

		// phase2: pump messages while available
		do
		{
			// pump message, but quit on WM_QUIT
			if (!PumpMessage())
				return ExitInstance();

			// reset "no idle" state after pumping "normal" message
			if (IsIdleMessage(&m_msgCur))
			{
				bIdle = TRUE;
				lIdleCount = 0;
			}

		} while (::PeekMessage(&m_msgCur, NULL, NULL, NULL, PM_NOREMOVE));

		Update();
	}

	ASSERT(FALSE);  // not reachable



	/*return CWinApp::Run();*/
}

BOOL CFractalGeneratorApp::OnIdle(LONG lCount)
{
	// TODO: Add your specialized code here and/or call the base class
	Update();

	return CWinApp::OnIdle(lCount);
}
#endif