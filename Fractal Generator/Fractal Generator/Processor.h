#pragma once

#include "FractalGlobals.h"

class CFractal;
class CThread;

class CProcessor : public CStateMachine
{
public:
	CProcessor(const char* strName = NULL, Bool bCreateStates = true);
	virtual ~CProcessor(void);

public:

	//Fractal to which this processor belongs
	CFractal*		m_pParent;

	//State of the processor
	EFractalState	m_State;

	//The plane the image makes up
	GLib::Rectd		m_Viewport;

	//Iteration count
	Int32			m_CurrentIteration;

	Float			m_PercentComplete;
	UInt32			m_ProcID;

	CThread*		m_pThread;

	TFunctionArray	m_Functions;

	TDataArray		m_Data;

	virtual	Int32 Init(CFractal& _parent, TArgumentArray* pArgs = NULL);
	virtual	Int32 Update(TArgumentArray* pArgs = NULL);
	virtual	Int32 Shutdown(TArgumentArray* pArgs = NULL);

	virtual Int32 Begin();

	//////////////////////////////////////////////
	// FSM stuff
	//////////////////////////////////////////////
public:
	Int32 FSM_Initialize(IFSMStateBase* pStartState);
	Int32 FSM_Update();
	Int32 FSM_Shutdown();

	Int32 FSM_SendMessage(TArgumentArray& args);

	class CUninitializedState;
	class CIdleState;
	class CProcessingState;
	//class CRenderingState;
	class CCancelledState;
};

typedef CFSMState<CProcessor>	TProcessorState;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class CProcessor::CUninitializedState : public TProcessorState
{
	friend CProcessor;
public:
	CUninitializedState(CProcessor* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};

//////////////////////////////////////////////////////////////////////////

class CProcessor::CIdleState : public TProcessorState
{
	friend CProcessor;
public:
	CIdleState(CProcessor* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};

//////////////////////////////////////////////////////////////////////////

class CProcessor::CProcessingState : public TProcessorState
{
	friend class CProcessor;
public:
	CProcessingState(CProcessor* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};

//////////////////////////////////////////////////////////////////////////

// class CProcessor::CRenderingState : public TProcessorState
// {
// 	friend CProcessor;
// public:
// 	CRenderingState(CProcessor* pFSM, const String& strName);
// 
// 	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
// 	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
// 	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
// 	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
// };

//////////////////////////////////////////////////////////////////////////

class CProcessor::CCancelledState : public TProcessorState
{
	friend CProcessor;
public:
	CCancelledState(CProcessor* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};