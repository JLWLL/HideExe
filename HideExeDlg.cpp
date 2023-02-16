
// HideExeDlg.cpp : ʵ���ļ�
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

// ģ�����WaitForSingleObjectEx�Ա�����APCע��
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
			printf("hEvent��������\n");
		}

		if (dwRet == WAIT_IO_COMPLETION)
		{
			printf("APC���ӵ�APC������,��������\n");
		}
		CloseHandle(hEvent);
		Sleep(10);
	}
	return 0;
}

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CHideExeDlg �Ի���




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


// CHideExeDlg ��Ϣ�������

BOOL CHideExeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	HANDLE	hThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ThreadFunc, NULL, NULL, NULL);
	CloseHandle(hThread);
	Sleep(500);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CHideExeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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

	// �������ListBox����
	pListBox->ResetContent();

	// ��ȡ��ǰ����PID
	nPid = GetCurrentProcessId();

	// ��ʼ����ģ��
	hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, nPid);
	if (hProcess == INVALID_HANDLE_VALUE)
	{
		strText.Format(_T("����ģ��ʧ�ܣ�������룺%d"), GetLastError());
		AfxMessageBox(strText);
		return;
	}

	bRet = Module32First(hProcess, &moudle);
	while(bRet)
	{
		strText.Format(_T("[%03u] ��ַ: 0x%8X �ļ���: %s"), i++, moudle.modBaseAddr, moudle.szModule);
		pListBox->AddString(strText);
		bRet = Module32Next(hProcess, &moudle);
	}

	CloseHandle(hProcess);
}

// �ڶ��߳��б���ɨ���ڴ�
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//StartEnumModule(&m_cList);

	// ����ɨ�裬������ڶ����߳�����
	DWORD	nTid = 0;
	HANDLE	hThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ThreadScan, this, NULL, &nTid);
	CloseHandle(hThread);
}

// ���������ķ�����Ҳ����ֱ��ʹ��Windows API: IsDebuggerPresent���������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(MyIsDebuggerPresent())
	{
		AfxMessageBox(_T("��ǰ���ڱ����ԣ�"));
	}
	else
	{
		AfxMessageBox(_T("δ��⵽��������"));
	}
}
