#include "StdAfx.h"

#include "LSystem.h"
#include "LSystemProcessor.h"

#include "GlobalFuncs.h"

CLSystem::CLSystem(const char* strName, Bool bCreateStates /* = true */) :
	CFractal(strName, false),	
		m_Theta(30.0f), m_StepSize(0.5f), 
		m_bShowAllSteps(false)
		
{
	m_eType = eFT_LSystem;

	if (bCreateStates)
	{
		TLSystemState	*pUnInitState		= new CUninitializedState(this,	k_strUnInit),
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

	m_bD3D = true;
}

CLSystem::~CLSystem(void)
{
}

//////////////////////////////////////////////////////////////////////////

Int32 CLSystem::Initialize(CWnd* parentFrame /* = NULL */, TArgumentArray* pArgs /* = NULL */)
{
	if (CFractal::Initialize(parentFrame, pArgs) != 0)
		return -1;

	TArgumentArray& args	= *pArgs;
	Pointer pInitStruct		= args[0];

	InitStruct& is			= *(reinterpret_cast<InitStruct*>(pInitStruct));

	m_Alphabet				= is.alphabet;
	m_Axiom					= is.axiom;
	
	m_Theta					= is.theta;
	m_StepSize				= is.stepSize;

	m_InitialTheta			= is.initialTheta;

	Float adj = is.stepAdjust;

	if (adj == 0.0f)
		adj = 1.0f / g_Phi;

	m_fStepAdjust			= adj;

	String rules			= is.rules;

	TStringArray	tokens;
	tokens = Tokenize(rules, " ");

	String rule, result;
	Char letter;

	for (UInt32 i = 0; i < tokens.Size(); i++)
	{
		rule = tokens[i];
		letter = rule[0];
		result = rule.substr(2);

		m_Rules[letter] = result;
	}

	m_CurrentString			= m_Axiom;

	m_nMaxIterations		= is.maxIters;

	//if (m_pGenAlg)
	//	CreateFromGenotype(static_cast<TLSysGenAlgPtr>(m_pGenAlg)->GetNextMember());

	g_D3D->SetOrthoDisplay(25.0f, 25.0f);

	//SetupProcessors();

	m_Vectors.Init();

	FSM_QueueState(k_strIdle);
	FSM_Update();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CLSystem::Update()
{
	FSM_Update();

	//if (FSM_GetCurrentStateName() == k_strIdle)

	//if (KEYDOWN(VK_NUMPAD1))


	return 0;
}

//////////////////////////////////////////////////////////////////////////

Bool CLSystem::ProcessString(String& strString)
{
	String result;

	Char letter = '\0';

	for (UInt32 l = 0; l < strString.length(); l++)
	{
		letter = strString[l];

		if (m_Alphabet.find(letter) != String::npos)
			result += ProcessRule(letter);
		else
		{
			switch (letter)
			{
			case '[':
			case ']':
			case '+':
			case '-':
			case '|':
				result += letter;
				break;
			
			default: //We don't accept any other symbols
				ASSERT(0);
			}
		}
	}

	strString = result;

	return true;
}

//////////////////////////////////////////////////////////////////////////

String CLSystem::ProcessRule(Char letter)
{
	String result = m_Rules[letter];
	return result;
}

//////////////////////////////////////////////////////////////////////////

Bool CLSystem::ConvertToTurtleCommands(const String& strString, TTurtleCommands& commands)
{
	Char letter = '\0';

	for (UInt32 l = 0; l < strString.length(); l++)
	{
		letter = strString[l];

		switch (letter)
		{
		case 'F':
			commands.Push(eTC_DrawForward);
			break;

		case 'f':
			commands.Push(eTC_MoveForward);
			break;

		case '+':
			commands.Push(eTC_TurnLeft);
			break;

		case '-':
			commands.Push(eTC_TurnRight);
			break;

		case '|':
			commands.Push(eTC_DrawSegment);
			break;

		case '[':
			commands.Push(eTC_PositionSave);
			break;

		case ']':
			commands.Push(eTC_PositionReturn);
			break;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

Int32 CLSystem::Shutdown()
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CLSystem::Reset()
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CLSystem::Begin()
{
	Int32 ret = CFractal::Begin();

	return ret;
}

//////////////////////////////////////////////////////////////////////////

Int32 CLSystem::Draw()
{
	m_CS.Lock();

	//DrawAlgae();
	DrawTurtle();

	m_CS.Unlock();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

void CLSystem::DrawTurtle()
{
	static String stringLengths;

	m_TurtleCommands.Clear();
	ConvertToTurtleCommands(m_CurrentString, m_TurtleCommands);

	ETurtleCommand command;

	//CImage& image = m_Image.m_Image;
	
	CDynamicArray<TurtleState> stateStack;
	TurtleState currentState;

	Vec2 tempvec;

	currentState.dir = m_InitialTheta;
	currentState.pos = Vec3(0.0f, 0.0f, 0.0f);
	currentState.stepSize = m_StepSize;
	currentState.theta = m_Theta;

	//CDC* cdc = CDC::FromHandle(image.GetDC());

	//CBrush brush(0xFFFFFF);
	//CRect rect(0, 0, 1024, 1024);

	//cdc->FillRect(&rect, &brush);

	//cdc->MoveTo(currentState.pos);

	for (UInt32 c = 0; c < m_TurtleCommands.Size(); c++)
	{
		command = m_TurtleCommands[c];

		switch (command)
		{
		case eTC_DrawForward:
		case eTC_DrawSegment:
			{
				while (currentState.dir < 0) currentState.dir += 360.0f;

				tempvec.SetSpeedDir(currentState.stepSize /** m_Processors[0]->m_CurrentIteration*/, currentState.dir);
				tempvec = Vec2(currentState.pos) + tempvec;

				m_Vectors.CreateLine(currentState.pos, tempvec);

				currentState.stepSize *= m_fStepAdjust;

				currentState.pos = tempvec;
				//cdc->LineTo(currentState.pos);
			}
			break;

		case eTC_MoveForward:
			{
				while (currentState.dir < 0) currentState.dir += 360.0f;

				tempvec.SetSpeedDir(currentState.stepSize /** m_Processors[0]->m_CurrentIteration*/, currentState.dir);
				tempvec = Vec2(currentState.pos) + tempvec;

				currentState.stepSize *= m_fStepAdjust;

				//We specifically don't want to draw, just move
				//m_Vectors.CreateLine(currentState.pos, tempvec);

				currentState.pos = tempvec;
				//cdc->LineTo(currentState.pos);
			}
			break;

		case eTC_TurnLeft:
			{
				currentState.dir -= currentState.theta;
			}
			break;

		case eTC_TurnRight:
			{
				currentState.dir += currentState.theta;
			}
			break;

		case eTC_PositionSave:
			{
				stateStack.Push(currentState);
			}
			break;

		case eTC_PositionReturn:
			{
				stateStack.Pop(currentState);
				//cdc->MoveTo(currentState.pos);
			}
			break;
		}
	}

	//image.ReleaseDC();
}

//////////////////////////////////////////////////////////////////////////

void CLSystem::DrawAlgae()
{
	Color color;
	Char letter;

	UInt32 numSegments = m_CurrentString.length();
	UInt32 width = m_GrowthTimer.Get() * 4;
	UInt32 localWidth = 0;

	UInt32	rx = 0,//512 - (numSegments * width / 2),
		ry = 512 - 5;

	for (UInt32 i = 0; i < numSegments; i++)
	{
		localWidth = width;

		letter = m_CurrentString[i];

		if (letter == 'A')
			color = Color(0.65f, 1.0f, 0, 0);
		else if (letter == 'B')
			color = Color(1.0f, 0.65f, 0, 0);
		else if (letter == 'C')
		{
			color = Color(1.0f, 1.0f, 1.0f, 0);
			localWidth = 20;
		}
		else if (letter == 'D')
		{
			color = Color(0, 0, 1.0f, 0);
			localWidth = 20;
		}

		//m_Vectors.
		/*for (UInt32 py = 0; py < 5; py++)
		{
			for (UInt32 px = 0; px < localWidth; px++)
			{
				m_Image.m_Image.SetPixel(px + i * localWidth + rx, py + ry, color);
			}
		}*/
	}
}

//////////////////////////////////////////////////////////////////////////

Bool CLSystem::CreateFromGenotype(CLSystemGenotype* gene)
{
	m_pGene = gene;
	SetTheta(gene->theta);
	SetStepSize(gene->stepSize);
	SetStepAdjust(gene->stepAdjust);

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool CLSystem::CreateFromRandomGenotype()
{
	CLSystemGenotype gene;
	
	TArgumentArray arrVars(9);
	arrVars[0]	= 0.0f;
	arrVars[1]	= 1.0f;
	arrVars[2]	= 179.0f;

	arrVars[3]	= 0.0f;
	arrVars[4]	= 0.01f;
	arrVars[5]	= 1.0f;
	
	arrVars[6]	= 0.0f;
	arrVars[7]	= 0.01f;
	arrVars[8]	= 1.0f;

	gene.SetValues(arrVars);
	
	gene.Randomize();

	return CreateFromGenotype(&gene);
}

//////////////////////////////////////////////////////////////////////////

Bool CLSystem::SetupProcessors()
{
	CFractal::SetupProcessors();

	return true;
}

//////////////////////////////////////////////////////////////////////////

DWORD CLSystem::ProcessingThread(LPVOID param)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////

void CLSystem::MeshThreadData(Int32 threadNum /* = -1 */)
{
	m_CurrentString.empty();

	for (UInt32 p = 0; p < m_Processors.Size(); p++)
	{
		m_CurrentString += static_cast<CLSystemProcessor*>(m_Processors[p])->GetCurrentString();
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Int32 CLSystem::FSM_Initialize(IFSMStateBase* pStartState)
{
	return __super::FSM_Initialize(pStartState);
}

//////////////////////////////////////////////////////////////////////////

Int32 CLSystem::FSM_Update()
{
	return __super::FSM_Update();
}

//////////////////////////////////////////////////////////////////////////

Int32 CLSystem::FSM_Shutdown()
{
	return __super::FSM_Shutdown();
}

//////////////////////////////////////////////////////////////////////////

Int32 CLSystem::FSM_SendMessage(TArgumentArray& args)
{
	return __super::FSM_SendMessage(args);
}

//////////////////////////////////////////////////////////////////////////

#define CAST_STATE_PARENT		CLSystem* pFrac = static_cast<CLSystem*>(m_pFSM)

//////////////////////////////////////////////////////////////////////////

CLSystem::CUninitializedState::CUninitializedState(CLSystem* pFSM, const String& strName)
: TLSystemState(pFSM, strName)
{
}

Int32 CLSystem::CUninitializedState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CLSystem::CUninitializedState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CLSystem::CUninitializedState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CLSystem::CUninitializedState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CLSystem::CIdleState::CIdleState(CLSystem* pFSM, const String& strName)
: TLSystemState(pFSM, strName)
{

}

Int32 CLSystem::CIdleState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CLSystem::CIdleState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	CAST_STATE_PARENT;

	Bool bKeyHit = false;

	if (KEYDOWN(VK_NUMPAD0))
	{
		pFrac->m_pGene->m_fRating = 10.0f;
		bKeyHit = true;
	}
	else if (KEYDOWN(VK_NUMPAD1))
	{
		pFrac->m_pGene->m_fRating = 1.0f;
		bKeyHit = true;
	}
	else if (KEYDOWN(VK_NUMPAD2))
	{
		pFrac->m_pGene->m_fRating = 2.0f;
		bKeyHit = true;
	}
	else if (KEYDOWN(VK_NUMPAD3))
	{
		pFrac->m_pGene->m_fRating = 3.0f;
		bKeyHit = true;
	}
	else if (KEYDOWN(VK_NUMPAD4))
	{
		pFrac->m_pGene->m_fRating = 4.0f;
		bKeyHit = true;
	}
	else if (KEYDOWN(VK_NUMPAD5))
	{
		pFrac->m_pGene->m_fRating = 5.0f;
		bKeyHit = true;
	}
	else if (KEYDOWN(VK_NUMPAD6))
	{
		pFrac->m_pGene->m_fRating = 6.0f;
		bKeyHit = true;
	}
	else if (KEYDOWN(VK_NUMPAD7))
	{
		pFrac->m_pGene->m_fRating = 7.0f;
		bKeyHit = true;
	}
	else if (KEYDOWN(VK_NUMPAD8))
	{
		pFrac->m_pGene->m_fRating = 8.0f;
		bKeyHit = true;
	}
	else if (KEYDOWN(VK_NUMPAD9))
	{
		pFrac->m_pGene->m_fRating = 9.0f;
		bKeyHit = true;
	}

	if (bKeyHit)
	{
		/*TLSysGenAlgPtr pGenAlg = static_cast<TLSysGenAlgPtr>(pFrac->m_pGenAlg);
		CLSystemGenotype* pGene = pGenAlg->GetNextMember();

		if (pGene)		
			pFrac->CreateFromGenotype(pGene);
		else
		{
			MessageBox(g_hWnd, "Next Generation", "NG", MB_OK);
			pGenAlg->CheckFitness();
			pGene = pGenAlg->GetNextMember();
			pFrac->CreateFromGenotype(pGene);
		}*/

		pFrac->FSM_QueueState(k_strProcessing);
	}

	return __super::OnUpdate(pArgs);
}

Int32 CLSystem::CIdleState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CLSystem::CIdleState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CLSystem::CProcessingState::CProcessingState(CLSystem* pFSM, const String& strName) 
: TLSystemState(pFSM, strName)
{

}

Int32 CLSystem::CProcessingState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	CAST_STATE_PARENT;

	pFrac->m_GrowthTimer.Reset();
	pFrac->m_nIterations = 0;

	pFrac->m_CurrentString = pFrac->m_Axiom;

	pFrac->m_TurtleCommands.Clear();

	//pFrac->Draw();
	//pFrac->m_Window->Invalidate(false);

	return __super::OnEnter(pArgs);
}

Int32 CLSystem::CProcessingState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	CAST_STATE_PARENT;

	if (!pFrac->m_bShowAllSteps || pFrac->m_GrowthTimer >= 1.0f)
	{
		pFrac->ProcessString(pFrac->m_CurrentString);

		if (++pFrac->m_nIterations >= pFrac->m_nMaxIterations)
		{
			pFrac->FSM_QueueState(k_strRender);
			return __super::OnUpdate(pArgs);
		}

		pFrac->m_GrowthTimer.Reset();
	}

	return __super::OnUpdate(pArgs);
}

Int32 CLSystem::CProcessingState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CLSystem::CProcessingState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CLSystem::CRenderingState::CRenderingState(CLSystem* pFSM, const String& strName)
: TLSystemState(pFSM, strName)
{

}

Int32 CLSystem::CRenderingState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CLSystem::CRenderingState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	CAST_STATE_PARENT;

	if (1)
	{
		pFrac->FSM_QueueState(k_strIdle);
		return __super::OnUpdate(pArgs);
	}

	return __super::OnUpdate(pArgs);
}

Int32 CLSystem::CRenderingState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CLSystem::CRenderingState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////

CLSystem::CCancelledState::CCancelledState(CLSystem* pFSM, const String& strName)
: TLSystemState(pFSM, strName)
{

}

Int32 CLSystem::CCancelledState::OnEnter(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnEnter(pArgs);
}

Int32 CLSystem::CCancelledState::OnUpdate(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnUpdate(pArgs);
}

Int32 CLSystem::CCancelledState::OnExit(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnExit(pArgs);
}

Int32 CLSystem::CCancelledState::OnMessage(TArgumentArray* pArgs /* = NULL */)
{
	return __super::OnMessage(pArgs);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#undef CAST_STATE_PARENT

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////