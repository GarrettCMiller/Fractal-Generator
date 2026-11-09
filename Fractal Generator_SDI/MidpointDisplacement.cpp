#include "StdAfx.h"
#include "MidpointDisplacement.h"

#include "MiscMath.h"

using namespace GLib;

CMidpointDisplacement::CMidpointDisplacement(int seed /* = -1 */, 
											 float roughness /* = DEFAULT_ROUGHNESS */, 
											 float roughnessMult /* = DEFAULT_ROUGHNESS_MULTIPLIER */, 
											 float upperRange /* = DEFAULT_UPPER_RANGE */, 
											 float lowerRange /* = DEFAULT_LOWER_RANGE */)
											 :	CFractal(), 
												m_bValid(false), m_bRandomize(true)
{
	//Init(seed, roughness, roughnessMult, upperRange, lowerRange);
}

CMidpointDisplacement::~CMidpointDisplacement(void)
{
}

//////////////////////////////////////////////////////////////////////////

void CMidpointDisplacement::Init(CWnd* parentFrame /* = NULL */, 
								 int seed /* = -1 */, 
								 float roughness /* = DEFAULT_ROUGHNESS */, 
								 float roughnessMult /* = DEFAULT_ROUGHNESS_MULTIPLIER */, 
								 float upperRange /* = DEFAULT_UPPER_RANGE */, 
								 float lowerRange /* = DEFAULT_LOWER_RANGE */)
{
	//CFractal::Init(parentFrame);

	Seed(seed);

	m_Viewport				= Rectd(0, -1, 1, 1);
	
	m_Roughness				= roughness;
	m_RoughnessMultiplier	= roughnessMult;

	m_RandomRangeHigh		= upperRange;
	m_RandomRangeLow		= lowerRange;

	m_Roughness				= roughness;

	//This fractal is now ready
	m_bValid				= true;
}

//////////////////////////////////////////////////////////////////////////

void CMidpointDisplacement::Render(CPaintDC& dc, 
								   int x /* = 0 */, int y /* = 0 */, 
								   int cx /* = 0 */, int cy /* = 0 */)
{
	static const COLORREF	k_PtColor = 0x0000FF,
							k_FGColor = 0xFFFFFF,
							k_BGColor = 0x555555;

	CBrush brush(k_BGColor);
	CPen pen(PS_SOLID, 1, k_FGColor);

	Vec2d p1 = m_Viewport.Left();
	Vec2d p2 = m_Viewport.Right();
	Vec2d p3;

	p1 *= m_vImageSize;
	p2 *= m_vImageSize;

	p1.Y += m_vImageSize.Y / 2;
	p2.Y += m_vImageSize.Y / 2;

	p3.Y = p1.Y;

	float dx = static_cast<float>(m_vImageSize.X) / m_Data.Size();

	dc.SelectObject(&pen);

	//if (m_nMaxIterations <= 11)
	{
		dc.MoveTo(p1);

		for (UINT i = 1; i < m_Data.Size(); i++)
		{
			p3 = Vec2d(i * dx, p3.Y + -m_Data[i] * m_vImageSize.Y * 0.125);
			dc.LineTo(p3);
		}

		dc.LineTo(p2);
		dc.LineTo(p1);
	}

 	for (UINT i = 0; i < m_Data.Size(); i++)
	{
		p3 = Vec2i(i * dx, p3.Y + -m_Data[i] * m_vImageSize.Y * 0.125);
		dc.SetPixelV(p3, k_PtColor);
	}
}

//////////////////////////////////////////////////////////////////////////

void CMidpointDisplacement::Process(int numIterations /* = -1 */)
{
	m_nMaxIterations = (numIterations == -1 ? m_nMaxIterations : numIterations);

	m_LineCount = static_cast<int>(pow(2.0f, m_nMaxIterations));

	m_VertexCount = m_LineCount + 1;

	m_CurrentIteration = 1;

	m_Data.Resize(m_VertexCount, 1, false);

	m_ParentVal = 0.5f;

	Generate(m_Viewport, eSide_Root, 0);
}

//////////////////////////////////////////////////////////////////////////

void CMidpointDisplacement::Generate(Rectd area, ESide side, double parentVal)
{
// 	if (Distance<double>(p1, p2) < 1.0)
// 		return;

	Vec2d p1 = area.Left();
	Vec2d p2 = area.Right();

	Vec2d mid = p1.Midpoint(p2);

	double disp = 0.0;
	
	if (m_bRandomize)
		disp = RandomizeMidpoint(p1, p2);

	mid.Y += disp;

	//ASSERT(mid.Y < 1.0);
// 	if (mid.Y > 1.0)
// 		mid.Y = 1.0;

	double index = 0;
	
	//Get the lowest common denominator for the fractional math
	double LCD = 1.0 / (pow(2.0, m_CurrentIteration));

	switch (side)
	{
	case eSide_Left:
		index = parentVal - LCD;
		break;

	case eSide_Right:
		index = parentVal + LCD;
		break;

	case eSide_Root:
		index = 0.5;
		break;
	}

	UINT dataIndex = static_cast<UINT>(index * m_Data.Size());

	//ASSERT(m_Data[dataIndex] == 0.0);
	m_Data[dataIndex] = disp;
	double newParentVal = index;

	if (m_CurrentIteration >= m_nMaxIterations)
		return;
	
	m_ParentVal = parentVal;

	float roughness = m_Roughness;
	float temprangeupper = m_RandomRangeHigh,
			temprangelower = m_RandomRangeLow;

	int tempitercount = ++m_CurrentIteration;
	Generate(Rectd(p1, mid), eSide_Left, newParentVal);

	m_Roughness = roughness;
	m_RandomRangeHigh = temprangeupper;
	m_RandomRangeLow = temprangelower;

	m_CurrentIteration = tempitercount;
	Generate(Rectd(mid, p2), eSide_Right, newParentVal);

	m_Roughness = roughness;
	m_RandomRangeHigh = temprangeupper;
	m_RandomRangeLow = temprangelower;
}

//////////////////////////////////////////////////////////////////////////
double CMidpointDisplacement::RandomizeMidpoint(Vec2d p1, Vec2d p2)
{
	//Decrease roughness
	m_Roughness *= m_RoughnessMultiplier;

	m_RandomRangeLow	*= m_Roughness;
	m_RandomRangeHigh	*= m_Roughness;

	double range = (m_bRandomize ? Random(m_RandomRangeHigh, m_RandomRangeLow) : 0.0);

	////////////////////////////////////////////////////////////////
	////
	////	TODO!!!!:	Find a way so that different iterations
	////				have the same vertices all across (i.e.
	////				the 10th iteration looks like the 3rd,
	////				just with the inbetweens different
	////////////////////////////////////////////////////////////////

	return range;
}