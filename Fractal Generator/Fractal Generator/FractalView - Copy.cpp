// FractalView.cpp : implementation of the CFractalView class
//

#include "stdafx.h"
#include "Fractal Generator.h"
#include "FractalView.h"

#include "GameMacros.h"

#include "Fractal.h"
#include "ComplexFractal.h"
#include "MidpointDisplacement.h"
#include "MRCM.h"
#include "LSystem.h"

#include "Rect.h"

#include "EffectMgr.h"
#include "Terrain.h"

#include "FontMgr.h"
#include "UIText.h"

#include "SpriteMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFractalView

CFractalView::CFractalView() : m_Fractal(NULL)
{
	0;
}

CFractalView::~CFractalView()
{
	//Timer waitTimer(true);

	//Wait for the fractal thread to exit
	/*while (g_ThreadMgr->Exists() && !g_ThreadMgr->AllThreadsKilled())
	{
		if (waitTimer >= 2.0f)
			g_ThreadMgr->KillAll();
	}*/
	
	SAFE_DELETE(m_Fractal);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CFractalView, CWnd)
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
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
// CFractalView message handlers
//////////////////////////////////////////////////////////////////////////

BOOL CFractalView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)/*COLOR_WINDOW+1*/), NULL);

	return TRUE;
}

 void CFractalView::OnPaint() 
 {
 	CPaintDC dc(this); // device context for painting

	//dc.FloodFill(0, 0, 0xFFFFFF);
 	
 	if (m_Fractal && !m_Fractal->IsLocked())
 	{
 		//m_Fractal->Draw();
 		m_Fractal->Render(dc);
 	}
 }

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
static int bAll = 2;
static int iType = 1;

//BOOL CFractalView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
//{
//	//String strFracName = 
//
//	//HACK, crashes when I remove ... WTF?!?!
//	m_Fractal = new CFractal("Blank");
//
//	Bool ret = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
//	return ret;
//}

void CFractalView::BeginFractal(EFractalType eType, UInt32 subType)
{
	//SAFE_DELETE(m_Fractal);

	//TArgumentArray args(5);

	//UInt32 imgSize = 1024;//512;

	//switch (eType)
	//{
	//case eFT_Complex:
	//	{
	//		m_Fractal = new CComplexFractal("Mandelbrot");

	//		CComplexFractal::InitStruct is;
	//		is.imgWidth = is.imgHeight = imgSize;

	//		args[0] = reinterpret_cast<Pointer>(&is);
	//	}
	//	break;

	//case eFT_MRCM:
	//	{
	//		m_Fractal = new CMRCM("MRCM");

	//		CMRCM::InitStruct is;
	//		is.imgWidth = is.imgHeight = imgSize;

	//		args[0] = reinterpret_cast<Pointer>(&is);
	//	}
	//	break;

	//case eFT_LSystem:
	//	{
	//		m_Fractal = new CLSystem("LSystem");

	//		CLSystem::InitStruct is;
	//		is.imgWidth = is.imgHeight = imgSize;

	//		is.alphabet = "F";
	//		is.alphabet = "FB";
	//		
	//		is.axiom = "F";
	//		is.axiom = "B";
	//		
	//		is.rules = "F:F[+F]F[-F]F";
	//		is.rules = "F:FF+[+F-F-F]-[-F+F+F]";
	//		is.rules = "F:FF B:F[+B]F[-B]+B";
	//		is.rules = "F:FFBF B:F[B[+F+F]]B-[-[+F-F]+F+F-[F-F++F]]-FB";
	//		is.rules = "F:FFF[[+F[-F]+F]-F[-F]+F]";

	//		args[0] = reinterpret_cast<Pointer>(&is);
	//	}
	//	break;

	//case eFT_MidptDisp:
	//	{
	//		m_Fractal = new CMidpointDisplacement("MidpointDisp");

	//		CMidpointDisplacement::InitStruct is;
	//		
	//		is.imgWidth = is.imgHeight = imgSize;

	//		args[0] = reinterpret_cast<Pointer>(&is);
	//	}
	//	break;
	//}
	//
	//if (m_Fractal)
	//	m_Fractal->Initialize(this, &args);

	//CStaticThreadFunctor functor(&CFractalView::ProcessingThread);
	//g_ThreadMgr->CreateThread(functor, m_Fractal, true, &m_Thread/*, CThread::eTPF_Low*/);

	////CProfileSample::ResetAll();

	//if (eType == eFT_MRCM)
	//{
 //		CMRCM* mrcm = static_cast<CMRCM*>(m_Fractal);
	// 
 //		switch (subType)
 //		{
 //		case 0:
 //			mrcm->SetupGasket();
 //			break;
	// 	
 //		case 1:
 //			mrcm->SetupSnowflake5Pt();
 //			break;
	// 
 //		case 2:
 //			mrcm->SetupTwinChristmasTree();
 //			break;
	// 
 //		case 3:
 //			mrcm->SetupFern();
 //			break;
	// 
 //		case 4:
 //			mrcm->SetupKochCurve();
 //			break;
 //		}
	//}

	//if (m_Fractal && bAll >= 2)
 //		m_Fractal->Begin();
}

