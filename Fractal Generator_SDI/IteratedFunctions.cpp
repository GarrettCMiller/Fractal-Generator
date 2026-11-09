#include "stdafx.h"

#include "IteratedFunctions.h"
#include "ProfileSample.h"

// 	if (m_bJulia)
// 		m_C = m_J;
// 
// 	m_Z = m_P;

int Init_DefaultMandelbrot(CFractal::Processor& proc)
{
	return 1;
}

int PreI_DefaultMandelbrot(CFractal::Processor& proc)
{
	//PROFILE_THIS(4);

	proc.m_C = proc.m_P;
	//m_C = Complex(m_fPx / (m_fPx * m_fPx + m_fPy * m_fPy), (-m_fPy) / (m_fPx * m_fPx + m_fPy * m_fPy));

	proc.m_Z = proc.m_P;

	return 1;
}

int Iter_DefaultMandelbrot(CFractal::Processor& proc)
{
	double x2, y2, orbit;

	//PROFILE_THIS(5);

 	x2 = proc.m_Z.real();
 	y2 = proc.m_Z.imag();
 
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
 
 	proc.m_Z = (proc.m_Z * proc.m_Z) - proc.m_C;

	return 1;
}

int Post_DefaultMandelbrot(CFractal::Processor& proc)
{
	int extra;
	double x2, y2, orbit;

	//PROFILE_THIS(6);
	
	//Extra iterations to find smoother escape time
	for (extra = 0; extra < 4; extra++, proc.m_CurrentIteration++)
	{
		x2 = proc.m_Z.real();
		y2 = proc.m_Z.imag();
		proc.m_Z = (proc.m_Z * proc.m_Z) - proc.m_C;
	}

	orbit = sqrt((x2 * x2) + (y2 * y2));

	proc.m_EscapeVal = proc.m_CurrentIteration + 1 - ((log(log(orbit))) / log(2.0));

	//EnterCriticalSection(&proc.m_pParent->m_CS);

	proc.m_Data(proc.m_vCurrentPixel - proc.m_vOrigin) = proc.m_EscapeVal;

	//LeaveCriticalSection(&proc.m_pParent->m_CS);

	return 1;
}

int Draw_DefaultMandelbrot(CFractal::Processor& proc)
{
	//PROFILE_THIS(7);

 	double escapeVal;
 	int i;

	i = proc.m_CurrentIteration;
	escapeVal = proc.m_EscapeVal;
 
 	//if (m_bSmoothShading)
 	//{
 		escapeVal = fabsf(escapeVal);
 
 		int index;
 		float delta;
 
 		index = int(escapeVal);
 		delta = escapeVal - index;
 
 		i = index;

		//EnterCriticalSection(&proc.m_pParent->m_CS);

		index = proc.m_pParent->m_Palette.GetColorGradient(i, delta);

		//LeaveCriticalSection(&proc.m_pParent->m_CS);
 
 		return index;
	//}
	//else
	//	return args[efPalette].GetColor(int(escapeVal));

	return 1;
}

int Done_DefaultMandelbrot(CFractal::Processor& proc)
{
	return 1;
}