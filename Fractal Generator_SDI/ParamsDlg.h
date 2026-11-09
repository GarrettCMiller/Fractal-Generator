#pragma once
#include "afxwin.h"


// ParamsDlg dialog

class ParamsDlg : public CDialog
{
	DECLARE_DYNAMIC(ParamsDlg)

public:
	ParamsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ParamsDlg();

// Dialog Data
	enum { IDD = IDD_FRACTAL_PARAMS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	double m_fTLx;
	double m_fBRx;
	double m_fTLy;
	double m_fBRy;
	double m_fBailValue;
	int m_nMaxIterations;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	CEdit m_XMin;
	CEdit m_XMax;
	CEdit m_YMin;
	CEdit m_YMax;
	CEdit m_Bailout;
	CEdit m_MaxIterations;
};
