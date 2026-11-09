//////////////////////////////////////////////////////////////////////////

#include "Globals.fx"

//////////////////////////////////////////////////////////////////////////

bool g_bShowXi		= true;

int g_nMaxIters		= 5000;
int g_nCutoff		= 0;

///////////

texture pTexture <>;

sampler sTexture = sampler_state
{
	Texture = <pTexture>;
	
	AddressU = WRAP;
	AddressV = WRAP;
}; 

float deltacoord = 0.0f;

//////

float g_fEpsilon	= 2.0f;
float g_fDeltaHue	= 22.222222222222222222222222222222;

//////////////////////////////////////////////////////////////////////////

PSOutput PS_Bifurcation(PSInput In)
{
	PSOutput Out = (PSOutput) 0;

	Out.Color.a = 0.0f;

	float hue = 0.0f;
	
	int i = 0;
	
	float xi = 0.99999911f;//Sine(1.0f, 0.5f, 0.5f, g_Time);
	float x = xi, xt = 0.0;
	
	const float a = In.TexCoord.x;
	const float y = 1.0 - In.TexCoord.y;
	const float dy = ddy(In.TexCoord.y) * g_fEpsilon;

	//if (g_bShowXi && abs(x - In.TexCoord.y/*a - 3.5699456*/) <= dy)
	//{
	//	Out.Color.a = 1.0f;
	//	Out.Color.rgb = 1.0f;
	//}
	//else
	//{	
		for (i = 0; i < g_nCutoff; i++)
		{
			// x = ax(1 - x)
			xt = a * x * (1 - x);
			
			// E = E0 * sqrt(1 + (v/c)^2)
			//xt = x * sqrt(1 + (a * a / 25));

			x = xt;
		}

		for (; i < g_nMaxIters; i++)
		{
			// x = ax(1 - x)
			xt = a * x * (1 - x);

			// E = E0 * sqrt(1 + (v/c)^2)
			//xt = x * sqrt(1 + (a * a / 25));

			x = xt;

			if (abs(x - y) <= dy)
				hue += g_fDeltaHue;
		}

		hue -= 60;

		/*if (hue >= 360)
			hue = 300.0f;*/

		if (hue > -60)
			Out.Color.a = 1.0f;

		Out.Color.rgb = FromHSV(hue, 1.0f, 1.0f);
	//}

	//deltacoord += 0.01666666667f;

	return Out;
}

//////////////////////////////////////////////////////////////////////////

VSOutput VS_Main(VSInput In)
{
	VSOutput Out = (VSOutput) 0;
	
	Out.Pos = mul(In.Pos, matViewProj);
	
	Out.TexCoord = In.TexCoord;

	//g_Time += 0.00416666667f;

	/*if (g_Time > 100.0f)
		g_Time = 0.0f;*/

	return Out;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

technique Main
{
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Main();
		PixelShader = compile ps_3_0 PS_Bifurcation();
	}
}