// WorkTimeCodeConfig.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Application.h"
#include "WorkTimeCodeConfig.h"
#include "afxdialogex.h"


// CWorkTimeCodeConfig �Ի���

IMPLEMENT_DYNAMIC(CWorkTimeCodeConfig, CDialogEx)

CWorkTimeCodeConfig::CWorkTimeCodeConfig(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWorkTimeCodeConfig::IDD, pParent)
{

}

CWorkTimeCodeConfig::~CWorkTimeCodeConfig()
{
}

void CWorkTimeCodeConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWorkTimeCodeConfig, CDialogEx)
	ON_BN_CLICKED(IDOK, &CWorkTimeCodeConfig::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CWorkTimeCodeConfig::OnBnClickedCancel)
END_MESSAGE_MAP()


// CWorkTimeCodeConfig ��Ϣ�������


void CWorkTimeCodeConfig::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnOK();
}


void CWorkTimeCodeConfig::OnBnClickedCancel()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnCancel();
}
