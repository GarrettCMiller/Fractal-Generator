#pragma once

#include "Fractal.h"

#include "DynamicArray.h"
#include "Vec.h"

static const float DEFAULT_ROUGHNESS =				0.75f;
static const float DEFAULT_ROUGHNESS_MULTIPLIER	=	0.95f;
static const float DEFAULT_UPPER_RANGE =			1.0f;
static const float DEFAULT_LOWER_RANGE =			0.0f;

class CMidpointDisplacement : public CFractal
{
public:
	CMidpointDisplacement(int seed = -1,
						float roughness = DEFAULT_ROUGHNESS,
						float roughnessMult = DEFAULT_ROUGHNESS_MULTIPLIER,
						float upperRange = DEFAULT_UPPER_RANGE, 
						float lowerRange = DEFAULT_LOWER_RANGE);

	virtual ~CMidpointDisplacement(void);

	static const int	mk_MaxResolution = 200;

	virtual void Render(CPaintDC& dc, int x = 0, int y = 0, int cx = 0, int cy = 0);

	virtual void Process(int numIterations = -1);

	virtual void Init(	CWnd* parentFrame = NULL,
						int seed = -1,
						float roughness = DEFAULT_ROUGHNESS,
						float roughnessMult = DEFAULT_ROUGHNESS_MULTIPLIER,
						float upperRange = DEFAULT_UPPER_RANGE, 
						float lowerRange = DEFAULT_LOWER_RANGE);

	enum ESide
	{
		eSide_Left,
		eSide_Right,

		eSide_Root,
	};

	int LineCount() const { return m_LineCount; }

	int VertexCount() const { return m_VertexCount; }

	int CurrentIteration() const { return m_CurrentIteration; }

	float Roughness() const { return m_Roughness; }
	void Roughness(float val) { m_Roughness = val; }

	float RoughnessMultiplier() const { return m_RoughnessMultiplier; }
	void RoughnessMultiplier(float val) { m_RoughnessMultiplier = val; }

	float RandomRangeLow() const { return m_RandomRangeLow; }
	void RandomRangeLow(float val) { m_RandomRangeLow = val; }

	float RandomRangeHigh() const { return m_RandomRangeHigh; }
	void RandomRangeHigh(float val) { m_RandomRangeHigh = val; }

	bool Randomize() const { return m_bRandomize; }
	void Randomize(bool val) { m_bRandomize = val; }
	
	int Seed() const { return m_Seed; }
	virtual void Seed(int val) 
	{ 
		m_Seed = val;
		if (m_Seed == -1)
			srand(static_cast<UINT>(time(NULL)));
		else
			srand(m_Seed);
	}

	bool IsValid() const { return m_bValid; }

	double RandomizeMidpoint(Vec2d p1, Vec2d p2);

protected:
	bool	m_bValid;
	
	bool	m_bRandomize;

	float	m_ParentVal;

	int		m_LineCount;

	int		m_VertexCount;

	int		m_Seed;

	int		m_CurrentIteration;
	
	float	m_Roughness;
	float	m_RoughnessMultiplier;

	float	m_RandomRangeLow;
	float	m_RandomRangeHigh;
	
protected:
	virtual void Generate(GLib::Rectd area, ESide side, double parentVal);
};
