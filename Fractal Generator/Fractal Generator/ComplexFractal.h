#pragma once
#include "fractal.h"

//////////////////////////////////////////////////////////////////////////

class CComplexFractal :	public CFractal
{
	friend class CComplexProcessor;
public:
	enum EFunctionArgs
	{
		efC,
		efJ,
		efZ,
		efP,

		efI,

		efPixelX,
		efPixelY,

		efSizeX,
		efSizeY,

		efEscapeVal,

		efPalette,

		efData,

		efNumFunctionArgs
	};

public:
	CComplexFractal(const char* strName = NULL, Bool bCreateStates = true);
	virtual ~CComplexFractal(void);

	void SetupJulia(UInt32 x, UInt32 y);

	virtual Int32 Initialize(CWnd* parentFrame = NULL, TArgumentArray* pArgs = NULL);
	virtual Int32 Update();

	virtual Int32 Reset();

	virtual Int32 Draw();

public:
	Bool	IsJuliaSet()		const	{	return m_bJulia;	}

	Double	GetPixelDeltaX()	const	{	return m_dX.real();	}
	Double	GetPixelDeltaY()	const	{	return m_dY.imag();	}
	Double	GetBailValue()		const	{	return m_fBailValue;}

protected:
	//Jump between complex coords
	TComplex		m_dX;
	TComplex		m_dY;

	Bool			m_bSmoothShading;

	Bool			m_bJulia;

	Double			m_fBailValue;

protected:
	virtual Bool SetupProcessors();

	static DWORD ProcessingThread(LPVOID param);


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

//////////////////////////////////////////////////////////////////////////

typedef CFSMState<CComplexFractal>		TComplexFractalState;

//////////////////////////////////////////////////////////////////////////

#define MAKE_STATE(_stateName)		MAKE_STATE_CLASS(CComplexFractal, _stateName, TComplexFractalState)

//////////////////////////////////////////////////////////////////////////

MAKE_STATE(Uninitialized);

MAKE_STATE(Idle);

MAKE_STATE(Processing);

MAKE_STATE(Rendering);

MAKE_STATE(Cancelled);

//////////////////////////////////////////////////////////////////////////

#undef MAKE_STATE