#include "StdAfx.h"
#include "MDProcessor.h"

//Make it static so that it persists and doesn't have to be reset every call
#define CAST_PARENT				static CMidpointDisplacement* pParent = static_cast<CMidpointDisplacement*>(m_pParent)

//////////////////////////////////////////////////////////////////////////

CMDProcessor::CMDProcessor(const char* strName /* = NULL */, Bool bCreateStates /* = true */) :
	CProcessor(strName, false)
{
	if (bCreateStates)
	{
		TMDProcessorState	*pUnInitState		= new CUninitializedState(this,	k_strUnInit),
							*pIdleState			= new CIdleState(this,			k_strIdle),
							*pProcState			= new CProcessingState(this,	k_strProcessing),
							//*pRenderState		= new CRenderingState(this,		k_strRender),
							*pCancelledState	= new CCancelledState(this,		k_strCancelled);

		pUnInitState->AddValidTransition(pIdleState);

		pIdleState->AddValidTransition(pProcState);
		//pIdleState->AddValidTransition(pRenderState);

		//pProcState->AddValidTransition(pRenderState);
		pProcState->AddValidTransition(pIdleState);
		pProcState->AddValidTransition(pCancelledState);

		//pRenderState->AddValidTransition(pIdleState);
		//pRenderState->AddValidTransition(pCancelledState);

		pCancelledState->AddValidTransition(pIdleState);

		FSM_Initialize(pUnInitState);
	}
}

CMDProcessor::~CMDProcessor(void)
{
}

//////////////////////////////////////////////////////////////////////////

