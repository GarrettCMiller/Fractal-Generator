#pragma once
#include "processor.h"

class CMRCMProcessor : public CProcessor
{
public:
	CMRCMProcessor();
	CMRCMProcessor(const char* strName = NULL, Bool bCreateStates = true);
	
	virtual ~CMRCMProcessor(void);

	virtual Int32 Init(CFractal& _parent, TArgumentArray* pArgs = NULL);
	virtual Int32 Update(TArgumentArray* pArgs = NULL);
	virtual Int32 Shutdown(TArgumentArray* pArgs = NULL);

	virtual Int32 Begin();

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

typedef CFSMState<CMRCMProcessor>	TMRCMProcessorState;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class CMRCMProcessor::CUninitializedState : public TMRCMProcessorState
{
	friend class CMRCMProcessor;
public:
	CUninitializedState(CMRCMProcessor* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};

//////////////////////////////////////////////////////////////////////////

class CMRCMProcessor::CIdleState : public TMRCMProcessorState
{
	friend class CMRCMProcessor;
public:
	CIdleState(CMRCMProcessor* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};

//////////////////////////////////////////////////////////////////////////

class CMRCMProcessor::CProcessingState : public TMRCMProcessorState
{
	friend class CMRCMProcessor;
public:
	CProcessingState(CMRCMProcessor* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};

//////////////////////////////////////////////////////////////////////////

// class CMRCMProcessor::CRenderingState : public TMRCMProcessorState
// {
// 	friend CMRCMProcessor;
// public:
// 	CRenderingState(CMRCMProcessor* pFSM, const String& strName);
// 
// 	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
// 	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
// 	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
// 	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
// };

//////////////////////////////////////////////////////////////////////////

class CMRCMProcessor::CCancelledState : public TMRCMProcessorState
{
	friend class CMRCMProcessor;
public:
	CCancelledState(CMRCMProcessor* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};