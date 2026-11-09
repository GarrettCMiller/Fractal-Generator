#include "StdAfx.h"

#include "ComplexProcessor.h"
#include "ComplexFractal.h"

//Make it static so that it persists and doesn't have to be reset every call
#define CAST_PARENT				static CComplexFractal* pParent = static_cast<CComplexFractal*>(m_pParent)
//////////////////////////////////////////////////////////////////////////

CComplexProcessor::CComplexProcessor(const char* strName /* = NULL */, Bool bCreateStates /* = true */) :
	CProcessor(strName, false),
		m_PreviewBlockSize(4)
{
	if (bCreateStates)
	{
		TComplexProcessorState	*pUnInitState		= new CUninitializedState(this,	k_strUnInit),
								*pIdleState			= new CIdleState(this,			k_strIdle),
								*pProcState			= new CProcessingState(this,	k_strProcessing),
								//*pRenderState		= new CRenderingState(this,		k_strRender),
								*pCancelledState	= new CCancelledState(this,		k_strCancelled);

		pUnInitState->AddValidTransition(pIdleState);

		pIdleState->AddValidTransition(pProcState);
		//pIdleState->AddValidTransition(pRenderState);

		//pProcState->AddValidTransition(pRenderState);
		pProcState->AddValidTransition(pIdleState);
		pProcState->AddValidTransition(pCancelledState);

		//pRenderState->AddValidTransition(pIdleState);
		//pRenderState->AddValidTransition(pCancelledState);

		pCancelledState->AddValidTransition(pIdleState);

		FSM_Initialize(pUnInitState);
	}
}

//////////////////////////////////////////////////////////////////////////

Int32 CComplexProcessor::Init(CFractal& _parent, TArgumentArray* pArgs /* = NULL */)
{
	CProcessor::Init(_parent, pArgs);

	ASSERT(pArgs);

	CAST_PARENT;

	TArgumentArray& args = *pArgs;

	UInt32		x = args[0],
				y = args[1],
				cx = args[2],
				cy = args[3];

	m_Data.Resize(cx, cy);

	m_Viewport.tlx = m_pParent->m_Viewport.tlx + x * pParent->m_dX.real();
	m_Viewport.tly = m_pParent->m_Viewport.tly + y * pParent->m_dY.imag();
	m_Viewport.brx = m_Viewport.tlx + cx * pParent->m_dX.real();
	m_Viewport.bry = m_Viewport.tly + cy * pParent->m_dY.imag();

	m_vCurrentPixel	= m_vOrigin = Vec2i(x, y);

	m_vImageSize = Vec2i(cx, cy);

	//m_PreviewStartBlock = Vec2i(0, 0);
	m_PreviewBlockSize = cx / 8;

	m_P	= TComplex(m_Viewport.tlx, m_Viewport.tly);

	SAFE_CALL(m_Functions[eFF_Init], this);

	FSM_QueueState(k_strIdle);
	FSM_Update();

	return 0;
}

Int32 CComplexProcessor::Update(TArgumentArray* pArgs /* = NULL */)
{
	if (CProcessor::Update() == -1)
		return -1;

	m_PercentComplete = float(m_vImageSize.X * m_vCurrentPixel.Y + m_vCurrentPixel.X) / float(m_vImageSize.X * m_vImageSize.Y) * 100.0f;

	//HACK
	m_PercentComplete = Clamp(m_PercentComplete, 0.0f, 100.0f);

	return 0;
}

Int32 CComplexProcessor::Shutdown(TArgumentArray* pArgs /* = NULL */)
{
	////

	CProcessor::Shutdown(pArgs);

	return 0;
}

//////////////////////////////////////////////////////////////////////////

