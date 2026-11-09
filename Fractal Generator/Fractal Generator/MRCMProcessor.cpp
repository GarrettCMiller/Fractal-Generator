#include "StdAfx.h"
#include "MRCMProcessor.h"
#include "MRCM.h"

//Make it static so that it persists and doesn't have to be reset every call
#define CAST_PARENT				static CMRCM* pParent = static_cast<CMRCM*>(m_pParent)
//////////////////////////////////////////////////////////////////////////

CMRCMProcessor::CMRCMProcessor(const char* strName /* = NULL */, Bool bCreateStates /* = true */) :
	CProcessor(strName, false)
{
	if (bCreateStates)
	{
		TMRCMProcessorState	*pUnInitState		= new CUninitializedState(this,	k_strUnInit),
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

CMRCMProcessor::~CMRCMProcessor(void)
{
}


//////////////////////////////////////////////////////////////////////////

Int32 CMRCMProcessor::Init(CFractal& _parent, TArgumentArray* pArgs /* = NULL */)
{
	CProcessor::Init(_parent, pArgs);

	ASSERT(pArgs);

	CAST_PARENT;

	TArgumentArray& args = *pArgs;

	FSM_QueueState(k_strIdle);
	FSM_Update();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CMRCMProcessor::Update(TArgumentArray* pArgs /* = NULL */)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CMRCMProcessor::Shutdown(TArgumentArray* pArgs /* = NULL */)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CMRCMProcessor::Begin()
{
	Int32 ret = CProcessor::Begin();

	return ret;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Int32 CMRCMProcessor::FSM_Initialize(IFSMStateBase* pStartState)
{
	return __super::FSM_Initialize(pStartState);
}

Int32 CMRCMProcessor::FSM_Update()
{
	return __super::FSM_Update();
}

Int32 CMRCMProcessor::FSM_Shutdown()
{
	return __super::FSM_Shutdown();
}

Int32 CMRCMProcessor::FSM_SendMessage(TArgumentArray& args)
{
	return __super::FSM_SendMessage(args);
}

//////////////////////////////////////////////////////////////////////////

#define CAST_STATE_PARENT		CMRCMProcessor* pProc = static_cast<CMRCMProcessor*>(m_pFSM)

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CMRCMProcessor::CUninitializedState::CUninitializedState(CMRCMProcessor* pFSM, const String& strName)
: TMRCMProcessorState(pFSM, strName)
{
}

Int32 CMRCMProcessor::CUninitializedState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CMRCMProcessor::CUninitializedState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CMRCMProcessor::CUninitializedState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CMRCMProcessor::CUninitializedState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CMRCMProcessor::CIdleState::CIdleState(CMRCMProcessor* pFSM, const String& strName)
: TMRCMProcessorState(pFSM, strName)
{

}

Int32 CMRCMProcessor::CIdleState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CMRCMProcessor::CIdleState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CMRCMProcessor::CIdleState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CMRCMProcessor::CIdleState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CMRCMProcessor::CProcessingState::CProcessingState(CMRCMProcessor* pFSM, const String& strName) 
: TMRCMProcessorState(pFSM, strName)
{

}

Int32 CMRCMProcessor::CProcessingState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	CAST_STATE_PARENT;

	if (pProc->m_pThread->Start() <= 0)
		return -1;

	return __super::OnEnter(pArgs);
}

Int32 CMRCMProcessor::CProcessingState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CMRCMProcessor::CProcessingState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CMRCMProcessor::CProcessingState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

// CMRCMProcessor::CRenderingState::CRenderingState(CMRCMProcessor* pFSM, const String& strName)
// : TMRCMProcessorState(pFSM, strName)
// {
// 
// }
// 
// Int32 CMRCMProcessor::CRenderingState::OnEnter(TArgumentArray* pArgs /* = NULL */)
// {
// 	return __super::OnEnter(pArgs);
// }
// 
// Int32 CMRCMProcessor::CRenderingState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
// {
// 	return __super::OnUpdate(pArgs);
// }
// 
// Int32 CMRCMProcessor::CRenderingState::OnExit(TArgumentArray* pArgs /* = NULL */)
// {
// 	return __super::OnExit(pArgs);
// }
// 
// Int32 CMRCMProcessor::CRenderingState::OnMessage(TArgumentArray* pArgs /* = NULL */)
// {
// 	return __super::OnMessage(pArgs);
// }

//////////////////////////////////////////////////////////////////////////

CMRCMProcessor::CCancelledState::CCancelledState(CMRCMProcessor* pFSM, const String& strName)
: TMRCMProcessorState(pFSM, strName)
{

}

Int32 CMRCMProcessor::CCancelledState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CMRCMProcessor::CCancelledState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CMRCMProcessor::CCancelledState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CMRCMProcessor::CCancelledState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

#undef CAST_STATE_PARENT