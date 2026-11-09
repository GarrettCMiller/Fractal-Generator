#pragma once

#include "ShaderFractal.h"

//////////////////////////////////////////////////////////////////////////
#define FRAC_PARENT_CLASS	CShaderFractal	//CFractal

class CMRCM : public FRAC_PARENT_CLASS
{
public:
	//Forward dec
	struct TransformObject;

	typedef CDynamicArray<TransformObject>	TransObjArray;

public:
	CMRCM(void) {	}
	CMRCM(const char* strName, Bool bCreateStates = true);
	
	virtual ~CMRCM(void);

	struct InitStruct : public FRAC_PARENT_CLASS::InitStruct
	{
	};

	//Standard inherited funcs
public:
	virtual Int32 Initialize(CWnd* parentFrame = NULL, TArgumentArray* pArgs = NULL);
	virtual Int32 Update();
	virtual Int32 Shutdown();

	virtual Int32 Begin();
	virtual Int32 Stop();

	virtual Int32 Reset();

	virtual void MeshThreadData(Int32 threadNum = -1);

	//MRCM specific
public:
	void AddOperation(TransformObject& op)
	{
		m_Operations.Push(op);
	}
	
	void RenderToOutput();


	Int32 SetupGasket();
	Int32 SetupTwinChristmasTree();
	Int32 SetupSnowflake5Pt();
	Int32 SetupFern();
	Int32 SetupKochCurve();

protected:
	virtual Bool SetupProcessors();
	static DWORD ProcessingThread(LPVOID param);

protected:
	Int32				m_CurrentIteration;

	TransObjArray		m_Operations;

	TransformObject& operator [ ] (UInt32 index)
	{
		return m_Operations[index];
	}

	const TransformObject& operator [ ] (UInt32 index) const
	{
		return m_Operations[index];
	}

	/*Int32 RenderPrim()
	{
		if (FAILS(g_D3D->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2)))
		{
			DxErr;
			return -1;
		}

		return 0;
	}*/

public:
	ADD_FSM_FUNCTIONS

	class CUninitializedState;
	class CIdleState;
	class CProcessingState;
	class CRenderingState;
	class CCancelledState;
};

typedef CFSMState<CMRCM>		TMRCMState;

//////////////////////////////////////////////////////////////////////////

#define MAKE_STATE(_stateName)		MAKE_STATE_CLASS(CMRCM, _stateName, TMRCMState)

//////////////////////////////////////////////////////////////////////////

MAKE_STATE(Uninitialized);

MAKE_STATE(Idle);

MAKE_STATE(Processing);

MAKE_STATE(Rendering);

MAKE_STATE(Cancelled);

//////////////////////////////////////////////////////////////////////////

#undef MAKE_STATE

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

struct CMRCM::TransformObject
{
	struct Transform;

	typedef CDynamicArray<Transform>	TransformArray;

	TransformObject(CMRCM* pParent = NULL) : m_pParent(pParent),
		m_State(eState_Uninitialized), m_PercentComplete(0),
		m_ThreadId(-1), m_CurrentIteration(0)
	{
		m_Transforms.Resize(10, false);
	}

	/////////////////////////

	CMRCM*				m_pParent;

	EFractalState		m_State;

	float				m_PercentComplete;
	DWORD				m_ThreadId;

	Vec2i				m_vImageSize;

	int					m_CurrentIteration;

	Int32				Init();
	Int32				Update();

	//////////////////////////

	Int32 Reset();

	ImageSet			m_ImageSet;

	void AddTransform(Transform& tform)
	{
		m_Transforms.Push(tform);
	}

	bool GetInputFromOutput();
	Int32 ApplyTransformations();

	Transform& operator [ ] (UInt32 index)
	{
		return m_Transforms[index];
	}

	const Transform& operator [ ] (UInt32 index) const
	{
		return m_Transforms[index];
	}

	TransformObject& operator = (const TransformObject& rhs)
	{
		m_pParent = rhs.m_pParent;
		m_State = rhs.m_State;
		m_ThreadId = rhs.m_ThreadId;
		m_vImageSize = rhs.m_vImageSize;
		m_CurrentIteration = rhs.m_CurrentIteration;
		m_ImageSet = rhs.m_ImageSet;
		m_Transforms = rhs.m_Transforms;

		return *this;
	}

protected:
	TransformArray		m_Transforms;
};

//////////////////////////////////////////////////////////////////////////

//struct CMRCM::Image

//////////////////////////////////////////////////////////////////////////

//struct CMRCM::ImageSet


//////////////////////////////////////////////////////////////////////////

struct CMRCM::TransformObject::Transform
{
	static const UInt32 mk_DefaultTransformCount = 5;

	Transform(TransformObject* pParent = NULL, 
			UInt32 numTransforms = 0) : pParent(pParent)
	{
		D3DXMatrixIdentity(&scale);
		transforms.Resize(!numTransforms ? mk_DefaultTransformCount : numTransforms, false);
	}

	Int32 Init(TransformObject& _parent);
	Int32 Apply(ImageSet& imageSet);

	TransformObject*	pParent;

	D3DMatrix			scale;
	TMatrixArray			transforms;

	Transform& operator = (const Transform& rhs)
	{
		pParent = rhs.pParent;
		scale = rhs.scale;
		transforms = rhs.transforms;
		return *this;
	}

	D3DMatrix& operator [ ] (UInt32 index)
	{
		return transforms[index];
	}

	const D3DMatrix& operator [ ] (UInt32 index) const
	{
		return transforms[index];
	}
};