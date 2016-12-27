
// ApplicationDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Application.h"
#include "ApplicationDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CApplicationDlg 对话框



CApplicationDlg::CApplicationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CApplicationDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CApplicationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CODE, m_list_work_time_type);
	DDX_Control(pDX, IDC_LIST_WORKTIME, m_list_work_time_list);
	DDX_Control(pDX, IDC_BTN_ADD, m_btn_add);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_timepick_work_date);
	DDX_Control(pDX, IDC_CHECK_AUTO_DATE, m_check_auto_date);
	DDX_Control(pDX, IDC_LIST_EMPLOYEES, m_list_employees);
	DDX_Control(pDX, IDC_DATETIMEPICKER_EXPORT_FROM, m_timepick_export_from);
	DDX_Control(pDX, IDC_DATETIMEPICKER_EXPORT_TO, m_timepick_export_to);
}

BEGIN_MESSAGE_MAP(CApplicationDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CApplicationDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CApplicationDlg::OnBnClickedCancel)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_ADD, &CApplicationDlg::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_EXPORT, &CApplicationDlg::OnBnClickedBtnExport)
END_MESSAGE_MAP()


// CApplicationDlg 消息处理程序

BOOL CApplicationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	DWORD dwStyle;

	//初始化员工列表控件
	m_list_employees.InsertColumn(_T("工号"), 50);
	m_list_employees.InsertColumn(_T("姓名"), 80);
	dwStyle = m_list_employees.GetExtendedStyle();
	dwStyle |= (DWORD(TVS_SHOWSELALWAYS) | DWORD(LVS_EX_FULLROWSELECT) | DWORD(LVS_EX_GRIDLINES));
	m_list_employees.SetExtendedStyle(dwStyle);
	
	//初始化工作列表控件
	m_list_work_time_type.InsertColumn(_T("代码"), 50);
	m_list_work_time_type.InsertColumn(_T("时段"), 300);
	dwStyle = m_list_work_time_type.GetExtendedStyle();
	dwStyle |= (DWORD(TVS_SHOWSELALWAYS) | DWORD(LVS_EX_FULLROWSELECT) | DWORD(LVS_EX_GRIDLINES));
	m_list_work_time_type.SetExtendedStyle(dwStyle);

	//初始化工作数据列表控件
	m_list_work_time_list.InsertColumn(_T("工号"), 60);
	m_list_work_time_list.InsertColumn(_T("姓名"), 80);
	m_list_work_time_list.InsertColumn(_T("日期"), 100);
	m_list_work_time_list.InsertColumn(_T("代码"), 100);
	m_list_work_time_list.InsertColumn(_T("上班时间"), 300);
	dwStyle = m_list_work_time_list.GetExtendedStyle();
	dwStyle |= (DWORD(TVS_SHOWSELALWAYS) | DWORD(LVS_EX_FULLROWSELECT) | DWORD(LVS_EX_GRIDLINES));
	m_list_work_time_list.SetExtendedStyle(dwStyle);

	//调整界面控件的布局
	PostMessage(WM_SIZE);

	//加载所有的员工数据
	CEmployeeManage2::TYPE_ALL_EMPLOYEES allEmployess = m_Employees.QueryAll();
	BOOST_FOREACH(const CEmployeeManage2::TYPE_ALL_EMPLOYEES::value_type &spEmployee, allEmployess)
	{
		//加载到员工列表
		vector<string> info;
		info.push_back(spEmployee->ID());
		info.push_back(spEmployee->Name());
		m_list_employees.InsertInfo(info);

		//加载到工作时间列表
		CEmployee2::TYPE_SP_ALL_BUSY_DAY spAllBusyDay = spEmployee->GetAllBusyDays();
		BOOST_FOREACH(CEmployee2::TYPE_ALL_BUSY_DAY::value_type &everyDay, *spAllBusyDay)
		{
			vector<string> info;
			info.push_back(spEmployee->ID());
			info.push_back(spEmployee->Name());
			info.push_back(
				(boost::format("%d-%02d-%02d")
				% everyDay.first.year()
				% (int)everyDay.first.month()
				% everyDay.first.day()).str()
				);
			//info.push_back(everyDay.second.str());
			info.push_back(everyDay.second);
			info.push_back(m_workTimeConfig.GetItem(everyDay.second).str());
			m_list_work_time_list.InsertInfo(info);
		}
	}

	//加载代码表
	CWorkTimeConfig::TYPE_SP_ALL_ITEM spAllItem = m_workTimeConfig.GetAllItems();

	BOOST_FOREACH(CWorkTimeConfig::TYPE_ALL_ITEM::value_type code, *spAllItem)
	{
		vector<string> info;

		info.push_back(code.first);
		info.push_back(code.second.str());
		m_list_work_time_type.InsertInfo(info);
	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CApplicationDlg::OnPaint()
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
HCURSOR CApplicationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CApplicationDlg::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	//CDialogEx::OnOK();
	OnBnClickedBtnAdd();

	
}


