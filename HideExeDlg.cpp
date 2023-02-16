
// HideExeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HideExe.h"
#include "HideExeDlg.h"
#include "afxdialogex.h"

#include <tlhelp32.h>
#include "EnumModule.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 模拟调用WaitForSingleObjectEx以备触发APC注入
UINT WINAPI ThreadFunc(void* arg)
{
	while(1)
	{
		HANDLE hEvent = NULL;
		DWORD dwRet = 0;

		hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
		Sleep(1000);

		dwRet = WaitForSingleObjectEx(hEvent,INFINITE,TRUE);
		if (dwRet == WAIT_OBJECT_0)
		{
			printf("hEvent被触发！\n");
		}

		if (dwRet == WAIT_IO_COMPLETION)
		{
			printf("APC项被添加到APC队列中,被触发！\n");
		}
		CloseHandle(hEvent);
		Sleep(10);
	}
	return 0;
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CHideExeDlg 对话框




CHideExeDlg::CHideExeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CHideExeDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHideExeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_cList);
	DDX_Control(pDX, IDC_BTN_UPDATE, m_cBtnGo);
}

BEGIN_MESSAGE_MAP(CHideExeDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_UPDATE, &CHideExeDlg::OnBnClickedBtnUpdate)
	ON_BN_CLICKED(IDC_BTN_ISDEBUG, &CHideExeDlg::OnBnClickedBtnIsdebug)
END_MESSAGE_MAP()


// CHideExeDlg 消息处理程序

BOOL CHideExeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	HANDLE	hThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ThreadFunc, NULL, NULL, NULL);
	CloseHandle(hThread);
	Sleep(500);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CHideExeDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CHideExeDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CHideExeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void StartEnumModule(CListBox *pListBox)
{
	// TODO: Add your control notification handler code here
	UINT			i = 0;
	CString			strText;
	MODULEENTRY32	moudle = {sizeof(MODULEENTRY32)};
	HANDLE			hProcess = INVALID_HANDLE_VALUE;
	BOOL			bRet = FALSE;
	DWORD			nPid = GetCurrentProcessId();

	// 清空所有ListBox内容
	pListBox->ResetContent();

	// 获取当前进程PID
	nPid = GetCurrentProcessId();

	// 开始遍历模块
	hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, nPid);
	if (hProcess == INVALID_HANDLE_VALUE)
	{
		strText.Format(_T("遍历模块失败，错误代码：%d"), GetLastError());
		AfxMessageBox(strText);
		return;
	}

	bRet = Module32First(hProcess, &moudle);
	while(bRet)
	{
		strText.Format(_T("[%03u] 地址: 0x%8X 文件名: %s"), i++, moudle.modBaseAddr, moudle.szModule);
		pListBox->AddString(strText);
		bRet = Module32Next(hProcess, &moudle);
	}

	CloseHandle(hProcess);
}

// 在多线程中暴力扫描内存
DWORD WINAPI ThreadScan(LPVOID lParam)
{
	BOOL			bRet = FALSE;
	CHideExeDlg		*pDlg = (CHideExeDlg *)lParam;
	if(NULL == pDlg || NULL == pDlg->GetSafeHwnd())
		return 0;

	pDlg->m_cBtnGo.EnableWindow(FALSE);
	InitlizeNativeAPI();
	bRet = StartEnumModuleDeep(&pDlg->m_cList);
	pDlg->m_cBtnGo.EnableWindow(TRUE);
	return bRet;
}

void CHideExeDlg::OnBnClickedBtnUpdate()
{
	// TODO: 在此添加控件通知处理程序代码
	//StartEnumModule(&m_cList);

	// 暴力扫描，建议放在独立线程运行
	DWORD	nTid = 0;
	HANDLE	hThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ThreadScan, this, NULL, &nTid);
	CloseHandle(hThread);
}

// 检测调试器的方法，也可以直接使用Windows API: IsDebuggerPresent函数来检测
BOOL MyIsDebuggerPresent()
{
	BOOL	bDebug = FALSE;
	__asm {
		mov eax, dword ptr fs:[0x30]
		movzx eax, byte ptr[eax+2]
		mov bDebug, eax
	}
	return bDebug;
}

void CHideExeDlg::OnBnClickedBtnIsdebug()
{
	// TODO: 在此添加控件通知处理程序代码
	if(MyIsDebuggerPresent())
	{
		AfxMessageBox(_T("当前正在被调试！"));
	}
	else
	{
		AfxMessageBox(_T("未检测到调试器！"));
	}
}
