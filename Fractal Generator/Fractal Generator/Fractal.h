#pragma once

#include "FractalImage.h"
 
#include "Processor.h"

#include "CritSection.h"
#include "ThreadMgr.h"

#include "Timer.h"

#include "Functor.h"

#include "Rect.h"

#include "StateMachine.h"

//////////////////////////////////////////////////////////////////////////

extern const String k_strUnInit;
extern const String k_strIdle;
extern const String k_strProcessing;
extern const String k_strRender;
extern const String k_strCancelled;

//////////////////////////////////////////////////////////////////////////

class CBaseGeneticAlgorithm;

//////////////////////////////////////////////////////////////////////////

class CFractal : public CStateMachine
{
public:		//For now
	//The plane the image makes up
	GLib::Rectd		m_Viewport;

	Bool			m_bD3D;

	Bool			m_bPreview;
	Bool			m_bValid;

	Timer			m_ProcessTimer;
	Float			m_ProcessingTime;

	Vec2u			m_vSize;
	Vec2u			m_vImageSize;

	EFractalState	m_State;
	EFractalType	m_eType;
	//CFractalStateMachine m_StateMachine;

	Int32			m_nMaxIterations;

	UInt32			m_nThreadCount;

	CFractalImage	m_Image;

	TProcessorArray	m_Processors;
	TFunctionArray	m_Functions;
	TDataArray		m_Data;

	CCritSection	m_CS;

	CWnd*			m_Window;

	CBaseGeneticAlgorithm*	m_pGenAlg;

	struct ProcessingData
	{
		CFractal*	m_parent;
		Int32		m_procID;
	};

	struct InitStruct
	{
		UInt32	imgWidth;
		UInt32	imgHeight;
		UInt32	maxIters;
	};

public:
	CFractal(const char* strName = NULL, Bool bCreateStates = true);
	virtual ~CFractal(void);

	//////////////////////////////////////
	// 
	virtual Int32 Initialize(CWnd* parentFrame = NULL, TArgumentArray* pArgs = NULL);
	virtual Int32 Update();

	virtual Int32 Reset();

	virtual Int32 Begin();
	virtual Int32 Stop();

	virtual Int32 AutoRender();
	virtual Int32 Render(CPaintDC& dc, 
						UInt32 x = 0,	UInt32 y = 0, 
						UInt32 cx = 0,	UInt32 cy = 0);

	virtual Int32 InitializeImage(UInt32 nWidth = 0, UInt32 nHeight = 0);

	virtual Int32 CreatePalette();
	virtual Int32 Draw();

	Bool IsValid()	const	{ return m_bValid; }
	Bool IsLocked()	const	{	return m_CS.IsLocked();	}

	void SetPalette(const CDynamicArray<Color>& pal, Bool bRedraw = false);

	//////////////////
	//View Stuff
	void SetCenterPoint( Double x, Double y, EZoomMode mode = eZoom_None, EZoomSpecial spec = eZoomSpec_Double );
	void SetCenterPoint( UInt32 x, UInt32 y, EZoomMode mode = eZoom_None, EZoomSpecial spec = eZoomSpec_Double );
	void Recenter( EZoomMode mode = eZoom_None, EZoomSpecial spec = eZoomSpec_Double );

	Double	GetData(UInt32 ix, UInt32 iy) const
	{
		return m_Data(ix, iy);
	}

	virtual void MeshThreadData(int threadNum = -1);


protected:
	virtual Bool SetupProcessors();

	static DWORD ProcessingThread(LPVOID param);
	static const int m_MaxThreads = 100;

	//////////////////////////////////////////////
	// FSM stuff
	//////////////////////////////////////////////
public:
	ADD_FSM_FUNCTIONS

	class CUninitializedState;
	class CIdleState;
	class CProcessingState;
	class CRenderingState;
	class CCancelledState;

};

typedef CFSMState<CFractal>		TFractalState;

//////////////////////////////////////////////////////////////////////////

class CFractal::CUninitializedState : public TFractalState
{
	friend CFractal;
public:
	CUninitializedState(CFractal* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};

//////////////////////////////////////////////////////////////////////////

class CFractal::CIdleState : public TFractalState
{
	friend CFractal;
public:
	CIdleState(CFractal* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};

//////////////////////////////////////////////////////////////////////////

class CFractal::CProcessingState : public TFractalState
{
	friend class CFractal;
public:
	CProcessingState(CFractal* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};

//////////////////////////////////////////////////////////////////////////

class CFractal::CRenderingState : public TFractalState
{
	friend CFractal;
public:
	CRenderingState(CFractal* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};

//////////////////////////////////////////////////////////////////////////

class CFractal::CCancelledState : public TFractalState
{
	friend CFractal;
public:
	CCancelledState(CFractal* pFSM, const String& strName);

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);
	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);
};
