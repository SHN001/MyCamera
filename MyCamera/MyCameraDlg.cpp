
// MyCameraDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "MyCamera.h"
#include "MyCameraDlg.h"
#include "afxdialogex.h"
#include "CSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
BOOL m_bThreadFlag = FALSE;
UINT ThreadImageCaptureFunc(LPVOID param);

void FillBitmapInfo(BITMAPINFO* bmi, int width, int height, int bpp, int origin)
{
	assert(bmi && width >= 0 && height >= 0 && (bpp == 8 || bpp == 24 || bpp == 32));
	BITMAPINFOHEADER *bmih = &(bmi->bmiHeader);
	memset(bmih, 0, sizeof(*bmih));
	bmih->biSize = sizeof(BITMAPINFOHEADER);
	bmih->biWidth = width;
	bmih->biHeight = origin ? abs(height) : -abs(height);
	bmih->biPlanes = 1;
	bmih->biBitCount = (unsigned short)bpp;
	bmih->biCompression = BI_RGB;
	if (bpp == 8)
	{
		RGBQUAD *palette = bmi->bmiColors;
		for (int i = 0; i < 256; i++)
		{
			palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
			palette[i].rgbReserved = 0;
		}
	}
}

void DisplayImage(CDC* pDC, CRect rect, cv::Mat& srcimg)
{
	cv::Mat img;
	int step = ((int)(rect.Width() / 4)) * 4;
	cv::resize(srcimg, img, cv::Size(step, rect.Height()));

	uchar buffer[sizeof(BITMAPINFOHEADER) * 1024];
	BITMAPINFO* bmi = (BITMAPINFO*)buffer;

	int bmp_w = img.cols;
	int bmp_h = img.rows;
	int depth = img.depth();
	int channels = img.channels();
	int bpp = 8 * channels;

	FillBitmapInfo(bmi, bmp_w, bmp_h, bpp, 0);

	int from_x = MIN(0, bmp_w - 1);
	int from_y = MIN(0, bmp_h - 1);
	int sw = MAX(MIN(bmp_w - from_x, rect.Width()), 0);
	int sh = MAX(MIN(bmp_h - from_y, rect.Height()), 0);

	SetDIBitsToDevice(pDC->m_hDC, rect.left, rect.top, sw, sh, from_x, from_y, 0, sh, img.data + from_y * img.step, bmi, 0);

	img.release();
}


class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMyCameraDlg 대화 상자



CMyCameraDlg::CMyCameraDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MYCAMERA_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyCameraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VIEW1, m_view);
	DDX_Control(pDX, IDC_VIEW2, m_corner_view);
}

BEGIN_MESSAGE_MAP(CMyCameraDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CAM_START, &CMyCameraDlg::OnBnClickedCamStart)
	ON_BN_CLICKED(IDC_CAM_STOP, &CMyCameraDlg::OnBnClickedCamStop)
	ON_BN_CLICKED(IDC_SAVE_BMP, &CMyCameraDlg::OnBnClickedSaveBmp)
	ON_COMMAND(ID_MENU_SETTINGS, &CMyCameraDlg::OnMenuSettings)
END_MESSAGE_MAP()


// CMyCameraDlg 메시지 처리기

BOOL CMyCameraDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_capture.open(0);
	if (m_capture.isOpened()) AfxMessageBox(_T("There is no camera captured!"));

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMyCameraDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMyCameraDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();

		if (m_image.data != NULL)
		{
			CRect rect;
			CDC* pDC;
			pDC = m_view.GetDC();
			m_view.GetClientRect(rect);
			DisplayImage(pDC, rect, m_image);
			ReleaseDC(pDC);


			cv::Mat corner_img;
			cv::cvtColor(m_image, corner_img, cv::COLOR_BGR2GRAY);
			pDC = m_corner_view.GetDC();
			m_corner_view.GetClientRect(rect);

			
			cv::Size patternsize(5, 4);
			std::vector<cv::Point2f> corners;
			
			bool patternfound = FALSE;
			
			if (m_chessboard_choice)
				patternfound = cv::findChessboardCornersSB(corner_img, patternsize, corners);
			else
				patternfound = cv::findChessboardCorners(corner_img, patternsize, corners, cv::CALIB_CB_ADAPTIVE_THRESH+cv::CALIB_CB_NORMALIZE_IMAGE);

			if (patternfound)
			{
				//cv::cornerSubPix(corner_img, corners, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.1));
				cv::drawChessboardCorners(corner_img, patternsize, cv::Mat(corners), patternfound);
			}
		
			DisplayImage(pDC, rect, corner_img);
			ReleaseDC(pDC);
		}
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMyCameraDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CMyCameraDlg::OnBnClickedCamStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bThreadFlag = TRUE;
	CWinThread *pThread = ::AfxBeginThread(ThreadImageCaptureFunc, this);
}


void CMyCameraDlg::OnBnClickedCamStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bThreadFlag = FALSE;
}


UINT ThreadImageCaptureFunc(LPVOID param)
{
	CMyCameraDlg *pDlg = (CMyCameraDlg*)param;
	while (m_bThreadFlag)
	{
		pDlg->m_capture >> pDlg->m_image;
		pDlg->Invalidate(FALSE);
	}
	return 0;
}

void CMyCameraDlg::OnBnClickedSaveBmp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	wchar_t szFilter[] = _T("Image (*.BMP) | *.BMP;*.GIF;*.JPG | AllFiles(*.*)|*.*||");
	CFileDialog dlg(FALSE, _T("bmp"), _T("test"), OFN_HIDEREADONLY, szFilter);

	if (dlg.DoModal() == IDOK)
	{
		//cv::flip(m_image, m_image, 0);
		std::string filename(CT2CA(dlg.GetPathName()));

		cv::imwrite(filename, m_image);
	}
}

void CMyCameraDlg::OnMenuSettings()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CSettings dlg;
	dlg.DoModal();
}

