#pragma once

#include "D3DWrapper.h"
#include "Fractal.h"

#include "EffectMgr.h"

//////////////////////////////////////////////////////////////////////////

class CShaderFractal : public CFractal
{
public:
	struct ImageSet
	{
		ImageSet() : m_pParent(NULL) {	}
		~ImageSet() {	}

		//Pass in reference to assure we get a valid parent
		Int32 Init(CShaderFractal& pParent, String texname = "");
		Int32 Reset();

		CFractalImage	m_Input;
		CFractalImage	m_Output;

		CShaderFractal*	m_pParent;
	};
public:
	CShaderFractal(void)	{	}
	CShaderFractal(const char* strName, Bool bCreateStates = true);

	virtual ~CShaderFractal(void);

	struct InitStruct : public CFractal::InitStruct
	{
		Bool bIsShader;
	};

	//TODO: MOVE TO COMPLEX
	Vec2d m_vCenter;
	Vec2d m_vDim;

	//HACK TEMP
	void ResetVerts()
	{
		m_ImageSet.m_Input.InitVertices(false);
		SetDirty();
	}

	void MoveUV(Vec2 dif)
	{
		m_vCenter += dif;

		m_ImageSet.m_Output.m_Vertices[3].tu = m_vCenter.X - m_vDim.X / 2;
		m_ImageSet.m_Output.m_Vertices[3].tv = m_vCenter.Y - m_vDim.Y / 2;

		m_ImageSet.m_Output.m_Vertices[0].tu = m_vCenter.X - m_vDim.X / 2;
		m_ImageSet.m_Output.m_Vertices[0].tv = m_vCenter.Y + m_vDim.Y / 2;
		
		m_ImageSet.m_Output.m_Vertices[1].tu = m_vCenter.X + m_vDim.X / 2;
		m_ImageSet.m_Output.m_Vertices[1].tv = m_vCenter.Y + m_vDim.Y / 2;
		
		m_ImageSet.m_Output.m_Vertices[2].tu = m_vCenter.X + m_vDim.X / 2;
		m_ImageSet.m_Output.m_Vertices[2].tv = m_vCenter.Y - m_vDim.Y / 2;

		m_ImageSet.m_Output.InitVertices(false);
	}

	void ZoomUV(Float zoomFactor)
	{
		if (zoomFactor > 0)
			m_vDim *= zoomFactor;
		else
			m_vDim = Vec2(fabsf(zoomFactor), fabsf(zoomFactor));

		MoveUV(k_v2Zero);
	}

	Vec2 GetMin() const
	{
		return Vec2(m_vCenter.X - m_vDim.X / 2, m_vCenter.Y - m_vDim.Y / 2);
	}

	Vec2 GetMax() const
	{
		return Vec2(m_vCenter.X + m_vDim.X / 2, m_vCenter.Y + m_vDim.Y / 2);
	}
	//END HACK

public:
	Bool ResetInitialTexture(String path = "");

	Int32 RenderPrim(Bool bStates = false, D3DTexture* tex = NULL)
	{
		if (bStates)
		{
			if (tex)
				g_D3D->GetDevice()->SetTexture(0,	*tex);

			g_D3D->GetDevice()->SetStreamSource(0,	m_ImageSet.m_Output.m_pVertexBuffer, 0, sizeof(CVertex));
			g_D3D->GetDevice()->SetIndices(			m_ImageSet.m_Output);
		}

		if (FAILS(g_D3D->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2)))
		{
			DxErr;
			return -1;
		}

		return 0;
	}

	void Animate(Bool bAnim = true)
	{	m_bAnimate = bAnim;			}

	void ToggleAnimate()
	{	m_bAnimate = !m_bAnimate;	}

	void ShowXi(Bool bShow = true)
	{	m_bShowXi = bShow;			}

	void ToggleShowXi()
	{	m_bShowXi = !m_bShowXi;	}

	//Standard inherited funcs
public:
	virtual Int32 Initialize(CWnd* parentFrame = NULL, TArgumentArray* pArgs = NULL);

	virtual Int32 Draw();

	void SetTechnique(D3DXHANDLE tech)
	{	m_pShader->Shader->SetTechnique(tech);	}

	Bool IsDirty() const
	{	return m_bDirty;				}
	
	Bool IsShader() const
	{	return m_bIsShader;				}

protected:
	void RenderToScreen();

	void SetDirty(Bool bDirty = true)
	{	m_bDirty = bDirty;	}

protected:
	CThread*			m_pThread;

	Bool				m_bDirty;
	Bool				m_bIsShader;

public:
	Bool				m_bAnimate;
	Bool				m_bShowXi;
	Int32				m_nCutoff;
	Float				m_fEpsilon;
	Float				m_fDeltaHue;

protected:
	Effect*				m_pShader;
	D3DXHANDLE			m_hTech;

	ImageSet			m_ImageSet;
};
