// PaletteDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Fractal Generator.h"
#include "PaletteDlg.h"
#include ".\palettedlg.h"

#include "MainFrm.h"
#include "Fractal.h"

#include <math.h>
// PaletteDlg dialog

static const float k_Freq = 25.0f;

IMPLEMENT_DYNAMIC(PaletteDlg, CDialog)
PaletteDlg::PaletteDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PaletteDlg::IDD, pParent)
	, m_bRed(TRUE)
	, m_bGreen(TRUE)
	, m_bBlue(TRUE)
	, m_fRedAmp(0.5f)
	, m_fRedFreq(k_Freq)
	, m_fRedBase(0.5f)
	, m_fGreenAmp(0.5f)
	, m_fGreenFreq(k_Freq)
	, m_fGreenBase(0.5f)
	, m_fBlueAmp(0.5f)
	, m_fBlueFreq(k_Freq)
	, m_fBlueBase(0.5f)
	, m_bSmoothShading(TRUE)
	, m_ColorPhase(0)
{
	m_RedOffset		= 85;//0;
	m_GreenOffset	= 170;//128;
	m_BlueOffset	= 0;//256;

	MakePal();
}

PaletteDlg::~PaletteDlg()
{
}

void PaletteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Slider(pDX, IDC_REDSLIDER, m_RedOffset);
	DDX_Slider(pDX, IDC_GREENSLIDER, m_GreenOffset);
	DDX_Slider(pDX, IDC_BLUESLIDER, m_BlueOffset);
	DDX_Check(pDX, IDC_ENABLERED, m_bRed);
	DDX_Check(pDX, IDC_ENABLEGREEN, m_bGreen);
	DDX_Check(pDX, IDC_ENABLEBLUE, m_bBlue);
	DDX_Text(pDX, IDC_AMP_RED, m_fRedAmp);
	DDX_Text(pDX, IDC_FREQ_RED, m_fRedFreq);
	DDX_Text(pDX, IDC_BASE_RED, m_fRedBase);
	DDX_Text(pDX, IDC_AMP_GREEN, m_fGreenAmp);
	DDX_Text(pDX, IDC_FREQ_GREEN, m_fGreenFreq);
	DDX_Text(pDX, IDC_BASE_GREEN, m_fGreenBase);
	DDX_Text(pDX, IDC_AMP_BLUE, m_fBlueAmp);
	DDX_Text(pDX, IDC_FREQ_BLUE, m_fBlueFreq);
	DDX_Text(pDX, IDC_BASE_BLUE, m_fBlueBase);
	DDX_Check(pDX, IDC_SMOOTH_SHADING, m_bSmoothShading);
	DDX_Slider(pDX, IDC_PHASESLIDER, m_ColorPhase);
	DDV_MinMaxInt(pDX, m_ColorPhase, -127, 127);
}


BEGIN_MESSAGE_MAP(PaletteDlg, CDialog)
	ON_WM_PAINT()
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_REDSLIDER, OnNMReleasedcaptureRedslider)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_GREENSLIDER, OnNMReleasedcaptureGreenslider)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_BLUESLIDER, OnNMReleasedcaptureBlueslider)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_REDSLIDER, OnNMCustomdrawRedslider)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_GREENSLIDER, OnNMCustomdrawGreenslider)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_BLUESLIDER, OnNMCustomdrawBlueslider)
	ON_BN_CLICKED(IDC_ENABLERED, OnBnClickedEnablered)
	ON_BN_CLICKED(IDC_ENABLEGREEN, OnBnClickedEnablegreen)
	ON_BN_CLICKED(IDC_ENABLEBLUE, OnBnClickedEnableblue)
	ON_EN_CHANGE(IDC_AMP_RED, OnEnChangeAmpRed)
	ON_EN_CHANGE(IDC_FREQ_RED, OnEnChangeFreqRed)
	ON_EN_CHANGE(IDC_BASE_RED, OnEnChangeBaseRed)
	ON_EN_CHANGE(IDC_AMP_GREEN, OnEnChangeAmpGreen)
	ON_EN_CHANGE(IDC_FREQ_GREEN, OnEnChangeFreqGreen)
	ON_EN_CHANGE(IDC_BASE_GREEN, OnEnChangeBaseGreen)
	ON_EN_CHANGE(IDC_AMP_BLUE, OnEnChangeAmpBlue)
	ON_EN_CHANGE(IDC_FREQ_BLUE, OnEnChangeFreqBlue)
	ON_EN_CHANGE(IDC_BASE_BLUE, OnEnChangeBaseBlue)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_PHASESLIDER, &PaletteDlg::OnNMCustomdrawPhaseslider)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_PHASESLIDER, &PaletteDlg::OnNMReleasedcapturePhaseslider)
