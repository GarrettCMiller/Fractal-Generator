#include "StdAfx.h"
#include "ComplexFractal.h"

#include "IteratedFunctions.h"

#include "ComplexProcessor.h"

#include "Fractal Generator.h"

//////////////////////////////////////////////////////////////////////////

CComplexFractal::CComplexFractal(const char* strName /* = NULL */, Bool bCreateStates /* = true */) :
	CFractal(strName, false),
		m_fBailValue(4.0)
{
	m_eType = eFT_Complex;

	if (bCreateStates)
	{
		TComplexFractalState	*pUnInitState		= new CUninitializedState(this,	k_strUnInit),
								*pIdleState			= new CIdleState(this,			k_strIdle),
								*pProcState			= new CProcessingState(this,	k_strProcessing),
								*pRenderState		= new CRenderingState(this,		k_strRender),
								*pCancelledState	= new CCancelledState(this,		k_strCancelled);

		pUnInitState->AddValidTransition(pIdleState);

		pIdleState->AddValidTransition(pProcState);
		pIdleState->AddValidTransition(pRenderState);

		pProcState->AddValidTransition(pRenderState);
		pProcState->AddValidTransition(pCancelledState);

		pRenderState->AddValidTransition(pIdleState);
		pRenderState->AddValidTransition(pCancelledState);

		pCancelledState->AddValidTransition(pIdleState);

		FSM_Initialize(pUnInitState);
	}

	m_Functions.Resize(6);

	m_Functions[eFF_Init]		= TProcessorFunction(&Init_DefaultMandelbrot);
	m_Functions[eFF_PreIter]	= TProcessorFunction(&PreI_DefaultMandelbrot);
	m_Functions[eFF_Iter]		= TProcessorFunction(&Iter_DefaultMandelbrot);
	m_Functions[eFF_PostIter]	= TProcessorFunction(&Post_DefaultMandelbrot);
	m_Functions[eFF_Draw]		= TProcessorFunction(&Draw_DefaultMandelbrot);
	m_Functions[eFF_Done]		= TProcessorFunction(&Done_DefaultMandelbrot);
}

CComplexFractal::~CComplexFractal(void)
{
}

//////////////////////////////////////////////////////////////////////////

void CComplexFractal::SetupJulia(UInt32 x, UInt32 y)
{
 	Double	mx = x * m_dX.real() + m_Viewport.tlx,
 			my = y * m_dY.imag() + m_Viewport.tly;
 
 	m_bJulia = true;
 
	//TEMP
 	//m_J = TComplex(mx, my);
	//mx = 0;
	//my = 0;
 
 	m_Viewport = GLib::Rectd(-2.0, 2.0, 2.0, -2.0);
}

//////////////////////////////////////////////////////////////////////////

