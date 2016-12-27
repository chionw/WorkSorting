#pragma once
#include "stdafx.h"
#include "CListControl.h"
#include "Tools.hpp"
using namespace IBox;
BEGIN_MESSAGE_MAP(CListDataDisplay, CListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, CListDataDisplay::OnLvnColumnclick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, CListDataDisplay::OnDbClickItem)
	ON_NOTIFY_REFLECT(NM_RCLICK, CListDataDisplay::OnRClickItem)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, CListDataDisplay::OnItemChanged)
END_MESSAGE_MAP()

CListDataDisplay::CListDataDisplay()
:CListCtrl(), m_iCurSortCol(-1)
{

}

CListDataDisplay::~CListDataDisplay()
{

}

/********************************
* ��������Create
* ��  ����
**    dwStyle:���������չ����Ѿ��̶�Ϊ����ѡ�У��������ߡ�
**       rect:�ڴ����е�λ�úʹ�С
** pParentWnd:������ָ��
**        nID:ID
**     bCheck:�Ƿ�ӵ�и�ѡ��
* ����ֵ���ɹ�����true��ʧ�ܷ���false
* ��  �ܣ������б��
********************************/
BOOL CListDataDisplay::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, bool bCheck)
{
	CListCtrl::Create(
		dwStyle,
		rect,
		pParentWnd,
		nID);

	//������չ���
	dwStyle = this->GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//ѡ��ĳ��ʹ���и�����ֻ������report����listctrl��
	dwStyle |= LVS_EX_GRIDLINES;//�����ߣ�ֻ������report����listctrl��
	if (bCheck)
	{
		dwStyle |= LVS_EX_CHECKBOXES;//itemǰ����checkbox�ؼ�

		InsertColumn(_T("  "), 20);//������Ϊ��ѡ��
	}
	this->SetExtendedStyle(dwStyle);

	return TRUE;
}

void CListDataDisplay::Resize(CRect rect)
{
	this->MoveWindow(rect);
}

void CListDataDisplay::OnLvnColumnclick(NMHDR* pNMHDR, LRESULT* pResult)
{

	//static int ncurSortCol = -1; // ���浱ǰ�������С�
	// һ��ʼ��ʾΪ-1����ʾ��δ���κ�������

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	LONG ws = GetWindowLong(this->m_hWnd, GWL_STYLE);

	int nSortOrder; // ����ķ�ʽ

	// ��������뵱ǰ�����в�ͬ���У���ı������򣬲�������ʽ��Ϊ����
	// ����ǰ��������������ͬ��������������������ʽ
	if (m_iCurSortCol == pNMListView->iSubItem)
	{
		if (ws & LVS_SORTASCENDING)
		{
			ws ^= LVS_SORTASCENDING;
			nSortOrder = LVS_SORTDESCENDING;
		}
		else
		{
			ws ^= LVS_SORTDESCENDING;
			nSortOrder = LVS_SORTASCENDING;
		}
	}
	else
	{
		m_iCurSortCol = pNMListView->iSubItem;
		nSortOrder = LVS_SORTASCENDING;

		//if (ws & LVS_SORTASCENDING)
		//	m_iCurSortCol = pNMListView->iSubItem;
		//if (ws & LVS_SORTASCENDING)
		//	nSortOrder = LVS_SORTASCENDING;
		//else
		//	nSortOrder = LVS_SORTDESCENDING;
	}

	// ����ǰ��������Ϣ�����ڴ���Style�У����Ժ�ʹ��
	ws |= nSortOrder;
	SetWindowLong(this->m_hWnd, GWL_STYLE, ws);

	// ����ITEM��LPARAM���������е������滻
	LVITEM li;
	li.mask = LVIF_PARAM | LVIF_TEXT;
	TCHAR szItemText[1024];
	for (int i = 0; i < this->GetItemCount(); i++)
	{
		li.iItem = i;
		li.iSubItem = m_iCurSortCol;
		li.cchTextMax = 1024;
		li.pszText = szItemText;
		this->GetItem(&li);
		TCHAR * szlparam = (TCHAR *)li.lParam;
		if (szlparam != NULL)
			// ɾ����ǰ����Ϣ���ͷſռ�
			// ���List ItemʱӦע�⽫lParam��ʼ��NULL
			delete szlparam;

		// ���Ƶ�ǰ�е�szItemText��Item��lParam��
		szlparam = new TCHAR[lstrlen(szItemText) + 1];
		lstrcpy(szlparam, szItemText);
		this->SetItemData(i, (DWORD_PTR)szlparam);
	}

	// ��ʼ����

	this->SortItems(ListViewCompareFunc, (LPARAM)(&nSortOrder));

	*pResult = 0;
}

