#pragma once


// CSettings 대화 상자

class CSettings : public CDialogEx
{
	DECLARE_DYNAMIC(CSettings)

public:
	CSettings(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CSettings();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CButton m_chessboard;
	CButton m_chessboard_sb;
	afx_msg void OnBnClickedSettingOk();
};