void CComplexProcessor::ResetRow()
{
	CAST_PARENT;

	//Move the pixel down one
	m_vCurrentPixel.Y += m_PreviewBlockSize;

	//Move the complex point down by the delta Y factor
	m_P.imag(m_P.imag() + (m_PreviewBlockSize * pParent->m_dY.imag()));

	//Reset the row info
	m_vCurrentPixel.X = m_vOrigin.X;
	m_P	= TComplex(m_Viewport.tlx, m_P.imag());

	//Have we finished the last row of pixels?
	if (m_vCurrentPixel.Y >= m_vOrigin.Y + m_vImageSize.Y)
	{
		if (m_PreviewBlockSize == 1)
		{
			//Perform any final cleanup
			m_Functions[eFF_Done](this);

			//Stop the processing timer
			//m_ProcessingTime = m_ProcessTimer.Get();

			//Tell the processor it is done
			FSM_QueueState(k_strIdle);
		}
		else
		{
			//m_PreviewStartBlock.X++;
			m_PreviewBlockSize /= 2;

			m_vCurrentPixel.Y = m_vOrigin.Y;//+ m_PreviewStartBlock.Y;
			m_P	= TComplex(m_Viewport.tlx /*+ (m_pParent->m_dX.real() * m_PreviewBlockSize)*/, m_Viewport.tly);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CComplexProcessor::UpdatePosition()
{
	CAST_PARENT;

	//PROFILE_THIS(2);

	//Move the pixel over
	m_vCurrentPixel.X += m_PreviewBlockSize;

	//Move the complex point over by the delta X factor
	m_P.real(m_P.real() + (pParent->m_dX.real() * m_PreviewBlockSize));

	//Have we finished the last pixel in the row?
	if (m_vCurrentPixel.X >= m_vOrigin.X + m_vImageSize.X)
		ResetRow();
}

//////////////////////////////////////////////////////////////////////////

void CComplexProcessor::IterateCurrentPoint()
{
	Int32 ret = -1;
	//PROFILE_THIS(1);

	{
		//PROFILE("Preiter", 8);
		//Call the pre-iter ONLY if we have one
		m_Functions[eFF_PreIter](this);
	}

	for (m_CurrentIteration = 0; m_CurrentIteration < m_pParent->m_nMaxIterations; m_CurrentIteration++)
	{
		//PROFILE("iters", 9);
		ret = m_Functions[eFF_Iter](this);

		if (ret == 0)	//REQUIRED
			break;
	}

	{
		//PROFILE("postiter", 10);
		//Call the post-iter ONLY if we have one
		m_Functions[eFF_PostIter](this);
	}

	{
		//PROFILE("draw", 11);
		//REQUIRED
		m_Functions[eFF_Draw](this);
	}
}

//////////////////////////////////////////////////////////////////////////

void CComplexProcessor::ColorCurrentPixel()
{
	m_pParent->m_CS.Lock();

	if (m_PreviewBlockSize > 1)
	{
		for (UInt32 y = 0; y < m_PreviewBlockSize; y++)
			for (UInt32 x = 0; x < m_PreviewBlockSize; x++)
				ColorPixel(m_vCurrentPixel + Vec2u(x, y));
	}
	else
		ColorPixel(m_vCurrentPixel);

	m_pParent->m_CS.Unlock();
}

//////////////////////////////////////////////////////////////////////////

void CComplexProcessor::ColorPixel(const Vec2u& pos)
{
	CAST_PARENT;

	//pParent->m_CS.Lock();

	static UInt8* pImage = static_cast<UInt8*>(m_pParent->m_Image.m_Image.GetBits());

	if (pos.X - m_vOrigin.X > m_vImageSize.X)
		return;
	if (pos.Y - m_vOrigin.Y > m_vImageSize.Y)
		return;

 	UInt32 index = (pos.Y * m_pParent->m_Image.m_Image.GetPitch()) + (pos.X * 3);
 
 	pImage[index + 0] = m_color.GetBlue();
 	pImage[index + 1] = m_color.GetGreen();
 	pImage[index + 2] = m_color.GetRed();

	//pParent->m_CS.Unlock();
		
	//memcpy(&m_pParent->m_img[index], &m_color, 3);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Int32 CComplexProcessor::FSM_Initialize(IFSMStateBase* pStartState)
{
	return __super::FSM_Initialize(pStartState);
}

Int32 CComplexProcessor::FSM_Update()
{
	return __super::FSM_Update();
}

Int32 CComplexProcessor::FSM_Shutdown()
{
	return __super::FSM_Shutdown();
}

Int32 CComplexProcessor::FSM_SendMessage(TArgumentArray& args)
{
	return __super::FSM_SendMessage(args);
}

//////////////////////////////////////////////////////////////////////////

#define CAST_STATE_PARENT		CComplexProcessor* pProc = static_cast<CComplexProcessor*>(m_pFSM)

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CComplexProcessor::CUninitializedState::CUninitializedState(CComplexProcessor* pFSM, const String& strName)
: TComplexProcessorState(pFSM, strName)
{
}

Int32 CComplexProcessor::CUninitializedState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CComplexProcessor::CUninitializedState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CComplexProcessor::CUninitializedState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CComplexProcessor::CUninitializedState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CComplexProcessor::CIdleState::CIdleState(CComplexProcessor* pFSM, const String& strName)
: TComplexProcessorState(pFSM, strName)
{

}

Int32 CComplexProcessor::CIdleState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CComplexProcessor::CIdleState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CComplexProcessor::CIdleState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CComplexProcessor::CIdleState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CComplexProcessor::CProcessingState::CProcessingState(CComplexProcessor* pFSM, const String& strName) 
: TComplexProcessorState(pFSM, strName)
{

}

Int32 CComplexProcessor::CProcessingState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	CAST_STATE_PARENT;

	if (pProc->m_pThread->Start() <= 0)
		return -1;

	return __super::OnEnter(pArgs);
}

Int32 CComplexProcessor::CProcessingState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	CAST_STATE_PARENT;

	static const Float k_MaxTimeSlice = 1.0f / 8.0f;
	static Timer tester(true);

	if (pProc->m_Data(pProc->m_vCurrentPixel - pProc->m_vOrigin) == 0.0)
	{
		pProc->IterateCurrentPoint();

		pProc->ColorCurrentPixel();
	}

	pProc->UpdatePosition();

	if (tester >= k_MaxTimeSlice)
	{
		pProc->m_pParent->MeshThreadData(pProc->m_ProcID);

		//if (::IsWindow(m_pParent->m_Window->GetSafeHwnd()))
		//m_pParent->m_Window->Invalidate(false);

		tester.Reset();
	}

	return __super::OnUpdate(pArgs);
}

Int32 CComplexProcessor::CProcessingState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CComplexProcessor::CProcessingState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

// CComplexProcessor::CRenderingState::CRenderingState(CComplexProcessor* pFSM, const String& strName)
// : TComplexProcessorState(pFSM, strName)
// {
// 
// }
// 
// Int32 CComplexProcessor::CRenderingState::OnEnter(TArgumentArray* pArgs /* = NULL */)
// {
// 	return __super::OnEnter(pArgs);
// }
// 
// Int32 CComplexProcessor::CRenderingState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
// {
// 	return __super::OnUpdate(pArgs);
// }
// 
// Int32 CComplexProcessor::CRenderingState::OnExit(TArgumentArray* pArgs /* = NULL */)
// {
// 	return __super::OnExit(pArgs);
// }
// 
// Int32 CComplexProcessor::CRenderingState::OnMessage(TArgumentArray* pArgs /* = NULL */)
// {
// 	return __super::OnMessage(pArgs);
// }

//////////////////////////////////////////////////////////////////////////

CComplexProcessor::CCancelledState::CCancelledState(CComplexProcessor* pFSM, const String& strName)
: TComplexProcessorState(pFSM, strName)
{

}

Int32 CComplexProcessor::CCancelledState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CComplexProcessor::CCancelledState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CComplexProcessor::CCancelledState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CComplexProcessor::CCancelledState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

#undef CAST_STATE_PARENT