#include "StdAfx.h"
#include "D3DWrapper.h"
#include "MRCM.h"

#include "GameMacros.h"

#include "Verts.h"

#include "Fractal Generator.h"

#include "VectorGraphics.h"

#include "SpriteMgr.h"

//////////////////////////////////////////////////////////////////////////

#define CHANGE_RENDER_STATES

CVectorGraphics vg;

//////////////////////////////////////////////////////////////////////////

Color defcolor = Color(0.0f, 0.0f, 1.0f, 0.0f);

//////////////////////////////////////////////////////////////////////////

CMRCM::CMRCM(const char* strName, Bool bCreateStates /* = true */) : 
			CShaderFractal(strName, bCreateStates),
				m_CurrentIteration(0)
{
	m_eType = eFT_MRCM;

	if (bCreateStates)
	{
		TMRCMState		*pUnInitState		= new CUninitializedState(this,	k_strUnInit),
						*pIdleState			= new CIdleState(this,			k_strIdle),
						*pProcState			= new CProcessingState(this,	k_strProcessing),
						*pRenderState		= new CRenderingState(this,		k_strRender),
						*pCancelledState	= new CCancelledState(this,		k_strCancelled);

		pUnInitState->AddValidTransition(pIdleState);

		pIdleState->AddValidTransition(pProcState);
		pIdleState->AddValidTransition(pRenderState);

		pProcState->AddValidTransition(pIdleState);
		pProcState->AddValidTransition(pRenderState);
		pProcState->AddValidTransition(pCancelledState);

		pRenderState->AddValidTransition(pIdleState);
		pRenderState->AddValidTransition(pCancelledState);

		pCancelledState->AddValidTransition(pIdleState);

		FSM_Initialize(pUnInitState);
	}

	m_Operations.Resize(5);
	m_bD3D = true;
}

CMRCM::~CMRCM(void)
{
}

//////////////////////////////////////////////////////////////////////////

