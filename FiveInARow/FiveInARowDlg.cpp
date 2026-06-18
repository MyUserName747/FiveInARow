
// FiveInARowDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "FiveInARow.h"
#include "FiveInARowDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CFiveInARowDlg 对话框



CFiveInARowDlg::CFiveInARowDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FIVEINAROW_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFiveInARowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFiveInARowDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CFiveInARowDlg 消息处理程序

BOOL CFiveInARowDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	SetWindowPos(NULL, 0, 0, 1024, 768, SWP_NOZORDER | SWP_NOMOVE);	//设置显示窗口大小
	m_FontTimer.CreatePointFont(250, L"Segoe UI Semibold", NULL);	//创建计时的显示字体
	m_FontOver.CreatePointFont(1666, L"微软雅黑", NULL);		//创建游戏结束的显示字体
	m_bState = false;						//游戏状态未开始
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CFiveInARowDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFiveInARowDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this); // 创建CPaintDC 对象
		CBitmap  bmp;
		BITMAP bm;
		bmp.LoadBitmap(IDB_BITMAP_BK);                  //载入背景图片资源
		bmp.GetObject(sizeof(BITMAP), &bm);             //取得图片信息
		CDC MemDC;
		MemDC.CreateCompatibleDC(&dc);                 //创建内存DC
		CBitmap* pOldBitmap = MemDC.SelectObject(&bmp); //载入图片资源
		dc.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &MemDC, 0, 0, SRCCOPY);//显示背景
		MemDC.SelectObject(pOldBitmap);                 //恢复位图
		m_Manager.Show(&dc);                           //显示所有棋子
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CFiveInARowDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CFiveInARowDlg::OnLButtonUp(UINT nFlags, CPoint point) {    //按下后弹起鼠标左键
	if (NewGame(point.x, point.y))                              //判断是否开始新游戏
		return;
	if (About(point.x, point.y))                                //判断是否显示游戏说明
		return;
	if (!m_bState) {                                            //判断游戏是否开始
		AfxMessageBox(L"请选择“开始”按钮开始新的游戏，按“ESC”退出游戏！");
		return;
	}
	int r = m_Manager.Add(point.x, point.y);                    //添加落子
	if (r == 0) {                                                //落子成功
		CClientDC dc(this);
		m_Manager.Show(&dc);                                    //显示所有落子
		if (m_Manager.GameOver()) {                               //判断游戏是否结束
			KillTimer(1);                                       //关闭游戏计时
			CString csTemp;
			if (m_Manager.GetWinner() == WHITE)                  //取得获胜方
				csTemp.Format(L"白方胜！");
			else
				csTemp.Format(L"黑方胜！");
			m_bState = false;
			CClientDC dc(this);
			CFont* pOldFont = dc.SelectObject(&m_FontOver);//设置输出字体
			int OldBkMode = dc.GetBkMode();                     //取得文字背景模式
			COLORREF OldColor, NewColor1 = RGB(60, 60, 60), NewColor2 = RGB(250, 50, 50);
			dc.SetBkMode(TRANSPARENT);                          //设置文字输出背景透明
			OldColor = dc.SetTextColor(NewColor1);              //设置文字阴影颜色
			dc.TextOut(158, 208, csTemp);                       //输出文字阴影
			dc.SetTextColor(NewColor2);                         //设置文字颜色
			dc.TextOut(150, 200, csTemp);                       //输出文字
			dc.SetTextColor(OldColor);                          //恢复文字颜色
			dc.SetBkMode(OldBkMode);                            //恢复文字背景模式
			dc.SelectObject(pOldFont);                          //恢复文字字体
		}
	}
	if (r == 1)                                                 //返还1，说明不在交叉点附近
		AfxMessageBox(L"请在棋盘交叉点落子！");
	else if (r == 2)                                            //返还2，说明该处已有落子
		AfxMessageBox(L"不可以重复落子！");
	CDialogEx::OnLButtonUp(nFlags, point);
}
bool CFiveInARowDlg::NewGame(int x, int y) {                    //判断开始新游戏
	int x0 = 35, y0 = 150, x1 = 200, y1 = 185;                       //设置接受区域
	if ((x >= x0 && x <= x1) && (y >= y0 && y <= y1)) {      //点击在接受区域内
		m_Manager.NewGame();                                 //开始新游戏
		Invalidate();                                        //让系统发送WM_PAINT消息
		m_iTime = 0;                                         //计时重新从0开始
		SetTimer(1, 1000, NULL);                             //创建定时器
		m_bState = true;                                     //设置游戏状态开始
		return true;
	}
	return false;
}

bool CFiveInARowDlg::About(int x, int y) {                     //判断是否显示游戏说明
	int x0 = 35, y0 = 70, x1 = 200, y1 = 95;                 //设置接受区域
	if ((x >= x0 && x <= x1) && (y >= y0 && y <= y1)) {      //点击在接受区域内
		CAboutDlg dlg;                                       //创建游戏说明对话框
		dlg.DoModal();                                       //显示游戏说明对话框
		return true;
	}
	return false;
}
void CFiveInARowDlg::OnTimer(UINT_PTR nIDEvent) {                //游戏定时器消息
	switch (nIDEvent) {
	case 1: {
		CClientDC dc(this);
		CFont* pOldFont;
		pOldFont = dc.SelectObject(&m_FontTimer);             //选择计时显示字体
		m_iTime++;                                            //计时
		CString csTemp;
		csTemp.Format(L"%04d ", m_iTime);
		COLORREF OldColor, NewColor = RGB(150, 50, 50);       //计时输出颜色
		OldColor = dc.SetTextColor(NewColor);                 //设置计时输出颜色
		dc.TextOut(725, 20, csTemp);                          //输出计时信息
		dc.SetTextColor(OldColor);                            //恢复文字颜色
		dc.SelectObject(pOldFont);                            //恢复文字输出字体
		break;
	}
	}
	CDialogEx::OnTimer(nIDEvent);
}
