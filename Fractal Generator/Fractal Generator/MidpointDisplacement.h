#pragma once

#include "Fractal.h"

#include "DynamicArray.h"
#include "Vec.h"

static const Float DEFAULT_ROUGHNESS =				0.75f;
static const Float DEFAULT_ROUGHNESS_MULTIPLIER	=	0.95f;
static const Float DEFAULT_UPPER_RANGE =			1.0f;
static const Float DEFAULT_LOWER_RANGE =			0.0f;

class CMidpointDisplacement : public CFractal
{
public:
	CMidpointDisplacement(const char* strName = NULL, Bool bCreateStates = true);
	virtual ~CMidpointDisplacement(void);

	struct InitStruct : public CFractal::InitStruct
	{
		UInt32	seed;
		Float	roughness;
		Float	roughMult;
		Float	lowerRange;
		Float	upperRange;
	};

	static const Int32	mk_MaxResolution = 200;

	virtual Int32 Render(CPaintDC& dc, UInt32 x = 0, UInt32 y = 0, UInt32 cx = 0, UInt32 cy = 0);

	virtual Int32 Begin();

	virtual Int32 Initialize(CWnd* parentFrame = NULL, TArgumentArray* pArgs = NULL);
	virtual Int32 Update();

	//virtual Int32 Shutdown();

	enum ESide
	{
		eSide_Left,
		eSide_Right,

		eSide_Root,
	};

	Int32 LineCount() const { return m_LineCount; }
	Int32 VertexCount() const { return m_VertexCount; }

	Int32 CurrentIteration() const { return m_CurrentIteration; }

	Float Roughness() const { return m_Roughness; }
	void Roughness(Float val) { m_Roughness = val; }

	Float RoughnessMultiplier() const { return m_RoughnessMultiplier; }
	void RoughnessMultiplier(Float val) { m_RoughnessMultiplier = val; }

	Float RandomRangeLow() const { return m_RandomRangeLow; }
	void RandomRangeLow(Float val) { m_RandomRangeLow = val; }

	Float RandomRangeHigh() const { return m_RandomRangeHigh; }
	void RandomRangeHigh(Float val) { m_RandomRangeHigh = val; }

	Bool Randomize() const { return m_bRandomize; }
	void Randomize(Bool val) { m_bRandomize = val; }
	
	Int32 Seed() const { return m_Seed; }
	virtual void Seed(Int32 val) 
	{ 
		m_Seed = val;
		if (m_Seed == -1)
			srand(static_cast<UInt32>(time(NULL)));
		else
			srand(m_Seed);
	}

	Double RandomizeMidpoint(Vec2d p1, Vec2d p2);

protected:
	
	Bool	m_bRandomize;

	Float	m_ParentVal;

	Int32	m_LineCount;
	Int32	m_VertexCount;

	Int32	m_Seed;

	Int32	m_CurrentIteration;
	
	Float	m_Roughness;
	Float	m_RoughnessMultiplier;

	Float	m_RandomRangeLow;
	Float	m_RandomRangeHigh;
	
protected:
	Bool SetupProcessors();

	static DWORD ProcessingThread(LPVOID param);

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
	class CRenderingState;
	class CCancelledState;
};

typedef CFSMState<CMidpointDisplacement>		TMDState;

//////////////////////////////////////////////////////////////////////////

class CMidpointDisplacement::CUninitializedState : public TMDState
{
	friend CMidpointDisplacement;
public:
	CUninitializedState(CMidpointDisplacement* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};

//////////////////////////////////////////////////////////////////////////

class CMidpointDisplacement::CIdleState : public TMDState
{
	friend CMidpointDisplacement;
public:
	CIdleState(CMidpointDisplacement* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};

//////////////////////////////////////////////////////////////////////////

class CMidpointDisplacement::CProcessingState : public TMDState
{
	friend class CMidpointDisplacement;
public:
	CProcessingState(CMidpointDisplacement* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};

//////////////////////////////////////////////////////////////////////////

class CMidpointDisplacement::CRenderingState : public TMDState
{
	friend CMidpointDisplacement;
public:
	CRenderingState(CMidpointDisplacement* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};

//////////////////////////////////////////////////////////////////////////

class CMidpointDisplacement::CCancelledState : public TMDState
{
	friend CMidpointDisplacement;
public:
	CCancelledState(CMidpointDisplacement* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};