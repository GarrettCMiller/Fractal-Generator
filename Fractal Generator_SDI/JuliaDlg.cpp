// JuliaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Fractal Generator.h"
#include "JuliaDlg.h"
#include ".\juliadlg.h"

#include "Fractal.h"

// JuliaDlg dialog

IMPLEMENT_DYNAMIC(JuliaDlg, CDialog)
JuliaDlg::JuliaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(JuliaDlg::IDD, pParent)
{
}

JuliaDlg::~JuliaDlg()
{
}

void JuliaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(JuliaDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


// JuliaDlg message handlers

void JuliaDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect r;
	
	GetWindowRect(&r);

	int cx = r.Width(),
		cy = r.Height();
	
	julia.Render(dc, 0, 0, cx, cy);

	char buf[80];

// 	sprintf(buf, "Julia Set - C = %f%s%fi", 
// 		julia.m_fJx, (julia.m_fJy >= 0.0 ? "+" : ""), julia.m_fJy );

	SetWindowText(buf);

	//GetParent()->GetParent()->SetFocus();
}

BOOL JuliaDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetSize();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void JuliaDlg::SetSize()
{
	//SetWindowPos(&wndTop, 0, 0, julia.m_ncX, julia.m_ncY, SWP_NOMOVE );
}

void JuliaDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect fr;

	//fr.SetRect(0, 0, julia.m_ncX, julia.m_ncY);

	if (!fr.PtInRect(point))
	{
		CWnd::OnLButtonDown(nFlags, point);
		return;
	}

	if ( nFlags & MK_CONTROL)
		julia.SetCenterPoint( point.x, point.y );
	else
		julia.SetCenterPoint( point.x, point.y, eZoom_In, ( nFlags & MK_SHIFT ? eZoomSpec_Double : eZoomSpec_None ) );

	julia.Init(this);
	julia.Update();

	Invalidate(false);

	CDialog::OnLButtonDown(nFlags, point);
}

void JuliaDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	CRect fr;

	//fr.SetRect(0, 0, julia.m_ncX, julia.m_ncY);

	if (!fr.PtInRect(point))
	{
		CWnd::OnRButtonDown(nFlags, point);
		return;
	}

	julia.SetCenterPoint( point.x, point.y, eZoom_Out, ( nFlags & MK_SHIFT ? eZoomSpec_Double : eZoomSpec_None ) );

	julia.Init(this);
	julia.Update();

	Invalidate(false);

	CDialog::OnRButtonDown(nFlags, point);
}