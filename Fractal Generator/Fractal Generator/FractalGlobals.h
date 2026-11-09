#pragma once

#include <complex>

#include "Globals.h"

#include "2dDynamicArray.h"

#include "Palette.h"
#include "Functor.h"

#include "variable.h"

#include "Vertex.h"

#include "Rect.h"

#include "D3DWrapper.h"

#include "StateMachine.h"

#include "Verts.h"
//////////////////////////////////////////////////////////////////////////

#define MAX_ITERATION_FUNCTIONS		5

//#define HIGH_DEF

#ifdef HIGH_DEF
	#define DEFAULT_SIZE	1024
	#define DEFAULT_WIDTH	DEFAULT_SIZE
	#define DEFAULT_HEIGHT	DEFAULT_SIZE
	#define MAX_ITERATIONS	2048
#else
	#define DEFAULT_SIZE	512
	#define DEFAULT_WIDTH	DEFAULT_SIZE
	#define DEFAULT_HEIGHT	DEFAULT_SIZE
	#define MAX_ITERATIONS	128
#endif

#define SAFE_CALL(pfnFunc, args)	if (pfnFunc) pfnFunc(args);
#define ARG_VAL(arg)				(*(arg))
#define ARGVAL						(*(pArgs))

//////////////////////////////////////////////////////////////////////////

enum EZoomMode
{
	eZoom_None,
	eZoom_In,
	eZoom_Out,

	eZoom_Max
};

enum EZoomSpecial
{
	eZoomSpec_None,
	eZoomSpec_Double,

	eZoomSpec_Max
};

enum EFractalState
{
	eState_Uninitialized,

	eState_Ready,
	eState_Processing,
	eState_Rendering,
	eState_Done,

	eState_Cancelled,

	eState_Count
};

enum EFractalFunctions
{
	eFF_Init,
		eFF_PreIter,
			eFF_Iter,
		eFF_PostIter,
		eFF_Draw,
		//eFF_UpdatePos,
	eFF_Done,

	eFF_NumFunctions
};

enum EFractalType
{
	eFT_Complex,
	eFT_MRCM,
	eFT_LSystem,
	eFT_MidptDisp,

	eFT_NumTypes,

	eFT_Invalid
};

//////////////////////////////////////////////////////////////////////////

typedef std::complex<Double> TComplex;
typedef std::complex<Float> TComplexF;

// union UVarTypes
// {
// 	TComplex				comp;
// 	Int32					i;
// 	Double					d;
// 	Palette					pal;
// 	CDynamicArray<Double>	arrD;
// 	CDynamicArray<UInt32>	arrU;
// };



class CProcessor;
class CVertex;

typedef Int32 (*pfnIterFunction)			(CProcessor* args);

typedef COpenFunctor<Int32, CProcessor*>	TProcessorFunction;

//typedef CDynamicArray<pfnIterFunction>	TFunctionArray;
typedef CDynamicArray<TProcessorFunction>	TFunctionArray;
typedef C2dDynamicArray<Double>				TDataArray;
typedef CDynamicArray<CProcessor*>			TProcessorArray;