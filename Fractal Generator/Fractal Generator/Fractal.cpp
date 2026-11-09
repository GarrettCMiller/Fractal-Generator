#include "StdAfx.h"
#include ".\fractal.h"
#include "math.h"

#include "Fractal Generator.h"

#include "ProfileSample.h"

//#include "ParticleEmitter.h"
 
//CParticleEmitter pe;

//////////////////////////////////////////////////////////////////////////

using namespace GLib;

const String k_strUnInit		= "uninitialized";
const String k_strIdle			= "idle";
const String k_strProcessing	= "processing";
const String k_strRender		= "render";
const String k_strCancelled		= "cancelled";


//////////////////////////////////////////////////////////////////////////

CFractal::CFractal(const char* strName /* = NULL */, Bool bCreateStates /* = true */) :
	CStateMachine(strName ? strName : "Fractal"),
		m_bPreview(true),
		m_bValid(false),
		m_State(eState_Uninitialized),
		m_nMaxIterations(0),
		m_Window(NULL),
		m_nThreadCount(4),
		m_bD3D(false),
		m_eType(eFT_Invalid),
		m_pGenAlg(NULL)
{
	/*if (bCreateStates)
	{
		TFractalState	*pUnInitState		= new CUninitializedState(this,	k_strUnInit),
						*pIdleState			= new CIdleState(this,			k_strIdle),
						*pProcState			= new CProcessingState(this,	k_strProcessing),
						*pRenderState		= new CRenderingState(this,		k_strRender),
						*pCancelledState	= new CCancelledState(this,		k_strCancelled);

		pUnInitState->AddValidTransition(pIdleState);

		pIdleState->AddValidTransition(pProcState);
		pIdleState->AddValidTransition(pRenderState);

		pProcState->AddValidTransition(pRenderState);
		pProcState->AddValidTransition(pCancelledState);

		pRenderState->AddValidTransition(pIdleState);
		pRenderState->AddValidTransition(pCancelledState);

		pCancelledState->AddValidTransition(pIdleState);

		FSM_Initialize(pUnInitState);
	}*/
}

//////////////////////////////////////////////////////////////////////////

CFractal::~CFractal(void)
{
	m_Processors.FreePointers();
	//m_Image.
}

//////////////////////////////////////////////////////////////////////////

