#include "StdAfx.h"
#include "FractalImage.h"

#include "D3DWrapper.h"
#include "SpriteMgr.h"

//HACK
#include "MRCM.h"

//////////////////////////////////////////////////////////////////////////

CFractalImage::CFractalImage(void) :	
			m_pVertexBuffer(NULL),	m_pIndexBuffer(NULL),
			m_pTexture(NULL),		m_pSurface(NULL),
			m_bVBLocked(false),		m_bIBLocked(false),
			m_pParent(NULL)
{
}

CFractalImage::~CFractalImage(void)
{
	ReleaseAll();
}

//////////////////////////////////////////////////////////////////////////

void CFractalImage::ReleaseAll()
{
	m_Image.Destroy();

	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pSurface);

	 m_bVBLocked = false;
	 m_bIBLocked = false;
}

//////////////////////////////////////////////////////////////////////////

Int32 CFractalImage::Init(CFractal& _parent, Vec2u imgSize, String initImage /* = "" */)
{
	//To be safe
	ReleaseAll();

	m_pParent = &_parent;

	m_vImageSize = imgSize;

	ASSERT(m_vImageSize.X > 0); 
	ASSERT(m_vImageSize.Y > 0);

	if (CreateSurface())
		return -1;

	//return 0;

	if (CreateBuffers())
		return -2;

	if (InitVertices())
		return -3;

	if (!initImage.empty())
		if (LoadTexture(initImage))
			return -4;

	if (CreateImageObject())
		return -5;

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CFractalImage::Reset()
{
	g_D3D->PushRenderTarget(*this);

	g_D3D->Clear(Color::km_Yellow);

	g_D3D->PopRenderTarget();

	return InitVertices();
}

//////////////////////////////////////////////////////////////////////////

Int32 CFractalImage::CreateSurface()
{
	if (FAILS(g_D3D->GetDevice()->
		CreateTexture(	m_vImageSize.X,
						m_vImageSize.Y,
						1, 
						D3DUSAGE_RENDERTARGET,
						D3DFMT_A8R8G8B8,
						D3DPOOL_DEFAULT,
						&m_pTexture,
						NULL)))
	{
		DxErr;
		return -1;
	}

	if (FAILS(m_pTexture->GetSurfaceLevel(0, &m_pSurface)))
	{
		DxErr;
		return -2;
	}

	g_D3D->PushRenderTarget(m_pSurface);
	
	g_D3D->Clear(Color::km_Red);
	
	g_D3D->PopRenderTarget();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CFractalImage::CreateBuffers()
{
	if (FAILS(g_D3D->GetDevice()->
		CreateVertexBuffer(	4 * sizeof(CVertex), 
							D3DUSAGE_WRITEONLY, 
							CVertex::FVF, 
							D3DPOOL_DEFAULT, 
							&m_pVertexBuffer, 
							NULL)))
	{
		DxErr;
		return -3;
	}

	if (FAILS(g_D3D->GetDevice()->
		CreateIndexBuffer(	6 * sizeof(WORD), 
							D3DUSAGE_WRITEONLY, 
							D3DFMT_INDEX16, 
							D3DPOOL_DEFAULT, 
							&m_pIndexBuffer, 
							NULL)))
	{
		DxErr;
		return -4;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CFractalImage::InitVertices(Bool bResizeVertArray /* = true */)
{
	CVertex* pV = NULL;
	UInt16* pI = NULL;

	if (bResizeVertArray)
	{
		m_Vertices.Resize(4);

		for (UInt32 i = 0; i < 4; i++)
			m_Vertices[i] = RectVerts[i];
	}

	if (FAILS(Lock(&pV, &pI)))
	{
		DxErr;
		return -1;
	}

	ASSERT(pV);
	ASSERT(pI);

	memcpy(pV, &m_Vertices[0], sizeof(CVertex) * 4);
	memcpy(pI, RectIndices, sizeof(UInt16) * 6);

	Unlock();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

void CFractalImage::InitPalette(UInt32 size /* = 0 */, UInt32 offset /* = 0 */)
{
	if (size == 0)
		size = 1024;

	m_Palette.Init(size, offset);
}

//////////////////////////////////////////////////////////////////////////

Int32 CFractalImage::LoadTexture(const String& path)
{
	//TEMP HACK
	//ASSERT(CMRCM::m_pBackBuffer);

	if (FAILS(g_D3D->PushRenderTarget(m_pSurface)))
	{
		DxErr;
		return -7;
	}

	D3DTexture texture = g_SpriteMgr->GetTexture(g_SpriteMgr->LoadSprite(path));
	ASSERT(texture);

	if (1)//g_D3D->BeginScene(true))
	{
		//g_D3D->Clear(Color::km_Transparent);
		//g_D3D->Clear(Color::km_Black);
		//g_D3D->Clear(Color::km_Green);
		g_D3D->Clear(Color::km_Cyan);
	
		g_D3D->GetDevice()->SetTexture(0, texture);

		//g_D3D->GetDevice()->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(CVertex));
		//g_D3D->GetDevice()->SetIndices(m_pIndexBuffer);

		if (m_pParent)
			static_cast<CMRCM*>(m_pParent)->RenderPrim(true);
	}
	else
		ASSERT(0);

	g_D3D->PopRenderTarget();

	//g_D3D->EndScene();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CFractalImage::CreateImageObject()
{
	m_Image.Destroy();
	return (m_Image.Create(m_vImageSize.X, -(static_cast<Int32>(m_vImageSize.Y)), 24) == 0);
}

//////////////////////////////////////////////////////////////////////////

HRESULT CFractalImage::Lock(CVertex** pV, UInt16** pI /* = NULL */)
{
	ASSERT(pV || pI);

	if (pV)
	{
		ASSERT(!m_bVBLocked);

		g_hr = m_pVertexBuffer->Lock(0, 4 * sizeof(CVertex), (void **) pV, 0);
		if (FAILED(m_bVBLocked = SUCCEEDED(g_hr)))	return g_hr;
	}

	if (pI)
	{
		ASSERT(!m_bIBLocked);

		g_hr = m_pIndexBuffer->Lock(0, 6 * sizeof(UInt16), (void**) pI, 0);
		m_bIBLocked = SUCCEEDED(g_hr);
	}

	return g_hr;
}

//////////////////////////////////////////////////////////////////////////

void CFractalImage::Unlock(bool bVB /* = true */, HRESULT* vb /* = NULL */, 
						  bool bIB /* = true */, HRESULT* ib /* = NULL */)
{
	HRESULT temp;
	if (bVB && m_bVBLocked)
	{
		temp = m_pVertexBuffer->Unlock();
		if (vb) *vb = temp;
		m_bVBLocked = !SUCCEEDED(temp);
	}
	if (bIB && m_bIBLocked)
	{
		temp = m_pIndexBuffer->Unlock();
		if (ib) *ib = temp;
		m_bIBLocked = !SUCCEEDED(temp);
	}
}

//////////////////////////////////////////////////////////////////////////

CFractalImage& CFractalImage::operator = (const CFractalImage& rhs)
{
	Init(*rhs.m_pParent, rhs.m_vImageSize);

	m_Vertices.CopyFrom(rhs.m_Vertices);

	m_bIBLocked		= false;
	m_bVBLocked		= false;

	return *this;
}