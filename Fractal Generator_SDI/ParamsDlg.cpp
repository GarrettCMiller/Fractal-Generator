// ParamsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Fractal Generator.h"
#include "ParamsDlg.h"
#include ".\paramsdlg.h"
#include "Fractal.h"

// ParamsDlg dialog

IMPLEMENT_DYNAMIC(ParamsDlg, CDialog)
ParamsDlg::ParamsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ParamsDlg::IDD, pParent)
	, m_fTLx(0)
	, m_fBRx(0)
	, m_fTLy(0)
	, m_fBRy(0)
	, m_fBailValue(0)
	, m_nMaxIterations(0)
{
}

ParamsDlg::~ParamsDlg()
{
}

void ParamsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_XMIN, m_XMin);
	DDX_Control(pDX, IDC_XMAX, m_XMax);
	DDX_Control(pDX, IDC_YMIN, m_YMin);
	DDX_Control(pDX, IDC_YMAX, m_YMax);
	DDX_Control(pDX, IDC_BAILOUT, m_Bailout);
	DDX_Control(pDX, IDC_MAXITERS, m_MaxIterations);
}


BEGIN_MESSAGE_MAP(ParamsDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// ParamsDlg message handlers

void ParamsDlg::OnBnClickedOk()
{
	//OnOK();

	UpdateData();

	CString text;

// 	m_XMin.					GetWindowText(text);
// 	frac.m_fTLx				= atof(text);
// 	
// 	m_XMax.					GetWindowText(text);
// 	frac.m_fBRx				= atof(text);
// 
// 	m_YMin.					GetWindowText(text);
// 	frac.m_fTLy				= atof(text);
// 
// 	m_YMax.					GetWindowText(text);
// 	frac.m_fBRy				= atof(text);
// 
// 	m_Bailout.				GetWindowText(text);
// 	frac.m_fBailValue		= atof(text);
// 	
// 	m_MaxIterations.		GetWindowText(text);
// 	frac.m_nMaxIterations	= atoi(text);
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

	char buffer[80] = {"0"};

	if (bShow)
	{
// 		m_XMin.SetWindowText(_gcvt(frac.m_fTLx, 10, buffer));
// 		m_XMax.SetWindowText(_gcvt(frac.m_fBRx, 10, buffer));
// 		m_YMin.SetWindowText(_gcvt(frac.m_fTLy, 10, buffer));
// 		m_YMax.SetWindowText(_gcvt(frac.m_fBRy, 10, buffer));
// 		m_Bailout.SetWindowText(_gcvt(frac.m_fBailValue, 10, buffer));
// 		m_MaxIterations.SetWindowText(itoa(frac.m_nMaxIterations, buffer, 10));
// 
// 		UpdateData(false);
	}
}
