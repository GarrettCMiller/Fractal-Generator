#pragma once
#include "processor.h"

#include "MidpointDisplacement.h"

using namespace GLib;

class CMDProcessor : public CProcessor
{
public:
	CMDProcessor(const char* strName = NULL, Bool bCreateStates = true);
	virtual ~CMDProcessor(void);

	Float	m_ParentVal;

	UInt32	m_LineCount;
	UInt32	m_VertexCount;

	Float	m_Roughness;
	Float	m_RoughnessMultiplier;

	Float	m_RandomRangeLow;
	Float	m_RandomRangeHigh;

	Int32	Init(CFractal& _parent, TArgumentArray* pArgs = NULL);
	Int32	Update(TArgumentArray* pArgs = NULL);
	Int32	Shutdown(TArgumentArray* pArgs = NULL);

	Double RandomizeMidpoint(Vec2d p1, Vec2d p2);
	virtual void Generate(GLib::Rectd area, CMidpointDisplacement::ESide side, Double parentVal);

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

typedef CFSMState<CMDProcessor>		TMDProcessorState;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class CMDProcessor::CUninitializedState : public TMDProcessorState
{
	friend CMDProcessor;
public:
	CUninitializedState(CMDProcessor* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};

//////////////////////////////////////////////////////////////////////////

class CMDProcessor::CIdleState : public TMDProcessorState
{
	friend CMDProcessor;
public:
	CIdleState(CMDProcessor* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};

//////////////////////////////////////////////////////////////////////////

class CMDProcessor::CProcessingState : public TMDProcessorState
{
	friend class CMDProcessor;
public:
	CProcessingState(CMDProcessor* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};

//////////////////////////////////////////////////////////////////////////

// class CMDProcessor::CRenderingState : public TMDProcessorState
// {
// 	friend CMDProcessor;
// public:
// 	CRenderingState(CMDProcessor* pFSM, const String& strName);
// 
// 	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
// 	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
// 	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
// 	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
// };

//////////////////////////////////////////////////////////////////////////

class CMDProcessor::CCancelledState : public TMDProcessorState
{
	friend CMDProcessor;
public:
	CCancelledState(CMDProcessor* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};