
// ApplicationDlg.h : ͷ�ļ�
//

#pragma once

#include "CListControl.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "afxdtctl.h"

//#include "CEmployee.hpp"
#include "CEmployee2.hpp"

// CApplicationDlg �Ի���
class CApplicationDlg : public CDialogEx
{
// ����
public:
	CApplicationDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_APPLICATION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	afx_msg void OnSize(UINT nType, int cx, int cy);

	CListDataDisplay m_list_work_time_type;			//�б���������
	CListDataDisplay m_list_work_time_list;			//�б������б�
	CListDataDisplay m_list_employees;				//�б�Ա���б�

	CButton m_btn_add;								//��ť�����һ����¼

	afx_msg void OnBnClickedBtnAdd();				//��������Ӧ���

	CDateTimeCtrl m_timepick_work_date;				//���ڣ���������
	CButton m_check_auto_date;						//��ѡ�������Ƿ��Զ���һ

public:/*֧��HTML��ʽ����Ľӿ�*/

	/*
	* ���Ϊ�����ʽ����������ָʾ��ת���Ŀ�ʼʱ��ͽ���ʱ��
	* �����ʽΪ��
	* ����	����	���	ʱ��
	* 01	һ		J2		07:00-12:00
	*						15:00-16:00
	*/
	string Html_General(TIME_DATE _beginDay, TIME_DATE _endDay)
	{
		string sOutText("<html><head><style type=\"text/css\">td{ font-family:����; }</style></head><body>");
		
		auto allEmployees = m_Employees.QueryAll();
		for (auto everyEmployee : allEmployees)//����ÿһ��Ա��
		{
			sOutText += "<table cellspacing=\"0\" border=\"1\" style=\"width:100%%; font-family:��Բ\"><tr align=\"left\"><th>����</th><th>����</th><th>���</th><th>ʱ��</th></tr>";
			
			TIME_DATE curDay = _beginDay;
			while (curDay != _endDay + boost::gregorian::days(1))//����Ҫ������ʱ������
			{
				auto curWorkCode = everyEmployee->GetBusyHours(curDay);//��ȡĳһ��Ĺ�������
				
				string sBackgroundcolor("");//������ɫ
				{
					if (curDay.week_number()%2 == 0)//��˫����ɫ��ͬ
					{
						sBackgroundcolor = "style=\"background-color:#75e967;\"";
					}
					else
					{
						sBackgroundcolor = "style=\"background-color:#4ebf4a;\"";
					}
				}
			

				if (curWorkCode.size() == 0)//Ϊ�գ���ʾû�и��������
				{
					sOutText += (boost::format("<tr %s><td>%02d</td><td>%s</td><td>%s</td><td>NULL</td>") % sBackgroundcolor % curDay.day() % GetTools::DayOfWeek(curDay) % curWorkCode).str();
				}
				else
				{
					auto busyHours = m_workTimeConfig.GetItem(curWorkCode);//���ݹ������룬��ѯ��������ʱ������

					switch (busyHours.size())//�жϵ����ж��ٸ�����ʱ���
					{
					case 0://Ϊ�գ�û�и�������
					{
						sOutText += (boost::format("<tr %s><td>%02d</td><td>%s</td><td>%s</td><td>NULL</td>") % sBackgroundcolor % curDay.day() % GetTools::DayOfWeek(curDay) % curWorkCode).str();
					}
						break;
					case 1://��������Ϣ��Ҳ�������ϰ࣬����ʱ���������
					{
						if (busyHours[0].first == busyHours[0].second)//������Ϣ
						{
							sOutText += (boost::format("<tr %s><td rowspan=\"3\">%02d</td><td rowspan=\"3\">%s</td><td rowspan=\"3\">%s</td><td>&nbsp;</td><tr  %s><td>&nbsp;</td></tr><tr  %s><td>&nbsp;</td></tr>") 
								% sBackgroundcolor 
								% curDay.day() 
								% GetTools::DayOfWeek(curDay) 
								% "��Ϣ"
								% sBackgroundcolor 
								% sBackgroundcolor).str();
							
						}
						else
						{
							switch (GetTools::AreaOfDay(busyHours[0]))
							{
							case GetTools::EnumAreaOfDay::enMorning://���������Ͽ�ʼ�İ�
							{
								sOutText += (boost::format("<tr %s><td rowspan=\"3\">%02d</td><td rowspan=\"3\">%s</td><td rowspan=\"3\">%s</td><td>%s</td><tr %s><td>&nbsp;</td></tr><tr %s><td>&nbsp;</td></tr>") % sBackgroundcolor % curDay.day() % GetTools::DayOfWeek(curDay) % curWorkCode %GetTools::IntToTime(busyHours[0]) % sBackgroundcolor% sBackgroundcolor).str();

							}
								break;
							case GetTools::EnumAreaOfDay::enAfternoon://���翪ʼ�İ�
							{
								sOutText += (boost::format("<tr %s><td rowspan=\"3\">%02d</td><td rowspan=\"3\">%s</td><td rowspan=\"3\">%s</td><td>&nbsp;</td><tr %s><td>%s</td></tr><tr %s><td>&nbsp;</td></tr>") % sBackgroundcolor % curDay.day() % GetTools::DayOfWeek(curDay) % curWorkCode %GetTools::IntToTime(busyHours[0]) % sBackgroundcolor% sBackgroundcolor).str();
							}
								break;
							case GetTools::EnumAreaOfDay::enNight://���Ͽ�ʼ�İ�
							{
								sOutText += (boost::format("<tr %s><td rowspan=\"3\">%02d</td><td rowspan=\"3\">%s</td><td rowspan=\"3\">%s</td><td>&nbsp;</td><tr %s><td>&nbsp;</td></tr><tr %s><td>%s</td></tr>")
									% sBackgroundcolor
									% curDay.day()
									% GetTools::DayOfWeek(curDay)
									% curWorkCode
									% sBackgroundcolor
									% sBackgroundcolor
									% GetTools::IntToTime(busyHours[0])).str();
							}
								break;
							default:
								break;
							}
						}
					}
						break;
					case 2://����������ʱ��Σ��򿴵�һ�������������ϻ������硣�Դ˾����ֲ�
					{
						if (GetTools::AreaOfDay(busyHours[0]) == GetTools::EnumAreaOfDay::enMorning)//��������ʱ�䣬���Ϻ�����
						{
							sOutText += (boost::format("<tr %s><td rowspan=\"3\">%02d</td><td rowspan=\"3\">%s</td><td rowspan=\"3\">%s</td><td>%s</td><tr %s><td>%s</td></tr><tr %s><td>&nbsp;</td></tr>")
								% sBackgroundcolor 
								% curDay.day()
								% GetTools::DayOfWeek(curDay) 
								% curWorkCode 
								% GetTools::IntToTime(busyHours[0])
								% sBackgroundcolor
								% GetTools::IntToTime(busyHours[1]) 
								% sBackgroundcolor).str();
						}
						else if (GetTools::AreaOfDay(busyHours[0]) == GetTools::EnumAreaOfDay::enAfternoon)//��������ʱ�䣬���������
						{
							sOutText += (boost::format("<tr %s><td rowspan=\"3\">%02d</td><td rowspan=\"3\">%s</td><td rowspan=\"3\">%s</td><td>&nbsp;</td><tr %s><td>%s</td></tr><tr %s><td>%s</td></tr>")
								% sBackgroundcolor % curDay.day() 
								% GetTools::DayOfWeek(curDay) 
								% curWorkCode 
								% sBackgroundcolor
								% GetTools::IntToTime(busyHours[0])
								% sBackgroundcolor
								% GetTools::IntToTime(busyHours[1])).str();
						} 
						else
						{
							sOutText += (boost::format("<tr %s><td>%02d</td><td>%s</td><td>%s</td><td>�쳣����1/2ʱ��������</td>") % sBackgroundcolor % curDay.day() % GetTools::DayOfWeek(curDay) % curWorkCode).str();
						}
					}
						break;
					case 3://�����������Σ���һ����һ��
					{
							   sOutText += (boost::format("<tr %s><td rowspan=\"3\">%02d</td><td rowspan=\"3\">%s</td><td rowspan=\"3\">%s</td><td>%s</td><tr %s><td>%s</td></tr><tr %s><td>%s</td></tr>") % sBackgroundcolor % curDay.day() % GetTools::DayOfWeek(curDay) % curWorkCode %GetTools::IntToTime(busyHours[0]) % GetTools::IntToTime(busyHours[1]) % GetTools::IntToTime(busyHours[2]) % sBackgroundcolor% sBackgroundcolor).str();
					}
						break;
					default:
						sOutText += (boost::format("<tr %s><td>%02d</td><td>%s</td><td>%s</td><td>�쳣������ʱ�ι���</td>") % sBackgroundcolor % curDay.day() % GetTools::DayOfWeek(curDay) % curWorkCode).str();
						break;
					}
				}

				curDay += boost::gregorian::days(1);
			}
			sOutText += "</table>";
		}
		sOutText += "</body></html>";

		return sOutText;
	}