//////////////////////////////////////////////////////////////////////////

void CFractalView::OnLButtonDown(UINT nFlags, CPoint point)
{
#if 0

	CRect fr;

	fr.SetRect(0, 0, m_Fractal->m_vImageSize.X, m_Fractal->m_vImageSize.Y);

	if (!fr.PtInRect(point))
	{
		CWnd::OnLButtonDown(nFlags, point);
		return;
	}

// 	if ( nFlags & MK_CONTROL)
// 		m_Fractal->SetCenterPoint( point.x, point.y );
// 	else
// 		m_Fractal->SetCenterPoint( point.x, point.y, eZoom_In, ( nFlags & MK_SHIFT ? eZoomSpec_Double : eZoomSpec_None ) );

	//BeginFractal();

	Invalidate(false);

#endif

	CWnd::OnLButtonDown(nFlags, point);
}

//////////////////////////////////////////////////////////////////////////

void CFractalView::OnRButtonDown(UINT nFlags, CPoint point)
{
#if 0
	CRect fr;

	fr.SetRect(0, 0, m_Fractal->m_vImageSize.X, m_Fractal->m_vImageSize.Y);

	if (!fr.PtInRect(point))
	{
		CWnd::OnRButtonDown(nFlags, point);
		return;
	}

	//m_Fractal->SetCenterPoint( point.x, point.y, eZoom_Out, ( nFlags & MK_SHIFT ? eZoomSpec_Double : eZoomSpec_None ) );

	//BeginFractal();

#endif

	CWnd::OnRButtonDown(nFlags, point);
}

//////////////////////////////////////////////////////////////////////////

void CFractalView::OnFractalBegin()
{
// 	if (m_Fractal->m_State != eState_Processing)
// 		BeginFractal();
}

//////////////////////////////////////////////////////////////////////////

void CFractalView::OnFractalTime()
{
}

//////////////////////////////////////////////////////////////////////////

void CFractalView::OnFileSaveas()
{
	CTime t = CTime::GetCurrentTime();

	String ctime	= t.Format("%m%d%y_%H%M%S"),
		fname	= "Mand_" + ctime;

	CFileDialog dlg(false,
		"jpg", fname.c_str(),
		OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_PATHMUSTEXIST,
		"JPEG Files",
		this->GetParent());

	if (dlg.DoModal() == IDOK)
	{
		m_Fractal->m_Image.m_Image.Save( dlg.GetPathName() );
	}
}

//////////////////////////////////////////////////////////////////////////

void CFractalView::OnUpdateFractalBegin(CCmdUI *pCmdUI)
{
	if (m_Fractal->m_State == eState_Processing)
		pCmdUI->SetText("Stop");
	else
		pCmdUI->SetText("Begin");
}

//////////////////////////////////////////////////////////////////////////

void CFractalView::OnMButtonDown(UINT nFlags, CPoint point)
{
#if 0
	CRect fr;

	fr.SetRect(0, 0, m_Fractal->m_vImageSize.X, m_Fractal->m_vImageSize.Y);

	if (!fr.PtInRect(point))
	{
		CWnd::OnRButtonDown(nFlags, point);
		return;
	}

	//((CMainFrame*)(theApp.m_pMainWnd))->m_wndStatusBar->SetWindowText( "Please wait, rendering in progress..." );

	//julia.SetupJulia(point.x, point.y);
	//julia.Initialize(this);
	//julia.Update();

	//( (CMainFrame*) theApp.m_pMainWnd )->m_wndStatusBar->SetWindowText( "Done!" );

	Invalidate(false);
#endif

	CWnd::OnMButtonDown(nFlags, point);
}

//////////////////////////////////////////////////////////////////////////

CPoint lastPoint(-1, -1);

