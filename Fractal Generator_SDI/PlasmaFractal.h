#pragma once

#include "midpointdisplacement.h"

#include "2dDynamicArray.h"

class CPlasmaFractal : public CMidpointDisplacement
{
public:
	CPlasmaFractal(	int seed			= -1, 
					float roughness		= DEFAULT_ROUGHNESS , 
					float roughnessMult	= DEFAULT_ROUGHNESS_MULTIPLIER , 
					float upperRange	= DEFAULT_UPPER_RANGE , 
					float lowerRange	= DEFAULT_LOWER_RANGE );
	
	virtual ~CPlasmaFractal(void);

	virtual void Render(CPaintDC& dc, int x = 0, int y = 0, int cx = 0, int cy = 0);

	virtual void Process(int numIterations = -1);

	virtual void Init(	int seed			= -1, 
						float roughness		= mk_DefaultRoughness , 
						float roughnessMult	= mk_DefaultRoughnessMultiplier, 
						float upperRange	= mk_DefaultUpperRange , 
						float lowerRange	= mk_DefaultLowerRange );

	enum ECorner
	{
		eCorner_TopLeft,
		eCorner_TopRight,
		eCorner_BottomRight,
		eCorner_BottomLeft,

		eNumCorners,

		eCorner_TopSide = 0,
		eCorner_RightSide,
		eCorner_BottomSide,
		eCorner_LeftSide,

		eNumSides,

		eCorner_Center
	};

public:
	static const DWORD mk_DefaultWidth	= 17;
	static const DWORD mk_DefaultHeight	= 17;
	
	static const float mk_DefaultRoughness;
	static const float mk_DefaultRoughnessMultiplier;
	static const float mk_DefaultUpperRange;
	static const float mk_DefaultLowerRange;

protected:

	int m_TempMaxIterations;

protected:
	void Generate(GLib::Rectd area, ECorner corner);

	Vec2d	GetVertexCoordByVector(Vec2d p)
	{
		float	xp = p.X,// / m_P3.X * double(Width() - 1),
				yp = p.Y;// / m_P3.Y * double(Height() - 1);
		return Vec2(xp, yp);
	}

	Vec2d	GetVectorFromVertexCoord(Vec2d p)
	{
		float	xp = p.X,// * m_P3.X / double(Width() - 1),
				yp = p.Y;// * m_P3.Y / double(Height() - 1);
		return Vec2(xp, yp);
	}

	double& GetValueByVector(Vec2d p)
	{
		Vec2 v = GetVertexCoordByVector(p);
// 		if (m_Values(v.X, v.Y) == -1)
// 			m_Values(v.X, v.Y) = 0.5;
		return m_Data(v.X, v.Y);
	}

	double AverageSquare(double c1, double c2, double c3, double c4);
	double AverageTriangle(double c1, double c2, double c3);

	Vec2d   Get4thPoint(Vec2d mid, Vec2d distVec, ECorner corner);

	double GetMidPointValue(Vec2d p1, Vec2d p2, Vec2d p3, Vec2d p4);
	double GetMidPointValue(Vec2d p1, Vec2d p2, Vec2d p3);
	double GetMidPointValue(Vec2d p1, Vec2d p2);

	void diamond_square(int mapsize, int detail, float range, float rough);
};