	/************************************************************************
	 * �԰��Ϊ���ߵĸ�ʽ
	 * ��� ʱ��	����	����
	 * J2	
	************************************************************************/
	string Html_WorkCode(TIME_DATE _beginDay, TIME_DATE _endDay)
	{
		

		string sOutText("<html><head><style type=\"text/css\">td{ font-family:����; }</style></head><body>");
		auto allEmployees = m_Employees.QueryAll();
		for (auto everyEmployee : allEmployees)//����ÿһ��Ա��
		{
			sOutText += "<table cellspacing=\"0\" border=\"0\" style=\"width:100%%; font-family:��Բ\"><tr align=\"left\"><th>���</th><th>�ϰ�ʱ��</th><th>����</th><th>����</th></tr>";
			std::map<string, vector<TIME_DATE> > data;
			//ͳ������
			TIME_DATE curDay = _beginDay;
			while (curDay != _endDay + boost::gregorian::days(1))//����Ҫ������ʱ������
			{

				auto curWorkCode = everyEmployee->GetBusyHours(curDay);//��ȡĳһ��Ĺ�������
				if (curWorkCode.size() != 0)
				{
					data[curWorkCode].push_back(curDay);
				}
				curDay += boost::gregorian::days(1);
			}

			

			for (auto everyWorkCode:data)
			{
				string sBackGroundColor;
				{
					static bool bSwitch(true);
					if (bSwitch)
					{
						sBackGroundColor = "#75e967";
						bSwitch = false;
					}
					else
					{
						sBackGroundColor = "#4ebf4a";
						bSwitch = true;
					}
					

				}
				string sWorkHours;
				{
					auto busyHours = m_workTimeConfig.GetItem(everyWorkCode.first);
					if (busyHours.size() == 0)
					{
						sWorkHours = "null";
					}
					else if (busyHours.size() == 1 && busyHours[0].first == busyHours[0].second)
					{
						sWorkHours = "��Ϣ";
					}
					else
					{
						for (int index(0); index < busyHours.size(); index++)
						{
							sWorkHours += GetTools::IntToTime(busyHours[index].first, busyHours[index].second);
							if (index + 1 != busyHours.size())//����������һ�����Ӹ��س�
							{
								sWorkHours += "<br/>";
							}
						}
					}

				}

				sOutText += (boost::format("<tr style=\"background-color:%s;\"><td rowspan=\"%d\">%s</td><td rowspan=\"%d\">%s</td><td>%02d</td><td>%s</td></tr>") % sBackGroundColor % everyWorkCode.second.size() % everyWorkCode.first %everyWorkCode.second.size() % sWorkHours %everyWorkCode.second[0].day() % GetTools::DayOfWeek(everyWorkCode.second[0])).str();

				for (int index = 1; index < everyWorkCode.second.size(); index++)
				{
					sOutText += (boost::format("<tr style=\"background-color:%s;\"><td>%02d</td><td>%s</td></tr>") % sBackGroundColor % everyWorkCode.second[index].day() % GetTools::DayOfWeek(everyWorkCode.second[index])).str();
				}
			}

			sOutText += "</table>";
					
		}
		sOutText += "</body></html>";

		return sOutText;
	}