void CApplicationDlg::OnBnClickedCancel()
{
	// TODO:  在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


void CApplicationDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);



	//CRect rect;
	//GetClientRect(&rect);
	//
	//CRect btnRect;
	//if (GetDlgItem(IDC_BTN_ADD))
	//{
	//	GetDlgItem(IDC_BTN_ADD)->GetWindowRect(&btnRect);
	//}
	//
	//static int iWidth(200);				//类型列表的宽度
	//static int iSpanToLeft(13);			//左边框的预留空隙
	//static int iSpanToTop(13);			//上边框的预留空隙
	//static int iSpanToRight(13);		//右边框的预留空隙
	//static int iSpanToButton(20);		//下边框的预留空隙
	//static int iWSpanBetweenControl(10);//控件之间的横向间距
	//static int iHSpanBetweenControl(10);//控件之间的纵向间距

	////重置员工列表位置和大小
	//if (m_list_employees.m_hWnd)
	//{
	//	m_list_employees.MoveWindow(
	//		rect.left + iSpanToLeft,
	//		rect.top + 20,
	//		iWidth,
	//		rect.Height() - 20 - iSpanToButton);
	//}

	////重置工作类型表格的位置和大小
	//if (m_list_work_time_type.m_hWnd)
	//{
	//	//固定的X,Y坐标，固定的宽度，高度为抵住下端为准。
	//	static int iY(83);
	//	
	//	m_list_work_time_type.MoveWindow(
	//		rect.left + iSpanToLeft + iWidth + iWSpanBetweenControl,
	//		rect.top + iY,
	//		iWidth,
	//		rect.Height() - iY - iSpanToButton);
	//}

	////重置添加按钮的位置
	//if (m_btn_add.m_hWnd)
	//{
	//	m_btn_add.MoveWindow(
	//		iSpanToLeft + iWidth + iWSpanBetweenControl,
	//		83,
	//		btnRect.Width(),
	//		btnRect.Height()
	//		);
	//}
	//

	////固定的XY起始坐标，充斥窗口的高度，抵住右边框
	//if (m_list_work_time_list.m_hWnd)
	//{
	//	int iLeft = iSpanToLeft + iWidth + iWSpanBetweenControl + btnRect.Width() + iWSpanBetweenControl;
	//	m_list_work_time_list.MoveWindow(
	//		iLeft,
	//		rect.top + iSpanToTop,
	//		rect.right - (iLeft + iSpanToRight),
	//		rect.Height() - iSpanToTop - iSpanToButton);
	//}
}


