#include "StdAfx.h"

#include "LSystemProcessor.h"
#include "LSystem.h"

//Make it static so that it persists and doesn't have to be reset every call
#define CAST_PARENT				static CLSystem* pParent = static_cast<CLSystem*>(m_pParent)
//////////////////////////////////////////////////////////////////////////

CLSystemProcessor::CLSystemProcessor(const char* strName /* = NULL */, Bool bCreateStates /* = true */) :
	CProcessor(strName, false)
{
	if (bCreateStates)
	{
		TLSystemProcessorState	*pUnInitState		= new CUninitializedState(this,	k_strUnInit),
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

CLSystemProcessor::~CLSystemProcessor(void)
{
}

//////////////////////////////////////////////////////////////////////////

Int32 CLSystemProcessor::Init(CFractal& _parent, TArgumentArray* pArgs /* = NULL */)
{
	CProcessor::Init(_parent, pArgs);

	ASSERT(pArgs);

	CAST_PARENT;

	TArgumentArray& args = *pArgs;

	String	Alphabet	= args[0],
			Axiom		= args[1],
			m_Rules		= args[2];

	FSM_QueueState(k_strIdle);
	FSM_Update();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CLSystemProcessor::Update(TArgumentArray* pArgs /* = NULL */)
{
	//m_PercentComplete = float(m_vImageSize.X * m_vCurrentPixel.Y + m_vCurrentPixel.X) / float(m_vImageSize.X * m_vImageSize.Y) * 100.0f;

	//HACK
	m_PercentComplete = Clamp(m_PercentComplete, 0.0f, 100.0f);

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CLSystemProcessor::Shutdown(TArgumentArray* pArgs /* = NULL */)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CLSystemProcessor::Begin()
{
	Int32 ret = CProcessor::Begin();

	return ret;
}

//////////////////////////////////////////////////////////////////////////

Bool CLSystemProcessor::ProcessString(String& strString)
{
	String result;

	Char letter = '\0';

	for (UInt32 l = 0; l < strString.length(); l++)
	{
		letter = strString[l];

		if (m_Alphabet.find(letter) != String::npos)
			result += ProcessRule(letter);
		else
		{
			switch (letter)
			{
			case '[':
			case ']':
			case '+':
			case '-':
				result += letter;
				break;

			default: //We don't accept any other symbols
				ASSERT(0);
			}
		}
	}

	strString = result;

	return true;
}

//////////////////////////////////////////////////////////////////////////

String CLSystemProcessor::ProcessRule(Char letter)
{
	String result = m_Rules[letter];
	return result;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Int32 CLSystemProcessor::FSM_Initialize(IFSMStateBase* pStartState)
{
	return __super::FSM_Initialize(pStartState);
}

Int32 CLSystemProcessor::FSM_Update()
{
	return __super::FSM_Update();
}

Int32 CLSystemProcessor::FSM_Shutdown()
{
	return __super::FSM_Shutdown();
}

Int32 CLSystemProcessor::FSM_SendMessage(TArgumentArray& args)
{
	return __super::FSM_SendMessage(args);
}

//////////////////////////////////////////////////////////////////////////

#define CAST_STATE_PARENT		CLSystemProcessor* pProc = static_cast<CLSystemProcessor*>(m_pFSM)

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CLSystemProcessor::CUninitializedState::CUninitializedState(CLSystemProcessor* pFSM, const String& strName)
: TLSystemProcessorState(pFSM, strName)
{
}

Int32 CLSystemProcessor::CUninitializedState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CLSystemProcessor::CUninitializedState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CLSystemProcessor::CUninitializedState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CLSystemProcessor::CUninitializedState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CLSystemProcessor::CIdleState::CIdleState(CLSystemProcessor* pFSM, const String& strName)
: TLSystemProcessorState(pFSM, strName)
{

}

Int32 CLSystemProcessor::CIdleState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CLSystemProcessor::CIdleState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CLSystemProcessor::CIdleState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CLSystemProcessor::CIdleState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CLSystemProcessor::CProcessingState::CProcessingState(CLSystemProcessor* pFSM, const String& strName) 
: TLSystemProcessorState(pFSM, strName)
{

}

Int32 CLSystemProcessor::CProcessingState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	CAST_STATE_PARENT;

	if (pProc->m_pThread->Start() <= 0)
		return -1;

	return __super::OnEnter(pArgs);
}

Int32 CLSystemProcessor::CProcessingState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CLSystemProcessor::CProcessingState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CLSystemProcessor::CProcessingState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

// CLSystemProcessor::CRenderingState::CRenderingState(CLSystemProcessor* pFSM, const String& strName)
// : TLSystemProcessorState(pFSM, strName)
// {
// 
// }
// 
// Int32 CLSystemProcessor::CRenderingState::OnEnter(TArgumentArray* pArgs /* = NULL */)
// {
// 	return __super::OnEnter(pArgs);
// }
// 
// Int32 CLSystemProcessor::CRenderingState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
// {
// 	return __super::OnUpdate(pArgs);
// }
// 
// Int32 CLSystemProcessor::CRenderingState::OnExit(TArgumentArray* pArgs /* = NULL */)
// {
// 	return __super::OnExit(pArgs);
// }
// 
// Int32 CLSystemProcessor::CRenderingState::OnMessage(TArgumentArray* pArgs /* = NULL */)
// {
// 	return __super::OnMessage(pArgs);
// }

//////////////////////////////////////////////////////////////////////////

CLSystemProcessor::CCancelledState::CCancelledState(CLSystemProcessor* pFSM, const String& strName)
: TLSystemProcessorState(pFSM, strName)
{

}

Int32 CLSystemProcessor::CCancelledState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CLSystemProcessor::CCancelledState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CLSystemProcessor::CCancelledState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CLSystemProcessor::CCancelledState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

#undef CAST_STATE_PARENT