Int32 CMRCM::Initialize(CWnd* parentFrame/* = NULL*/, TArgumentArray* pArgs /* = NULL */)
{
	if (CFractal::Initialize(parentFrame, pArgs) != 0)
		return -1;

	m_Viewport			= GLib::Rectd(0, 0, 1, 1);

	TArgumentArray& args = *pArgs;
	Pointer pInitStruct = args[0];

	InitStruct& is = *(reinterpret_cast<InitStruct*>(pInitStruct));

	UInt32	nWidth = is.imgWidth,
			nHeight = is.imgHeight;

	if (nWidth == 0)
		nWidth = 1024;
	if (nHeight == 0)
		nHeight = nWidth;

	m_nMaxIterations	= is.maxIters;

	m_vImageSize		= Vec2(nWidth, nHeight);

	m_bIsShader			= is.bIsShader;

	g_D3D->BeginScene(true);

	//if (m_bIsShader)
		if (CShaderFractal::Initialize(parentFrame, pArgs) != 0)
			return -1;

	//Establish the bb
	//g_D3D->GetDevice()->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer);

	vg.Init();
	//TODO
	SetupProcessors();

	g_D3D->EndScene(false);

  	g_ThreadMgr->CreateThread(	(LPTHREAD_START_ROUTINE) &CMRCM::ProcessingThread, 
  								this, false, &m_pThread, CThread::eTPF_Default);
  
  	//m_ThreadID = m_Thread.GetID();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Bool CMRCM::SetupProcessors()
{
	return true;
}

//////////////////////////////////////////////////////////////////////////

Int32 CMRCM::Begin()
{
#ifdef MRCM_FSM
	Int32 ret = CFractal::Begin();
#else
	m_State = eState_Processing;
#endif

	return 0;
}

//////////////////////////////////////////////////////////////////////////

//void CMRCM::Pause

//////////////////////////////////////////////////////////////////////////

Int32 CMRCM::Update()
{
#ifdef MRCM_FSM
	
	FSM_Update();

#else

	if (m_bIsShader)
		return 1;

	static bool bDone = true;

	//Init every frame
	bDone = true;

	if (m_State == eState_Processing)
	{
		for (UInt32 t = 0; t < m_Operations.Size(); t++)
		{
			if (m_Operations[t].m_State == eState_Processing)
			{
				m_Operations[t].Update();
				bDone = false;
			}
		}

		if (bDone)
			m_State = eState_Done;
	}

	RenderToOutput();
	RenderToScreen();

//  	if (m_State == eState_Done)
//  		return 1;
#endif

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CMRCM::Shutdown()
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CMRCM::Reset()
{
	//m_Thread.Stop();
	CFractal::Reset();

	m_ImageSet.Reset();

	for (UInt32 i = 0; i < m_Operations.NumItems(); i++)
		m_Operations[i].Reset();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CMRCM::Stop()
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////

DWORD CMRCM::ProcessingThread(LPVOID param)
{
	CMRCM* mrcm = static_cast<CMRCM*>(param);

	bool bDone = false;

	while	(	!theApp.m_bQuit						&&
				g_D3D->Exists()						&&
				g_D3D->IsDeviceWindowValid()		&&
				mrcm != NULL						&&
				mrcm->m_State != eState_Cancelled
			)
	{
		g_D3D->BeginScene(true);
		
		if (mrcm->Update() != 0)
			bDone = true;
		
		g_D3D->EndScene();

		if (mrcm->m_State == eState_Processing)
			Sleep(1000);
	}

	//TODO
// 	if (g_ThreadMgr->Exists())
// 		g_ThreadMgr->GetThread(mrcm->m_ThreadID).Invalidate();

	mrcm->m_State = eState_Uninitialized;

	return 42;
}

//////////////////////////////////////////////////////////////////////////

void CMRCM::RenderToOutput()
{
	/*if (!IsDirty())
		return;*/

	g_D3D->PushRenderTarget(m_ImageSet.m_Output);
	//g_D3D->Clear(Color::km_Yellow);
 	
	g_D3D->GetDevice()->SetStreamSource(0,	m_ImageSet.m_Input, 0, sizeof(CVertex));
	g_D3D->GetDevice()->SetIndices(			m_ImageSet.m_Input);

	for (UInt32 i = 0; i < m_Operations.Size(); i++)
	{
		g_D3D->GetDevice()->SetTexture(0,		m_Operations[i].m_ImageSet.m_Output);
		//g_D3D->GetDevice()->SetTexture(0,		g_SpriteMgr->GetTexture("texture.bmp"));
		
		RenderPrim();
	}

	g_D3D->PopRenderTarget();
}

//////////////////////////////////////////////////////////////////////////

void CMRCM::MeshThreadData(Int32 threadNum /* = -1 */)
{
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Int32 CMRCM::FSM_Initialize(IFSMStateBase* pStartState)
{
	return __super::FSM_Initialize(pStartState);
}

//////////////////////////////////////////////////////////////////////////

Int32 CMRCM::FSM_Update()
{
	return __super::FSM_Update();
}

//////////////////////////////////////////////////////////////////////////

Int32 CMRCM::FSM_Shutdown()
{
	return __super::FSM_Shutdown();
}

//////////////////////////////////////////////////////////////////////////

Int32 CMRCM::FSM_SendMessage(TArgumentArray& args)
{
	return __super::FSM_SendMessage(args);
}

//////////////////////////////////////////////////////////////////////////

#define CAST_STATE_PARENT		CMRCM* pFrac = static_cast<CMRCM*>(m_pFSM)

//////////////////////////////////////////////////////////////////////////

CMRCM::CUninitializedState::CUninitializedState(CMRCM* pFSM, const String& strName)
: TMRCMState(pFSM, strName)
{
}

Int32 CMRCM::CUninitializedState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CMRCM::CUninitializedState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CMRCM::CUninitializedState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CMRCM::CUninitializedState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CMRCM::CIdleState::CIdleState(CMRCM* pFSM, const String& strName)
: TMRCMState(pFSM, strName)
{

}

Int32 CMRCM::CIdleState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CMRCM::CIdleState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CMRCM::CIdleState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CMRCM::CIdleState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CMRCM::CProcessingState::CProcessingState(CMRCM* pFSM, const String& strName) 
: TMRCMState(pFSM, strName)
{

}

Int32 CMRCM::CProcessingState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	CAST_STATE_PARENT;

	return __super::OnEnter(pArgs);
}

Int32 CMRCM::CProcessingState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	static Int32 nIterations = 0;

	CAST_STATE_PARENT;

	return __super::OnUpdate(pArgs);
}

Int32 CMRCM::CProcessingState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CMRCM::CProcessingState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CMRCM::CRenderingState::CRenderingState(CMRCM* pFSM, const String& strName)
: TMRCMState(pFSM, strName)
{

}

Int32 CMRCM::CRenderingState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CMRCM::CRenderingState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CMRCM::CRenderingState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CMRCM::CRenderingState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CMRCM::CCancelledState::CCancelledState(CMRCM* pFSM, const String& strName)
: TMRCMState(pFSM, strName)
{
}

Int32 CMRCM::CCancelledState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CMRCM::CCancelledState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CMRCM::CCancelledState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CMRCM::CCancelledState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#undef CAST_STATE_PARENT

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
extern String DefTexturePath;

//////////////////////////////////////////////////////////////////////////
//
//	TRANSFORM STUFF
//
//////////////////////////////////////////////////////////////////////////

Int32 CMRCM::TransformObject::Init()
{
	m_ImageSet.Init(*m_pParent, DefTexturePath);
	m_Transforms.Resize(10, false);

	m_State = eState_Processing;

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CMRCM::TransformObject::Reset()
{
	m_ImageSet.Reset();
	m_CurrentIteration = 0;

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CMRCM::TransformObject::Update()
{
	HRESULT hr;
 	if (m_CurrentIteration++ >= m_pParent->m_nMaxIterations)
 	{
		m_State = eState_Done;
 		return 1;
 	}

	if (GetInputFromOutput() == false)
		return -1;

	if (ApplyTransformations() != 0)
		return -2;

	return 0;
}

//////////////////////////////////////////////////////////////////////////

bool CMRCM::TransformObject::GetInputFromOutput()
{
	g_D3D->PushRenderTarget(m_ImageSet.m_Input);
	
	//g_D3D->Clear(defcolor);
	g_D3D->Clear(Color::km_Green);

	g_D3D->GetDevice()->SetTexture(0, m_pParent->m_ImageSet.m_Output);
	g_D3D->GetDevice()->SetIndices(m_pParent->m_ImageSet.m_Output);
	g_D3D->GetDevice()->SetStreamSource(0, m_pParent->m_ImageSet.m_Output, 0, sizeof(CVertex));

	m_pParent->RenderPrim();

	g_D3D->PopRenderTarget();

	return true;
}

//////////////////////////////////////////////////////////////////////////

Int32 CMRCM::TransformObject::ApplyTransformations()
{
	CVertex* pv = NULL;

	g_D3D->PushRenderTarget(m_ImageSet.m_Output);

	g_D3D->Clear(defcolor);

	m_ImageSet.m_Input.m_Vertices.CopyFrom(m_pParent->m_ImageSet.m_Input.m_Vertices);

	for (UInt32 i = 0; i < m_Transforms.NumItems(); i++)
	{
		m_Transforms[i].Apply(m_ImageSet);

		if (FAILS(m_ImageSet.m_Output.Lock(&pv)))
		{
			DxErr;
			return -1;
		}

		pv[0] = m_ImageSet.m_Output(0);
		pv[1] = m_ImageSet.m_Output(1);
		pv[2] = m_ImageSet.m_Output(2);
		pv[3] = m_ImageSet.m_Output(3);

		m_ImageSet.m_Output.Unlock();

		g_D3D->GetDevice()->SetTexture(0, m_ImageSet.m_Input);
		g_D3D->GetDevice()->SetIndices(m_ImageSet.m_Output);
		g_D3D->GetDevice()->SetStreamSource(0, m_ImageSet.m_Output, 0, sizeof(CVertex));

		m_pParent->RenderPrim();

		//m_ImageSet.m_Input.m_Vertices.CopyFrom(m_pParent->m_ImageSet.m_Input.m_Vertices);
	}

	//vg.CreateLine(0, 0, 0, 1, 1, 0);

	g_D3D->PopRenderTarget();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CMRCM::TransformObject::Transform::Init(CMRCM::TransformObject& _parent)
{
	//We take a reference and THEN convert to pointer so that we 
	//don't have to worry about NULL pointers
	pParent = &_parent;

	D3DXMatrixIdentity(&scale);

	transforms.Resize(mk_DefaultTransformCount, false);

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CMRCM::TransformObject::Transform::Apply(ImageSet& imageSet)
{
	static bool bFlip = false;
	D3DXMATRIX mStack;
	D3DXMatrixIdentity(&mStack);

	//I think scaling should come first??
	mStack *= scale;

	for (UInt32 t = 0; t < transforms.NumItems(); t++)
	{
		mStack *= transforms[t];
	}

	//mStack *= scale;

	UInt32 numVerts = imageSet.m_Output.m_Vertices.Size();

	D3DXVec3TransformCoordArray(&imageSet.m_Output.m_Vertices[0].position, sizeof(CVertex), 
								&imageSet.m_Input.m_Vertices[0].position, sizeof(CVertex), 
								&mStack, numVerts);

	//imageSet.m_Input.m_Vertices.CopyFrom(imageSet.m_Output.m_Vertices);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
////
////
////	FORMULAS
////
////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
Int32 CMRCM::SetupGasket()
{
	ResetInitialTexture("texture.bmp");

	TransformObject tobj(this);
	TransformObject tobj2(this);
	TransformObject tobj3(this);

	TransformObject::Transform tform;
	TransformObject::Transform tform2;
	TransformObject::Transform tform3;

	Float scaleval = 1 / 2.0f;
	Float val = scaleval * 0.5f;

	D3DMatrix mtemp;
	
	Vec3 scale;
	Vec3 move;

	//////////////////////////////////////////////////////////////////////////

	scale = Vec3(scaleval, scaleval, scaleval);
	move = Vec3(0, val, 0);

	tobj.Init();
	tform.Init(tobj);

	D3DXQUATERNION qrot;
	D3DXMatrixRotationZ(&mtemp, 0.0f * Deg2Rad);
	D3DXQuaternionRotationMatrix(&qrot, &mtemp);

	D3DXMatrixTransformation(&mtemp, &Vec3(0.5f, 0.5f), NULL, &scale, NULL, NULL, NULL);
	tform.scale = mtemp;
	D3DXMatrixTransformation(&mtemp, NULL, NULL, NULL, &Vec3(0.5f, 0.5f), &qrot, &move);
	tform.transforms.Push(mtemp);

	tobj.AddTransform(tform);
	AddOperation(tobj);

	//////////////////////////////////////////////////////////////////////////

	tobj2.Init();
	tform2.Init(tobj2);

	move = Vec3(val, -val, 0);

	D3DXMatrixRotationZ(&mtemp, 0.0f * Deg2Rad);
	D3DXQuaternionRotationMatrix(&qrot, &mtemp);

	D3DXMatrixTransformation(&mtemp, &Vec3(0.5f, 0.5f), NULL, &scale, NULL, NULL, NULL);
	tform2.scale = mtemp;
	D3DXMatrixTransformation(&mtemp, NULL, NULL, NULL, &Vec3(0.5f, 0.5f), &qrot, &move);
	tform2.transforms.Push(mtemp);

	tobj2.AddTransform(tform2);
	AddOperation(tobj2);

	//////////////////////////////////////////////////////////////////////////

	tobj3.Init();
	tform3.Init(tobj3);

	move = Vec3(-val, -val, 0);

	D3DXMatrixRotationZ(&mtemp, 0.0f * Deg2Rad);
	D3DXQuaternionRotationMatrix(&qrot, &mtemp);

	D3DXMatrixTransformation(&mtemp, &Vec3(0.5f, 0.5f), NULL, &scale, NULL, NULL, NULL);
	tform3.scale = mtemp;
	D3DXMatrixTransformation(&mtemp, NULL, NULL, NULL, &Vec3(0.5f, 0.5f), &qrot, &move);
	tform3.transforms.Push(mtemp);

	tobj3.AddTransform(tform3);
	AddOperation(tobj3);

	m_nMaxIterations = 10;

	return 1;
}

Int32 CMRCM::SetupTwinChristmasTree()
{
	ResetInitialTexture("texture.bmp");

	D3DTexture tex = g_SpriteMgr->GetTexture("texture.bmp");
	
	g_D3D->PushRenderTarget(this->m_ImageSet.m_Output);
	g_D3D->BeginScene(true);
	RenderPrim(true, &tex);
	g_D3D->EndScene();
	g_D3D->PopRenderTarget();

	TransformObject tobj(this);
	TransformObject tobj2(this);
	TransformObject tobj3(this);

	TransformObject::Transform tform;
	TransformObject::Transform tform2;
	TransformObject::Transform tform3;

	Float scaleval = 1 / 2.0f;
	Float val = scaleval * 0.5f;

	D3DMatrix mtemp;
	
	Vec3 scale;
	Vec3 move;

	//////////////////////////////////////////////////////////////////////////

	scale = Vec3(scaleval, scaleval, scaleval);
	move = Vec3(0, val, 0);

	tobj.Init();
	tform.Init(tobj);

	D3DXQUATERNION qrot;
	D3DXMatrixRotationZ(&mtemp, 0.0f * Deg2Rad);
	D3DXQuaternionRotationMatrix(&qrot, &mtemp);

	D3DXMatrixTransformation(&mtemp, &Vec3(0.5f, 0.5f), NULL, &scale, NULL, NULL, NULL);
	tform.scale = mtemp;
	D3DXMatrixTransformation(&mtemp, NULL, NULL, NULL, &Vec3(0.5f, 0.5f), &qrot, &move);
	tform.transforms.Push(mtemp);

	tobj.AddTransform(tform);
	AddOperation(tobj);

	//////////////////////////////////////////////////////////////////////////

	tobj2.Init();
	tform2.Init(tobj2);

	move = Vec3(val, -val, 0);

	D3DXMatrixRotationZ(&mtemp, -90.0f * Deg2Rad);
	D3DXQuaternionRotationMatrix(&qrot, &mtemp);

	D3DXMatrixTransformation(&mtemp, &Vec3(0.5f, 0.5f), NULL, &scale, NULL, NULL, NULL);
	tform2.scale = mtemp;
	D3DXMatrixTransformation(&mtemp, NULL, NULL, NULL, &Vec3(0.5f, 0.5f), &qrot, &move);
	tform2.transforms.Push(mtemp);

	tobj2.AddTransform(tform2);
	AddOperation(tobj2);

	//////////////////////////////////////////////////////////////////////////

	tobj3.Init();
	tform3.Init(tobj3);

	move = Vec3(-val, -val, 0);

	D3DXMatrixRotationZ(&mtemp, 90.0f * Deg2Rad);
	D3DXQuaternionRotationMatrix(&qrot, &mtemp);

	D3DXMatrixTransformation(&mtemp, &Vec3(0.5f, 0.5f), NULL, &scale, NULL, NULL, NULL);
	tform3.scale = mtemp;
	D3DXMatrixTransformation(&mtemp, NULL, NULL, NULL, &Vec3(0.5f, 0.5f), &qrot, &move);
	tform3.transforms.Push(mtemp);

	tobj3.AddTransform(tform3);
	AddOperation(tobj3);

	m_nMaxIterations	= 2;//10

	return 1;
}
///////

Int32 CMRCM::SetupSnowflake5Pt()
{
	ResetInitialTexture("texture.bmp");

	TransformObject tobj(this);
	TransformObject tobj2(this);
	TransformObject tobj3(this);
	TransformObject tobj4(this);
	TransformObject tobj5(this);

	TransformObject::Transform tform;
	TransformObject::Transform tform2;
	TransformObject::Transform tform3;
	TransformObject::Transform tform4;
	TransformObject::Transform tform5;

	Float scaleval = 2 / 5.0f;
	Float val = scaleval * 0.5f;

	D3DMatrix mtemp;
	
	Vec3 scale;
	Vec3 move;

	//////////////////////////////////////////////////////////////////////////

	scale = Vec3(scaleval, scaleval, scaleval);
	move = Vec3(0, 1.4*val, 0);

	tobj.Init();
	tform.Init(tobj);

	D3DXQUATERNION qrot;
	D3DXMatrixRotationZ(&mtemp, 0.0f * Deg2Rad);
	D3DXQuaternionRotationMatrix(&qrot, &mtemp);

	D3DXMatrixTransformation(&mtemp, &Vec3(0.5f, 0.5f), NULL, &scale, NULL, NULL, NULL);
	tform.scale = mtemp;
	D3DXMatrixTransformation(&mtemp, NULL, NULL, NULL, &Vec3(0.5f, 0.5f), &qrot, &move);
	tform.transforms.Push(mtemp);

	tobj.AddTransform(tform);
	AddOperation(tobj);

	//////////////////////////////////////////////////////////////////////////

	tobj2.Init();
	tform2.Init(tobj2);

	move = Vec3(1.5*val, 0.2 * val, 0);
	//scale = Vec3(1.0f, 1/3.0f, 0);

	D3DXMatrixRotationZ(&mtemp, 0.0f * Deg2Rad);
	D3DXQuaternionRotationMatrix(&qrot, &mtemp);

	D3DXMatrixTransformation(&mtemp, &Vec3(0.5f, 0.5f), NULL, &scale, NULL, NULL, NULL);
	tform2.scale = mtemp;
	D3DXMatrixTransformation(&mtemp, NULL, NULL, NULL, &Vec3(0.5f, 0.5f), &qrot, &move);
	tform2.transforms.Push(mtemp);

	tobj2.AddTransform(tform2);
	AddOperation(tobj2);

	//////////////////////////////////////////////////////////////////////////

	tobj3.Init();
	tform3.Init(tobj3);

	move = Vec3(0.9*val, -1.333*val, 0);
	//scale = Vec3(1.0f, 1/3.0f, 0);

	D3DXMatrixRotationZ(&mtemp, 0.0f * Deg2Rad);
	D3DXQuaternionRotationMatrix(&qrot, &mtemp);

	D3DXMatrixTransformation(&mtemp, &Vec3(0.5f, 0.5f), NULL, &scale, NULL, NULL, NULL);
	tform3.scale = mtemp;
	D3DXMatrixTransformation(&mtemp, NULL, NULL, NULL, &Vec3(0.5f, 0.5f), &qrot, &move);
	tform3.transforms.Push(mtemp);

	tobj3.AddTransform(tform3);
	AddOperation(tobj3);

	//////////////////////////////////////////////////////////////////////////

	tobj4.Init();
	tform4.Init(tobj4);

	move = Vec3(-0.9*val, -1.333*val, 0);

	D3DXMatrixRotationZ(&mtemp, 0.0f * Deg2Rad);
	D3DXQuaternionRotationMatrix(&qrot, &mtemp);

	D3DXMatrixTransformation(&mtemp, &Vec3(0.5f, 0.5f), NULL, &scale, NULL, NULL, NULL);
	tform4.scale = mtemp;
	D3DXMatrixTransformation(&mtemp, NULL, NULL, NULL, &Vec3(0.5f, 0.5f), &qrot, &move);
	tform4.transforms.Push(mtemp);

	tobj4.AddTransform(tform4);
	AddOperation(tobj4);

	//////////////////////////////////////////////////////////////////////////

	tobj5.Init();
	tform5.Init(tobj5);

	move = Vec3(-1.5*val, 0.2*val, 0);

	D3DXMatrixRotationZ(&mtemp, 0.0f * Deg2Rad);
	D3DXQuaternionRotationMatrix(&qrot, &mtemp);

	D3DXMatrixTransformation(&mtemp, &Vec3(0.5f, 0.5f), NULL, &scale, NULL, NULL, NULL);
	tform5.scale = mtemp;
	D3DXMatrixTransformation(&mtemp, NULL, NULL, NULL, &Vec3(0.5f, 0.5f), &qrot, &move);
	tform5.transforms.Push(mtemp);

	tobj5.AddTransform(tform5);
	AddOperation(tobj5);

	m_nMaxIterations	= 5;

	return 1;
}

Int32 CMRCM::SetupFern()
{
	Bool retVal = ResetInitialTexture("texture_leaf.bmp");

	TransformObject tobj(this);
	TransformObject tobj2(this);
	TransformObject tobj3(this);
	TransformObject tobj4(this);

	TransformObject::Transform tform;
	TransformObject::Transform tform2;
	TransformObject::Transform tform3;
	TransformObject::Transform tform4;

	Float scaleval = 0.85f;
	Float val = scaleval * 0.5f;

	D3DMatrix mtemp;

	Vec3 scale;
	Vec3 move;

	//////////////////////////////////////////////////////////////////////////

	scale = Vec3(scaleval, scaleval, 0.0f);
	move = Vec3(0, 0.07f, 0);

	tobj.Init();
	tform.Init(tobj);

	D3DXQUATERNION qrot;
	D3DXMatrixRotationZ(&mtemp, -2.5f * Deg2Rad);
	D3DXQuaternionRotationMatrix(&qrot, &mtemp);

	D3DXMatrixTransformation(&mtemp, &Vec3(0.5f, 0.5f), NULL, &scale, NULL, NULL, NULL);
	tform.scale = mtemp;
	D3DXMatrixTransformation(&mtemp, NULL, NULL, NULL, &Vec3(0.5f, 0.5f), &qrot, &move);
	tform.transforms.Push(mtemp);

	tobj.AddTransform(tform);
	AddOperation(tobj);

	//////////////////////////////////////////////////////////////////////////

	tobj2.Init();
	tform2.Init(tobj2);

	move = Vec3(0.11, -0.42f, 0);
	scale = Vec3(0.3f, 0.37f, 0);

	D3DXMatrixRotationZ(&mtemp, -120.0f * Deg2Rad);
	D3DXQuaternionRotationMatrix(&qrot, &mtemp);

	D3DXMatrixTransformation(&mtemp, &Vec3(0.5f, 0.5f), NULL, &scale, NULL, NULL, NULL);
	tform2.scale = mtemp;
	D3DXMatrixTransformation(&mtemp, NULL, NULL, NULL, &Vec3(0.5f, 0.5f), &qrot, &move);
	tform2.transforms.Push(mtemp);

	tobj2.AddTransform(tform2);
	AddOperation(tobj2);

	//////////////////////////////////////////////////////////////////////////

	tobj3.Init();
	tform3.Init(tobj3);

	move = Vec3(-0.11, -0.3f, 0);
	scale = Vec3(0.3f, 0.34f, 0);

	D3DXMatrixRotationZ(&mtemp, 49.0f * Deg2Rad);
	D3DXQuaternionRotationMatrix(&qrot, &mtemp);

	D3DXMatrixTransformation(&mtemp, &Vec3(0.5f, 0.5f), NULL, &scale, NULL, NULL, NULL);
	tform3.scale = mtemp;
	D3DXMatrixTransformation(&mtemp, NULL, NULL, NULL, &Vec3(0.5f, 0.5f), &qrot, &move);
	tform3.transforms.Push(mtemp);

	tobj3.AddTransform(tform3);
	AddOperation(tobj3);

	//////////////////////////////////////////////////////////////////////////

	tobj4.Init();
	tform4.Init(tobj4);

	move = Vec3(0, -0.1, 0);
	scale = Vec3(0.02f, 0.75, 0);

	D3DXMatrixRotationZ(&mtemp, 0.0f * Deg2Rad);
	D3DXQuaternionRotationMatrix(&qrot, &mtemp);

	D3DXMatrixTransformation(&mtemp, &Vec3(0.5f, 0.5f), NULL, &scale, NULL, NULL, NULL);
	tform4.scale = mtemp;
	D3DXMatrixTransformation(&mtemp, NULL, NULL, NULL, &Vec3(0.5f, 0.5f), &qrot, &move);
	tform4.transforms.Push(mtemp);

	tobj4.AddTransform(tform4);
	AddOperation(tobj4);

	m_nMaxIterations	= 14;

	return 1;
}

Int32 CMRCM::SetupKochCurve()
{
	Bool retVal = ResetInitialTexture("texture_rect.bmp");

	TransformObject tobj(this);
	TransformObject tobj2(this);
	TransformObject tobj3(this);
	TransformObject tobj4(this);

	TransformObject::Transform tform;
	TransformObject::Transform tform2;
	TransformObject::Transform tform3;
	TransformObject::Transform tform4;

	Float scaleval = 1/3.0f;
	Float val = scaleval * 0.5f;

	D3DMatrix mtemp;

	Vec3 scale;
	Vec3 move;

	//////////////////////////////////////////////////////////////////////////

	scale = Vec3(scaleval, scaleval, 0.0f);
	move = Vec3(-0.33, 0, 0);

	tobj.Init();
	tform.Init(tobj);

	D3DXQUATERNION qrot;
	D3DXMatrixRotationZ(&mtemp, 0.0f * Deg2Rad);
	D3DXQuaternionRotationMatrix(&qrot, &mtemp);

	D3DXMatrixTransformation(&mtemp, &Vec3(0.5f, 0.5f), NULL, &scale, NULL, NULL, NULL);
	tform.scale = mtemp;
	D3DXMatrixTransformation(&mtemp, NULL, NULL, NULL, &Vec3(0.5f, 0.5f), &qrot, &move);
	tform.transforms.Push(mtemp);

	tobj.AddTransform(tform);
	AddOperation(tobj);

	//////////////////////////////////////////////////////////////////////////

	tobj2.Init();
	tform2.Init(tobj2);

	move = Vec3(0.085, 0.13, 0);

	D3DXMatrixRotationZ(&mtemp, -60.0f * Deg2Rad);
	D3DXQuaternionRotationMatrix(&qrot, &mtemp);

	D3DXMatrixTransformation(&mtemp, &Vec3(0.5f, 0.5f), NULL, &scale, NULL, NULL, NULL);
	tform2.scale = mtemp;
	D3DXMatrixTransformation(&mtemp, NULL, NULL, NULL, &Vec3(0.5f, 0.5f), &qrot, &move);
	tform2.transforms.Push(mtemp);

	tobj2.AddTransform(tform2);
	AddOperation(tobj2);

	//////////////////////////////////////////////////////////////////////////

	tobj3.Init();
	tform3.Init(tobj3);

	move = Vec3(-0.085, 0.13, 0);

	D3DXMatrixRotationZ(&mtemp, 60.0f * Deg2Rad);
	D3DXQuaternionRotationMatrix(&qrot, &mtemp);

	D3DXMatrixTransformation(&mtemp, &Vec3(0.5f, 0.5f), NULL, &scale, NULL, NULL, NULL);
	tform3.scale = mtemp;
	D3DXMatrixTransformation(&mtemp, NULL, NULL, NULL, &Vec3(0.5f, 0.5f), &qrot, &move);
	tform3.transforms.Push(mtemp);

	tobj3.AddTransform(tform3);
	AddOperation(tobj3);

	//////////////////////////////////////////////////////////////////////////

	tobj4.Init();
	tform4.Init(tobj4);

	move = Vec3(0.33, 0, 0);

	D3DXMatrixRotationZ(&mtemp, 0.0f * Deg2Rad);
	D3DXQuaternionRotationMatrix(&qrot, &mtemp);

	D3DXMatrixTransformation(&mtemp, &Vec3(0.5f, 0.5f), NULL, &scale, NULL, NULL, NULL);
	tform4.scale = mtemp;
	D3DXMatrixTransformation(&mtemp, NULL, NULL, NULL, &Vec3(0.5f, 0.5f), &qrot, &move);
	tform4.transforms.Push(mtemp);

	tobj4.AddTransform(tform4);
	AddOperation(tobj4);

	m_nMaxIterations	= 3;

	return 1;
}