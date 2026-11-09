#include "StdAfx.h"
#include ".\fractal.h"
#include "math.h"

#include "Fractal Generator.h"

#include "ProfileSample.h"

#include "IteratedFunctions.h"

//#include "ParticleEmitter.h"
 
//CParticleEmitter pe;

//////////////////////////////////////////////////////////////////////////

using namespace GLib;

//////////////////////////////////////////////////////////////////////////

CFractal frac;
CFractal julia(true);

//////////////////////////////////////////////////////////////////////////

CFractal::CFractal(bool bJulia) : 
	m_bPreview( true ),
	m_bJulia( bJulia ),
	m_nThreadCount(4)
{
	m_bSmoothShading = true;

	m_vImageSize = //Vec2i(DEFAULT_WIDTH, DEFAULT_HEIGHT);
					Vec2i(1024, 1024);
	CreateBitmap();

	m_Viewport			=	Rectd(-1.5, 2, 2.5, -2);
							//Rectd(-1, 2, 3, -2);

	m_Data.Resize(DEFAULT_WIDTH, DEFAULT_HEIGHT,	false);
	m_Functions.Resize(eFF_NumFunctions,			false);
	m_Processors.Resize(m_MaxThreads,				false);

	m_Functions[eFF_Init]		= &Init_DefaultMandelbrot;
	m_Functions[eFF_PreIter]	= &PreI_DefaultMandelbrot;
	m_Functions[eFF_Iter]		= &Iter_DefaultMandelbrot;
	m_Functions[eFF_PostIter]	= &Post_DefaultMandelbrot;
	m_Functions[eFF_Draw]		= &Draw_DefaultMandelbrot;
	m_Functions[eFF_Done]		= &Done_DefaultMandelbrot;

	CreatePalette();

	InitializeCriticalSection(&m_CS);
}

//////////////////////////////////////////////////////////////////////////

CFractal::~CFractal(void)
{
	DeleteCriticalSection(&m_CS);

	m_State = eState_Uninitialized;

	m_Image.Destroy();
}

//////////////////////////////////////////////////////////////////////////

void CFractal::SetSize(int cx, int cy)
{
	m_vImageSize = Vec2i(abs(cx), abs(cy));
	CreateBitmap();
}

//////////////////////////////////////////////////////////////////////////

void CFractal::CreateBitmap()
{
	m_Image.Destroy();
	m_Image.Create(m_vImageSize.X, -m_vImageSize.Y, 24);
}

//////////////////////////////////////////////////////////////////////////

