#pragma once
#ifndef DEFINE_LIST_CONTROL
#define DEFINE_LIST_CONTROL

/*
* �ÿؼ��ṩһ�������б����չʾ��ص����ݡ�
*
*/
class CListDataDisplay :public CListCtrl
{
public:
	CListDataDisplay();
	~CListDataDisplay();
public:
	//����
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, bool bCheck);

	void InsertColumn(LPCTSTR lpszColumnHeading, int nWidth);

	void RemoveColumn(int iColumnIndex);

	void SetColumnTitle(const int &iColumnIndex, const string &sTitle);
	//֧���е�����
	void OnLvnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	static int CALLBACK ListViewCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	//��Ӧ˫��item�¼�
	afx_msg void OnDbClickItem(NMHDR* pNMHDR, LRESULT* pResult);

	//��Ӧitem�仯�¼�
	afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnRClickItem(NMHDR* pNMHDR, LRESULT* pResult);
	//�����б��Ĵ�С
	void Resize(CRect rect);

	//���ݲ���
	int UpdateInfo(int iItem, int iSubItem, LPCTSTR lpszInfo);
	int InsertInfo(const vector<string>& _vctDatas, int _iIndex = 0);

public:
	DECLARE_MESSAGE_MAP()
private:
	int m_iCurSortCol;			//��¼��ǰ�����������ĸ���
};


#endif