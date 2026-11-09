// HelpDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Fractal Generator.h"
#include "HelpDialog.h"


// HelpDialog dialog

IMPLEMENT_DYNAMIC(HelpDialog, CDialog)
HelpDialog::HelpDialog(CWnd* pParent /*=NULL*/)
	: CDialog(HelpDialog::IDD, pParent)
{
}

HelpDialog::~HelpDialog()
{
}

void HelpDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(HelpDialog, CDialog)
END_MESSAGE_MAP()


// HelpDialog message handlers
