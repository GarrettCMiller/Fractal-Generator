#pragma once


// HelpDialog dialog

class HelpDialog : public CDialog
{
	DECLARE_DYNAMIC(HelpDialog)

public:
	HelpDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~HelpDialog();

// Dialog Data
	enum { IDD = IDD_HELPDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
