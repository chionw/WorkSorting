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
* 函数名：Create
* 参  数：
**    dwStyle:风格。其中扩展风格已经固定为整行选中，加网格线。
**       rect:在窗口中的位置和大小
** pParentWnd:父窗口指针
**        nID:ID
**     bCheck:是否拥有复选框
* 返回值：成功返回true，失败返回false
* 功  能：创建列表框
********************************/
BOOL CListDataDisplay::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, bool bCheck)
{
	CListCtrl::Create(
		dwStyle,
		rect,
		pParentWnd,
		nID);

	//设置扩展风格
	dwStyle = this->GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
	if (bCheck)
	{
		dwStyle |= LVS_EX_CHECKBOXES;//item前生成checkbox控件

		InsertColumn(_T("  "), 20);//第零列为复选框
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

	//static int ncurSortCol = -1; // 保存当前的排序列。
	// 一开始表示为-1，表示尚未按任何列排序。

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	LONG ws = GetWindowLong(this->m_hWnd, GWL_STYLE);

	int nSortOrder; // 排序的方式

	// 若点击列与当前排序列不同的列，则改变排序序，并将排序方式改为增序。
	// 若当前排序列与点击列相同，则更改增、减序的排序方式
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

	// 将当前的排序信息保存在窗口Style中，供以后使用
	ws |= nSortOrder;
	SetWindowLong(this->m_hWnd, GWL_STYLE, ws);

	// 将各ITEM的LPARAM用新排序列的内容替换
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
			// 删除以前的信息，释放空间
			// 添加List Item时应注意将lParam初始化NULL
			delete szlparam;

		// 复制当前列的szItemText到Item的lParam中
		szlparam = new TCHAR[lstrlen(szItemText) + 1];
		lstrcpy(szlparam, szItemText);
		this->SetItemData(i, (DWORD_PTR)szlparam);
	}

	// 开始排序

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

	//检查状态变化
	if (pNMListView->uOldState == 0 && pNMListView->uNewState == 0)

		return;	// No change

	//检查旧的checkbox状态
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
	// 得到排序方式
	int * pisortorder = (int *)lParamSort;

	// 得到两个列的排序信息
	TCHAR * sz1 = (TCHAR *)lParam1;
	TCHAR * sz2 = (TCHAR *)lParam2;

	// 比较列的信息并返回比较结果。
	// 若为减序，则将比较结果乘上-1。
	if (*pisortorder == LVS_SORTASCENDING)
		return lstrcmp(sz1, sz2);
	else
		return lstrcmp(sz1, sz2) * (-1);


}

/********************************
* 函数名：InsertColumn
* 参  数：
** lpszColumnHeading:列名
**            nWidth:列的宽度大小
* 返回值：void
* 功  能：在最后列的后面插入一个列。
********************************/
void CListDataDisplay::InsertColumn(LPCTSTR lpszColumnHeading, int nWidth)
{
	//不接受插入空列名的列。
	if (CString(lpszColumnHeading).IsEmpty())
	{
		return;
	}

	//获取列数
	int iColumnCount = this->GetHeaderCtrl()->GetItemCount();

	CListCtrl::InsertColumn(iColumnCount, lpszColumnHeading, LVCFMT_LEFT, nWidth);
	if (0 != iColumnCount)
	{

	}
	CString csStr;
	this->GetHeaderCtrl()->GetDlgItemText(0, csStr);
}

/********************************
* 函数名：RemoveColumn
* 参  数：
** iColumnIndex:列名
* 返回值：void
* 功  能：移除一个列。如果用户设置了复选复选框，则第零列不可移除。
********************************/
void CListDataDisplay::RemoveColumn(int iColumnIndex)
{
	//获取扩展风格
	DWORD dwStyle = this->GetExtendedStyle();

	//如果设置了复选框，则第一列不要移除。
	if ((dwStyle & LVS_EX_CHECKBOXES) && (0 == iColumnIndex))
	{
		return;
	}

	CListCtrl::DeleteColumn(iColumnIndex);
}

/********************************
* 函数名：SetColumnTitle
* 参  数：
** iColumnIndex:列索引
** sTitle:列名
* 返回值：void
* 功  能：设置一个列的列名，如果该列不存在，则失败
************************************/
void CListDataDisplay::SetColumnTitle(const int &iColumnIndex, const string &sTitle)
{
	CHeaderCtrl *pHead = CListCtrl::GetHeaderCtrl();
	if (pHead && pHead->GetSafeHwnd())
	{
		TCHAR *titleBuffer = new TCHAR[sTitle.size() + 1];
		wmemcpy(titleBuffer, Tools::_TO_MFC(sTitle).c_str(), sTitle.size() + 1);

		HDITEM hdItem = { 0 };
		hdItem.mask = HDI_TEXT; //取字符掩码   
		hdItem.pszText = titleBuffer;
		hdItem.cchTextMax = sTitle.size() + 1; //缓冲区大小   
		pHead->SetItem(iColumnIndex, &hdItem); //设置题头 
		delete[] titleBuffer;
	}

}

/********************************
* 函数名：UpdateInfo
* 参  数：
** iItem:行序号，从零算起。当该参数为负数或者超过当前最大序号时（即溢出时）操作失败。
** iSubItem:列序号，从零算起。当该参数为负数或者超过当前最大序号时（即溢出时）操作失败。
* 返回值：成功时返回被修改的行序号，失败时返回-1
* 功  能：更新列表框中的数据
********************************/
int CListDataDisplay::UpdateInfo(int iItem, int iSubItem, LPCTSTR lpszInfo)
{
	//获取行数，列数
	int iItemCount = CListCtrl::GetItemCount();
	int iColumnCount = CListCtrl::GetHeaderCtrl()->GetItemCount();

	//获取扩展风格,检查是否设置了复选框
	DWORD dwStyle = this->GetExtendedStyle();
	bool bCheck = (dwStyle & LVS_EX_CHECKBOXES) ? true : false;

	//检查参数是否溢出
	if (
		(iItem < 0 || iItem >= iItemCount)		//行溢出
		||
		(
		bCheck ? (iSubItem < 1 || iSubItem >= iColumnCount)//有复选框时，因为复选框占据了第零列，所以<1时算溢出
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
	{//更新数据

	}
	else
	{//插入数据
		iItem = 0;
		CListCtrl::InsertItem(iItem, _T(""));
		CListCtrl::SetItemText(iItem, iSubItem, lpszInfo);
	}
	return iItem;
}


/********************************
* 函数名：InsertInfo
* 参  数：
** _vctDatas:要插入的数据。
** _iIndex:要插入数据的位置,默认为0，当溢出时，负数则取零，超出则取最大的可取值。
* 返回值：成功时返回被插入的数据行序号，失败时返回-1
* 功  能：增加一条数据。原则上_vctDatas的size应该与现有的列数量一致，以便每列插入一个数据。
*		  但是当size不足时，后续的列留空，当size超出时，后续数据舍弃。
********************************/
int CListDataDisplay::InsertInfo(const vector<string>& _vctDatas, int _iIndex)
{
	//获取行数，列数
	int iItemCount = CListCtrl::GetItemCount();
	int iColumnCount = CListCtrl::GetHeaderCtrl()->GetItemCount();

	//获取扩展风格,检查是否设置了复选框
	DWORD dwStyle = this->GetExtendedStyle();
	bool bCheck = (dwStyle & LVS_EX_CHECKBOXES) ? true : false;

	//修正_iIndex值
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