void CApplicationDlg::OnBnClickedBtnAdd()
{
	//获取用户选择的员工
	string sID;
	{
		auto pos = m_list_employees.GetFirstSelectedItemPosition();
		if (pos != NULL)
		{
			int iItem = m_list_employees.GetNextSelectedItem(pos);
			sID.assign( GetTools::UnicodeToGBK(m_list_employees.GetItemText(iItem, 0).GetString()));
		}
		else
		{
			MessageBox(_T("请选择要添加的员工对象"), _T("提示"), MB_OK);
			return;
		}
	}
	

	//获取用户选择的日期
	shared_ptr<TIME_DATE> spDate;
	{
		CTime _selectDate;
		m_timepick_work_date.GetTime(_selectDate);
		spDate = shared_ptr<TIME_DATE>(new TIME_DATE(_selectDate.GetYear(), _selectDate.GetMonth(), _selectDate.GetDay()));
	}
	
	
	////查询代码,转换出时间区间
	//TYPE_BUSY_HOUR busyHours;
	//{
	//	string sCode;
	//	auto pos = m_list_work_time_type.GetFirstSelectedItemPosition();
	//	if (pos != NULL)
	//	{
	//		int iItem = m_list_work_time_type.GetNextSelectedItem(pos);
	//		sCode.assign(GetTools::UnicodeToGBK(m_list_work_time_type.GetItemText(iItem, 0).GetString()));
	//	}
	//	else
	//	{
	//		MessageBox(_T("请选择代码"), _T("提示"), MB_OK);
	//		return;
	//	}
	//	busyHours = m_workTimeConfig.GetItem(sCode);
	//}

	//获取用户选择的代码
	string sCode;
	{
		auto pos = m_list_work_time_type.GetFirstSelectedItemPosition();
		if (pos != NULL)
		{
			int iItem = m_list_work_time_type.GetNextSelectedItem(pos);
			sCode.assign(GetTools::UnicodeToGBK(m_list_work_time_type.GetItemText(iItem, 0).GetString()));
		}
		else
		{
			MessageBox(_T("请选择代码"), _T("提示"), MB_OK);
			return;
		}
	}
	

	//对用户添加
	auto spEmployee = m_Employees.Query(sID);
	if (spEmployee)
	{
		if (spEmployee->Add(*spDate, sCode))
		{
			//更新界面的显示
			vector<string> info;
			info.push_back(spEmployee->ID());
			info.push_back(spEmployee->Name());
			info.push_back((boost::format("%d-%02d-%02d")%spDate->year() %((int)spDate->month()) %spDate->day()).str());
			//info.push_back(busyHours.str());
			info.push_back(sCode);
			info.push_back(m_workTimeConfig.GetItem(sCode).str());
			m_list_work_time_list.InsertInfo(info);
		}
	}

	//日期加一
	if (m_check_auto_date.GetCheck())
	{
		*spDate += boost::gregorian::days(1);
		CTime t(spDate->year(), spDate->month(), spDate->day(), 0, 0, 0);
		m_timepick_work_date.SetTime(&t);
	}

	m_list_work_time_type.SetFocus();
}



void CApplicationDlg::OnBnClickedBtnExport()
{
	//检查时间的范围是否正确
	CTime fromTime;
	CTime toTime;
	m_timepick_export_from.GetTime(fromTime);
	m_timepick_export_to.GetTime(toTime);
	if (fromTime > toTime)
	{
		MessageBox(_T("导出的开始时间不能晚于结束时间。请重新选择。"), _T("提示"), MB_OK);
		return;
	}

	//选择要保存的位置
	string strSavePath;
	{
		CString csSavePath;
		TCHAR cBuffer[MAX_PATH];
		BROWSEINFO bi;
		bi.hwndOwner = NULL;
		bi.iImage = IDR_MAINFRAME;
		bi.lParam;
		bi.lpfn = NULL;
		bi.lpszTitle = _T("文件保存路径");
		bi.pidlRoot = NULL;
		bi.pszDisplayName = cBuffer;
		//bi.ulFlags = BIF_BROWSEINCLUDEFILES;
		bi.ulFlags = BIF_EDITBOX;
		LPITEMIDLIST plDList = SHBrowseForFolder(&bi);
		if (!plDList)
		{
			return;
		}

		SHGetPathFromIDList(plDList, cBuffer);
		csSavePath = cBuffer;
		LPMALLOC lpMalloc;
		if (!FAILED(SHGetMalloc(&lpMalloc)))
		{
			lpMalloc->Free(plDList);
			lpMalloc->Release();
		}
		if (csSavePath.IsEmpty())
		{
			return;
		}
		if (csSavePath.GetAt(csSavePath.GetLength() - 1) != '/'
			&&
			csSavePath.GetAt(csSavePath.GetLength() - 1) != '\\')
		{
			csSavePath += '/';
		}
		strSavePath = GetTools::UnicodeToGBK(csSavePath.GetString());
		strSavePath += (boost::format("%04d-%02d-%02d_%04d-%02d-%02d/") % fromTime.GetYear() % fromTime.GetMonth() % fromTime.GetDay() % toTime.GetYear() % toTime.GetMonth() % toTime.GetDay()).str();
	}
	TIME_DATE from(fromTime.GetYear(), fromTime.GetMonth(), fromTime.GetDay());
	TIME_DATE to(toTime.GetYear(), toTime.GetMonth(), toTime.GetDay());

	SaveFile(strSavePath, "workCode.html", Html_WorkCode(from, to));

	SaveFile(strSavePath, "General.html", Html_General(from, to));

	SaveFile(strSavePath, "General.txt", Text_General(from, to));

	SaveFile(strSavePath, "week.html", Html_Week(from, to));

	MessageBoxA(this->GetSafeHwnd(), "保存成功", "提示", MB_OK);
}