Int32 CComplexFractal::Initialize(CWnd* parentFrame /* = NULL */, TArgumentArray* pArgs /* = NULL */)
{
	if (CFractal::Initialize(parentFrame, pArgs) != 0)
		return -1;

	if (m_Functions[eFF_Iter] == NULL)
	{
		MessageBox(m_Window->GetSafeHwnd(), "No iteration function specified!", "Cannot Process Fractal", MB_OK);
		return -1;
	}

	if (m_Functions[eFF_Draw] == NULL)
	{
		MessageBox(m_Window->GetSafeHwnd(), "No drawing function specified!", "Cannot Process Fractal", MB_OK);
		return -2;
	}

	//Account for -Y screen space by making the dY negative
	m_dX = TComplex(m_vSize.X / static_cast<Float>(m_vImageSize.X), 0);
	m_dY = TComplex(0, -(m_vSize.Y / static_cast<Double>(m_vImageSize.Y)));

	m_Data.Resize(m_vImageSize);

	SetupProcessors();

	FSM_QueueState(k_strIdle);
	FSM_Update();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Bool CComplexFractal::SetupProcessors()
{
	CFractal::SetupProcessors();

	UInt32	numcols = (m_nThreadCount > 1 ? static_cast<UInt32>(sqrt(Float(m_nThreadCount))) : m_nThreadCount),
			numrows = numcols,
			imgSizeX = m_vImageSize.X,
			imgSizeY = m_vImageSize.Y;

	Var		px,
			py,
			pcx,
			pcy;

	TArgumentArray args(4);
	CComplexProcessor* proc = NULL;

	CDynamicArray<ProcessingData*> data;
	data.Resize(m_nThreadCount);

	CStaticThreadFunctor	functor(&CComplexFractal::ProcessingThread);

	char temp[16];

	//Initialize all processors first
	for (UInt32 threadID = 0; threadID < m_nThreadCount; threadID++)
	{
		_itoa_s(threadID, temp, 8, 10);
		String procName = "ComplexProcessor #" + String(temp);
		
		proc = new CComplexProcessor(procName.c_str());
		data[threadID] = new ProcessingData;

		data[threadID]->m_parent = this;
		data[threadID]->m_procID = proc->m_ProcID = threadID;

		g_ThreadMgr->CreateThread(functor, data[threadID], false, &proc->m_pThread, CThread::eTPF_Low);

		px = (threadID % numcols) * (imgSizeX / numcols);
		py = (threadID / numcols) * (imgSizeY / numrows);
		pcx = imgSizeX / numcols;
		pcy = imgSizeY / numrows;

		if (px > 0)
			--px;

		if (py > 0)
			--py;

		args[0] = px;
		args[1] = py;
		args[2] = pcx;
		args[3] = pcy;

		//TODO FIX PROCESSORS
		if (proc->Init(*this, &args) < 0)
		{
			g_ThreadMgr->KillAll();
			m_Processors.FreePointers();

			for (UInt32 i = 0; i < threadID; i++)
				delete data[i];
		}

		m_Processors.Push(proc);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CComplexFractal::Update()
{
	return CFractal::Update();
}

//////////////////////////////////////////////////////////////////////////

Int32 CComplexFractal::Reset()
{
	return CFractal::Reset();
}

//////////////////////////////////////////////////////////////////////////

DWORD CComplexFractal::ProcessingThread(LPVOID param)
{
	ProcessingData* data = static_cast<ProcessingData *>(param);

	TProcessorArray& pProcArray = data->m_parent->m_Processors;
	UInt32 pID = data->m_procID;

	CComplexProcessor* proc = static_cast<CComplexProcessor*>(pProcArray[pID]);

	while (theApp.m_bQuit != true && theApp.m_bCloseWindow != true)
	{
		if (proc->Update() != 0)
			break;
	}

	//Make sure to tell the thread that it has exited
	proc->m_pThread->Invalidate();

	delete data;

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CComplexFractal::Draw()
{
	m_CS.Lock();

	Float max = 0.0f;
	Float escVal = 0.0f;
	Color color;

	for (UInt32 y = 0; y < m_vImageSize.Y; y++)
	{
		for ( UInt32 x = 0; x < m_vImageSize.X; x++ )
		{
			escVal = m_Data(x ,y);

			if (max < escVal)
				max = escVal;

			static_cast<CComplexProcessor*>(m_Processors[0])->m_EscapeVal = escVal;

			color = m_Functions[eFF_Draw](m_Processors[0]);

			m_Image.m_Image.SetPixel(x, y, color);
		}
	}

	m_CS.Unlock();

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Int32 CComplexFractal::FSM_Initialize(IFSMStateBase* pStartState)
{
	return __super::FSM_Initialize(pStartState);
}

Int32 CComplexFractal::FSM_Update()
{
	return __super::FSM_Update();
}

Int32 CComplexFractal::FSM_Shutdown()
{
	return __super::FSM_Shutdown();
}

Int32 CComplexFractal::FSM_SendMessage(TArgumentArray& args)
{
	return __super::FSM_SendMessage(args);
}

//////////////////////////////////////////////////////////////////////////

#define CAST_STATE_PARENT		CComplexFractal* pFrac = static_cast<CComplexFractal*>(m_pFSM)

//////////////////////////////////////////////////////////////////////////

CComplexFractal::CUninitializedState::CUninitializedState(CComplexFractal* pFSM, const String& strName)
: TComplexFractalState(pFSM, strName)
{
}

Int32 CComplexFractal::CUninitializedState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	CAST_STATE_PARENT;

	pFrac->m_Viewport =	//Rectd(-2, 1.5, 2.8, -1.5);
		GLib::Rectd(-1.5, 2, 2.5, -2);
	//Rectd(-1, 2, 3, -2);

	pFrac->m_Data.Resize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
	pFrac->m_Functions.Resize(eFF_NumFunctions);
	pFrac->m_Processors.Resize(m_MaxThreads);

	pFrac->CreatePalette();

	return __super::OnEnter(pArgs);
}

Int32 CComplexFractal::CUninitializedState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CComplexFractal::CUninitializedState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CComplexFractal::CUninitializedState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CComplexFractal::CIdleState::CIdleState(CComplexFractal* pFSM, const String& strName)
: TComplexFractalState(pFSM, strName)
{

}

Int32 CComplexFractal::CIdleState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CComplexFractal::CIdleState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CComplexFractal::CIdleState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CComplexFractal::CIdleState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CComplexFractal::CProcessingState::CProcessingState(CComplexFractal* pFSM, const String& strName) 
: TComplexFractalState(pFSM, strName)
{

}

Int32 CComplexFractal::CProcessingState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CComplexFractal::CProcessingState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CComplexFractal::CProcessingState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CComplexFractal::CProcessingState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CComplexFractal::CRenderingState::CRenderingState(CComplexFractal* pFSM, const String& strName)
: TComplexFractalState(pFSM, strName)
{

}

Int32 CComplexFractal::CRenderingState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CComplexFractal::CRenderingState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CComplexFractal::CRenderingState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CComplexFractal::CRenderingState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CComplexFractal::CCancelledState::CCancelledState(CComplexFractal* pFSM, const String& strName)
: TComplexFractalState(pFSM, strName)
{

}

Int32 CComplexFractal::CCancelledState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CComplexFractal::CCancelledState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CComplexFractal::CCancelledState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CComplexFractal::CCancelledState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#undef CAST_STATE_PARENT

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////