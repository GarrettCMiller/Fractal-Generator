// ChildFrm.cpp : implementation of the CChildFrame class
//
#include "stdafx.h"
#include "Fractal Generator.h"

#include "ChildFrm.h"

#include "Fractal.h"

#include "D3DWrapper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	ON_COMMAND(ID_FILE_CLOSE, &CChildFrame::OnFileClose)
	ON_WM_SETFOCUS()
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	bBlueprintWnd = false;
	// TODO: add member initialization code here
}

CChildFrame::~CChildFrame()
{
	
}


BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}


// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG


// CChildFrame message handlers
void CChildFrame::OnFileClose() 
{
	theApp.m_bCloseWindow = true;
	// To close the frame, just send a WM_CLOSE, which is the equivalent
	// choosing close from the system menu.
	SendMessage(WM_CLOSE);
}

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//AttachToDirectX
	
	// create a view to occupy the client area of the frame
 	if (!m_wndView.Create(NULL, "New Fractal", AFX_WS_DEFAULT_VIEW | WS_MAXIMIZE, 
 		CRect(0, 0, 512, 512), this, AFX_IDW_PANE_FIRST, NULL))
 	{
 		TRACE0("Failed to create view window\n");
 		return -1;
 	}

	if (!bBlueprintWnd)
	{
		if (!AttachToDirectX())
			return -1;
	}

	return 0;
}

void CChildFrame::BeginFractal(EFractalType eType, UInt32 subType)
{
	if (!bBlueprintWnd && eType != eFT_Invalid)
		m_wndView.BeginFractal(eType, subType);

	SetupWindowPos();
}

void CChildFrame::OnSetFocus(CWnd* pOldWnd) 
{
	CMDIChildWnd::OnSetFocus(pOldWnd);

	m_wndView.SetFocus();
}

BOOL CChildFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
	
	// otherwise, do default handling
	return CMDIChildWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//////////////////////////////////////////////////////////////////////////

void CChildFrame::SetupWindowPos()
{
	if (bBlueprintWnd)
	{
		SetWindowPos(&wndTop, 0, 0, 320, 320, SWP_NOZORDER | SWP_NOMOVE);
	}
	else
	{
		//make sure we have a fractal to base window size on
		if (!m_wndView.m_Fractal)
			return;

		CRect thisRect, clientRect;

		GetWindowRect(&thisRect);
		m_wndView.GetClientRect(&clientRect);

		int xwidth = thisRect.Width() - clientRect.Width(),
			yheight = thisRect.Height() - clientRect.Height();

		UInt32	imageWidth = m_wndView.m_Fractal->m_vImageSize.X,
				imageHeight = m_wndView.m_Fractal->m_vImageSize.Y;

		if (imageWidth == 0)
			imageWidth = 1024;
		if (imageHeight == 0)
			imageHeight = 768;

		SetWindowPos(&wndTop, 0, 0, imageWidth + xwidth, imageHeight + yheight, SWP_NOZORDER | SWP_NOMOVE);
	}
}

BOOL CChildFrame::DestroyWindow()
{
	theApp.m_bCloseWindow = true;

	while (!g_ThreadMgr->AllThreadsKilled(true))
	{
		//g_Output << "Waiting for fractal threads to exit...\n";
		Sleep(30);
	}

	g_D3D->SetDestWindowOverride(NULL);
	//g_D3D->SetDeviceWindow(/*theApp.m_pMainWnd->GetSafeHwnd()*/NULL);

	return CMDIChildWnd::DestroyWindow();
}

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
// 	pContext->m_pNewViewClass = RUNTIME_CLASS(CFractalView);
// 	//pContext->m_pLastView = 
// 
// 	// TODO: Add your specialized code here and/or call the base class
// 	if (!m_wndView.Create(this, 2, 2, CSize(100, 100), pContext))
// 		return -1;

	return CMDIChildWnd::OnCreateClient(lpcs, pContext);
}

Bool CChildFrame::AttachToDirectX()
{
	static Float pSize = 2.0f;

	g_D3D->SetDestWindowOverride(GetSafeHwnd());
	//if (!g_D3D->SetDeviceWindow(GetSafeHwnd()))
	//	return false;

	g_D3D->SetClipRange(500.0f, 0.1f, false);


	//g_D3D->SetLookAt(k_v3Zero);


	g_D3D->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	g_D3D->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

	g_D3D->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_D3D->GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_D3D->GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_D3D->GetDevice()->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&pSize));
	g_D3D->GetDevice()->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);

	//g_D3D->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return true;
}
void CChildFrame::OnSize(UINT nType, int cx, int cy)
{
	CMDIChildWnd::OnSize(nType, cx, cy);

	Invalidate(false);
}
