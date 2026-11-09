#include "StdAfx.h"
#include "PlasmaFractal.h"

#include "MiscMath.h"
#include "MathFuncs.h"

using GLib::Rectd;

//////////////////////////////////////////////////////////////////////////

const float CPlasmaFractal::mk_DefaultRoughness				= 0.99f;
const float CPlasmaFractal::mk_DefaultRoughnessMultiplier	= 0.99f;
const float CPlasmaFractal::mk_DefaultUpperRange			= 0.25f;
const float CPlasmaFractal::mk_DefaultLowerRange			= -0.25f;

//////////////////////////////////////////////////////////////////////////

CPlasmaFractal::CPlasmaFractal(int seed				/* = -1 */, 
							   float roughness		/* = DEFAULT_ROUGHNESS  */, 
							   float roughnessMult	/* = DEFAULT_ROUGHNESS_MULTIPLIER  */, 
							   float upperRange		/* = DEFAULT_UPPER_RANGE  */, 
							   float lowerRange		/* = DEFAULT_LOWER_RANGE  */)
							   : CMidpointDisplacement()
{
	Initialize(seed, roughness, roughnessMult, upperRange, lowerRange);
}

CPlasmaFractal::~CPlasmaFractal(void)
{
}

//////////////////////////////////////////////////////////////////////////

void CPlasmaFractal::Initialize(int seed				/* = -1 */, 
						  float roughness		/* = DEFAULT_ROUGHNESS  */, 
						  float roughnessMult	/* = DEFAULT_ROUGHNESS_MULTIPLIER  */, 
						  float upperRange		/* = DEFAULT_UPPER_RANGE  */, 
						  float lowerRange		/* = DEFAULT_LOWER_RANGE  */)
{
	//CMidpointDisplacement::Initialize(NULL, seed, roughness, roughnessMult, upperRange, lowerRange);

	m_TempMaxIterations = -1;

	m_bRandomize =	false;
					//true;
}

//////////////////////////////////////////////////////////////////////////

Int32 CPlasmaFractal::Render(CPaintDC& dc, 
							int x /* = 0 */, int y /* = 0 */, 
							int cx /* = 0 */, int cy /* = 0 */)
{
	int rsize = m_Viewport.brx / pow(2.0, double(m_nMaxIterations));

	float val = 1.0f;
	CRect rect;

// 	CBrush bgbrush(color);
// 	CPen bgpen(PS_SOLID, 1, color);

	dc.SelectStockObject(BLACK_BRUSH/*&bgbrush*/);
	dc.SelectStockObject(NULL_PEN/*&bgpen*/);

	rect = CRect(0, 0, m_Viewport.brx + rsize + 50, m_Viewport.bry + rsize + 50);
	dc.Rectangle(&rect);

	for (DWORD y = 0; y < m_vImageSize.Y; y++)
	{
		for (DWORD x = 0; x < m_vImageSize.X; x++)
		{
			val = m_Data(x, y);

			//val += 0.5f;

			val = Clamp(val, 0.0f, 1.0f);

			BYTE v = int(val * 255.0f);

			if (m_nMaxIterations < 8)
			//if (m_Width < 256)
			{
				if (v > 0)
				{
					rect = CRect(x /** rsize*/, y /** rsize*/, (x /*+ 1*/) + rsize, (y /*+ 1*/) + rsize);
					//rect = CRect(x * rsize, y * rsize, x * rsize + rsize, y * rsize + rsize);
					COLORREF color = RGB(v, v, v);

					CBrush brush(color);
					CPen pen(PS_SOLID, 1, color);

					dc.SelectObject(&brush);
					dc.SelectObject(&pen);

					dc.Rectangle(&rect);
				}
				else
					dc.SetPixelV(x, y, 0x00FF00);
			}
 			else
 				dc.SetPixelV(x, y, RGB(v, v, v));
		}
	}

	//dc.BitBlt(0, 0, 512, 512, &m_Bitmap, 0, 0, SRCCOPY);

	return 0;
}

//////////////////////////////////////////////////////////////////////////