void CFractalView::OnMouseMove(UINT nFlags, CPoint point)
{
	static D3DXVECTOR3 eyePt;


	static const float mx = 0.25f, my = 0.25f;

	if (nFlags & MK_LBUTTON)
	{
		if (lastPoint.x != -1)
		{
			int dx = lastPoint.x - point.x,
				dy = lastPoint.y - point.y;

			g_D3D->RotateCameraX(dy * mx);
			g_D3D->RotateCameraY(dx * mx);
			//g_D3D->RotateWorldY(dx * mx);
			//g_D3D->RotateWorldX(dy * mx);
		}

		lastPoint = point;
	}
	else if (nFlags & MK_RBUTTON)
	{
		if (lastPoint.x != -1)
		{
			int dx = lastPoint.x - point.x,
				dy = lastPoint.y - point.y;

			g_D3D->MoveCameraRight(dx * mx);
			g_D3D->MoveCameraUp(dy * mx);
		}

		lastPoint = point;
	}
	else
	{
		lastPoint = CPoint(-1, -1);
	}


	//if (m_bAutoJulia)
	//{
	//	CRect fr;

	//	fr.SetRect(0, 0, frac.m_vImageSize.X, frac.m_vImageSize.Y);

	//	if (!fr.PtInRect(point))
	//	{
	//		CWnd::OnRButtonDown(nFlags, point);
	//		return;
	//	}

	//	if (julia.m_State != eState_Processing)
	//	{
	//		julia.SetupJulia(point.x, point.y);
	//		julia.Initialize(this);
	//		//julia.Update();

	//		//TODO: Update Julia Window
	//		//((CMainFrame *) GetParent())->m_JuliaDlg.Invalidate(NULL);

	//		//Invalidate(false);
	//	}
	//}

	CWnd::OnMouseMove(nFlags, point);
}

//////////////////////////////////////////////////////////////////////////

void CFractalView::OnFractalAutojulia()
{
	m_bAutoJulia = !m_bAutoJulia;
}

//////////////////////////////////////////////////////////////////////////

void CFractalView::OnUpdateFractalAutojulia(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bAutoJulia);
}

//////////////////////////////////////////////////////////////////////////

void CFractalView::OnSaveSavesecondaryas()
{
	CTime t = CTime::GetCurrentTime();

	String ctime	= t.Format("%m%d%y_%H%M%S"),
		fname	= "Julia_" + ctime;

	CFileDialog dlg(false,
		"jpg", fname.c_str(),
		OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_PATHMUSTEXIST,
		"JPEG Files",
		this->GetParent());

// 	if (dlg.DoModal() == IDOK)
// 	{
// 		julia.m_Image.Save( dlg.GetPathName() );
// 	}
}

//////////////////////////////////////////////////////////////////////////

