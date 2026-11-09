#pragma once


// JuliaDlg dialog

class JuliaDlg : public CDialog
{
	DECLARE_DYNAMIC(JuliaDlg)

public:
	JuliaDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~JuliaDlg();

// Dialog Data
	enum { IDD = IDD_JULIA_WINDOW };

	void	SetSize();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};