Int32 CMDProcessor::Init(CFractal& _parent, TArgumentArray* pArgs /* = NULL */)
{
	CProcessor::Init(_parent, pArgs);

	ASSERT(pArgs);

	CAST_PARENT;

	TArgumentArray& args = *pArgs;
	
	Pointer pRect = args[0];

	Rectd vport = *reinterpret_cast<Rectd*>(pRect);

	m_Viewport = vport;

	m_Roughness = args[1];
	m_RoughnessMultiplier = args[2];
	m_RandomRangeLow = args[3];
	m_RandomRangeHigh = args[4];

	m_LineCount = static_cast<UInt32>(pow(2.0f, m_pParent->m_nMaxIterations));

	m_VertexCount = m_LineCount + 1;

	m_CurrentIteration = 1;

	m_Data.Resize(m_VertexCount, 1, false);

	m_ParentVal = 0.5f;

	//SAFE_CALL(m_Functions[eFF_Init], this);

	FSM_QueueState(k_strIdle);
	FSM_Update();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CMDProcessor::Update(TArgumentArray* pArgs /* = NULL */)
{
	if (CProcessor::Update() == -1)
		return -1;

	Generate(m_Viewport, CMidpointDisplacement::eSide_Root, 0.5);

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CMDProcessor::Shutdown(TArgumentArray* pArgs /* = NULL */)
{
	CProcessor::Shutdown(pArgs);

	return 0;
}

//////////////////////////////////////////////////////////////////////////

void CMDProcessor::Generate(GLib::Rectd area, CMidpointDisplacement::ESide side, Double parentVal)
{
	// 	if (Distance<double>(p1, p2) < 1.0)
	// 		return;

	CAST_PARENT;

	Vec2d p1 = area.Left();
	Vec2d p2 = area.Right();

	Vec2d mid = p1.Midpoint(p2);

	Double disp = 0.0;

	Bool bGenerateThreads = false;

	if (pParent->Randomize())
		disp = RandomizeMidpoint(p1, p2);

	mid.Y += disp;

	//ASSERT(mid.Y < 1.0);
	// 	if (mid.Y > 1.0)
	// 		mid.Y = 1.0;

	Double index = 0;

	//Get the lowest common denominator for the fractional math
	Double LCD = 1.0 / (pow(2.0, m_CurrentIteration));

	switch (side)
	{
	case CMidpointDisplacement::eSide_Left:
		index = parentVal - LCD;
		break;

	case CMidpointDisplacement::eSide_Right:
		index = parentVal + LCD;
		break;

	case CMidpointDisplacement::eSide_Root:
		index = 0.5;
		bGenerateThreads = true;
		break;
	}

	UInt32 dataIndex = static_cast<UInt32>(index * m_Data.Size());

	//ASSERT(m_Data[dataIndex] == 0.0);
	m_Data[dataIndex] = disp;
	Double newParentVal = index;

	if (m_CurrentIteration >= m_pParent->m_nMaxIterations)
		return;

	m_ParentVal = parentVal;

	Float	roughness = m_Roughness;
	Float	temprangeupper = m_RandomRangeHigh,
		temprangelower = m_RandomRangeLow;

	Int32 tempitercount = ++m_CurrentIteration;
	Generate(Rectd(p1, mid), CMidpointDisplacement::eSide_Left, newParentVal);

	m_Roughness = roughness;
	m_RandomRangeHigh = temprangeupper;
	m_RandomRangeLow = temprangelower;

	m_CurrentIteration = tempitercount;
	Generate(Rectd(mid, p2), CMidpointDisplacement::eSide_Right, newParentVal);

	m_Roughness = roughness;
	m_RandomRangeHigh = temprangeupper;
	m_RandomRangeLow = temprangelower;

	//m_pParent->MeshThreadData(m_ProcID);

	if (side == CMidpointDisplacement::eSide_Root)
	{
		FSM_QueueState(k_strIdle);
		FSM_Update();
	}
}

//////////////////////////////////////////////////////////////////////////

Double CMDProcessor::RandomizeMidpoint(Vec2d p1, Vec2d p2)
{
	CAST_PARENT;

	//Decrease roughness
	m_Roughness *= m_RoughnessMultiplier;

	m_RandomRangeLow	*= m_Roughness;
	m_RandomRangeHigh	*= m_Roughness;

	Double range = (pParent->Randomize() ? Random(m_RandomRangeHigh, m_RandomRangeLow) : 0.0);

	////////////////////////////////////////////////////////////////
	////
	////	TODO!!!!:	Find a way so that different iterations
	////				have the same vertices all across (i.e.
	////				the 10th iteration looks like the 3rd,
	////				just with the inbetweens different
	////////////////////////////////////////////////////////////////

	return range;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Int32 CMDProcessor::FSM_Initialize(IFSMStateBase* pStartState)
{
	return __super::FSM_Initialize(pStartState);
}

Int32 CMDProcessor::FSM_Update()
{
	return __super::FSM_Update();
}

Int32 CMDProcessor::FSM_Shutdown()
{
	return __super::FSM_Shutdown();
}

Int32 CMDProcessor::FSM_SendMessage(TArgumentArray& args)
{
	return __super::FSM_SendMessage(args);
}

//////////////////////////////////////////////////////////////////////////

#define CAST_STATE_PARENT		CMDProcessor* pProc = static_cast<CMDProcessor*>(m_pFSM)

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CMDProcessor::CUninitializedState::CUninitializedState(CMDProcessor* pFSM, const String& strName)
: TMDProcessorState(pFSM, strName)
{
}

Int32 CMDProcessor::CUninitializedState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CMDProcessor::CUninitializedState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CMDProcessor::CUninitializedState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CMDProcessor::CUninitializedState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CMDProcessor::CIdleState::CIdleState(CMDProcessor* pFSM, const String& strName)
: TMDProcessorState(pFSM, strName)
{

}

Int32 CMDProcessor::CIdleState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CMDProcessor::CIdleState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CMDProcessor::CIdleState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CMDProcessor::CIdleState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CMDProcessor::CProcessingState::CProcessingState(CMDProcessor* pFSM, const String& strName) 
: TMDProcessorState(pFSM, strName)
{

}

Int32 CMDProcessor::CProcessingState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	CAST_STATE_PARENT;

	if (pProc->m_pThread->Start() <= 0)
		return -1;

	return __super::OnEnter(pArgs);
}

Int32 CMDProcessor::CProcessingState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CMDProcessor::CProcessingState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CMDProcessor::CProcessingState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

// CMDProcessor::CRenderingState::CRenderingState(CMDProcessor* pFSM, const String& strName)
// : TMDProcessorState(pFSM, strName)
// {
// 
// }
// 
// Int32 CMDProcessor::CRenderingState::OnEnter(TArgumentArray* pArgs /* = NULL */)
// {
// 	return __super::OnEnter(pArgs);
// }
// 
// Int32 CMDProcessor::CRenderingState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
// {
// 	return __super::OnUpdate(pArgs);
// }
// 
// Int32 CMDProcessor::CRenderingState::OnExit(TArgumentArray* pArgs /* = NULL */)
// {
// 	return __super::OnExit(pArgs);
// }
// 
// Int32 CMDProcessor::CRenderingState::OnMessage(TArgumentArray* pArgs /* = NULL */)
// {
// 	return __super::OnMessage(pArgs);
// }

//////////////////////////////////////////////////////////////////////////

CMDProcessor::CCancelledState::CCancelledState(CMDProcessor* pFSM, const String& strName)
: TMDProcessorState(pFSM, strName)
{

}

Int32 CMDProcessor::CCancelledState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CMDProcessor::CCancelledState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CMDProcessor::CCancelledState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CMDProcessor::CCancelledState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

#undef CAST_STATE_PARENT