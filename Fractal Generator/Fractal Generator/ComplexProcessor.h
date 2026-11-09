#pragma once

#include "Processor.h"

class CComplexProcessor : public CProcessor
{
public:

	CComplexProcessor(const char* strName = NULL, Bool bCreateStates = true);

	///////////////////////////////////

	UInt32			m_PreviewBlockSize;

	//The value at which we can safely assume it goes to infinity
	Double			m_EscapeVal;

	//C - The Constant
	TComplex		m_C;

	//Julia Point
	TComplex		m_J;

	//Current point iterator
	TComplex		m_Z;

	//Current Point
	TComplex		m_P;

	//Pixel Origin
	Vec2u			m_vOrigin;

	//Color of the pixel
	//Int32			m_color;
	Color			m_color;

	//Current pixel x/y
	Vec2u			m_vCurrentPixel;

	//Pixel Width/height (of the current section)
	Vec2u			m_vImageSize;

	Int32 Init(CFractal& _parent, TArgumentArray* pArgs = NULL);
	Int32 Update(TArgumentArray* pArgs = NULL);
	Int32 Shutdown(TArgumentArray* pArgs = NULL);

	void IterateCurrentPoint();
	void ColorCurrentPixel();
	void ColorPixel(const Vec2u& pos);

	void ResetRow();
	void UpdatePosition();

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

typedef CFSMState<CComplexProcessor>	TComplexProcessorState;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class CComplexProcessor::CUninitializedState : public TComplexProcessorState
{
	friend CComplexProcessor;
public:
	CUninitializedState(CComplexProcessor* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};

//////////////////////////////////////////////////////////////////////////

class CComplexProcessor::CIdleState : public TComplexProcessorState
{
	friend CComplexProcessor;
public:
	CIdleState(CComplexProcessor* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};

//////////////////////////////////////////////////////////////////////////

class CComplexProcessor::CProcessingState : public TComplexProcessorState
{
	friend class CComplexProcessor;
public:
	CProcessingState(CComplexProcessor* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};

//////////////////////////////////////////////////////////////////////////

// class CComplexProcessor::CRenderingState : public TComplexProcessorState
// {
// 	friend CComplexProcessor;
// public:
// 	CRenderingState(CComplexProcessor* pFSM, const String& strName);
// 
// 	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
// 	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
// 	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
// 	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
// };

//////////////////////////////////////////////////////////////////////////

class CComplexProcessor::CCancelledState : public TComplexProcessorState
{
	friend CComplexProcessor;
public:
	CCancelledState(CComplexProcessor* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};