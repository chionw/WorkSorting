#pragma once
#ifndef DEFINE_LIST_CONTROL
#define DEFINE_LIST_CONTROL

/*
* 该控件提供一个多列列表框。以展示相关的数据。
*
*/
class CListDataDisplay :public CListCtrl
{
public:
	CListDataDisplay();
	~CListDataDisplay();
public:
	//创建
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, bool bCheck);

	void InsertColumn(LPCTSTR lpszColumnHeading, int nWidth);

	void RemoveColumn(int iColumnIndex);

	void SetColumnTitle(const int &iColumnIndex, const string &sTitle);
	//支持列的排序
	void OnLvnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	static int CALLBACK ListViewCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	//响应双击item事件
	afx_msg void OnDbClickItem(NMHDR* pNMHDR, LRESULT* pResult);

	//响应item变化事件
	afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnRClickItem(NMHDR* pNMHDR, LRESULT* pResult);
	//重置列表框的大小
	void Resize(CRect rect);

	//数据操作
	int UpdateInfo(int iItem, int iSubItem, LPCTSTR lpszInfo);
	int InsertInfo(const vector<string>& _vctDatas, int _iIndex = 0);

public:
	DECLARE_MESSAGE_MAP()
private:
	int m_iCurSortCol;			//记录当前的排序列是哪个。
};


#endif