#pragma once

#include "Processor.h"
#include "LSystem.h"

class CLSystemProcessor : public CProcessor
{
public:
	CLSystemProcessor();
	CLSystemProcessor(const char* strName = NULL, Bool bCreateStates = true);
	
	virtual ~CLSystemProcessor(void);

	virtual Int32 Init(CFractal& _parent, TArgumentArray* pArgs = NULL);
	virtual Int32 Update(TArgumentArray* pArgs = NULL);
	virtual Int32 Shutdown(TArgumentArray* pArgs = NULL);

	virtual Int32 Begin();

	String GetCurrentString() const
	{
		return m_CurrentString;
	}

protected:
	Bool ProcessString(String& strString);
	String ProcessRule(Char letter);

protected:
	Bool				m_bRandomize;

	String				m_Alphabet;
	String				m_Axiom;

	CLSystem::TRuleMap	m_Rules;

	String				m_CurrentString;

	Timer				m_GrowthTimer;


	//////////////////////////////////////////////
	// FSM stuff
	//////////////////////////////////////////////
public:
	ADD_FSM_FUNCTIONS

	class CUninitializedState;
	class CIdleState;
	class CProcessingState;
	//class CRenderingState;
	class CCancelledState;
};

typedef CFSMState<CLSystemProcessor>	TLSystemProcessorState;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class CLSystemProcessor::CUninitializedState : public TLSystemProcessorState
{
	friend class CLSystemProcessor;
public:
	CUninitializedState(CLSystemProcessor* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};

//////////////////////////////////////////////////////////////////////////

class CLSystemProcessor::CIdleState : public TLSystemProcessorState
{
	friend class CLSystemProcessor;
public:
	CIdleState(CLSystemProcessor* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};

//////////////////////////////////////////////////////////////////////////

class CLSystemProcessor::CProcessingState : public TLSystemProcessorState
{
	friend class CLSystemProcessor;
public:
	CProcessingState(CLSystemProcessor* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};

//////////////////////////////////////////////////////////////////////////

// class CLSystemProcessor::CRenderingState : public TLSystemProcessorState
// {
// 	friend class CLSystemProcessor;
// public:
// 	CRenderingState(CLSystemProcessor* pFSM, const String& strName);
// 
// 	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
// 	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
// 	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
// 	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
// };

//////////////////////////////////////////////////////////////////////////

class CLSystemProcessor::CCancelledState : public TLSystemProcessorState
{
	friend class CLSystemProcessor;
public:
	CCancelledState(CLSystemProcessor* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};
