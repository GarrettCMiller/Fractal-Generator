#include "StdAfx.h"
#include "ShaderFractal.h"

#include "SpriteMgr.h"
#include "EffectMgr.h"

//////////////////////////////////////////////////////////////////////////

String DefTexturePath = "texture.bmp";

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CShaderFractal::CShaderFractal(const char* strName, Bool bCreateStates /* = true */) : 
			m_pShader(NULL)
		,	m_hTech(NULL)
		,	m_pThread(NULL)
		,	m_bDirty(true)
{
	m_eType = eFT_Invalid;

	m_vCenter = //Vec2(-0.23171622, 0.22686760);
				Vec2(2.0f, 0.5f);
	m_vDim = k_v2One * 1.0f;
	m_vDim.X *= 4;
}

CShaderFractal::~CShaderFractal(void)
{
	SAFE_DELETE(m_pShader);
}

//////////////////////////////////////////////////////////////////////////

Bool CShaderFractal::ResetInitialTexture(String path)
{
	if (path.empty())
		path = DefTexturePath;

	return (m_ImageSet.m_Output.LoadTexture(path)	!= 0 ||
			m_ImageSet.m_Input.LoadTexture(path)	!= 0);
}

//////////////////////////////////////////////////////////////////////////

void CShaderFractal::RenderToScreen()
{
	if (IsDirty())
		g_D3D->Clear(Color::km_Yellow);
	
	g_D3D->GetDevice()->SetTexture(0,		m_ImageSet.m_Output);
	g_D3D->GetDevice()->SetStreamSource(0,	m_ImageSet.m_Input, 0, sizeof(CVertex));
	g_D3D->GetDevice()->SetIndices(			m_ImageSet.m_Input);

	RenderPrim();
}

Int32 CShaderFractal::Draw()
{
	/*if (!IsDirty())
		return 1;*/

	static Timer t(true);

	static Bool bFirst = true;

	if (!m_bIsShader)
		return 1;

	if (bFirst)
	{
		bFirst = false;
		m_pShader->Shader->SetMatrix("matViewProj", &g_D3D->GetViewProj());
	}
	

	UInt32 np = 0;
	
	if (m_bAnimate)
		m_pShader->Shader->SetFloat("g_Time", t);

	m_pShader->Shader->SetFloat("g_fEpsilon", m_fEpsilon);
	m_pShader->Shader->SetFloat("g_fDeltaHue", m_fDeltaHue);
	
	m_pShader->Shader->SetInt("g_nMaxIters", m_nMaxIterations);
	m_pShader->Shader->SetInt("g_nCutoff", m_nCutoff);

	//m_pShader->Shader->SetBool("g_bAnimate", m_bAnimate);
	
	m_pShader->Shader->Begin(&np, 0);

	m_pShader->Shader->BeginPass(0);
	RenderPrim(true);
	m_pShader->Shader->EndPass();

	m_pShader->Shader->End();

	//SetDirty(false);

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CShaderFractal::Initialize(CWnd* parentFrame /* = NULL */, TArgumentArray* pArgs /* = NULL */)
{
	if (m_ImageSet.Init(*this, DefTexturePath) != 0)
		return -1;

	TArgumentArray& args = *pArgs;
	Pointer pInitStruct = args[0];

	InitStruct& is = *(reinterpret_cast<InitStruct*>(pInitStruct));

	if (m_bIsShader)
	{
		m_pShader = new Effect;

		g_EffectMgr->LoadEffect("Mandelbrot.fx", m_pShader);

		m_pShader->Shader->SetTechnique("Main");

		m_pShader->Shader->GetInt("g_nMaxIters",	&m_nMaxIterations);
		m_pShader->Shader->GetInt("g_nCutoff",		&m_nCutoff);
		m_pShader->Shader->GetFloat("g_fEpsilon",	&m_fEpsilon);
		m_pShader->Shader->GetFloat("g_fDeltaHue",	&m_fDeltaHue);

		//m_pShader->Shader->SetTexture("pTexture", g_SpriteMgr->GetTexture(g_SpriteMgr->LoadSprite("fractal_texture.bmp")));
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//	Image Set Stuff
//
//////////////////////////////////////////////////////////////////////////

//Pass in reference to assure we get a valid parent
Int32 CShaderFractal::ImageSet::Init(CShaderFractal& pParent, String texname/* = ""*/)
{
	m_pParent = &pParent;

	int ret = 0;

	Vec2u vSize = Vec2u(m_pParent->m_vImageSize.X, m_pParent->m_vImageSize.Y);

	//Non-zero = error
	if (ret = m_Input.Init(pParent, vSize, texname))
 	{
 		//ASSERT(0);
 		return ret;
 	}

	return m_Output.Init(pParent, vSize, texname);
	//return ret;
}

//////////////////////////////////////////////////////////////////////////

Int32 CShaderFractal::ImageSet::Reset()
{
	Int32 ret = 0;
	if ((ret = m_Input.Reset()) != 0)	return ret;
	return m_Output.Reset();
}