	/************************************************************************
	 * ������Ϊ���ߵ���ͼ
	 * ���� ����	���	ʱ��
	 *
	************************************************************************/
	string Html_Week(TIME_DATE _beginDay, TIME_DATE _endDay)
	{
		string sOutText("<html><head><style type=\"text/css\">td{ font-family:����; }</style></head><body>");

		auto allEmployees = m_Employees.QueryAll();
		for (auto everyEmployee : allEmployees)//����ÿһ��Ա��
		{
			sOutText += "<table cellspacing=\"0\" border=\"1\" style=\"width:100%; font-family:��Բ\"><tr align=\"left\" style=\"background-color:yellowgreen;\"><th>����</th><th>����</th><th>���</th><th>�ϰ�ʱ��</th></tr>";

			map<int/*����*/, vector<TIME_DATE>> data;

			//ͳ������
			TIME_DATE curDay = _beginDay;
			while (curDay != _endDay + boost::gregorian::days(1))//����Ҫ������ʱ������
			{
				data[curDay.day_of_week()==0?7:curDay.day_of_week()].push_back(curDay);//�������죨0����Ϊ��7�����ں��档
			
				curDay += boost::gregorian::days(1);
			}

			for (auto everyWeekDay:data)
			{//��������һ�������������

				string sBackGroundColor;
				{
					static bool bSwitch(true);
					if (bSwitch)
					{
						sBackGroundColor = "#75e967";
						bSwitch = false;
					}
					else
					{
						sBackGroundColor = "#4ebf4a";
						bSwitch = true;
					}
				}

				int iCount(everyWeekDay.second.size());	//�������*�ж���������


				sOutText += (boost::format("<tr style=\"background-color:%s;\"><td rowspan=\"%d\">%s</td><td>%02d</td><td>%s</td>%s</tr>")%sBackGroundColor %iCount %GetTools::DayOfWeek(everyWeekDay.second[0]) %everyWeekDay.second[0].day() %everyEmployee->GetBusyHours(everyWeekDay.second[0]) %m_workTimeConfig.GetItem(everyEmployee->GetBusyHours(everyWeekDay.second[0])).HtmlTDStr()).str();

				for (int index(1); index < everyWeekDay.second.size(); ++index)
				{
					sOutText += (boost::format("<tr style=\"background-color:%s;\"><td>%02d</td><td>%s</td>%s</tr>")%sBackGroundColor  % everyWeekDay.second[index].day() % everyEmployee->GetBusyHours(everyWeekDay.second[index]) % m_workTimeConfig.GetItem(everyEmployee->GetBusyHours(everyWeekDay.second[index])).HtmlTDStr()).str();
				}

			}
			sOutText += "</table>";

		}
		sOutText += "</body></html>";

		return sOutText;
	}

