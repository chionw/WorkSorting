#pragma once


// CWorkTimeCodeConfig �Ի���

class CWorkTimeCodeConfig : public CDialogEx
{
	DECLARE_DYNAMIC(CWorkTimeCodeConfig)

public:
	CWorkTimeCodeConfig(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWorkTimeCodeConfig();

// �Ի�������
	enum { IDD = IDD_DIALOG_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
