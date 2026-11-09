#pragma once

#include <atlimage.h>

#include "FractalGlobals.h"

//////////////////////////////////////////////////////////////////////////

class CFractalImage
{
	friend class CFractal;

public:
	CFractalImage(void);
	virtual ~CFractalImage(void);

	void ReleaseAll();

	CFractalImage& operator = (const CFractalImage& rhs);

	Int32 Init(CFractal& _parent, Vec2u imgSize, String initImage = "");
	Int32 Reset();

	Int32 InitVertices(Bool bResizeVertArray = true);
	void InitPalette(UInt32 size = 0, UInt32 offset = 0);

	Int32 CreateSurface();
	Int32 CreateBuffers();
	Int32 CreateImageObject();
	
	Int32 LoadTexture(const String& path);

	//////////////////////////////////////////

	HRESULT Lock(CVertex** pV, UInt16** pI = NULL);

	void Unlock(bool bVB = true, HRESULT* vb = NULL, 
				bool bIB = true, HRESULT* ib = NULL);

	///////////////////////////////////////////

	//Direct vertex access
	CVertex& operator () (UInt32 index)
	{	
		return m_Vertices[index];	
	}

	//////////////////////////////////////////

	Bool VertexBufferLocked() const
	{	return m_bVBLocked;		}

	Bool IndexBufferLocked() const
	{	return m_bIBLocked;		}

	const Vec2u& ImageSize() const
	{	return m_vImageSize;	}

	Palette&	GetPalette()
	{	return m_Palette;		}

public:

	AUTO_CLASS_CAST(D3DVertexBuffer,	m_pVertexBuffer);
	AUTO_CLASS_CAST(D3DIndexBuffer,		m_pIndexBuffer);
	AUTO_CLASS_CAST(D3DTexture,			m_pTexture);
	AUTO_CLASS_CAST(D3DSurface,			m_pSurface);
	
	AUTO_CLASS_CAST(CImage,				m_Image);

	TVertexArray						m_Vertices;

private:
	Palette			m_Palette;

	CFractal*		m_pParent;

	bool			m_bVBLocked;
	bool			m_bIBLocked;

	Vec2u			m_vImageSize;
};