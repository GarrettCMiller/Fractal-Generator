#include "StdAfx.h"
#include "Processor.h"

#include "Fractal.h"

CProcessor::CProcessor(const char* strName /* = NULL */, Bool bCreateStates /* = true */) : 
	CStateMachine(strName ? strName : "Processor"),
		m_pParent(NULL),
		m_CurrentIteration(0),
		m_PercentComplete(0)
{
	if (bCreateStates)
	{
		TProcessorState	*pUnInitState		= new CUninitializedState(this,	k_strUnInit),
						*pIdleState			= new CIdleState(this,			k_strIdle),
						*pProcState			= new CProcessingState(this,	k_strProcessing),
						//*pRenderState		= new CRenderingState(this,		k_strRender),
						*pCancelledState	= new CCancelledState(this,		k_strCancelled);

		pUnInitState->AddValidTransition(pIdleState);

		pIdleState->AddValidTransition(pProcState);
		//pIdleState->AddValidTransition(pRenderState);

		//pProcState->AddValidTransition(pRenderState);
		pProcState->AddValidTransition(pCancelledState);
		pProcState->AddValidTransition(pIdleState);

		//pRenderState->AddValidTransition(pIdleState);
		//pRenderState->AddValidTransition(pCancelledState);

		pCancelledState->AddValidTransition(pIdleState);

		FSM_Initialize(pUnInitState);
	}
}

CProcessor::~CProcessor(void)
{
}

//////////////////////////////////////////////////////////////////////////

Int32 CProcessor::Init(CFractal& _parent, TArgumentArray* pArgs /* = NULL */)
{
	//We won't take a pointer to a fractal, but a
	//reference so that NULL can never passed in
	m_pParent = &_parent;

	m_Functions = m_pParent->m_Functions;

	//Int32 ret = FSM_QueueState(k_strProcessing);

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CProcessor::Update(TArgumentArray* pArgs /* = NULL */)
{
	Int32 ret = 0;

	if (FSM_GetCurrentStateName() == k_strCancelled)
		return -1;

	if (m_pParent->FSM_GetCurrentStateName() == k_strCancelled)
		ret = FSM_QueueState(k_strCancelled);

	ret = FSM_Update();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CProcessor::Shutdown(TArgumentArray* pArgs /* = NULL */)
{
	Int32 ret = FSM_Shutdown();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CProcessor::Begin()
{
	FSM_QueueState(k_strProcessing);

	return FSM_Update();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Int32 CProcessor::FSM_Initialize(IFSMStateBase* pStartState)
{
	return __super::FSM_Initialize(pStartState);
}

Int32 CProcessor::FSM_Update()
{
	return __super::FSM_Update();
}

Int32 CProcessor::FSM_Shutdown()
{
	return __super::FSM_Shutdown();
}

Int32 CProcessor::FSM_SendMessage(TArgumentArray& args)
{
	return __super::FSM_SendMessage(args);
}

//////////////////////////////////////////////////////////////////////////

#define CAST_STATE_PARENT		CProcessor* pProc = static_cast<CProcessor*>(m_pFSM)

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CProcessor::CUninitializedState::CUninitializedState(CProcessor* pFSM, const String& strName)
: TProcessorState(pFSM, strName)
{
}

Int32 CProcessor::CUninitializedState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CProcessor::CUninitializedState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CProcessor::CUninitializedState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CProcessor::CUninitializedState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CProcessor::CIdleState::CIdleState(CProcessor* pFSM, const String& strName)
: TProcessorState(pFSM, strName)
{

}

Int32 CProcessor::CIdleState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CProcessor::CIdleState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CProcessor::CIdleState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CProcessor::CIdleState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CProcessor::CProcessingState::CProcessingState(CProcessor* pFSM, const String& strName) 
: TProcessorState(pFSM, strName)
{

}

Int32 CProcessor::CProcessingState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	CAST_STATE_PARENT;

	if (pProc->m_pThread->Start() <= 0)
		return -1;

	return __super::OnEnter();
}

Int32 CProcessor::CProcessingState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CProcessor::CProcessingState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CProcessor::CProcessingState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

// CProcessor::CRenderingState::CRenderingState(CProcessor* pFSM, const String& strName)
// : TProcessorState(pFSM, strName)
// {
// 
// }
// 
// Int32 CProcessor::CRenderingState::OnEnter(TArgumentArray* pArgs /* = NULL */)
// {
// 	return __super::OnEnter(pArgs);
// }
// 
// Int32 CProcessor::CRenderingState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
// {
// 	return __super::OnUpdate(pArgs);
// }
// 
// Int32 CProcessor::CRenderingState::OnExit(TArgumentArray* pArgs /* = NULL */)
// {
// 	return __super::OnExit(pArgs);
// }
// 
// Int32 CProcessor::CRenderingState::OnMessage(TArgumentArray* pArgs /* = NULL */)
// {
// 	return __super::OnMessage(pArgs);
// }

//////////////////////////////////////////////////////////////////////////

CProcessor::CCancelledState::CCancelledState(CProcessor* pFSM, const String& strName)
: TProcessorState(pFSM, strName)
{

}

Int32 CProcessor::CCancelledState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CProcessor::CCancelledState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CProcessor::CCancelledState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CProcessor::CCancelledState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

#undef CAST_STATE_PARENT