Int32 CFractal::InitializeImage(UInt32 nWidth /* = 0 */, UInt32 nHeight /* = 0 */)
{
	if (nWidth == 0)
		nWidth = 1024;
	if (nHeight == 0)
		nHeight = nWidth;

	if (m_Image.Init(*this, Vec2u(nWidth, nHeight)) != 0)
		return -1;

	m_vImageSize = Vec2u(nWidth, nHeight);

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CFractal::Initialize(CWnd* parentFrame /* = NULL */, TArgumentArray* pArgs /* = NULL */)
{
	ASSERT_STATE(k_strUnInit);
	ASSERT(pArgs);
	ASSERT(parentFrame);

	m_Window			= parentFrame;	
	m_nMaxIterations	= 1024;

	TArgumentArray& args = *pArgs;
	Pointer pInitStruct = args[0];

	InitStruct& is = *(reinterpret_cast<InitStruct*>(pInitStruct));

	UInt32	nWidth = is.imgWidth,
			nHeight = is.imgHeight;

	if (nWidth == 0)
		nWidth = 1024;
	if (nHeight == 0)
		nHeight = nWidth;

	InitializeImage(nWidth, nHeight);

	CreatePalette();

	m_ProcessTimer.Reset();

	m_vSize = Vec2d(m_Viewport.Width(), m_Viewport.Height());

	//SetupProcessors();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CFractal::Begin()
{
	Int32 ret = 0;

	if (m_Processors.Size() > 0)
	{
		//Then fire all the threads
		for (UInt32 procID = 0; procID < m_nThreadCount; procID++)
		{
			if ((ret = m_Processors[procID]->Begin()) < 0)
				return -1;
		}
	}

	FSM_QueueState(k_strProcessing);
	FSM_Update();

	return ret;
}

//////////////////////////////////////////////////////////////////////////

Int32 CFractal::Stop()
{
	ASSERT_STATE(k_strProcessing);

	FSM_QueueState(k_strCancelled);

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CFractal::Reset()
{
	ASSERT_STATE(k_strProcessing);

	FSM_QueueState(k_strCancelled);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Bool CFractal::SetupProcessors()
{
	//Clear out old threads if any
	//TODO: Make GLib thread family class
	g_ThreadMgr->KillAll();

	//TODO: ?
	//FSM_QueueState(k_strProcessing);
	
	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

DWORD CFractal::ProcessingThread(LPVOID param)
{
	ProcessingData* data = static_cast<ProcessingData *>(param);

	CProcessor& proc = *(data->m_parent->m_Processors[data->m_procID]);

	while (theApp.m_bQuit != true)
	{
		if (proc.Update() != 0)
			break;
	}

	//Make sure to tell the thread that it has exited
	proc.m_pThread->Invalidate();

	delete data;

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Int32 CFractal::Update()
{
	Int32 ret = FSM_Update();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CFractal::Draw()
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CFractal::Render(CPaintDC& dc, UInt32 x, UInt32 y, UInt32 cx, UInt32 cy)
{
	m_CS.Lock();

	CDC* cdc = CDC::FromHandle(m_Image.m_Image.GetDC());

	//dc.BitBlt(x, y, (cx ? cx : m_vImageSize.X), (cy ? cy : m_vImageSize.Y), cdc, 0, 0, SRCCOPY);
	dc.StretchBlt(x, y, (cx ? cx : m_vImageSize.X), (cy ? cy : m_vImageSize.Y), cdc, 0, 0, m_vImageSize.X, m_vImageSize.Y, SRCCOPY);

	m_Image.m_Image.ReleaseDC();

	m_CS.Unlock();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CFractal::AutoRender()
{
 	CPaintDC dc(m_Window);
 	CDC* cdc = CDC::FromHandle(m_Image.m_Image.GetDC());

	dc.BitBlt(0, 0, m_vImageSize.X, m_vImageSize.Y, cdc, 0, 0, SRCCOPY);

	m_Image.m_Image.ReleaseDC();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CFractal::CreatePalette()
{
	m_Image.InitPalette(/*m_nMaxIterations*/1024);

	float	r = 0.0f,
			g = 0.0f,
			b = 0.0f;

	for (int i = 0; i < 1024; i++)
	{
		//if (m_bRed)
			r = Sine(25.0f, 0.5f, 0.5f, fabsf(float(i + 0) / 1024));

		//if (m_bGreen)
			g = Sine(25.0f, 0.5f, 0.5f, fabsf(float(i + 85) / 1024));

		//if (m_bBlue)
			b = Sine(25.0f, 0.5f, 0.5f, fabsf(float(i + 170) / 1024));

		m_Image.GetPalette()[i]	= Color(r, g, b);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////

void CFractal::SetPalette(const CDynamicArray<Color>& pal, bool bRedraw)
{
	m_Image.m_Palette.Copy(pal, m_nMaxIterations);

	if ( bRedraw )
		Draw();
}

//////////////////////////////////////////////////////////////////////////

void CFractal::SetCenterPoint( UInt32 /*x*/, UInt32 /*y*/, EZoomMode /*mode*/, EZoomSpecial /*spec*/ )
{
// 	Double	mx = x * m_dX.real() + m_Viewport.tlx,
// 			my = y * m_dY.imag() + m_Viewport.tly;
// 
// 	SetCenterPoint(mx, my, mode, spec);
}

//////////////////////////////////////////////////////////////////////////

void CFractal::SetCenterPoint(Double /*x*/, Double /*y*/, EZoomMode /*mode*/, EZoomSpecial /*spec*/ )
{
//  	m_vCenter = Vec2d(x, y);
//  
//  	Recenter(mode, spec);
}

//////////////////////////////////////////////////////////////////////////

void CFractal::Recenter( EZoomMode mode, EZoomSpecial spec )
{
	double	fFactor = 1.0;

	switch (mode)
	{
	case eZoom_In:
		fFactor = 0.5 * (spec == eZoomSpec_Double ? 0.5 : 1.0);
		break;

	case eZoom_Out:
		fFactor = 2.0 * (spec == eZoomSpec_Double ? 2.0 : 1.0);
		break;
	}

	double	cx = m_Viewport.Width(),
			cy = m_Viewport.Height();

// 	m_Viewport.tlx = m_vCenter.X - cx * fFactor * 0.5;
// 	m_Viewport.tly = m_vCenter.Y + cy * fFactor * 0.5;
// 	m_Viewport.brx = m_vCenter.X + cx * fFactor * 0.5;
// 	m_Viewport.bry = m_vCenter.Y - cy * fFactor * 0.5;
}

//////////////////////////////////////////////////////////////////////////

void CFractal::MeshThreadData(Int32 threadNum /* = -1 */)
{
	m_CS.Lock();

	DWORD arrSize = m_Data.Size() / m_nThreadCount;

	if (threadNum == -1)
	{
		for (UInt32 t = 0; t < m_Processors.Size(); t++)
			memcpy(&m_Data[t * arrSize], &m_Processors[t]->m_Data[0], arrSize);
	}
	else
	{
		void* src = &m_Processors[threadNum]->m_Data[0],
				*dst = &m_Data[threadNum * arrSize];

		memcpy(dst, src, arrSize);
	}

	m_CS.Unlock();
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Int32 CFractal::FSM_Initialize(IFSMStateBase* pStartState)
{
	return __super::FSM_Initialize(pStartState);
}

Int32 CFractal::FSM_Update()
{
	return __super::FSM_Update();
}

Int32 CFractal::FSM_Shutdown()
{
	return __super::FSM_Shutdown();
}

Int32 CFractal::FSM_SendMessage(TArgumentArray& args)
{
	return __super::FSM_SendMessage(args);
}

//////////////////////////////////////////////////////////////////////////

#define CAST_STATE_PARENT		CFractal* pFrac = static_cast<CFractal*>(m_pFSM)

//////////////////////////////////////////////////////////////////////////

CFractal::CUninitializedState::CUninitializedState(CFractal* pFSM, const String& strName)
	: TFractalState(pFSM, strName)
{
}

Int32 CFractal::CUninitializedState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CFractal::CUninitializedState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CFractal::CUninitializedState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	CAST_STATE_PARENT;

	pFrac->m_Viewport =	//Rectd(-2, 1.5, 2.8, -1.5);
		Rectd(-1.5, 2, 2.5, -2);
	//Rectd(-1, 2, 3, -2);

	pFrac->m_Data.Resize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
	pFrac->m_Functions.Resize(eFF_NumFunctions);
	pFrac->m_Processors.Resize(m_MaxThreads);

	pFrac->CreatePalette();

	return __super::OnExit(pArgs);
}

Int32 CFractal::CUninitializedState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CFractal::CIdleState::CIdleState(CFractal* pFSM, const String& strName)
	: TFractalState(pFSM, strName)
{

}

Int32 CFractal::CIdleState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CFractal::CIdleState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CFractal::CIdleState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CFractal::CIdleState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CFractal::CProcessingState::CProcessingState(CFractal* pFSM, const String& strName) 
	: TFractalState(pFSM, strName)
{

}

Int32 CFractal::CProcessingState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	CAST_STATE_PARENT;
	
	pFrac->Begin();

	return 1;
}

Int32 CFractal::CProcessingState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	CAST_STATE_PARENT;

	bool bDone = true;
	CProcessor* pCurrProc = NULL;

	for (UInt32 i = 0; i < pFrac->m_Processors.NumItems(); i++)
	{
		pCurrProc = pFrac->m_Processors[i];

		if (pCurrProc != NULL && 
			pCurrProc->FSM_GetCurrentState()->GetName() == k_strIdle)
			bDone = false;
	}

	if (bDone)
		pFrac->FSM_QueueState(k_strRender);

	return __super::OnUpdate(pArgs);
}

Int32 CFractal::CProcessingState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CFractal::CProcessingState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CFractal::CRenderingState::CRenderingState(CFractal* pFSM, const String& strName)
	: TFractalState(pFSM, strName)
{

}

Int32 CFractal::CRenderingState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	CAST_STATE_PARENT;

	pFrac->MeshThreadData();

	return __super::OnEnter(pArgs);
}

Int32 CFractal::CRenderingState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CFractal::CRenderingState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CFractal::CRenderingState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CFractal::CCancelledState::CCancelledState(CFractal* pFSM, const String& strName)
	: TFractalState(pFSM, strName)
{

}

Int32 CFractal::CCancelledState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CFractal::CCancelledState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	Sleep(250);

	CAST_STATE_PARENT;

	pFrac->FSM_QueueState("idle");

	return __super::OnUpdate(pArgs);
}

Int32 CFractal::CCancelledState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CFractal::CCancelledState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#undef CAST_STATE_PARENT

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
