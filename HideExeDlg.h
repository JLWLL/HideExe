
// HideExeDlg.h : ͷ�ļ�
//

#pragma once


// CHideExeDlg �Ի���
class CHideExeDlg : public CDialogEx
{
// ����
public:
	CHideExeDlg(CWnd* pParent = NULL);	// ��׼���캯��

public:
// �Ի�������
	enum { IDD = IDD_HIDEEXE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnUpdate();
	CListBox m_cList;
	afx_msg void OnBnClickedBtnIsdebug();
	CButton m_cBtnGo;
};