LRESULT CFractalView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	/*if (message == WM_PAINT)
		return 0;*/

	if (m_Fractal->FSM_GetCurrentStateName() == k_strProcessing)
	{
		Float pctComplete = 0.0f;
		char buffer[80];

		for (UInt32 p = 0; p < m_Fractal->m_Processors.Size(); p++)
			pctComplete += m_Fractal->m_Processors[p]->m_PercentComplete;

		pctComplete /= m_Fractal->m_Processors.Size();

		_gcvt(pctComplete, 4, buffer);
		
		String title = buffer;
		title += "% complete...";

		GetParentFrame()->SetWindowTextA(title.c_str());
	}

	return CWnd::WindowProc(message, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////

DWORD CFractalView::ProcessingThread(LPVOID param)
{
	static const float k_TimeSlice = 1.0f / 8.0f;

	CFractal* pFrac = static_cast<CFractal*>(param);

	///////////////////////////////////////

	char *strBuffer = NULL;

	g_D3D->EnableSpriteDevice();
	g_D3D->SetClearColor(Color::km_DarkGray);

	Float width = 1.0f, height = 1.0f;

	g_FontMgr->LoadFontSet(16);

	UIText debugOutput;
	debugOutput.SetColor(0, 255, 0);
	debugOutput.SetMultiline(true);
	debugOutput.SetScale(k_v2One);
	debugOutput.SetPosition(10.0f, 5.0f);
	debugOutput.SetVisible(true);

	CCameraView cam;
	cam.SetPosition(Vec3(0.5f, 0.5f, -0.5f));
 
	//g_D3D->SetEyePt(Vec3(0.0f, 0.0f, -10.0f));
	//g_D3D->SetLookAt(k_v3Zero);

	//g_D3D->MoveCameraForward(-10.0f);

	g_SpriteMgr->LoadSprite("texturegreen.bmp");

	g_D3D->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	//////////////////////////////////////

	CVectorGraphics vg;
	if (vg.Init() == false)
	{
		::MessageBox(NULL, "Couldn't init vg", "error", MB_OK);
		return -1;
	}

	UInt32 shaderID = g_EffectMgr->LoadEffect("mesh.fx");
	Effect& fx = g_EffectMgr->GetEffect(shaderID);

	D3DMatrix mat;

	//////////////////////////////////////////

	CVertex		origin(k_v3Zero, k_v3Zero, 0.0f, 0.0f, Color::km_White),
				xVert(k_v3XAxis, k_v3Zero, 0.0f, 0.0f, Color::km_Red),
				yVert(k_v3YAxis, k_v3Zero, 0.0f, 0.0f, Color::km_Green),
				zVert(k_v3ZAxis, k_v3Zero, 0.0f, 0.0f, Color::km_Blue);

	vg.PushVertex(origin);
	vg.PushVertex(xVert);
	vg.PushVertex(origin);
	vg.PushVertex(yVert);
	vg.PushVertex(origin);
	vg.PushVertex(zVert);

	//////////////////////////////////////////

	bool bDone = false;
	Timer renderTimer(true);
	Timer inputTimer(true);

	while	(	!theApp.m_bQuit
				//&&	g_D3D->Exists()
				//&&	g_D3D->IsDeviceWindowValid()
			)
	{
		if (pFrac->m_bD3D)
		{
			if (!(g_D3D->Exists() && g_D3D->IsDeviceWindowValid()))
				continue;

			//g_D3D->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

			if (KEYDOWN(VK_INSERT) && inputTimer > 0.25f)
			{
				width *= 0.8f;
				height *= 0.8f;

				g_D3D->SetOrthoDisplay(width, height);

				inputTimer.Reset();
			}
			else if (KEYDOWN(VK_DELETE) && inputTimer > 0.25f)
			{
				width *= 1.25f;
				height *= 1.25f;

				g_D3D->SetOrthoDisplay(width, height);

				inputTimer.Reset();
			}

			g_D3D->SetCamera(cam);
			g_D3D->UpdateCamera();

			g_D3D->BeginScene(true);

			//g_D3D->RotateCameraY(renderTimer.Get() * 500.0f);

			//g_D3D->UpdateCamera();

			//mat = g_D3D->matWorld * g_D3D->matView * g_D3D->matProj;
			//fx.Shader->SetMatrix("matWorldViewProj", &mat);

			//fx.Shader->SetFloat("time", t);
		}

		if (pFrac->Update() != 0)
			bDone = true;

		//if (renderTimer >= 0.125f)
		//{
		//	//pFrac->Draw();
		//	pFrac->m_Window->Invalidate(FALSE);
		//	renderTimer.Reset();
		//}

		if (pFrac->m_bD3D)
		{
			//g_SpriteMgr->DrawSprite(1, 1.5f, 1.5f);
			//g_SpriteMgr->DrawSprite(1, 512, 512);

			//mesh.Render();

			//g_D3D->Clear(Color::km_Blue, true, false);

			//vg.DrawCurrentLineList();
			//vg.DrawCurrentLineStrip();
			//vg.DrawCurrentPointList();
			
			//vg.CreateLine(k_v3Zero, k_v3XAxis);
			//vg.CreateLine(k_v3Zero, k_v3YAxis);
			//vg.CreateLine(k_v3Zero, k_v3ZAxis);

			//g_D3D->EndScene();

			//g_D3D->BeginScene(false, false);
			//
			//CLSystem* pLS = static_cast<CLSystem*>(pFrac);

			//UInt32 len = pLS->m_CurrentString.length(),
			//		totLen = UInt32(len * 1.25f) + 1;
			//UInt32 cur = 0;

			//strBuffer = new char [totLen];
			//
			//for (UInt32 digit = 0; digit < len && cur < totLen; digit++, cur++)
			//{
			//	strBuffer[cur]	= pLS->m_CurrentString[digit];

			//	if (digit % 175 == 174)
			//		strBuffer[cur++] = '\n';
			//}

			//strBuffer[cur] = '\0';

			////debugOutput =	"Alphabet: ";
			////debugOutput +=	pLS->m_Alphabet;
			//debugOutput =	"Current Size: ";
			//debugOutput +=	Int32(pLS->m_CurrentString.length());
			//debugOutput +=	"   String:\n";
			//debugOutput +=	strBuffer;
			//debugOutput.Update();

			//delete [] strBuffer;

			g_D3D->EndScene();
		}

		//renderTimer.Reset();

		/*if (pFrac->m_State == eState_Processing)
			Sleep(2000);*/

#if 0
		if (	pFrac->m_State != eState_Done		&&
				renderTimer >= k_TimeSlice
			)
		{
			//pFrac->m_CS.Lock();

			pFrac->m_Window->Invalidate(false);

			//pFrac->m_CS.Unlock();

			renderTimer.Reset();
		}
#endif
		if (g_ThreadMgr->Exists() && pFrac && bAll >= 1 &&
			static_cast<CFractalView*>(pFrac->m_Window)->m_Thread->NeedsToClose())
		{
			static_cast<CFractalView*>(pFrac->m_Window)->m_Thread->Invalidate();
			return -1;
		}

	}

	if (g_ThreadMgr->Exists() && pFrac && bAll >= 1)
		static_cast<CFractalView*>(pFrac->m_Window)->m_Thread->Invalidate();

	return 0;
}

BOOL CFractalView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	g_D3D->MoveCameraForward(zDelta * 0.005f);

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}
