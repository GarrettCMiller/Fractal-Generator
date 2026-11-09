#include "StdAfx.h"
#include "MidpointDisplacement.h"

#include "MDProcessor.h"

#include "MiscMath.h"

#include "Fractal Generator.h"

using namespace GLib;

CMidpointDisplacement::CMidpointDisplacement(const char* strName /* = NULL */, Bool bCreateStates /* = true */) : 
	CFractal(strName, false), 
		m_bRandomize(true)
{
	if (bCreateStates)
	{
		TMDState	*pUnInitState		= new CUninitializedState(this,	k_strUnInit),
					*pIdleState			= new CIdleState(this,			k_strIdle),
					*pProcState			= new CProcessingState(this,	k_strProcessing),
					*pRenderState		= new CRenderingState(this,		k_strRender),
					*pCancelledState	= new CCancelledState(this,		k_strCancelled);

		pUnInitState->AddValidTransition(pIdleState);

		pIdleState->AddValidTransition(pProcState);
		pIdleState->AddValidTransition(pRenderState);

		pProcState->AddValidTransition(pIdleState);
		pProcState->AddValidTransition(pRenderState);
		pProcState->AddValidTransition(pCancelledState);

		pRenderState->AddValidTransition(pIdleState);
		pRenderState->AddValidTransition(pCancelledState);

		pCancelledState->AddValidTransition(pIdleState);

		FSM_Initialize(pUnInitState);
	}
}

CMidpointDisplacement::~CMidpointDisplacement(void)
{
}

//////////////////////////////////////////////////////////////////////////

