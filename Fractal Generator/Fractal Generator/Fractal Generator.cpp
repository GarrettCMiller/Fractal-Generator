// Fractal Generator.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Fractal Generator.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "BlueprintFrame.h"

#include "D3DWrapper.h"
#include "SpriteMgr.h"

#include "Globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


extern String DefTexturePath;

// CFractalGeneratorApp

BEGIN_MESSAGE_MAP(CFractalGeneratorApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CFractalGeneratorApp::OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, &CFractalGeneratorApp::OnFileNew)
	ON_COMMAND(ID_NEW_GASKET, &CFractalGeneratorApp::OnNewGasket)
	ON_COMMAND(ID_NEW_TWINXMASTREE, &CFractalGeneratorApp::OnNewTwinXmasTree)
	ON_COMMAND(ID_NEW_5PTSNOWFLAKE, &CFractalGeneratorApp::OnNew5ptsnowflake)
	ON_COMMAND(ID_NEW_FERN, &CFractalGeneratorApp::OnNewFern)
	ON_COMMAND(ID_NEW_KOCHCURVE, &CFractalGeneratorApp::OnNewKochcurve)
	ON_COMMAND(ID_NEW_MANDELBROT, &CFractalGeneratorApp::OnNewMandelbrot)
	ON_COMMAND(ID_NEW_LSYSTEM_1, &CFractalGeneratorApp::OnNewLSystem1)
	ON_COMMAND(ID_NEW_LSYSTEM_2, &CFractalGeneratorApp::OnNewLSystem2)
	ON_COMMAND(ID_NEW_LSYSTEM_3, &CFractalGeneratorApp::OnNewLSystem3)
	ON_COMMAND(ID_NEW_RT_SHADER, &CFractalGeneratorApp::OnNewRtShader)
END_MESSAGE_MAP()


// CFractalGeneratorApp construction

CFractalGeneratorApp::CFractalGeneratorApp()
{
	m_bQuit = m_bCloseWindow = false;
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CFractalGeneratorApp::~CFractalGeneratorApp()
{
	
}


// The one and only CFractalGeneratorApp object

CFractalGeneratorApp theApp;


// CFractalGeneratorApp initialization

BOOL CFractalGeneratorApp::InitInstance()
{
	//_CrtSetBreakAlloc(169);

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("GWare"));

	WriteProfileString("Defaults", "FractalType", "PSMandel");

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CMDIFrameWnd* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// create main MDI frame window
	if (!pFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	// try to load shared MDI menus and accelerator table
	//TODO: add additional member variables and load calls for
	//	additional menu types your application may need
	HINSTANCE hInst = AfxGetResourceHandle();
	m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_FractalGeneratoTYPE));
	m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_FractalGeneratoTYPE));


	//g_Log.OpenNewLog("Logger");

	g_hInst = hInst;
	g_hWnd = pFrame->GetSafeHwnd();

	if (g_D3D->Init(g_hWnd, g_ImgSize, g_ImgSize) == 0)
		return false;

	if (g_SpriteMgr->LoadSprite(DefTexturePath) == -1)
	{
		ASSERT(0);
	}

	// The main window has been initialized, so show and update it
	pFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pFrame->UpdateWindow();

	/*if (KEYUP(VK_SHIFT))
		OnFileNew();*/

	/*Double e = 0.0010;
	Double o = 0, maxo = 0;
	UInt32 i = 0;

	for (UInt32 j = 0; j < 10; j++)
	{
		TComplex z = 0;
		TComplex c = TComplex(-0.75, e);
		
		maxo = 0;
		i = 0;
		
		do
		{
			z = z * z + c;

			i++;

			if (i == 31415)
				i = i;

			o = z.real() * z.real() + z.imag() * z.imag();
			maxo = Max(maxo, o);

		} while (o < 4.0);

		e *= 0.10;
	}*/

	return TRUE;
}


// CFractalGeneratorApp message handlers

int CFractalGeneratorApp::ExitInstance() 
{
	//TODO: handle additional resources you may have added
	if (m_hMDIMenu != NULL)
		FreeResource(m_hMDIMenu);
	if (m_hMDIAccel != NULL)
		FreeResource(m_hMDIAccel);

	return CWinApp::ExitInstance();
}

CChildFrame* CFractalGeneratorApp::CreateFractalWindow(Bool bBlueprintWindow /* = false */)
{
	CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);

	/*if (bBlueprintWindow)
	{
		pFrame->CreateNewChild(RUNTIME_CLASS(CBlueprintFrame), 
								IDR_FractalGeneratoTYPE, 
								m_hMDIMenu, m_hMDIAccel);
	}*/
	return static_cast<CChildFrame*>(pFrame->CreateNewChild(RUNTIME_CLASS(CChildFrame), 
								IDR_FractalGeneratoTYPE, 
								m_hMDIMenu, m_hMDIAccel));
}

