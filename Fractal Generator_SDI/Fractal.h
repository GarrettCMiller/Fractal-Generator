#pragma once

#include <atlimage.h>

#include <complex>

#include "Timer.h"
#include "Palette.h"

#include "Rect.h"
#include "2dDynamicArray.h"

#include "ThreadMgr.h"

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

	eState_Count
};

enum EFractalFunctionArgs
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

typedef std::complex<double> TComplex;

union UVarTypes
{
	TComplex*				comp;
	int*					i;
	double*					d;
	Palette*				pal;
	CDynamicArray<double>*	arr;
};

typedef CDynamicArray<UVarTypes> TArgumentArray;

#define SAFE_CALL(pfnFunc, args)	if (pfnFunc) pfnFunc(args);
#define ARG_VAL(arg)				(*(arg))

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class CFractal
{
public:		//For now

	class Processor;
	
	//typedefs
	typedef int (*pfnIterFunction)(Processor& args);

	typedef CDynamicArray<pfnIterFunction>	TFunctionArray;
	typedef C2dDynamicArray<double>			TDataArray;
	typedef CDynamicArray<Processor>		TProcessorArray;

	//The plane the image makes up
	GLib::Rectd		m_Viewport;

	//Center point
	Vec2d			m_vCenter;

	//Width and height
	Vec2d			m_vSize;

	//TOTAL Pixel Width/height
	Vec2i			m_vImageSize;

	//Jump between complex coords
	TComplex		m_dX;
	TComplex		m_dY;

	bool			m_bPreview;
	bool			m_bJulia;
	bool			m_bSmoothShading;

	CImage			m_Image;

	Palette			m_Palette;

	Timer			m_ProcessTimer;
	float			m_ProcessingTime;

	EFractalState	m_State;

	int				m_nMaxIterations;

	double			m_fBailValue;

	int				m_nThreadCount;

	TDataArray		m_Data;

	TProcessorArray	m_Processors;
	TFunctionArray	m_Functions;

	CRITICAL_SECTION m_CS;

	CWnd*			m_Window;

	////////////
	//During iteration
	BYTE* m_img;
	int m_Pitch;

	class Processor
	{
	public:
		CFractal*		m_pParent;

		double			m_EscapeVal;

		//Top left and bottom right
		GLib::Rectd		m_Viewport;

		//Iteration count
		int				m_CurrentIteration;

		//Color of the pixel
		int				m_color;

		//Origin
		Vec2i			m_vOrigin;

		//Current pixel x/y
		Vec2i			m_vCurrentPixel;

		//Pixel Width/height (of the current section)
		Vec2i			m_vImageSize;

		//C - The Constant
		TComplex		m_C;

		//Julia Point
		TComplex		m_J;

		//Current point iterator
		TComplex		m_Z;

		//Current Point
		TComplex		m_P;

		EFractalState	m_State;

		float			m_PercentComplete;
		DWORD			m_ThreadId;

		int				m_PreviewBlockSize;
		Vec2i			m_PreviewStartBlock;

		//Proc data
		TDataArray		m_Data;

		TFunctionArray	m_Functions;

		void IterateCurrentPoint();
		void ColorCurrentPixel();
		void ColorPixel(const Vec2u& pos);

		void ResetRow();
		void UpdatePosition();

		void Init(int x, int y, int cx, int cy);
		void Update();
	};

	struct ProcessingData
	{
		CFractal*	m_parent;
		int			m_procId;
	};
	//
	////////////

	void MeshThreadData(int threadNum = -1);

	static DWORD ProcessingThread(LPVOID param);
	static const int m_MaxThreads = 100;

public:
	CFractal(bool bJulia = false);
	~CFractal(void);

	void Init(CWnd* parentFrame = NULL);
	void Update();

	//void Begin()

	void Render(CPaintDC& dc, int x = 0, int y = 0, int cx = 0, int cy = 0);
	void Render();

	void SetCenterPoint( double x, double y, EZoomMode mode = eZoom_None, EZoomSpecial spec = eZoomSpec_Double );
	void SetCenterPoint( int x, int y, EZoomMode mode = eZoom_None, EZoomSpecial spec = eZoomSpec_Double );
	void Recenter( EZoomMode mode = eZoom_None, EZoomSpecial spec = eZoomSpec_Double );

	void SetupJulia(int x, int y);

	void SetSize(int cx, int cy);

	void CreateBitmap();

	void CreatePalette();
	void Draw();

	void SetPalette(const CDynamicArray<Color>& pal, bool bRedraw = false);
};

extern CFractal frac;
extern CFractal	julia;