Int32 CMidpointDisplacement::Initialize(CWnd* parentFrame /* = NULL */, TArgumentArray* pArgs /* = NULL */)
{
	CFractal::Initialize(parentFrame, pArgs);

	TArgumentArray& args	= *pArgs;
	Pointer pInitStruct		= args[0];

	InitStruct& is			= *(reinterpret_cast<InitStruct*>(pInitStruct));

	Int32 seed				= is.seed;
	Float roughness			= is.roughness;
	Float roughnessMult		= is.roughMult;
	Float lowerRange		= is.lowerRange;
	Float upperRange		= is.upperRange;

	m_nMaxIterations		= 2;

	Seed(seed);

	m_Viewport				= Rectd(0, -1, 1, 1);
	
	m_Roughness				= roughness == 0 ? DEFAULT_ROUGHNESS : roughness;
	m_RoughnessMultiplier	= roughnessMult == 0 ? DEFAULT_ROUGHNESS_MULTIPLIER : roughnessMult;

	m_RandomRangeHigh		= upperRange == 0 ? DEFAULT_UPPER_RANGE : upperRange;
	m_RandomRangeLow		= lowerRange == 0 ? DEFAULT_LOWER_RANGE : lowerRange;
	
	m_nThreadCount			= 1;

	SetupProcessors();

	//This fractal is now ready
	m_bValid				= true;

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CMidpointDisplacement::Render(CPaintDC& dc, 
								   UInt32 x /* = 0 */, UInt32 y /* = 0 */, 
								   UInt32 cx /* = 0 */, UInt32 cy /* = 0 */)
{
	static const COLORREF	k_PtColor = 0x0000FF,
							k_FGColor = 0xFFFFFF,
							k_BGColor = 0x555555;

	CBrush brush(k_BGColor);
	CPen pen(PS_SOLID, 1, k_FGColor);

	Vec2d p1 = m_Viewport.Left();
	Vec2d p2 = m_Viewport.Right();
	Vec2d p3;

	p1 *= m_vImageSize;
	p2 *= m_vImageSize;

	p1.Y += m_vImageSize.Y / 2;
	p2.Y += m_vImageSize.Y / 2;

	p3.Y = p1.Y;

	Float dx = static_cast<float>(m_vImageSize.X) / m_Data.Size();

	dc.SelectObject(&pen);

	//if (m_nMaxIterations <= 11)
	{
		dc.MoveTo(p1);

		for (UInt32 i = 1; i < m_Data.Size(); i++)
		{
			p3 = Vec2d(i * dx, p3.Y + -m_Data[i] * m_vImageSize.Y * 0.125);
			dc.LineTo(p3);
		}

		dc.LineTo(p2);
		dc.LineTo(p1);
	}

 	for (UInt32 i = 0; i < m_Data.Size(); i++)
	{
		p3 = Vec2i(i * dx, p3.Y + -m_Data[i] * m_vImageSize.Y * 0.125);
		dc.SetPixelV(p3, k_PtColor);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Bool CMidpointDisplacement::SetupProcessors()
{
	CFractal::SetupProcessors();

	CMDProcessor* proc = NULL;

	Float sectionWidth = m_Viewport.Width() / m_nThreadCount;

	Var	vport;
	
	Var roughness, roughmult, randomlow, randomhigh;

	Rectd viewport;
	TArgumentArray args(5);

	CStaticThreadFunctor functor(&CMidpointDisplacement::ProcessingThread);
	ProcessingData* data = NULL;

	for (UInt32 threadID = 0; threadID < m_nThreadCount; threadID++)
	{
		proc = new CMDProcessor;

		data = new ProcessingData;

		data->m_parent = this;
		data->m_procID = proc->m_ProcID = threadID;

		g_ThreadMgr->CreateThread(functor, data, false, &proc->m_pThread, CThread::eTPF_Low);

		viewport.tlx = sectionWidth * threadID;
		viewport.tly = m_Viewport.tly;

		viewport.brx = viewport.tlx + sectionWidth;
		viewport.bry = m_Viewport.bry;

		vport = reinterpret_cast<Pointer>(&viewport);

		roughness = m_Roughness;
		roughmult = m_RoughnessMultiplier;
		randomhigh = m_RandomRangeHigh;
		randomlow = m_RandomRangeLow;

		args[0] = vport;
		args[1] = roughness;
		args[2] = roughmult;
		args[3] = randomlow;
		args[4] = randomhigh;

		proc->Init(*this, &args);

		m_Processors.Push(proc);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CMidpointDisplacement::Update()
{
	return CFractal::Update();
}

//////////////////////////////////////////////////////////////////////////

Int32 CMidpointDisplacement::Begin()
{
	Int32 ret = 0;

	FSM_QueueState(k_strProcessing);
	FSM_Update();

	//Then fire all the threads
	for (UInt32 procID = 0; procID < m_nThreadCount; procID++)
	{
		if ((ret = m_Processors[procID]->Begin()) < 0)
			return -1;
	}
}

//////////////////////////////////////////////////////////////////////////

DWORD CMidpointDisplacement::ProcessingThread(LPVOID param)
{
	ProcessingData* data = static_cast<ProcessingData*>(param);

	TProcessorArray& pProcArray = data->m_parent->m_Processors;
	UInt32 pID = data->m_procID;

	CMDProcessor* proc = static_cast<CMDProcessor*>(pProcArray[pID]);

	while (theApp.m_bQuit != true)
	{
		if (proc->Update() != 0)
			break;
	}

	//Make sure to tell the thread that it has exited
	proc->m_pThread->Invalidate();

	delete data;

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Int32 CMidpointDisplacement::FSM_Initialize(IFSMStateBase* pStartState)
{
	return __super::FSM_Initialize(pStartState);
}

Int32 CMidpointDisplacement::FSM_Update()
{
	return __super::FSM_Update();
}

Int32 CMidpointDisplacement::FSM_Shutdown()
{
	return __super::FSM_Shutdown();
}

Int32 CMidpointDisplacement::FSM_SendMessage(TArgumentArray& args)
{
	return __super::FSM_SendMessage(args);
}

//////////////////////////////////////////////////////////////////////////

#define CAST_STATE_PARENT		CMidpointDisplacement* pFrac = static_cast<CMidpointDisplacement*>(m_pFSM)

//////////////////////////////////////////////////////////////////////////

CMidpointDisplacement::CUninitializedState::CUninitializedState(CMidpointDisplacement* pFSM, const String& strName)
: TMDState(pFSM, strName)
{
}

Int32 CMidpointDisplacement::CUninitializedState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CMidpointDisplacement::CUninitializedState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CMidpointDisplacement::CUninitializedState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CMidpointDisplacement::CUninitializedState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CMidpointDisplacement::CIdleState::CIdleState(CMidpointDisplacement* pFSM, const String& strName)
: TMDState(pFSM, strName)
{

}

Int32 CMidpointDisplacement::CIdleState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CMidpointDisplacement::CIdleState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CMidpointDisplacement::CIdleState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CMidpointDisplacement::CIdleState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CMidpointDisplacement::CProcessingState::CProcessingState(CMidpointDisplacement* pFSM, const String& strName) 
: TMDState(pFSM, strName)
{

}

Int32 CMidpointDisplacement::CProcessingState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	CAST_STATE_PARENT;

	pFrac->Begin();

	return 1;
}

Int32 CMidpointDisplacement::CProcessingState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
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

Int32 CMidpointDisplacement::CProcessingState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CMidpointDisplacement::CProcessingState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CMidpointDisplacement::CRenderingState::CRenderingState(CMidpointDisplacement* pFSM, const String& strName)
: TMDState(pFSM, strName)
{

}

Int32 CMidpointDisplacement::CRenderingState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	CAST_STATE_PARENT;

	pFrac->MeshThreadData();

	return __super::OnEnter(pArgs);
}

Int32 CMidpointDisplacement::CRenderingState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CMidpointDisplacement::CRenderingState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CMidpointDisplacement::CRenderingState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CMidpointDisplacement::CCancelledState::CCancelledState(CMidpointDisplacement* pFSM, const String& strName)
: TMDState(pFSM, strName)
{

}

Int32 CMidpointDisplacement::CCancelledState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CMidpointDisplacement::CCancelledState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	Sleep(250);

	CAST_STATE_PARENT;

	pFrac->FSM_QueueState(k_strIdle);

	return __super::OnUpdate(pArgs);
}

Int32 CMidpointDisplacement::CCancelledState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CMidpointDisplacement::CCancelledState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#undef CAST_STATE_PARENT

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////