	//����Ϊ��ͨ�ı���ʽ
	string Text_General(TIME_DATE _beginDay, TIME_DATE _endDay)
	{
		string sOutText;//���е�Ա������
		//�������µ�����
		auto vctEmployee = m_Employees.QueryAll();	//ȡ�����е�Ա������

		//��ȡ����Ա�������ݡ�ÿ��Ա�������ݱ���Ϊһ��TXT�ĵ�
		BOOST_FOREACH(CEmployeeManage2::TYPE_ALL_EMPLOYEES::value_type each, vctEmployee)//�������е�Ա��
		{
			sOutText += (boost::format("%s\r\n")%each->Name()).str();
			//ȡ����Ա������������
			string strText;
			{
				//���ÿ�ʼʱ��
				boost::gregorian::date beginDate(_beginDay);

				//���ý���ʱ��
				boost::gregorian::date endDate(_endDay);

				while (beginDate != (endDate + boost::gregorian::days(1)))
				{
					strText += (boost::format("%d��%02d��(%s):") % (int)beginDate.month() % beginDate.day() % GetTools::DayOfWeek(beginDate)).str();


					auto workCode = each->GetBusyHours(beginDate);


					strText += m_workTimeConfig.GetItem(workCode).str();
					strText += ("[" + workCode + "]");
					strText += "\r\n";

					beginDate += boost::gregorian::days(1);
				}
			}

			sOutText += (strText + "\r\n\r\n");
		}

		return sOutText;
	}

public:
	void SaveFile(const string& path, const string& fileName, const string& sText)
	{
		boost::filesystem::create_directory(path);
		//��������
		fstream fFile(
			path + fileName,
			ios::trunc | ios::out);
		if (fFile.good())
		{
			fFile << sText;
		}
		fFile.close();
	}
public:
	CWorkTimeConfig m_workTimeConfig;				//��������������
	
	//CEmployeeManage m_Employees;					//Ա���������
	CEmployeeManage2 m_Employees;					//Ա���������

	afx_msg void OnBnClickedBtnExport();			//��������
	CDateTimeCtrl m_timepick_export_from;
	CDateTimeCtrl m_timepick_export_to;
};
