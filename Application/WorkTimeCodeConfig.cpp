// WorkTimeCodeConfig.cpp : 实现文件
//

#include "stdafx.h"
#include "Application.h"
#include "WorkTimeCodeConfig.h"
#include "afxdialogex.h"


// CWorkTimeCodeConfig 对话框

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


// CWorkTimeCodeConfig 消息处理程序


void CWorkTimeCodeConfig::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}


void CWorkTimeCodeConfig::OnBnClickedCancel()
{
	// TODO:  在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}