END_MESSAGE_MAP()


// PaletteDlg message handlers

void PaletteDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	int height	= 15,
		yadj	= 30,
		ix		= 33,
		iy		= 0,
		index	= 0;
	
	float	sf	= 1,//m_fRedFreq, //256 * m_Palette.Size(),
			sfC	= sf;

	for (int x = 0; x < 256; x++ )
	{
		for ( int y = 0; y < height * 2; y++)
		{
			index = x / sf;

			dc.SetPixelV( x + ix, y + iy, m_Palette[index]);
			dc.SetPixelV( x + ix, y + iy, m_Palette[index]);
			dc.SetPixelV( x + ix, y + iy, m_Palette[index]);
			dc.SetPixelV( x + ix, y + iy, m_Palette[index]);
			dc.SetPixelV( x + ix, y + iy, m_Palette[index]);
		}
	}

	iy += height + 25 + yadj;

	for (int x = 0; x < 256; x++ )
	{
		for ( int y = 0; y < height; y++)
		{
			index = x / sfC;

			dc.SetPixelV( x + ix, y + iy, RGB(m_Reds[index], 0, 0));
			dc.SetPixelV( x + ix, y + iy, RGB(m_Reds[index], 0, 0));
			dc.SetPixelV( x + ix, y + iy, RGB(m_Reds[index], 0, 0));
			dc.SetPixelV( x + ix, y + iy, RGB(m_Reds[index], 0, 0));
			dc.SetPixelV( x + ix, y + iy, RGB(m_Reds[index], 0, 0));
		}
	}

	iy += height + yadj;

	for (int x = 0; x < 256; x++ )
	{
		for ( int y = 0; y < height; y++)
		{
			index = x / sfC;

			dc.SetPixelV( x + ix, y + iy, RGB(0, m_Greens[index], 0));
			dc.SetPixelV( x + ix, y + iy, RGB(0, m_Greens[index], 0));
			dc.SetPixelV( x + ix, y + iy, RGB(0, m_Greens[index], 0));
			dc.SetPixelV( x + ix, y + iy, RGB(0, m_Greens[index], 0));
			dc.SetPixelV( x + ix, y + iy, RGB(0, m_Greens[index], 0));
		}
	}

	iy += height + yadj;

	for (int x = 0; x < 256; x++ )
	{
		for ( int y = 0; y < height; y++)
		{
			index = x / sfC;

			dc.SetPixelV( x + ix, y + iy, RGB(0, 0, m_Blues[index]));
			dc.SetPixelV( x + ix, y + iy, RGB(0, 0, m_Blues[index]));
			dc.SetPixelV( x + ix, y + iy, RGB(0, 0, m_Blues[index]));
			dc.SetPixelV( x + ix, y + iy, RGB(0, 0, m_Blues[index]));
			dc.SetPixelV( x + ix, y + iy, RGB(0, 0, m_Blues[index]));
		}
	}

	char buf[80];

	sprintf(buf, "R: %d   G: %d   B: %d", m_RedOffset, m_GreenOffset, m_BlueOffset);

	SetWindowText(buf);
}

void PaletteDlg::MakePal()
{
	int MaxIterations = 1024;

	m_Palette.Resize(MaxIterations, false);
	m_Reds.Resize(MaxIterations, false);
	m_Greens.Resize(MaxIterations, false);
	m_Blues.Resize(MaxIterations, false);

	float	r = 0.0f,
			g = 0.0f,
			b = 0.0f;

	for (int i = 0; i < MaxIterations; i++)
	{
		if (m_bRed)
			r = Sine( m_fRedFreq, m_fRedBase, m_fRedAmp,		fabsf(float(i + m_RedOffset + m_ColorPhase) / MaxIterations));

		if (m_bGreen)
			g = Sine( m_fGreenFreq, m_fGreenBase, m_fGreenAmp,	fabsf(float(i + m_GreenOffset + m_ColorPhase) / MaxIterations));

		if (m_bBlue)
			b = Sine( m_fBlueFreq, m_fBlueBase, m_fBlueAmp,		fabsf(float(i + m_BlueOffset + m_ColorPhase) / MaxIterations));

		m_Reds[i]		= r * 255;
		m_Greens[i]		= g * 255;
		m_Blues[i]		= b * 255;

		m_Palette[i]	= Color( r * 255, g * 255, b * 255 );
	}
}

void PaletteDlg::OnNMReleasedcaptureRedslider(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(true);

	MakePal();

	Invalidate(false);

	SetFocus();

	*pResult = 0;
}