void CPlasmaFractal::Process(int numIterations /* = -1 */)
{
 	m_nMaxIterations = (numIterations == -1 ? m_nMaxIterations : numIterations);
 
 	m_VertexCount = m_vImageSize.X * m_vImageSize.Y;
 
 	m_CurrentIteration = 0;
 	
 	m_Data.Resize(m_vImageSize.X, m_vImageSize.Y, false);
 
  	for (DWORD y = 0; y < m_vImageSize.Y; y++)
  		for (DWORD x = 0; x < m_vImageSize.X; x++)
  			m_Data(x, y) = -1.0;
 
 	//Seed initial corners
  	m_Data(0,0)						= 0.0;
  	m_Data(m_vImageSize.X - 1, 0)			= 0.5;
  	m_Data(0, m_vImageSize.Y - 1)			= 0.5;
  	m_Data(m_vImageSize.X - 1, m_vImageSize.Y - 1)	= 1.0;

	//diamond_square(Width() - 1, 1, m_Roughness, m_RoughnessMultiplier);
	//Generate(m_P1, m_P2, m_P3, m_P4, eCorner_Center);
	Generate(m_Viewport, eCorner_Center);
}

////////////////////////////////////////////////////////////////
////
////	TODO!!!!:	Find a way so that different iterations
////				have the same vertices all across (i.e.
////				the 10th iteration looks like the 3rd,
////				just with the inbetweens different
////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

void CPlasmaFractal::Generate(Rectd area, ECorner corner)
{
//  	if (Distance<float>(p1, p2) < 1.0 || Distance<float>(p2, p3) < 1.0 || 
//  		Distance<float>(p1, p4) < 1.0 || Distance<float>(p3, p4) < 1.0)
//  		return;

	Vec2d	p1 = area.TopLeft(),
			p2 = area.TopRight(),
			p3 = area.BottomRight(),
			p4 = area.BottomLeft();

	//Do the center first
	Vec2d mid = p1.Midpoint(p3);//Vec2(p1.Midpoint(p2).X, p1.Midpoint(p4).Y);

	m_Roughness					*= m_RoughnessMultiplier;

	m_RandomRangeLow			*= m_Roughness;
	m_RandomRangeHigh			*= m_Roughness;

	double range = (m_bRandomize ? Random(m_RandomRangeHigh, m_RandomRangeLow) : 0.0);

	//ASSERT(range != 0.0);
	GetValueByVector(mid) = Clamp(GetMidPointValue(p1, p2, p3, p4) + range, 0.0, 1.0);

	//Make sure to only do the midpoint one level down
	if (m_TempMaxIterations != -1 && m_CurrentIteration >= m_TempMaxIterations)
		return;

 	if (m_CurrentIteration >= m_nMaxIterations)
 		return;

	//store temp variables for the parameters that will be altered within subdivisions
	float	roughness = m_Roughness;
	float	tempupper = m_RandomRangeHigh,
			templower = m_RandomRangeLow;

	//Get the midpoints of the sides
	Vec2d midT = p1.Midpoint(p2);
	Vec2d midR = p2.Midpoint(p3);
	Vec2d midB = p4.Midpoint(p3);
	Vec2d midL = p1.Midpoint(p4);

	//
	//Now the other points
	//
	double val = 0.0;

	//find the left value from the top left and bottom left
	val = GetMidPointValue(p1, p4, mid, Get4thPoint(midL, mid, eCorner_LeftSide));

	if (m_bRandomize)
		val += Random(m_RandomRangeHigh, m_RandomRangeLow);

	GetValueByVector(midL)	= Clamp(val, 0.0, 1.0);

	m_Roughness = roughness;
	m_RandomRangeHigh = tempupper;
	m_RandomRangeLow = templower;

	//find the top value from the top left and top right	
	val = GetMidPointValue(p1, p2, mid, Get4thPoint(midT, mid, eCorner_TopSide));

	if (m_bRandomize)
		val += Random(m_RandomRangeHigh, m_RandomRangeLow);

	//ASSERT(val != 0.0);
	GetValueByVector(midT)	= Clamp(val, 0.0, 1.0);

	m_Roughness = roughness;
	m_RandomRangeHigh = tempupper;
	m_RandomRangeLow = templower;

	//find the right value from the top right and bottom right
	val = GetMidPointValue(p2, p3, mid, Get4thPoint(midR, mid, eCorner_RightSide));

	if (m_bRandomize)
		val += Random(m_RandomRangeHigh, m_RandomRangeLow);

	GetValueByVector(midR)	= Clamp(val, 0.0, 1.0);

	m_Roughness = roughness;
	m_RandomRangeHigh = tempupper;
	m_RandomRangeLow = templower;

	//find the bottom value from the bottom left and bottom right
	val = GetMidPointValue(p4, p3, mid, Get4thPoint(midB, mid, eCorner_BottomSide));

	if (m_bRandomize)
		val += Random(m_RandomRangeHigh, m_RandomRangeLow);

	GetValueByVector(midB)	= Clamp(val, 0.0, 1.0);

	m_Roughness = roughness;
	m_RandomRangeHigh = tempupper;
	m_RandomRangeLow = templower;

	//
	//Now subdivide
	//
	m_CurrentIteration++;

	if (m_CurrentIteration >= m_nMaxIterations)
		return;

	int tempiteration = m_CurrentIteration;

	//Make sure to only do the midpoint one level down
// 	if (m_TempMaxIterations != -1 && m_CurrentIteration >= m_TempMaxIterations)
// 		return;

	Rectd view;
#if 1
	m_TempMaxIterations = tempiteration;
	//(Comment one of the corners out to get the Sierpinski triangle)
	//TopLeft
	view = Rectd(p1, mid);
	m_Roughness = roughness;
	m_CurrentIteration = tempiteration;
	m_RandomRangeHigh = tempupper;
	m_RandomRangeLow = templower;
	Generate(view, eCorner_TopLeft);

	//TopRight
	view = Rectd(midT, midR);
	m_Roughness = roughness;
	m_CurrentIteration = tempiteration;
	m_RandomRangeHigh = tempupper;
	m_RandomRangeLow = templower;
	Generate(view, eCorner_TopRight);

	//BottomRight
	view = Rectd(mid, p3);
	m_Roughness = roughness;
	m_CurrentIteration = tempiteration;
	m_RandomRangeHigh = tempupper;
	m_RandomRangeLow = templower;	
	Generate(view, eCorner_BottomRight);

	//BottomLeft
	view = Rectd(midL, midB);
	m_Roughness = roughness;
	m_CurrentIteration = tempiteration;
	m_RandomRangeHigh = tempupper;
	m_RandomRangeLow = templower;
	Generate(view, eCorner_BottomLeft);
#endif

	///////////////////////////////
#if 1
 	m_TempMaxIterations = -1;
	//(Comment one of the corners out to get the Sierpinski triangle)
	//TopLeft
	view = Rectd(p1, mid);
	m_Roughness = roughness;
	m_CurrentIteration = tempiteration;
	m_RandomRangeHigh = tempupper;
	m_RandomRangeLow = templower;
	Generate(view, eCorner_TopLeft);

	//TopRight
	view = Rectd(midT, midR);
	m_Roughness = roughness;
	m_CurrentIteration = tempiteration;
	m_RandomRangeHigh = tempupper;
	m_RandomRangeLow = templower;
	Generate(view, eCorner_TopRight);

	//BottomRight
	view = Rectd(mid, p3);
	m_Roughness = roughness;
	m_CurrentIteration = tempiteration;
	m_RandomRangeHigh = tempupper;
	m_RandomRangeLow = templower;	
	Generate(view, eCorner_BottomRight);

	//BottomLeft
	view = Rectd(midL, midB);
	m_Roughness = roughness;
	m_CurrentIteration = tempiteration;
	m_RandomRangeHigh = tempupper;
	m_RandomRangeLow = templower;
	Generate(view, eCorner_BottomLeft);
#endif
}

