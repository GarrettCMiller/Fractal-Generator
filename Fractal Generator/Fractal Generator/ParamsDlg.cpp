// ParamsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Fractal Generator.h"
#include "ParamsDlg.h"
#include ".\paramsdlg.h"
#include "Fractal.h"

#include "ChildFrm.h"

// ParamsDlg dialog

IMPLEMENT_DYNAMIC(ParamsDlg, CPropertyPage)
ParamsDlg::ParamsDlg()
	: CPropertyPage(ParamsDlg::IDD)
	, m_nMaxIterations(1)
{
}

ParamsDlg::~ParamsDlg()
{
}

void ParamsDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MAXITERS, m_nMaxIterations);
}


BEGIN_MESSAGE_MAP(ParamsDlg, CPropertyPage)
	ON_BN_CLICKED(IDAPPLY, OnBnClickedOk)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

/*
#define THREAD_SAFE(call)			if (g_pCurrentEmitter->m_bActive) {						\
g_pCurrentEmitter->m_csCriticalSection.Lock();	\
call;									\
g_pCurrentEmitter->m_csCriticalSection.Unlock(); 		\
} else call;
*/
// ParamsDlg message handlers

void ParamsDlg::OnBnClickedOk()
{
	//OnOK();

	CFractal* pFrac = theApp.GetActiveFractal();

	if (!pFrac)	return;

	pFrac->m_CS.Lock();

	pFrac->m_nMaxIterations = m_nMaxIterations;
	pFrac->Reset();
	//pFrac->Begin();

	pFrac->m_CS.Unlock();

	UpdateData();
}

BOOL ParamsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void ParamsDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if (bShow)
	{
		CFractal* pFrac = theApp.GetActiveFractal();

		if (!pFrac)	return;

//  		m_XMin.SetWindowText(_gcvt(pFrac->m_fTLx, 10, buffer));
//  		m_XMax.SetWindowText(_gcvt(pFrac->m_fBRx, 10, buffer));
//  		m_YMin.SetWindowText(_gcvt(pFrac->m_fTLy, 10, buffer));
//  		m_YMax.SetWindowText(_gcvt(pFrac->m_fBRy, 10, buffer));
 		//m_Bailout.SetWindowText(_gcvt(pFrac->m_fBailValue, 10, buffer));
 		//m_MaxIterations.SetWindowText(itoa(pFrac->m_nMaxIterations, buffer, 10));

		m_nMaxIterations = pFrac->m_nMaxIterations;
 
 		UpdateData(false);
	}
}
