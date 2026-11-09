#include "StdAfx.h"
#include ".\fractal.h"
#include "math.h"

#include "ProfileSample.h"

CFractal frac;
CFractal julia(true);

bool CFractal::m_bSmoothShading = true;

CFractal::CFractal(bool bJulia) : 
	m_bPreview( true ),
	m_bJulia( bJulia )
{
	if (m_bJulia)
	{
		m_ncX	= DEFAULT_WIDTH;
		m_ncY	= DEFAULT_HEIGHT;

		m_fTLx = -2.0;
		m_fTLy = 2.0;
		m_fBRx = 2.0;
		m_fBRy = -2.0;
	}
	else
	{
		m_ncX	= DEFAULT_WIDTH;
		m_ncY	= DEFAULT_HEIGHT;

		m_fTLx = -2.0;
		m_fTLy = 2.0;
		m_fBRx = 2.0;
		m_fBRy = -2.0;

		/*m_fTLx = -1.0;
		m_fTLy = 1.125;
		m_fBRx = 2.0;
		m_fBRy = -1.125;*/
	}

	ZeroMemory(m_Data, sizeof(int) * DEFAULT_WIDTH * DEFAULT_HEIGHT);

	m_State = eState_Uninitialized;

	m_nMaxIterations	= MAX_ITERATIONS;
	m_fBailValue		= 4.0;

	m_Image.Create(m_ncX, -m_ncY, 24);

	MakePal();
}

CFractal::~CFractal(void)
{
	m_State = eState_Uninitialized;

	m_Image.Destroy();
}

void CFractal::SetSize(int cx, int cy)
{
	m_ncX = abs(cx);
	m_ncY = abs(cy);

	CreateBitmap();
}

void CFractal::CreateBitmap()
{
	m_Image.Destroy();

	m_Image.Create(m_ncX, -m_ncY, 24);
}

void CFractal::Init()
{
	m_nX	= 0;
	m_nY	= 0;

	m_fcX	= m_fBRx - m_fTLx;
	m_fcY	= m_fBRy - m_fTLy;

	m_fdX	= m_fcX / m_ncX;
	m_fdY	= m_fcY / m_ncY;

	m_fPy	= m_fTLy;
	m_fPx	= m_fTLx;

	m_img = (BYTE *) m_Image.GetBits();
	m_Pitch = m_Image.GetPitch();

	m_State	= eState_Running;
}

bool CFractal::UpdateColumn()
{
	//Move the pixel down one
	m_nY++;

	//Move the complex point down
	m_fPy += m_fdY;

	//Reset the row info
	m_nX	= 0;
	m_fPx	= m_fTLx;

	if (m_nY >= m_ncY)
	{
		m_State = eState_Rendering;
		return false;
	}
}

bool CFractal::UpdateRow()
{
	//Move the pixel over one
	m_nX++;

	//Move the complex point over
	m_fPx += m_fdX;

	if (m_nX >= m_ncX)
	{
		UpdateColumn();
		return false;
	}
}

void CFractal::Update()
{
	static Timer tester;
	static const float k_MaxTimeSlice = 1.0f / 4;

	tester.Reset();

	while (m_State == eState_Running)
	{
		IterateCurrentPoint();
		UpdateRow();

		if (tester >= k_MaxTimeSlice)
			return;
	}
}

void CFractal::IterateCurrentPoint()
{
	m_ind = 0;
	m_mu = 0;
	m_i = 0;

	if (m_bJulia)
	{
		m_fCx = m_fJx;
		m_fCy = m_fJy;
	}
	else
	{
		m_fCx = m_fPx;
		m_fCy = m_fPy;
	}

	m_fZx = m_fPx;
	m_fZy = m_fPy;

	for (m_i = 0; m_i < m_nMaxIterations; m_i++)
	{
		m_x2 = m_fZx;
		m_y2 = m_fZy;

		m_orbit =	(m_x2 * m_x2) + (m_y2 * m_y2);
					//(m_x2 * m_y2) + (m_x2 * m_y2);
					//m_x2 * m_y2;
					//m_x2 + m_y2;
					//m_x2 / m_y2;
					//m_x2 - m_y2;
					//(m_x2 * m_x2) - (m_y2 * m_y2);
					//(m_x2 * m_x2) * (m_y2 * m_y2);
					//(m_x2 * m_x2) / (m_y2 * m_y2);

		if (m_orbit > m_fBailValue)
			break;

// 		m_fZx = m_x2 * m_x2 - m_y2 * m_y2 - m_fCx;
// 		m_fZy = 2.0 * m_x2 * m_y2 - m_fCy;

		m_fZx = m_x2 * m_x2 - m_y2 * m_y2 - (m_fCx / (m_fCx * m_fCx + m_fCy * m_fCy));
		m_fZy = 2.0 * m_x2 * m_y2 - (-m_fCy / (m_fCx * m_fCx + m_fCy * m_fCy));
	}

	for (m_extra = 0; m_extra < 2; m_extra++)
	{
		m_x2 = m_fZx;
		m_y2 = m_fZy;

		m_fZx = m_x2 * m_x2 - m_y2 * m_y2 - m_fCx;
		m_fZy = 2.0 * m_x2 * m_y2 - m_fCy;

		m_i++;
	}

	m_orbit = sqrt((m_x2 * m_x2) + (m_y2 * m_y2));

	m_mu = m_i + 1 - ((log(log(m_orbit))) / log(2.0));

	m_Data[m_nY * m_ncX + m_nX] = m_mu;

	ColorCurrentPixel();
}

