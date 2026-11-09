#pragma once
#include "afxwin.h"


// ImgPropsDlg dialog

class ImgPropsDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(ImgPropsDlg)

public:
	ImgPropsDlg();   // standard constructor
	virtual ~ImgPropsDlg();

// Dialog Data
	enum { IDD = IDD_IMAGE_PROPS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_MainWidth;
	CEdit m_MainHeight;
	CEdit m_SecondaryWidth;
	CEdit m_SecondaryHeight;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
