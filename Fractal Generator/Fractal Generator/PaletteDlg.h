#pragma once

#include "DynamicArray.h"
#include "Palette.h"
// PaletteDlg dialog

class PaletteDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(PaletteDlg)

public:
	PaletteDlg();   // standard constructor
	virtual ~PaletteDlg();

// Dialog Data
	enum { IDD = IDD_PALETTEDLG };

	void MakePal();

	CDynamicArray<UInt8>	m_Reds;
	CDynamicArray<UInt8>	m_Greens;
	CDynamicArray<UInt8>	m_Blues;

	CDynamicArray<Color>	m_Palette;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();

	afx_msg void OnNMReleasedcapturePhaseslider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureRedslider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureGreenslider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureBlueslider(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnNMCustomdrawPhaseslider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawRedslider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawGreenslider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawBlueslider(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnBnClickedOk();
	
	afx_msg void OnBnClickedEnablered();
	afx_msg void OnBnClickedEnablegreen();
	afx_msg void OnBnClickedEnableblue();
	
	afx_msg void OnEnChangeAmpRed();
	afx_msg void OnEnChangeFreqRed();
	afx_msg void OnEnChangeBaseRed();
	afx_msg void OnEnChangeAmpGreen();
	afx_msg void OnEnChangeFreqGreen();
	afx_msg void OnEnChangeBaseGreen();
	afx_msg void OnEnChangeAmpBlue();
	afx_msg void OnEnChangeFreqBlue();
	afx_msg void OnEnChangeBaseBlue();

	virtual BOOL OnInitDialog();

	int m_ColorPhase;

	int m_RedOffset;
	int m_GreenOffset;
	int m_BlueOffset;
	
	BOOL m_bRed;
	BOOL m_bGreen;
	BOOL m_bBlue;

	BOOL m_bSmoothShading;

	float m_fRedAmp;
	float m_fRedFreq;
	float m_fRedBase;

	float m_fGreenAmp;
	float m_fGreenFreq;
	float m_fGreenBase;
	
	float m_fBlueAmp;
	float m_fBlueFreq;
	float m_fBlueBase;
};