//////////////////////////////////////////////////////////////////////////

double CPlasmaFractal::AverageSquare(double c1, double c2, double c3, double c4)
{
	//ASSERT(c1 != -1.0f);ASSERT(c2 != -1.0f);ASSERT(c3 != -1.0f);ASSERT(c4 != -1.0f);
	return (c1 + c2 + c3 + c4) / 4.0;
}

//////////////////////////////////////////////////////////////////////////

double CPlasmaFractal::AverageTriangle(double c1, double c2, double c3)
{
	return (c1 + c2 + c3) / 3.0;
}

//////////////////////////////////////////////////////////////////////////

double CPlasmaFractal::GetMidPointValue(Vec2d p1, Vec2d p2, Vec2d p3, Vec2d p4)
{
	double	c1	= GetValueByVector(p1),
			c2	= GetValueByVector(p2),
			c3	= GetValueByVector(p3);

	double	c4	= -1.0;
	
	if (p4.X >= 0 && p4.X < m_vImageSize.X && p4.Y >= 0 && p4.Y < m_vImageSize.Y)
		c4 = GetValueByVector(p4);

	ASSERT(c1 != -1.0f);ASSERT(c2 != -1.0f);ASSERT(c3 != -1.0f);//ASSERT(c4 != -1.0f);
	
	if (c4 == -1.0f)
	{
		//ASSERT(p1.X == 0 || p1.X == p3.X || p1.Y == 0 || p1.Y == p3.Y);
		return AverageTriangle(c1, c2, c3);
	}
	else
		return AverageSquare(c1, c2, c3, c4);
}

