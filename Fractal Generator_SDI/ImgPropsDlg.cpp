// ImgPropsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Fractal Generator.h"
#include "ImgPropsDlg.h"
#include ".\imgpropsdlg.h"

#include "Fractal.h"
#include "MainFrm.h"
// ImgPropsDlg dialog

IMPLEMENT_DYNAMIC(ImgPropsDlg, CDialog)
ImgPropsDlg::ImgPropsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ImgPropsDlg::IDD, pParent)
{
}

ImgPropsDlg::~ImgPropsDlg()
{
}

void ImgPropsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAIN_WIDTH, m_MainWidth);
	DDX_Control(pDX, IDC_MAIN_HEIGHT, m_MainHeight);
	DDX_Control(pDX, IDC_SECONDARY_WIDTH, m_SecondaryWidth);
	DDX_Control(pDX, IDC_SECONDARY_HEIGHT, m_SecondaryHeight);
}


BEGIN_MESSAGE_MAP(ImgPropsDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// ImgPropsDlg message handlers

void ImgPropsDlg::OnBnClickedOk()
{
	UpdateData();

	CString text;
	int temp;

	bool	mainchg = false, 
			secchg = false;

	m_MainWidth.GetWindowText(text);
	temp = atoi(text);

// 	if (temp != frac.m_ncX)
// 	{
// 		mainchg		= true;
// 		frac.m_ncX	= temp;
// 	}
// 
// 	m_MainHeight.GetWindowText(text);
// 	temp = atoi(text);
// 
// 	if ( temp != frac.m_ncY )
// 	{
// 		mainchg		= true;
// 		frac.m_ncY	= temp;
// 	}
// 
// 	m_SecondaryWidth.GetWindowText(text);
// 	temp = atoi(text);
// 	
// 	if ( temp != julia.m_ncX )
// 	{
// 		secchg		= true;
// 		julia.m_ncX	= temp;
// 	}
// 
// 	m_SecondaryHeight.GetWindowText(text);
// 	temp = atoi(text);
// 
// 	if (temp != julia.m_ncY)
// 	{
// 		secchg		= true;
// 		julia.m_ncY	= temp;
// 	}
// 
// 	if (mainchg)
// 	{
// 		frac.CreateBitmap();
// 		((CMainFrame*) GetParent())->m_wndView.Invalidate(false);
// 	}
// 
// 	if (secchg)
// 	{
// 		julia.CreateBitmap();
// 		((CMainFrame*) GetParent())->m_JuliaDlg.SetSize();
// 	}
}

BOOL ImgPropsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	char buffer[80];

// 	m_MainWidth.SetWindowText( itoa( frac.m_ncX, buffer, 10 ) );
// 	m_MainHeight.SetWindowText( itoa( frac.m_ncY, buffer, 10 ) );
// 
// 	m_SecondaryWidth.SetWindowText( itoa( julia.m_ncX, buffer, 10 ) );
// 	m_SecondaryHeight.SetWindowText( itoa( julia.m_ncY, buffer, 10 ) );

	UpdateData( true );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
