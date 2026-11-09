int gp : SasGlobal
<
  int3 SasVersion = {3,0,0};
>;

//////////////////////////////////////////////////////////////////////////

float4x4 matWorldViewProj;
float4x4 matViewProj;
float4x4 matWorld;

float	g_Time = 0.5f;

//////////////////////////////////////////////////////////////////////////

sampler Sprite : register(s0);

//////////////////////////////////////////////////////////////////////////

struct Complex
{
	float	real;
	float	imag;
};

//////////////////////////////////////////////////////////////////////////

static const float PI = 3.1415926535897932384626433832795;

static const float TWOPI = 6.283185307179586476925286766559;

//////////////////////////////////////////////////////////////////////////

float Sine(float freq, float base, float amp, float phase)
{
	return amp * sin(phase * freq * TWOPI) + base;
}

float Cosine(float freq, float base, float amp, float phase)
{
	return amp * cos(phase * freq * TWOPI) + base;
}

float Tangent(float freq, float base, float amp, float phase)
{
	return amp * tan(phase * freq * TWOPI) + base;
}

////////////////////////////////////////////////

struct PSInput
{
	float4	Color		: COLOR;
	float2	TexCoord	: TEXCOORD0;
	//float2	Pos			: VPOS;
};

struct PSOutput
{
    float4 Color		: COLOR;
};

///////////////////////////////////////////////////////////////

struct VSInput
{
	float4	Color		: COLOR;
	float4	Pos			: POSITION;
	float4	Normal		: NORMAL0;
	float2	TexCoord	: TEXCOORD0;
};

struct VSOutput
{
	float4	Color		: COLOR;
	float4	Pos			: POSITION;
	float2	TexCoord	: TEXCOORD0;
};

///////////////////////////////////////////////////////////////

float3 FromHSV(float hue, float saturation, float value)
{
	float3 colorVal = 0;

	if (saturation == 0)
	{
		colorVal.rgb = value;
		return colorVal;		
	}

	hue /= 60;

	int i = int(hue);

	float	f	= hue - i,
			p	= value * (1 - saturation),
			q	= value * (1 - saturation * f),
			t	= value * (1 - saturation * (1 - f));

	float	r, g, b;

	if (i == 0)
	{
		colorVal.r = value;
		colorVal.g = t;
		colorVal.b = p;
	}
	else if (i == 1)
	{
		colorVal.r = q;
		colorVal.g = value;
		colorVal.b = t;
	}
	else if (i == 2)
	{
		colorVal.r = p;
		colorVal.g = value;
		colorVal.b = t;
	}
	else if (i == 3)
	{
		colorVal.r = p;
		colorVal.g = q;
		colorVal.b = value;
	}
	else if (i == 4)
	{
		colorVal.r = t;
		colorVal.g = p;
		colorVal.b = value;
	}
	else
	{
		colorVal.r = value;
		colorVal.g = p;
		colorVal.b = q;
	}

	return colorVal;
}