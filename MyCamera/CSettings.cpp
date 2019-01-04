// CSettings.cpp: 구현 파일
//

#include "stdafx.h"
#include "MyCamera.h"
#include "CSettings.h"
#include "afxdialogex.h"
#include "MyCameraDlg.h"


// CSettings 대화 상자

IMPLEMENT_DYNAMIC(CSettings, CDialogEx)

CSettings::CSettings(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CSettings::~CSettings()
{
}

void CSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHSBORD1, m_chessboard);
	DDX_Control(pDX, IDC_CHSBORD2, m_chessboard_sb);
}


BEGIN_MESSAGE_MAP(CSettings, CDialogEx)
	ON_BN_CLICKED(IDC_SETTING_OK, &CSettings::OnBnClickedSettingOk)
END_MESSAGE_MAP()


// CSettings 메시지 처리기




void CSettings::OnBnClickedSettingOk()
{
	
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_chessboard_sb.GetCheck())
		((CMyCameraDlg*)GetParent())->m_chessboard_choice = TRUE;
	else
		((CMyCameraDlg*)GetParent())->m_chessboard_choice = FALSE;
	
	CDialogEx::OnOK();
	
}