void CFractal::ColorCurrentPixel()
{
	m_color = ColorLookup(m_mu, m_i);

	m_ind = (m_nY * m_Pitch) + (m_nX * 3);

	m_img[m_ind + 0] = GetBValue(m_color);
	m_img[m_ind + 1] = GetGValue(m_color);
	m_img[m_ind + 2] = GetRValue(m_color);
}

int CFractal::ColorLookup(float mu, int i)
{
	if (m_bSmoothShading)
	{
		mu = fabsf(mu);
	
		static int index;
		static float delta;

		index = int(mu);
		delta = mu - index;

		i = index;

		return m_Palette.GetColorGradient(i, delta);
	}
	else
		return m_Palette.GetColor(int(mu));
}

void CFractal::Draw()
{
	for (int y = 0; y < m_ncY; y++)
	{
		for ( int x = 0; x < m_ncX; x++ )
		{
			m_Image.SetPixel(x, y, ColorLookup(m_Data[y * m_ncX + x]));
		}
	}
}

void CFractal::Render(CPaintDC& dc, int x, int y, int cx, int cy)
{
	//PRender.Begin();

	CDC* cdc = CDC::FromHandle(m_Image.GetDC());

	dc.BitBlt( x, y, (cx ? cx : m_ncX), (cy ? cy : m_ncY), cdc, 0, 0, SRCCOPY );

	m_Image.ReleaseDC();

	//m_State = eState_Done;

	//PRender.End();
}

void CFractal::MakePal()
{
	m_Palette.Init(1024/*m_nMaxIterations*/);
}

void CFractal::SetupJulia(int x, int y)
{
	double	mx = x * m_fdX + m_fTLx,
			my = y * m_fdY + m_fTLy;

	m_bJulia = true;

	m_fJx = mx;
	m_fJy = my;

	m_fTLx = -2.0;
	m_fTLy = 2.0;
	m_fBRx = 2.0;
	m_fBRy = -2.0;
}

void CFractal::SetPalette(const CDynamicArray<Color>& pal, bool bRedraw)
{
	m_Palette.Copy(pal, m_nMaxIterations);

	if ( bRedraw )
	{
		Draw();
	}
}

void CFractal::SetCenterPoint( int x, int y, EZoomMode mode, EZoomSpecial spec )
{
	double	mx = x * m_fdX + m_fTLx,
			my = y * m_fdY + m_fTLy;

	SetCenterPoint( mx, my, mode, spec );
}

void CFractal::SetCenterPoint(double x, double y, EZoomMode mode, EZoomSpecial spec )
{
	m_fMx = x;
	m_fMy = y;

	Recenter( mode, spec );
}

void CFractal::Recenter( EZoomMode mode, EZoomSpecial spec )
{
	double	fFactor = 1.0;

	switch ( mode )
	{
	case eZoom_In:
		fFactor = 0.5 * ( spec == eZoomSpec_Double ? 0.5 : 1.0);
		break;

	case eZoom_Out:
		fFactor = 2.0 * ( spec == eZoomSpec_Double ? 2.0 : 1.0);
		break;
	}

	double	cx = m_fBRx - m_fTLx,
		cy = m_fBRy - m_fTLy;

	m_fTLx = m_fMx - cx * fFactor * 0.5;
	m_fTLy = m_fMy - cy * fFactor * 0.5;
	m_fBRx = m_fMx + cx * fFactor * 0.5;
	m_fBRy = m_fMy + cy * fFactor * 0.5;
}