void PaletteDlg::OnNMReleasedcaptureGreenslider(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(true);

	MakePal();

	Invalidate(false);

	SetFocus();

	*pResult = 0;
}

void PaletteDlg::OnNMReleasedcaptureBlueslider(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(true);

	MakePal();

	Invalidate(false);

	SetFocus();

	*pResult = 0;
}

void PaletteDlg::OnBnClickedOk()
{
	UpdateData();

	//CFractal::m_bSmoothShading = m_bSmoothShading;

	frac.SetPalette(m_Palette, true);
	julia.SetPalette(m_Palette, true);

	((CMainFrame *) GetParent())->m_wndView.Invalidate(false);
	((CMainFrame *) GetParent())->m_JuliaDlg.Invalidate(false);
}

BOOL PaletteDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CSliderCtrl*)GetDlgItem(IDC_REDSLIDER))->		SetRange(0, 256);
	((CSliderCtrl*)GetDlgItem(IDC_GREENSLIDER))->	SetRange(0, 256);
	((CSliderCtrl*)GetDlgItem(IDC_BLUESLIDER))->	SetRange(0, 256);

	m_RedOffset		= frac.m_Palette.GetRedOffset();
	m_GreenOffset	= frac.m_Palette.GetGreenOffset();
	m_BlueOffset	= frac.m_Palette.GetBlueOffset();

	MakePal();

	UpdateData( false );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void PaletteDlg::OnNMCustomdrawRedslider(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	
	if (pNMCD->uItemState == CDIS_FOCUS)
	{
		UpdateData(true);

		MakePal();

		Invalidate(false);
	}

	((CSliderCtrl*)GetDlgItem(pNMHDR->idFrom))->EnableWindow(m_bRed);

	*pResult = 0;
}

void PaletteDlg::OnNMCustomdrawGreenslider(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	
	if (pNMCD->uItemState == CDIS_FOCUS)
	{
		UpdateData(true);

		MakePal();

		Invalidate(false);
	}

	((CSliderCtrl*)GetDlgItem(pNMHDR->idFrom))->EnableWindow(m_bGreen);

	*pResult = 0;
}

void PaletteDlg::OnNMCustomdrawBlueslider(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	
	if (pNMCD->uItemState == CDIS_FOCUS)
	{
		UpdateData(true);

		MakePal();

		Invalidate(false);
	}

	((CSliderCtrl*)GetDlgItem(pNMHDR->idFrom))->EnableWindow(m_bBlue);

	*pResult = 0;
}

void PaletteDlg::OnBnClickedEnablered()
{
	UpdateData(true);

	MakePal();

	Invalidate(false);
}

void PaletteDlg::OnBnClickedEnablegreen()
{
	UpdateData(true);

	MakePal();

	Invalidate(false);
}

void PaletteDlg::OnBnClickedEnableblue()
{
	UpdateData(true);

	MakePal();

	Invalidate(false);
}

void PaletteDlg::OnEnChangeAmpRed()
{
	UpdateData(true);
	MakePal();
	Invalidate(false);
}

void PaletteDlg::OnEnChangeFreqRed()
{
	UpdateData(true);
	MakePal();
	Invalidate(false);
}

void PaletteDlg::OnEnChangeBaseRed()
{
	UpdateData(true);
	MakePal();
	Invalidate(false);
}

void PaletteDlg::OnEnChangeAmpGreen()
{
	UpdateData(true);
	MakePal();
	Invalidate(false);
}

void PaletteDlg::OnEnChangeFreqGreen()
{
	UpdateData(true);
	MakePal();
	Invalidate(false);
}

void PaletteDlg::OnEnChangeBaseGreen()
{
	UpdateData(true);
	MakePal();
	Invalidate(false);
}

void PaletteDlg::OnEnChangeAmpBlue()
{
	UpdateData(true);
	MakePal();
	Invalidate(false);
}

void PaletteDlg::OnEnChangeFreqBlue()
{
	UpdateData(true);
	MakePal();
	Invalidate(false);
}

void PaletteDlg::OnEnChangeBaseBlue()
{
	UpdateData(true);
	MakePal();
	Invalidate(false);
}

void PaletteDlg::OnNMCustomdrawPhaseslider(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	
	if (pNMCD->uItemState == CDIS_FOCUS)
	{
		UpdateData(true);

		MakePal();

		Invalidate(false);
	}

	*pResult = 0;
}

void PaletteDlg::OnNMReleasedcapturePhaseslider(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(true);

	MakePal();

	Invalidate(false);

	SetFocus();

	*pResult = 0;
}