void CListDataDisplay::OnDbClickItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;
	::PostMessage(GetParent()->m_hWnd, WM_CUSTOM_LIST_DBCLICK, NULL, NULL);
}

void CListDataDisplay::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	//���״̬�仯
	if (pNMListView->uOldState == 0 && pNMListView->uNewState == 0)

		return;	// No change

	//���ɵ�checkbox״̬
	BOOL bPrevState = (BOOL)(((pNMListView->uOldState & LVIS_STATEIMAGEMASK) >> 12) - 1);   // Old check box state

	if (bPrevState < 0)	// On startup there's no previous state 
	{
		bPrevState = 0; // so assign as false (unchecked)
	}
	// New check box state

	BOOL bChecked = (BOOL)(((pNMListView->uNewState & LVIS_STATEIMAGEMASK) >> 12) - 1);

	if (bChecked < 0) // On non-checkbox notifications assume false
	{
		bChecked = 0;
	}
	if (bPrevState != bChecked) // a change in check box
	{
		POSITION pos = GetFirstSelectedItemPosition();
		set<int> _setSelected;
		while (pos)
		{
			int iItem = GetNextSelectedItem(pos);
			_setSelected.insert(iItem);

		}
		if (_setSelected.find(pNMListView->iItem) != _setSelected.end())
		{
			for (set<int>::iterator it = _setSelected.begin();
				it != _setSelected.end();
				++it)
			{
				SetCheck(*it, bChecked);
			}
		}
	}
	return;

}

void CListDataDisplay::OnRClickItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;

	::PostMessage(GetParent()->m_hWnd, WM_CUSTOM_LIST_RCLICK, NULL, NULL);
}

int CALLBACK CListDataDisplay::ListViewCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	// �õ�����ʽ
	int * pisortorder = (int *)lParamSort;

	// �õ������е�������Ϣ
	TCHAR * sz1 = (TCHAR *)lParam1;
	TCHAR * sz2 = (TCHAR *)lParam2;

	// �Ƚ��е���Ϣ�����رȽϽ����
	// ��Ϊ�����򽫱ȽϽ������-1��
	if (*pisortorder == LVS_SORTASCENDING)
		return lstrcmp(sz1, sz2);
	else
		return lstrcmp(sz1, sz2) * (-1);


}

/********************************
* ��������InsertColumn
* ��  ����
** lpszColumnHeading:����
**            nWidth:�еĿ�ȴ�С
* ����ֵ��void
* ��  �ܣ�������еĺ������һ���С�
********************************/
void CListDataDisplay::InsertColumn(LPCTSTR lpszColumnHeading, int nWidth)
{
	//�����ܲ�����������С�
	if (CString(lpszColumnHeading).IsEmpty())
	{
		return;
	}

	//��ȡ����
	int iColumnCount = this->GetHeaderCtrl()->GetItemCount();

	CListCtrl::InsertColumn(iColumnCount, lpszColumnHeading, LVCFMT_LEFT, nWidth);
	if (0 != iColumnCount)
	{

	}
	CString csStr;
	this->GetHeaderCtrl()->GetDlgItemText(0, csStr);
}

/********************************
* ��������RemoveColumn
* ��  ����
** iColumnIndex:����
* ����ֵ��void
* ��  �ܣ��Ƴ�һ���С�����û������˸�ѡ��ѡ��������в����Ƴ���
********************************/
void CListDataDisplay::RemoveColumn(int iColumnIndex)
{
	//��ȡ��չ���
	DWORD dwStyle = this->GetExtendedStyle();

	//��������˸�ѡ�����һ�в�Ҫ�Ƴ���
	if ((dwStyle & LVS_EX_CHECKBOXES) && (0 == iColumnIndex))
	{
		return;
	}

	CListCtrl::DeleteColumn(iColumnIndex);
}