//Custom MRCM
void CFractalGeneratorApp::OnFileNew() 
{
	OnNewRtShader();

	//Try creating blueprint frame first
	CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);

// 	static_cast<CChildFrame*>(pFrame->CreateNewChild(RUNTIME_CLASS(CBlueprintFrame), 
// 		IDR_FractalGeneratoTYPE, 
// 		m_hMDIMenu, m_hMDIAccel));

	CChildFrame* pNew = CreateFractalWindow();

	if (pNew)
	{
		//pNew->BeginFractal(eFT_LSystem, 0);
		//pNew->BeginFractal(eFT_Complex, 0);
		pNew->BeginFractal(eFT_MRCM, 0);
	}
	else
		MessageBox(NULL, "Failed to create fractal window!", "EPIC FAIL!", MB_OK | MB_ICONERROR);
}

//////////////////////////////////////////////////////////////////////////

CFractal* CFractalGeneratorApp::GetActiveFractal()
{
	CMainFrame* pMFrame = static_cast<CMainFrame*>(m_pMainWnd);
	
	ASSERT(pMFrame);

	CChildFrame* pFrame = static_cast<CChildFrame*>(pMFrame->MDIGetActive());

	if (!pFrame)
		return NULL;
	
	CFractalView& pView = pFrame->m_wndView;
	CFractal*	pFrac = pView.m_Fractal;
	//ASSERT(pFrac);

	return pFrac;
}

//////////////////////////////////////////////////////////////////////////

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


// CFractalGeneratorApp message handlers


void CFractalGeneratorApp::OnNewGasket()
{
	CChildFrame* pNew = CreateFractalWindow(true);

	if (pNew)
		pNew->BeginFractal(eFT_MRCM, 0);
	else
		MessageBox(NULL, "Failed to create fractal window!", "EPIC FAIL!", MB_OK | MB_ICONERROR);
}

void CFractalGeneratorApp::OnNewTwinXmasTree()
{
	CChildFrame* pNew = CreateFractalWindow(true);

	if (pNew)
		pNew->BeginFractal(eFT_MRCM, 2);
	else
		MessageBox(NULL, "Failed to create fractal window!", "EPIC FAIL!", MB_OK | MB_ICONERROR);
}

void CFractalGeneratorApp::OnNew5ptsnowflake()
{
	CChildFrame* pNew = CreateFractalWindow(true);

	if (pNew)
		pNew->BeginFractal(eFT_MRCM, 1);
	else
		MessageBox(NULL, "Failed to create fractal window!", "EPIC FAIL!", MB_OK | MB_ICONERROR);
}

void CFractalGeneratorApp::OnNewFern()
{
	CChildFrame* pNew = CreateFractalWindow(true);

	if (pNew)
		pNew->BeginFractal(eFT_MRCM, 3);
	else
		MessageBox(NULL, "Failed to create fractal window!", "EPIC FAIL!", MB_OK | MB_ICONERROR);
}

void CFractalGeneratorApp::OnNewKochcurve()
{
	CChildFrame* pNew = CreateFractalWindow(true);

	if (pNew)
		pNew->BeginFractal(eFT_MRCM, 4);
	else
		MessageBox(NULL, "Failed to create fractal window!", "EPIC FAIL!", MB_OK | MB_ICONERROR);
}

void CFractalGeneratorApp::OnNewMandelbrot()
{
	CChildFrame* pNew = CreateFractalWindow();

	if (pNew)
		pNew->BeginFractal(eFT_Complex, 1);
	else
		MessageBox(NULL, "Failed to create fractal window!", "EPIC FAIL!", MB_OK | MB_ICONERROR);
}

void CFractalGeneratorApp::OnNewLSystem1()
{
	CChildFrame* pNew = CreateFractalWindow();

	if (pNew)
		pNew->BeginFractal(eFT_LSystem, 0);
	else
		MessageBox(NULL, "Failed to create fractal window!", "EPIC FAIL!", MB_OK | MB_ICONERROR);
}

void CFractalGeneratorApp::OnNewLSystem2()
{
	CChildFrame* pNew = CreateFractalWindow();

	if (pNew)
		pNew->BeginFractal(eFT_LSystem, 1);
	else
		MessageBox(NULL, "Failed to create fractal window!", "EPIC FAIL!", MB_OK | MB_ICONERROR);
}

void CFractalGeneratorApp::OnNewLSystem3()
{
	CChildFrame* pNew = CreateFractalWindow();

	if (pNew)
		pNew->BeginFractal(eFT_LSystem, 2);
	else
		MessageBox(NULL, "Failed to create fractal window!", "EPIC FAIL!", MB_OK | MB_ICONERROR);
}

void CFractalGeneratorApp::OnNewRtShader()
{
	CChildFrame* pNew = CreateFractalWindow();

	if (pNew)
		pNew->BeginFractal(eFT_MRCM, 7);
	else
		MessageBox(NULL, "Failed to create fractal window!", "EPIC FAIL!", MB_OK | MB_ICONERROR);
}