//////////////////////////////////////////////////////////////////////////

double CPlasmaFractal::GetMidPointValue(Vec2d p1, Vec2d p2, Vec2d p3)
{
	return AverageTriangle(	GetValueByVector(p1),
							GetValueByVector(p2),
							GetValueByVector(p3));
}

//////////////////////////////////////////////////////////////////////////

double CPlasmaFractal::GetMidPointValue(Vec2d p1, Vec2d p2)
{
	double val = (GetValueByVector(p1) + GetValueByVector(p2)) / 2;

	return val;
}

//////////////////////////////////////////////////////////////////////////

Vec2d CPlasmaFractal::Get4thPoint(Vec2d mid, Vec2d distVec, ECorner corner)
{
	Vec2d coord = GetVertexCoordByVector(mid);

	Vec2d dvCoord = GetVertexCoordByVector(distVec);

	float dist = Distance(dvCoord, coord);// / 2.0;

// 	if (coord.X == 0.0 || coord.X == Width() ||
// 		coord.Y == 0.0 || coord.Y == Height())
	{
		switch (corner)
		{
		case eCorner_TopSide:
			coord.Y -= dist;
			break;

		case eCorner_RightSide:
			coord.X += dist;
			break;

		case eCorner_BottomSide:
			coord.Y += dist;
			break;

		case eCorner_LeftSide:
			coord.X -= dist;
			break;
		}
	}
	
	//Subtracting a negative is the same as adding a positive
// 	if (coord.X < 0)
// 		coord.X = fabs((Width() - 1) + coord.X);
// 	else if (coord.X >= Width())
// 		coord.X = fabs((Width() - 1) - coord.X);

// 	if (coord.Y < 0)
// 		coord.Y = fabs((Height() - 1) + coord.Y);
// 	else if (coord.Y >= Height())
// 		coord.Y = fabs((Height() - 1) - coord.Y);

	coord = GetVectorFromVertexCoord(coord);

	return coord;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CPlasmaFractal::diamond_square(int mapsize,int detail,float range,float rough)
{
	// mapsize and detail must be a power of 2. eg 256
	float ave;
	float r;
	int midx;
	int midy;
	int x;
	int y;
	int step;


	step=mapsize/detail;

	for(x=0;x<mapsize;x=x+step)
	{
		for(y=0;y<mapsize;y=y+step)
		{
			// Compute midpoint of square
			midx=x+step/2;
			midy=y+step/2;
			ave=(m_Data(x, y)+m_Data(x+step, y)+m_Data(x, y+step)+m_Data(x+step, y+step))/4;
			r=Random(range,-1*range);
			m_Data(midx, midy)=ave+r;			

			// Compute the four diamond mid-points
			// left
			midx=x;
			midy=y+step/2;
			ave=(m_Data(x, y)+m_Data(x, y+step)+m_Data(x+step/2, y+step/2))/3;
			if(x-step/2>=0)
			{
				ave=(ave+m_Data(x-step/2, y+step/2))/2;
			}
			r=Random(range,-1*range);
			m_Data(midx, midy)=ave+r;			

			// top
			midx=x+step/2;
			midy=y;
			ave=(m_Data(x, y)+m_Data(x+step, y)+m_Data(x+step/2, y+step/2))/3;
			if(y-step/2>0)
			{
				ave=(ave+m_Data(x+step/2, y-step/2))/2;
			}
			r=Random(range,-1*range);
			m_Data(midx, midy)=ave+r;			

			// right
			midx=x+step;
			midy=y+step/2;
			ave=(m_Data(x+step, y)+m_Data(x+step, y+step)+m_Data(x+step/2, y+step/2))/3;
			if(midx+step/2<mapsize)
			{
				ave=(ave+m_Data(midx+step/2, midy))/2;
			}
			r=Random(range,-1*range);
			m_Data(midx, midy)=ave+r;			

			// bottom
			midx=x+step/2;
			midy=y+step;
			ave=(m_Data(x, y+step)+m_Data(x+step, y+step)+m_Data(x+step/2, y+step/2))/3;
			if(midy+step/2<mapsize)
			{
				ave=(ave+m_Data(midx, midy+step/2))/2;
			}
			r=Random(range,-1*range);
			m_Data(midx, midy)=ave+r;			

		}
	}
	if(step>3)
	{
		diamond_square(mapsize,detail*2,range*expf(-1*rough),rough);
	}

}