void CFractal::Init(CWnd* parentFrame)
{
	m_Window			= parentFrame;

	//m_Viewport			= Rectd(-2, 2, 2, -2);
	//m_vImageSize		= Vec2i(DEFAULT_WIDTH, DEFAULT_HEIGHT);
	
	m_State				= eState_Uninitialized;
	
	m_nMaxIterations	= 1024;//MAX_ITERATIONS;
	m_fBailValue		= 4.0;
	
	m_vSize				= Vec2d(m_Viewport.Width(), m_Viewport.Height());
	
	//Account for -Y screen space by making the dY negative
	m_dX				= TComplex(m_vSize.X / m_vImageSize.X, 0);
	m_dY				= TComplex(0, -(m_vSize.Y / m_vImageSize.Y));

	m_img				= static_cast<BYTE *>(m_Image.GetBits());
	m_Pitch				= m_Image.GetPitch();

	//CreateBitmap();

	m_ProcessTimer.Reset();

	if (m_Functions[eFF_Iter] == NULL)
	{
		MessageBox(NULL, "No iteration function specified!", "Cannot Process Fractal", MB_OK);
		return;
	}

	if (m_Functions[eFF_Draw] == NULL)
	{
		MessageBox(NULL, "No drawing function specified!", "Cannot Process Fractal", MB_OK);
		return;
	}

	//Clear out old threads if any
	g_ThreadMgr->KillAll();

	int	px = 0, 
		py = 0, 
		pcx = 0, 
		pcy = 0,
		numcols = (m_nThreadCount > 1 ? sqrt(float(m_nThreadCount)) : m_nThreadCount),
		numrows = numcols;

	//Init all processors at first
	for (int threadId = 0; threadId < m_nThreadCount; threadId++)
	{
		m_Processors[threadId].m_pParent = this;
		m_Processors[threadId].m_ThreadId = threadId;
		m_Processors[threadId].m_Functions.CopyFrom(m_Functions);

		px = (threadId % numcols) * (m_vImageSize.X / numcols);
		py = (threadId / numcols) * (m_vImageSize.Y / numrows);
		pcx = m_vImageSize.X / numcols;
		pcy = m_vImageSize.Y / numrows;

		m_Processors[threadId].Init(px ? px - 1 : px, py ? py - 1 : py, pcx, pcy);
	}

	m_State	= eState_Processing;

	ProcessingData* data = NULL;//new ProcessingData;

	//Then create all the threads
	for (DWORD threadId = 0; threadId < m_nThreadCount; threadId++)
	{
		data = new ProcessingData;

		data->m_parent = this;
		data->m_procId = threadId;

		g_ThreadMgr->CreateThread((LPTHREAD_START_ROUTINE) &CFractal::ProcessingThread, 
								data, false);
	}

	//The fire all the threads
	for (DWORD threadId = 0; threadId < m_nThreadCount; threadId++)
		g_ThreadMgr->GetThread(threadId).Start();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

DWORD CFractal::ProcessingThread(LPVOID param)
{
	Timer	renderTimer(true);
	ProcessingData* data = static_cast<ProcessingData *>(param);
	Processor& proc = data->m_parent->m_Processors[data->m_procId];

	while (data->m_parent->m_State != eState_Uninitialized && 
			proc.m_State == eState_Processing &&
			theApp.m_bQuit != true)
	{
		proc.Update();
	}

	delete data;

	return 10;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CFractal::Processor::Init(int x, int y, int cx, int cy)
{
	m_Data.Resize(cx, cy, false);

	m_Viewport.tlx = m_pParent->m_Viewport.tlx + x * m_pParent->m_dX.real();
	m_Viewport.tly = m_pParent->m_Viewport.tly + y * m_pParent->m_dY.imag();
	m_Viewport.brx = m_Viewport.tlx + cx * m_pParent->m_dX.real();
	m_Viewport.bry = m_Viewport.tly - cy * m_pParent->m_dY.imag();

	m_vCurrentPixel	= m_vOrigin = Vec2i(x, y);

	m_vImageSize = Vec2i(cx, cy);

	m_PreviewStartBlock = Vec2i(0, 0);
	m_PreviewBlockSize = cx / 8;

	m_P	= TComplex(m_Viewport.tlx, m_Viewport.tly);

	m_State	= eState_Processing;

	SAFE_CALL(m_pParent->m_Functions[eFF_Init], *this);
}

//////////////////////////////////////////////////////////////////////////

void CFractal::Processor::ResetRow()
{
	//PROFILE_THIS(3);

	//Move the pixel down one
	//m_vCurrentPixel.Y++;
	m_vCurrentPixel.Y += m_PreviewBlockSize;

	//Move the complex point down by the delta Y factor
	//m_P += m_pParent->m_dY;
	m_P.imag(m_P.imag() + (m_PreviewBlockSize * m_pParent->m_dY.imag()));

	//Reset the row info
	m_vCurrentPixel.X = m_vOrigin.X;// + m_PreviewStartBlock.X;
	m_P	= TComplex(m_Viewport.tlx /*+ (m_pParent->m_dX.real() * m_PreviewBlockSize)*/, m_P.imag());

	//Have we finished the last row of pixels?
	if (m_vCurrentPixel.Y >= m_vOrigin.Y + m_vImageSize.Y)
	{
		if (m_PreviewBlockSize == 1)//m_PreviewStartBlock.Y >= m_PreviewBlockSize)
		{
			//Perform any final cleanup
			m_pParent->m_Functions[eFF_Done](*this);

			//Stop the processing timer
			//m_ProcessingTime = m_ProcessTimer.Get();

			//Tell the fractal to begin rendering itself
			m_State = eState_Done;
		}
 		else
 		{
 			//m_PreviewStartBlock.X++;
 			m_PreviewBlockSize /= 2;
 
 			m_vCurrentPixel.Y = m_vOrigin.Y;//+ m_PreviewStartBlock.Y;
 			m_P	= TComplex(m_Viewport.tlx /*+ (m_pParent->m_dX.real() * m_PreviewBlockSize)*/, m_Viewport.tly);
 		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CFractal::Processor::UpdatePosition()
{
	//PROFILE_THIS(2);

	//Move the pixel over one
	//m_vCurrentPixel.X++;
	m_vCurrentPixel.X += m_PreviewBlockSize;

	//Move the complex point over by the delta X factor
	//m_P += m_pParent->m_dX;
	m_P.real(m_P.real() + (m_pParent->m_dX.real() * m_PreviewBlockSize));

	//Have we finished the last pixel in the row?
	if (m_vCurrentPixel.X >= m_vOrigin.X + m_vImageSize.X)
		ResetRow();
}

//////////////////////////////////////////////////////////////////////////

void CFractal::Processor::Update()
{
	//PROFILE_THIS(1);

	static const float k_MaxTimeSlice = 1.0f / 8.0f;
	static Timer tester(true);

	if (m_Data(m_vCurrentPixel - m_vOrigin) == 0.0)
	{
		IterateCurrentPoint();

		ColorCurrentPixel();
	}
// 	else
// 	{
// 		m_pParent->MeshThreadData(m_ThreadId);
// 		tester.Reset();
// 	}

	UpdatePosition();

	if (tester >= k_MaxTimeSlice)
	{
		m_pParent->MeshThreadData(m_ThreadId);
		tester.Reset();
	}
}

//////////////////////////////////////////////////////////////////////////

void CFractal::Processor::IterateCurrentPoint()
{
	int ret = -1;
	//PROFILE_THIS(1);

	{
		//PROFILE("Preiter", 8);
		//Call the pre-iter ONLY if we have one
		m_Functions[eFF_PreIter](*this);
	}

	for (m_CurrentIteration = 0; m_CurrentIteration < m_pParent->m_nMaxIterations; m_CurrentIteration++)
	{
		//PROFILE("iters", 9);
		ret = m_Functions[eFF_Iter](*this);

		if (ret == 0)	//REQUIRED
			break;
	}

	{
		//PROFILE("postiter", 10);
		//Call the post-iter ONLY if we have one
		m_Functions[eFF_PostIter](*this);
	}

	{
		//PROFILE("draw", 11);
		//REQUIRED
		m_color = m_Functions[eFF_Draw](*this);
	}
}

//////////////////////////////////////////////////////////////////////////

void CFractal::Processor::ColorCurrentPixel()
{
 	if (m_PreviewBlockSize > 1)
 	{
 		for (int y = 0; y < m_PreviewBlockSize; y++)
 			for (int x = 0; x < m_PreviewBlockSize; x++)
 				ColorPixel(m_vCurrentPixel + Vec2u(x, y));
 	}
	else
		ColorPixel(m_vCurrentPixel);
}

//////////////////////////////////////////////////////////////////////////

void CFractal::Processor::ColorPixel(const Vec2u& pos)
{
	int index = (pos.Y * m_pParent->m_Pitch) + (pos.X * 3);

 	m_pParent->m_img[index + 0] = GetBValue(m_color);
 	m_pParent->m_img[index + 1] = GetGValue(m_color);
 	m_pParent->m_img[index + 2] = GetRValue(m_color);
	//memcpy(&m_pParent->m_img[index], &m_color, 3);
}

//////////////////////////////////////////////////////////////////////////

void CFractal::Update()
{
	bool bDone = true;

	for (int i = 0; i < m_Processors.Size(); i++)
	{
		if (m_Processors[i].m_State != eState_Done)
			bDone = false;
	}

	if (bDone && m_State == eState_Processing)
	{
		MeshThreadData();
		m_State = eState_Rendering;
	}
}

//////////////////////////////////////////////////////////////////////////

void CFractal::Draw()
{
	for (int y = 0; y < m_vImageSize.Y; y++)
	{
		for ( int x = 0; x < m_vImageSize.X; x++ )
		{
			m_Processors[0].m_EscapeVal = m_Data(x ,y);

			m_Image.SetPixel(x, y, m_Functions[eFF_Draw](m_Processors[0]));
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CFractal::Render(CPaintDC& dc, int x, int y, int cx, int cy)
{
	CDC* cdc = CDC::FromHandle(m_Image.GetDC());

	dc.BitBlt(x, y, (cx ? cx : m_vImageSize.X), (cy ? cy : m_vImageSize.Y), cdc, 0, 0, SRCCOPY);
	//dc.StretchBlt(x, y, (cx ? cx : m_vImageSize.X), (cy ? cy : m_vImageSize.Y), cdc, 0, 0, m_vImageSize.X, m_vImageSize.Y, SRCCOPY);

	m_Image.ReleaseDC();

	if (m_State == eState_Rendering)
		m_State = eState_Done;
}

//////////////////////////////////////////////////////////////////////////

void CFractal::Render()
{
	CPaintDC dc(m_Window);
	CDC* cdc = CDC::FromHandle(m_Image.GetDC());

	dc.BitBlt(0, 0, m_vImageSize.X, m_vImageSize.Y, cdc, 0, 0, SRCCOPY);

	m_Image.ReleaseDC();
}

//////////////////////////////////////////////////////////////////////////

void CFractal::CreatePalette()
{
	m_Palette.Init(/*m_nMaxIterations*/1024);
}

//////////////////////////////////////////////////////////////////////////

void CFractal::SetupJulia(int x, int y)
{
	double	mx = x * m_dX.real() + m_Viewport.tlx,
			my = y * m_dY.imag() + m_Viewport.tly;

	m_bJulia = true;

	//m_J = TComplex(mx, my);

	m_Viewport = Rectd(-2.0, 2.0, 2.0, -2.0);
}

//////////////////////////////////////////////////////////////////////////

void CFractal::SetPalette(const CDynamicArray<Color>& pal, bool bRedraw)
{
	m_Palette.Copy(pal, m_nMaxIterations);

	if ( bRedraw )
		Draw();
}

//////////////////////////////////////////////////////////////////////////

void CFractal::SetCenterPoint( int x, int y, EZoomMode mode, EZoomSpecial spec )
{
	double	mx = x * m_dX.real() + m_Viewport.tlx,
			my = y * m_dY.imag() + m_Viewport.tly;

	SetCenterPoint(mx, my, mode, spec);
}

//////////////////////////////////////////////////////////////////////////

void CFractal::SetCenterPoint(double x, double y, EZoomMode mode, EZoomSpecial spec )
{
	m_vCenter = Vec2d(x, y);

	Recenter(mode, spec);
}

//////////////////////////////////////////////////////////////////////////

void CFractal::Recenter( EZoomMode mode, EZoomSpecial spec )
{
	double	fFactor = 1.0;

	switch (mode)
	{
	case eZoom_In:
		fFactor = 0.5 * (spec == eZoomSpec_Double ? 0.5 : 1.0);
		break;

	case eZoom_Out:
		fFactor = 2.0 * (spec == eZoomSpec_Double ? 2.0 : 1.0);
		break;
	}

	double	cx = m_Viewport.Width(),
			cy = m_Viewport.Height();

	m_Viewport.tlx = m_vCenter.X - cx * fFactor * 0.5;
	m_Viewport.tly = m_vCenter.Y + cy * fFactor * 0.5;
	m_Viewport.brx = m_vCenter.X + cx * fFactor * 0.5;
	m_Viewport.bry = m_vCenter.Y - cy * fFactor * 0.5;
}

//////////////////////////////////////////////////////////////////////////

void CFractal::MeshThreadData(int threadNum /* = -1 */)
{
	DWORD arrSize = m_Data.Size() / m_nThreadCount;

	if (threadNum == -1)
	{
		for (int t = 0; t < m_Processors.Size(); t++)
			memcpy(&m_Data[t * arrSize], &m_Processors[t].m_Data[0], arrSize);
	}
	else
		memcpy(&m_Data[threadNum * arrSize], &m_Processors[threadNum].m_Data[0], arrSize);
}

//////////////////////////////////////////////////////////////////////////