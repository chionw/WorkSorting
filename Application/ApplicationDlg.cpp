
// ApplicationDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Application.h"
#include "ApplicationDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CApplicationDlg �Ի���



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


// CApplicationDlg ��Ϣ�������

BOOL CApplicationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	DWORD dwStyle;

	//��ʼ��Ա���б�ؼ�
	m_list_employees.InsertColumn(_T("����"), 50);
	m_list_employees.InsertColumn(_T("����"), 80);
	dwStyle = m_list_employees.GetExtendedStyle();
	dwStyle |= (DWORD(TVS_SHOWSELALWAYS) | DWORD(LVS_EX_FULLROWSELECT) | DWORD(LVS_EX_GRIDLINES));
	m_list_employees.SetExtendedStyle(dwStyle);
	
	//��ʼ�������б�ؼ�
	m_list_work_time_type.InsertColumn(_T("����"), 50);
	m_list_work_time_type.InsertColumn(_T("ʱ��"), 300);
	dwStyle = m_list_work_time_type.GetExtendedStyle();
	dwStyle |= (DWORD(TVS_SHOWSELALWAYS) | DWORD(LVS_EX_FULLROWSELECT) | DWORD(LVS_EX_GRIDLINES));
	m_list_work_time_type.SetExtendedStyle(dwStyle);

	//��ʼ�����������б�ؼ�
	m_list_work_time_list.InsertColumn(_T("����"), 60);
	m_list_work_time_list.InsertColumn(_T("����"), 80);
	m_list_work_time_list.InsertColumn(_T("����"), 100);
	m_list_work_time_list.InsertColumn(_T("����"), 100);
	m_list_work_time_list.InsertColumn(_T("�ϰ�ʱ��"), 300);
	dwStyle = m_list_work_time_list.GetExtendedStyle();
	dwStyle |= (DWORD(TVS_SHOWSELALWAYS) | DWORD(LVS_EX_FULLROWSELECT) | DWORD(LVS_EX_GRIDLINES));
	m_list_work_time_list.SetExtendedStyle(dwStyle);

	//��������ؼ��Ĳ���
	PostMessage(WM_SIZE);

	//�������е�Ա������
	CEmployeeManage2::TYPE_ALL_EMPLOYEES allEmployess = m_Employees.QueryAll();
	BOOST_FOREACH(const CEmployeeManage2::TYPE_ALL_EMPLOYEES::value_type &spEmployee, allEmployess)
	{
		//���ص�Ա���б�
		vector<string> info;
		info.push_back(spEmployee->ID());
		info.push_back(spEmployee->Name());
		m_list_employees.InsertInfo(info);

		//���ص�����ʱ���б�
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

	//���ش����
	CWorkTimeConfig::TYPE_SP_ALL_ITEM spAllItem = m_workTimeConfig.GetAllItems();

	BOOST_FOREACH(CWorkTimeConfig::TYPE_ALL_ITEM::value_type code, *spAllItem)
	{
		vector<string> info;

		info.push_back(code.first);
		info.push_back(code.second.str());
		m_list_work_time_type.InsertInfo(info);
	}
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CApplicationDlg::OnPaint()
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
HCURSOR CApplicationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CApplicationDlg::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//CDialogEx::OnOK();
	OnBnClickedBtnAdd();

	
}


void CApplicationDlg::OnBnClickedCancel()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
	//static int iWidth(200);				//�����б�Ŀ��
	//static int iSpanToLeft(13);			//��߿��Ԥ����϶
	//static int iSpanToTop(13);			//�ϱ߿��Ԥ����϶
	//static int iSpanToRight(13);		//�ұ߿��Ԥ����϶
	//static int iSpanToButton(20);		//�±߿��Ԥ����϶
	//static int iWSpanBetweenControl(10);//�ؼ�֮��ĺ�����
	//static int iHSpanBetweenControl(10);//�ؼ�֮���������

	////����Ա���б�λ�úʹ�С
	//if (m_list_employees.m_hWnd)
	//{
	//	m_list_employees.MoveWindow(
	//		rect.left + iSpanToLeft,
	//		rect.top + 20,
	//		iWidth,
	//		rect.Height() - 20 - iSpanToButton);
	//}

	////���ù������ͱ���λ�úʹ�С
	//if (m_list_work_time_type.m_hWnd)
	//{
	//	//�̶���X,Y���꣬�̶��Ŀ�ȣ��߶�Ϊ��ס�¶�Ϊ׼��
	//	static int iY(83);
	//	
	//	m_list_work_time_type.MoveWindow(
	//		rect.left + iSpanToLeft + iWidth + iWSpanBetweenControl,
	//		rect.top + iY,
	//		iWidth,
	//		rect.Height() - iY - iSpanToButton);
	//}

	////������Ӱ�ť��λ��
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

	////�̶���XY��ʼ���꣬��ⴰ�ڵĸ߶ȣ���ס�ұ߿�
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
	//��ȡ�û�ѡ���Ա��
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
			MessageBox(_T("��ѡ��Ҫ��ӵ�Ա������"), _T("��ʾ"), MB_OK);
			return;
		}
	}
	

	//��ȡ�û�ѡ�������
	shared_ptr<TIME_DATE> spDate;
	{
		CTime _selectDate;
		m_timepick_work_date.GetTime(_selectDate);
		spDate = shared_ptr<TIME_DATE>(new TIME_DATE(_selectDate.GetYear(), _selectDate.GetMonth(), _selectDate.GetDay()));
	}
	
	
	////��ѯ����,ת����ʱ������
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
	//		MessageBox(_T("��ѡ�����"), _T("��ʾ"), MB_OK);
	//		return;
	//	}
	//	busyHours = m_workTimeConfig.GetItem(sCode);
	//}

	//��ȡ�û�ѡ��Ĵ���
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
			MessageBox(_T("��ѡ�����"), _T("��ʾ"), MB_OK);
			return;
		}
	}
	

	//���û����
	auto spEmployee = m_Employees.Query(sID);
	if (spEmployee)
	{
		if (spEmployee->Add(*spDate, sCode))
		{
			//���½������ʾ
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

	//���ڼ�һ
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
	//���ʱ��ķ�Χ�Ƿ���ȷ
	CTime fromTime;
	CTime toTime;
	m_timepick_export_from.GetTime(fromTime);
	m_timepick_export_to.GetTime(toTime);
	if (fromTime > toTime)
	{
		MessageBox(_T("�����Ŀ�ʼʱ�䲻�����ڽ���ʱ�䡣������ѡ��"), _T("��ʾ"), MB_OK);
		return;
	}

	//ѡ��Ҫ�����λ��
	string strSavePath;
	{
		CString csSavePath;
		TCHAR cBuffer[MAX_PATH];
		BROWSEINFO bi;
		bi.hwndOwner = NULL;
		bi.iImage = IDR_MAINFRAME;
		bi.lParam;
		bi.lpfn = NULL;
		bi.lpszTitle = _T("�ļ�����·��");
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

	MessageBoxA(this->GetSafeHwnd(), "����ɹ�", "��ʾ", MB_OK);
}
