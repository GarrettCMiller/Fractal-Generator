#pragma once

#include <atlimage.h>

#include "Timer.h"
#include "Palette.h"

#define HIGH_DEF

#ifdef HIGH_DEF
	#define DEFAULT_SIZE	1024
	#define DEFAULT_WIDTH	DEFAULT_SIZE
	#define DEFAULT_HEIGHT	DEFAULT_SIZE
	#define MAX_ITERATIONS	2048
#else
	#define DEFAULT_SIZE	512
	#define DEFAULT_WIDTH	DEFAULT_SIZE
	#define DEFAULT_HEIGHT	DEFAULT_SIZE
	#define MAX_ITERATIONS	512
#endif

enum EZoomMode
{
	eZoom_None,
	eZoom_In,
	eZoom_Out,

	//eZoom_Julia,	//Specifies that a Julia set is requested

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
	eState_Rendering,
	eState_Running,
	eState_Done,

	eState_Count
};

class CFractal
{
public:		//For now
	//Top left and bottom right
	double			m_fTLx;
	double			m_fTLy;
	double			m_fBRx;
	double			m_fBRy;

	//Center point
	double			m_fMx;
	double			m_fMy;

	//Width and height
	double			m_fcX;
	double			m_fcY;

	//Current Point
	double			m_fPx;
	double			m_fPy;

	//C - The Constant
	double			m_fCx;
	double			m_fCy;

	//Julia Point
	double			m_fJx;
	double			m_fJy;

	//Jump between complex coords
	double			m_fdX;
	double			m_fdY;

	//Current point iterator
	double			m_fZx;
	double			m_fZy;

	double			m_fBailValue;

	double			maxdelta;
	double			mindelta;
	///////////////////////////////////////////

	//Pixel Width/height
	int				m_ncX;
	int				m_ncY;

	//Current pixel x/y
	int				m_nX;
	int				m_nY;

	int				m_nMaxIterations;

	bool			m_bInProgress;
	bool			m_bPreview;
	bool			m_bJulia;

	static bool		m_bSmoothShading;

	EFractalState	m_State;

	CImage			m_Image;

	////////////
	//During iteration
	BYTE* m_img;
	int m_Pitch;
	int m_ind;

	float m_mu;

	double m_x2, m_y2, m_orbit;
	int m_i, m_color, m_extra;
	//
	////////////

public:
	CFractal(bool bJulia = false);
	~CFractal(void);

	void Init();
	void Update();

	void IterateCurrentPoint();
	void ColorCurrentPixel();
	
	bool UpdateColumn();
	bool UpdateRow();

	void Render(CPaintDC& dc, int x = 0, int y = 0, int cx = 0, int cy = 0);

	void SetCenterPoint( double x, double y, EZoomMode mode = eZoom_None, EZoomSpecial spec = eZoomSpec_Double );
	void SetCenterPoint( int x, int y, EZoomMode mode = eZoom_None, EZoomSpecial spec = eZoomSpec_Double );
	void Recenter( EZoomMode mode = eZoom_None, EZoomSpecial spec = eZoomSpec_Double );

	void SetupJulia(int x, int y);

	void SetSize(int cx, int cy);

	void CreateBitmap();

	void MakePal();

	void SetPalette(const CDynamicArray<Color>& pal, bool bRedraw = false);
	int ColorLookup(float mu, int i = 0);

	void Draw();

	float		m_Data[DEFAULT_WIDTH * DEFAULT_HEIGHT];

	Palette		m_Palette;
};

extern CFractal frac;
extern CFractal	julia;