#include "stdafx.h"

#include "IteratedFunctions.h"
#include "ProfileSample.h"
#include "ComplexProcessor.h"

#include "ComplexFractal.h"

// 	if (m_bJulia)
// 		m_C = m_J;
// 
// 	m_Z = m_P;

int Init_DefaultMandelbrot(CProcessor* pProc)
{
	CComplexProcessor* proc = static_cast<CComplexProcessor*>(pProc);

	return 1;
}

int PreI_DefaultMandelbrot(CProcessor* pProc)
{
	//PROFILE_THIS(4);
	CComplexProcessor* proc = static_cast<CComplexProcessor*>(pProc);

	proc->m_C = proc->m_P;
	//m_C = Complex(m_fPx / (m_fPx * m_fPx + m_fPy * m_fPy), (-m_fPy) / (m_fPx * m_fPx + m_fPy * m_fPy));

	proc->m_Z = proc->m_P;

	return 1;
}

int Iter_DefaultMandelbrot(CProcessor* pProc)
{
	double x2, y2, orbit;

	CComplexProcessor* proc = static_cast<CComplexProcessor*>(pProc);

	//PROFILE_THIS(5);

 	x2 = proc->m_Z.real();
 	y2 = proc->m_Z.imag();
 
 	orbit =	(x2 * x2) + (y2 * y2);
			//(m_x2 * m_y2) + (m_x2 * m_y2);
			//m_x2 * m_y2;
			//m_x2 + m_y2;
			//m_x2 / m_y2;
			//m_x2 - m_y2;
			//(m_x2 * m_x2) - (m_y2 * m_y2);
			//(m_x2 * m_x2) * (m_y2 * m_y2);
 			//(m_x2 * m_x2) / (m_y2 * m_y2);
 
 	if (orbit > /*m_fBailValue*/4.0f)
 		return 0;
 
 	proc->m_Z = (proc->m_Z * proc->m_Z) - proc->m_C;

	return 1;
}

int Post_DefaultMandelbrot(CProcessor* pProc)
{
	int extra;
	double x2, y2, orbit;

	CComplexProcessor* proc = static_cast<CComplexProcessor*>(pProc);

	//PROFILE_THIS(6);
	
	//Extra iterations to find smoother escape time
	for (extra = 0; extra < 4; extra++, proc->m_CurrentIteration++)
	{
		x2 = proc->m_Z.real();
		y2 = proc->m_Z.imag();
		proc->m_Z = (proc->m_Z * proc->m_Z) - proc->m_C;
	}

	orbit = sqrt((x2 * x2) + (y2 * y2));

	proc->m_EscapeVal = proc->m_CurrentIteration + 1 - ((log(log(orbit))) / log(2.0));

	//EnterCriticalSection(&proc->m_pParent->m_CS);
	proc->m_pParent->m_CS.Lock();

	proc->m_Data(proc->m_vCurrentPixel - proc->m_vOrigin) = proc->m_EscapeVal;

	//LeaveCriticalSection(&proc->m_pParent->m_CS);
	proc->m_pParent->m_CS.Unlock();

	return 1;
}

Int32 Draw_DefaultMandelbrot(CProcessor* pProc)
{
	//PROFILE_THIS(7);

 	Double escapeVal;
 	Int32 i;

	CComplexProcessor* proc = static_cast<CComplexProcessor*>(pProc);

	i = proc->m_CurrentIteration;
	escapeVal = proc->m_EscapeVal;
 
 	//if (m_bSmoothShading)
 	//{
 		escapeVal = fabsf(escapeVal);
 
 		Int32 index;
 		Float delta;
 
 		index = Int32(escapeVal);
 		delta = escapeVal - index;
 
 		i = index;

		static Color palColor;

		//proc->m_pParent->m_CS.Lock();
		
		palColor = proc->m_pParent->m_Image.GetPalette().GetColorGradient(i, delta);

		//proc->m_pParent->m_CS.Unlock();

		proc->m_color = palColor;
	//}
	//else
	//	return args[efPalette].GetColor(int(escapeVal));

	return 1;
}

int Done_DefaultMandelbrot(CProcessor* pProc)
{
	CComplexProcessor* proc = static_cast<CComplexProcessor*>(pProc);

	return 1;
}