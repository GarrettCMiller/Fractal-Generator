#pragma once

#include "fractal.h"

#include "VectorGraphics.h"
#include "GeneticAlgorithm.h"

//////////////////////////////////////////////////////////////////////////

class CLSystemGenotype : public CBaseGenotype
{
public:
	CLSystemGenotype() : CBaseGenotype(), m_fRating(1.0f)
	{	
		CBaseGenotype::mk_numElements	= 3; 
		CBaseGenotype::mk_numTotalBits	= (sizeof(Float) * mk_numElements);
	}

	void SetValues(const TArgumentArray& arrVars)
	{
		theta			= arrVars[0];
		thetaMin		= arrVars[1];
		thetaMax		= arrVars[2];
		
		stepSize		= arrVars[3];
		stepSizeMin		= arrVars[4];
		stepSizeMax		= arrVars[5];
		
		stepAdjust		= arrVars[6];
		stepAdjustMin	= arrVars[7];
		stepAdjustMax	= arrVars[8];

		m_VarArray		= arrVars;
		
		CreateBitString();
	}

	void SetDefaults()
	{
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

		SetValues(arrVars);
	}

	Float Evaluate() const
	{
		return m_fRating;
	}

	void Randomize()
	{
		if (m_VarArray.IsEmpty())
			SetDefaults();

		m_VarArray[0]	=	theta		= RandomG(thetaMax, thetaMin, 3);
		m_VarArray[3]	=	stepSize	= RandomG(stepSizeMax, stepSizeMin, 3);
		m_VarArray[6]	=	stepAdjust	= RandomG(stepAdjustMax, stepAdjustMin, 3);

		CreateBitString();
	}

public:
	RangedVariable(Float, theta)
	RangedVariable(Float, stepSize)
	RangedVariable(Float, stepAdjust)

	Float m_fRating;
};

//////////////////////////////////////////////////////////////////////////

class CLSystemGeneticAlgorithm;

//////////////////////////////////////////////////////////////////////////

class CLSystem : public CFractal
{
public:
	enum ETurtleCommand
	{
		eTC_MoveForward,
		eTC_DrawForward,

		eTC_TurnLeft,
		eTC_TurnRight,

		eTC_TurnDown,
		eTC_TurnUp,

		eTC_DrawSegment,

		eTC_PositionSave,
		eTC_PositionReturn
	};

	typedef CDynamicArray<ETurtleCommand>	TTurtleCommands;
	typedef std::map<Char, String>			TRuleMap;

public:
	CLSystem(void) { }
	CLSystem(const char* strName, Bool bCreateStates = true);

	virtual ~CLSystem(void);

	struct InitStruct : public CFractal::InitStruct
	{
		String	alphabet;
		String	axiom;
		String	rules;
		Float	theta;
		Float	initialTheta;
		Float	stepSize;
		Float	stepAdjust;
	};

	void SetTheta(Float theta)
	{
		m_Theta = Clamp(theta, 0.0f, 360.0f);
	}

	Float GetTheta() const
	{
		return m_Theta;
	}

	void SetStepSize(Float step)
	{
		m_StepSize = Clamp(step, 0.001f, 10.0f);
	}

	Float GetStepSize() const
	{
		return m_StepSize;
	}
	
	void SetStepAdjust(Float adj)
	{
		m_fStepAdjust = adj;//Clamp(adj, 0.001f, 10.0f);
	}

	Float GetStepAdjust() const
	{
		return m_fStepAdjust;
	}
	//Standard inherited funcs
public:
	virtual Int32 Initialize(CWnd* parentFrame = NULL, TArgumentArray* pArgs = NULL);
	virtual Int32 Update();
	virtual Int32 Shutdown();

	virtual Int32 Reset();

	virtual Int32 Draw();

	virtual void MeshThreadData(Int32 threadNum = -1);

	virtual Int32 Begin();

	Bool CreateFromGenotype(CLSystemGenotype* gene);
	Bool CreateFromRandomGenotype();

protected:
	virtual Bool SetupProcessors();
	static DWORD ProcessingThread(LPVOID param);

protected:
	struct TurtleState
	{
		Float	theta;
		Float	stepSize;
		Vec3	pos;
		Float	dir;
	};

	Bool ProcessString(String& strString);
	String ProcessRule(Char letter);

	Bool ConvertToTurtleCommands(const String& strString, TTurtleCommands& commands);

	void DrawAlgae();
	void DrawTurtle();

public:
	Bool				m_bRandomize;
	Bool				m_bShowAllSteps;

	String				m_Alphabet;
	String				m_Axiom;

	Float				m_InitialTheta;
	Float				m_Theta;
	
	Float				m_StepSize;
	Float				m_fStepAdjust;

	Int32				m_nIterations;

	TRuleMap			m_Rules;

	String				m_CurrentString;

	Timer				m_GrowthTimer;

	TTurtleCommands		m_TurtleCommands;

	CLSystemGenotype*	m_pGene;

protected:
	CVectorGraphics		m_Vectors;

	//////////////////////////////////////////////
	// FSM stuff
	//////////////////////////////////////////////
public:
	ADD_FSM_FUNCTIONS

	class CUninitializedState;
	class CIdleState;
	class CProcessingState;
	class CRenderingState;
	class CCancelledState;
};

typedef CFSMState<CLSystem>		TLSystemState;

//////////////////////////////////////////////////////////////////////////

#define MAKE_STATE(_stateName)		MAKE_STATE_CLASS(CLSystem, _stateName, TLSystemState)

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

class CLSystemGeneticAlgorithm : public CGeneticAlgorithm<CLSystemGenotype>
{
	UInt32	m_CurrentIndex;

public:
	CLSystemGeneticAlgorithm() : CGeneticAlgorithm(), m_CurrentIndex(0)	{	}
	virtual ~CLSystemGeneticAlgorithm()				{	}

	void CreateInitialPopulation(UInt32 popSize)
	{
		CGeneticAlgorithm<CLSystemGenotype>::CreateInitialPopulation(popSize);
	}

	CLSystemGenotype* GetNextMember()
	{
		if (m_CurrentPopulation.Size() == m_CurrentIndex)
			return NULL;

		return static_cast<CLSystemGenotype*>(m_CurrentPopulation[m_CurrentIndex++]);
	}

	CLSystemGenotype* GetCurrentMember(UInt32 index)
	{
		return static_cast<CLSystemGenotype*>(m_CurrentPopulation[index]);
	}

	CLSystemGenotype* GetSurvivor(UInt32 index)
	{
		return static_cast<CLSystemGenotype*>(m_CurrentSurvivors[index]);
	}

	void CheckFitness()
	{
		CGeneticAlgorithm<CLSystemGenotype>::CheckFitness();
		m_CurrentIndex = 0;
	}
};

typedef CLSystemGeneticAlgorithm	TLSysGenAlg;
typedef TLSysGenAlg*				TLSysGenAlgPtr;