/********************************
* ��������SetColumnTitle
* ��  ����
** iColumnIndex:������
** sTitle:����
* ����ֵ��void
* ��  �ܣ�����һ���е�������������в����ڣ���ʧ��
************************************/
void CListDataDisplay::SetColumnTitle(const int &iColumnIndex, const string &sTitle)
{
	CHeaderCtrl *pHead = CListCtrl::GetHeaderCtrl();
	if (pHead && pHead->GetSafeHwnd())
	{
		TCHAR *titleBuffer = new TCHAR[sTitle.size() + 1];
		wmemcpy(titleBuffer, Tools::_TO_MFC(sTitle).c_str(), sTitle.size() + 1);

		HDITEM hdItem = { 0 };
		hdItem.mask = HDI_TEXT; //ȡ�ַ�����   
		hdItem.pszText = titleBuffer;
		hdItem.cchTextMax = sTitle.size() + 1; //��������С   
		pHead->SetItem(iColumnIndex, &hdItem); //������ͷ 
		delete[] titleBuffer;
	}

}

/********************************
* ��������UpdateInfo
* ��  ����
** iItem:����ţ��������𡣵��ò���Ϊ�������߳�����ǰ������ʱ�������ʱ������ʧ�ܡ�
** iSubItem:����ţ��������𡣵��ò���Ϊ�������߳�����ǰ������ʱ�������ʱ������ʧ�ܡ�
* ����ֵ���ɹ�ʱ���ر��޸ĵ�����ţ�ʧ��ʱ����-1
* ��  �ܣ������б���е�����
********************************/
int CListDataDisplay::UpdateInfo(int iItem, int iSubItem, LPCTSTR lpszInfo)
{
	//��ȡ����������
	int iItemCount = CListCtrl::GetItemCount();
	int iColumnCount = CListCtrl::GetHeaderCtrl()->GetItemCount();

	//��ȡ��չ���,����Ƿ������˸�ѡ��
	DWORD dwStyle = this->GetExtendedStyle();
	bool bCheck = (dwStyle & LVS_EX_CHECKBOXES) ? true : false;

	//�������Ƿ����
	if (
		(iItem < 0 || iItem >= iItemCount)		//�����
		||
		(
		bCheck ? (iSubItem < 1 || iSubItem >= iColumnCount)//�и�ѡ��ʱ����Ϊ��ѡ��ռ���˵����У�����<1ʱ�����
		:
		(iSubItem < 0 || iSubItem >= iColumnCount)
		)
		)
	{
		return -1;
	}

	CListCtrl::SetItemText(iItem, iSubItem, lpszInfo);
	return iItem;
	if (iItem >= 0 && iItem < iItemCount)
	{//��������

	}
	else
	{//��������
		iItem = 0;
		CListCtrl::InsertItem(iItem, _T(""));
		CListCtrl::SetItemText(iItem, iSubItem, lpszInfo);
	}
	return iItem;
}


/********************************
* ��������InsertInfo
* ��  ����
** _vctDatas:Ҫ��������ݡ�
** _iIndex:Ҫ�������ݵ�λ��,Ĭ��Ϊ0�������ʱ��������ȡ�㣬������ȡ���Ŀ�ȡֵ��
* ����ֵ���ɹ�ʱ���ر��������������ţ�ʧ��ʱ����-1
* ��  �ܣ�����һ�����ݡ�ԭ����_vctDatas��sizeӦ�������е�������һ�£��Ա�ÿ�в���һ�����ݡ�
*		  ���ǵ�size����ʱ�������������գ���size����ʱ����������������
********************************/
int CListDataDisplay::InsertInfo(const vector<string>& _vctDatas, int _iIndex)
{
	//��ȡ����������
	int iItemCount = CListCtrl::GetItemCount();
	int iColumnCount = CListCtrl::GetHeaderCtrl()->GetItemCount();

	//��ȡ��չ���,����Ƿ������˸�ѡ��
	DWORD dwStyle = this->GetExtendedStyle();
	bool bCheck = (dwStyle & LVS_EX_CHECKBOXES) ? true : false;

	//����_iIndexֵ
	_iIndex = _iIndex < 0 ?
		0 : _iIndex >= iItemCount ?
	iItemCount : _iIndex;

	CListCtrl::InsertItem(_iIndex, _T(""));
	vector<string>::const_iterator it = _vctDatas.begin();
	int iColumnIndex = bCheck ? 1 : 0;
	for (; iColumnIndex < iColumnCount && it != _vctDatas.end(); ++iColumnIndex, ++it)
	{
		CListCtrl::SetItemText(_iIndex, iColumnIndex, IBox::Tools::_TO_MFC(*it).c_str());
	}
	return _iIndex;
}