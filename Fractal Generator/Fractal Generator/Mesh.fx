int gp : SasGlobal
<
  int3 SasVersion = {3,0,0};
>;

///////////////////////////////////////////////////////////////

float	time = 0.0;
float	timeOffset = 0.0f;

static const float TWOPI = 6.283185307179586476925286766559;

float4x4 matWorldViewProj: WORLDVIEWPROJECTION;

float4 g_Origin = {0, 0, 0, 1};

///////////////////////////////////////////////////////////////

struct VSInput
{
	float4	Color		: COLOR0;
	float4	Normal		: NORMAL0;
	float4	Pos			: POSITION0;
	float4	Tangent		: TANGENT0;
};

struct VSOutput
{
	float4	Color		: COLOR0;
	float4	Pos			: POSITION0;
	
};

//////////////////////////////////////////////////////////////////

struct PSInput
{
	float4	Color		: COLOR;
	float2	TexCoord	: TEXCOORD0;
	float2  Pos			: VPOS;
};

struct PSOutput
{
    float4 Color		: COLOR;
};

///////////////////////////////////////////////////////////////

float4 ConvertFromHSV(float hue, float sat, float value)
{
	float4 color;
	
	if (sat == 0)
	{
		color.rgb = value;
		return color;		
	}

	hue /= 60;

	int i = hue;

	float	f	= hue - i,
			p	= value * (1 - sat),
			q	= value * (1 - sat * f),
			t	= value * (1 - sat * (1 - f));

	float	r, g, b;

	if (i == 0)
	{
		r = value;
		g = t;
		b = p;
	}
	else if (i == 1)
	{
		r = q;
		g = value;
		b = t;
	}
	else if (i == 2)
	{
		r = p;
		g = value;
		b = t;
	}
	else if (i == 3)
	{
		r = p;
		g = q;
		b = value;
	}
	else if (i == 4)
	{
		r = t;
		g = p;
		b = value;
	}
	else
	{
		r = value;
		g = p;
		b = q;
	}

	color.r = r;
	color.g = g;
	color.b = b;
	color.a = 1.0f;
	
	return color;
}

///////////////////////////////////////////////



///////////////////////////////////////////////////////////////

PSOutput PS_Default(PSInput In)
{
	PSOutput Out = (PSOutput) 0;
	
	/*
	float y;
	float yval = 0.0f;
	
	float hue;	
	float4 tempcolor;
	
	tempcolor = float4(y, y, y, 1.0f);
	
	hue = (y + 225) / 360.0f;
	hue = modf(hue, yval);
	hue *= 360.0f;
	
	Out.Color = ConvertFromHSV(hue, 1.0f, y);
	*/
	
	Out.Color = In.Color;
	
	return Out;
}

VSOutput VS_Default(VSInput In)
{
	VSOutput	Out = (VSOutput) 0;

	float hue;
	//float4 tempcolor = float4(y, y, y, 1.0f);
	
	float	x = In.Pos.x,
			z = In.Pos.z;
	
	if (x == 0 && z == 0)
		hue = 0;
	else
		hue = atan2(x, z) * 100.0f;
	
	float dist = 0.5f;//distance(In.Pos, g_Origin);
	
	Out.Color = ConvertFromHSV(hue, 1.0f, dist);
	
	//Finally project
	//In.Pos.y = 0;
	Out.Pos = mul(In.Pos, matWorldViewProj);
	
	return Out;
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

technique MeshFunction
{
	pass P0
	{
		VertexShader	= compile vs_3_0 VS_Default();
		//PixelShader		= compile ps_3_0 PS_FreeParticle();
	}
}