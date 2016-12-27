
// ApplicationDlg.h : 头文件
//

#pragma once

#include "CListControl.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "afxdtctl.h"

//#include "CEmployee.hpp"
#include "CEmployee2.hpp"

// CApplicationDlg 对话框
class CApplicationDlg : public CDialogEx
{
// 构造
public:
	CApplicationDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_APPLICATION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	afx_msg void OnSize(UINT nType, int cx, int cy);

	CListDataDisplay m_list_work_time_type;			//列表：工作代码
	CListDataDisplay m_list_work_time_list;			//列表：工作列表
	CListDataDisplay m_list_employees;				//列表：员工列表

	CButton m_btn_add;								//按钮：添加一条记录

	afx_msg void OnBnClickedBtnAdd();				//函数：响应添加

	CDateTimeCtrl m_timepick_work_date;				//日期：工作日期
	CButton m_check_auto_date;						//复选框：日期是否自动加一

public:/*支持HTML格式输出的接口*/

	/*
	* 输出为常规格式。两个参数指示了转换的开始时间和结束时间
	* 常规格式为：
	* 日期	星期	班次	时间
	* 01	一		J2		07:00-12:00
	*						15:00-16:00
	*/
	string Html_General(TIME_DATE _beginDay, TIME_DATE _endDay)
	{
		string sOutText("<html><head><style type=\"text/css\">td{ font-family:楷体; }</style></head><body>");
		
		auto allEmployees = m_Employees.QueryAll();
		for (auto everyEmployee : allEmployees)//遍历每一个员工
		{
			sOutText += "<table cellspacing=\"0\" border=\"1\" style=\"width:100%%; font-family:幼圆\"><tr align=\"left\"><th>日期</th><th>星期</th><th>班次</th><th>时间</th></tr>";
			
			TIME_DATE curDay = _beginDay;
			while (curDay != _endDay + boost::gregorian::days(1))//遍历要导出的时间区间
			{
				auto curWorkCode = everyEmployee->GetBusyHours(curDay);//获取某一天的工作代码
				
				string sBackgroundcolor("");//背景颜色
				{
					if (curDay.week_number()%2 == 0)//单双周颜色不同
					{
						sBackgroundcolor = "style=\"background-color:#75e967;\"";
					}
					else
					{
						sBackgroundcolor = "style=\"background-color:#4ebf4a;\"";
					}
				}
			

				if (curWorkCode.size() == 0)//为空，表示没有该天的数据
				{
					sOutText += (boost::format("<tr %s><td>%02d</td><td>%s</td><td>%s</td><td>NULL</td>") % sBackgroundcolor % curDay.day() % GetTools::DayOfWeek(curDay) % curWorkCode).str();
				}
				else
				{
					auto busyHours = m_workTimeConfig.GetItem(curWorkCode);//根据工作代码，查询到工作的时间数据

					switch (busyHours.size())//判断当天有多少个工作时间段
					{
					case 0://为空，没有该天数据
					{
						sOutText += (boost::format("<tr %s><td>%02d</td><td>%s</td><td>%s</td><td>NULL</td>") % sBackgroundcolor % curDay.day() % GetTools::DayOfWeek(curDay) % curWorkCode).str();
					}
						break;
					case 1://可能是休息，也可能是上班，根据时间段来处理
					{
						if (busyHours[0].first == busyHours[0].second)//当天休息
						{
							sOutText += (boost::format("<tr %s><td rowspan=\"3\">%02d</td><td rowspan=\"3\">%s</td><td rowspan=\"3\">%s</td><td>&nbsp;</td><tr  %s><td>&nbsp;</td></tr><tr  %s><td>&nbsp;</td></tr>") 
								% sBackgroundcolor 
								% curDay.day() 
								% GetTools::DayOfWeek(curDay) 
								% "休息"
								% sBackgroundcolor 
								% sBackgroundcolor).str();
							
						}
						else
						{
							switch (GetTools::AreaOfDay(busyHours[0]))
							{
							case GetTools::EnumAreaOfDay::enMorning://当天是早上开始的班
							{
								sOutText += (boost::format("<tr %s><td rowspan=\"3\">%02d</td><td rowspan=\"3\">%s</td><td rowspan=\"3\">%s</td><td>%s</td><tr %s><td>&nbsp;</td></tr><tr %s><td>&nbsp;</td></tr>") % sBackgroundcolor % curDay.day() % GetTools::DayOfWeek(curDay) % curWorkCode %GetTools::IntToTime(busyHours[0]) % sBackgroundcolor% sBackgroundcolor).str();

							}
								break;
							case GetTools::EnumAreaOfDay::enAfternoon://下午开始的班
							{
								sOutText += (boost::format("<tr %s><td rowspan=\"3\">%02d</td><td rowspan=\"3\">%s</td><td rowspan=\"3\">%s</td><td>&nbsp;</td><tr %s><td>%s</td></tr><tr %s><td>&nbsp;</td></tr>") % sBackgroundcolor % curDay.day() % GetTools::DayOfWeek(curDay) % curWorkCode %GetTools::IntToTime(busyHours[0]) % sBackgroundcolor% sBackgroundcolor).str();
							}
								break;
							case GetTools::EnumAreaOfDay::enNight://晚上开始的班
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
					case 2://当天有两个时间段，则看第一个段是属于早上还是下午。以此决定分布
					{
						if (GetTools::AreaOfDay(busyHours[0]) == GetTools::EnumAreaOfDay::enMorning)//当天两个时间，早上和下午
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
						else if (GetTools::AreaOfDay(busyHours[0]) == GetTools::EnumAreaOfDay::enAfternoon)//当天两个时间，下午和晚上
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
							sOutText += (boost::format("<tr %s><td>%02d</td><td>%s</td><td>%s</td><td>异常：第1/2时段在晚上</td>") % sBackgroundcolor % curDay.day() % GetTools::DayOfWeek(curDay) % curWorkCode).str();
						}
					}
						break;
					case 3://当天有三个段，则一个段一个
					{
							   sOutText += (boost::format("<tr %s><td rowspan=\"3\">%02d</td><td rowspan=\"3\">%s</td><td rowspan=\"3\">%s</td><td>%s</td><tr %s><td>%s</td></tr><tr %s><td>%s</td></tr>") % sBackgroundcolor % curDay.day() % GetTools::DayOfWeek(curDay) % curWorkCode %GetTools::IntToTime(busyHours[0]) % GetTools::IntToTime(busyHours[1]) % GetTools::IntToTime(busyHours[2]) % sBackgroundcolor% sBackgroundcolor).str();
					}
						break;
					default:
						sOutText += (boost::format("<tr %s><td>%02d</td><td>%s</td><td>%s</td><td>异常：工作时段过多</td>") % sBackgroundcolor % curDay.day() % GetTools::DayOfWeek(curDay) % curWorkCode).str();
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
	 * 以班次为主线的格式
	 * 班次 时间	日期	星期
	 * J2	
	************************************************************************/
	string Html_WorkCode(TIME_DATE _beginDay, TIME_DATE _endDay)
	{
		

		string sOutText("<html><head><style type=\"text/css\">td{ font-family:楷体; }</style></head><body>");
		auto allEmployees = m_Employees.QueryAll();
		for (auto everyEmployee : allEmployees)//遍历每一个员工
		{
			sOutText += "<table cellspacing=\"0\" border=\"0\" style=\"width:100%%; font-family:幼圆\"><tr align=\"left\"><th>班次</th><th>上班时间</th><th>日期</th><th>星期</th></tr>";
			std::map<string, vector<TIME_DATE> > data;
			//统计数据
			TIME_DATE curDay = _beginDay;
			while (curDay != _endDay + boost::gregorian::days(1))//遍历要导出的时间区间
			{

				auto curWorkCode = everyEmployee->GetBusyHours(curDay);//获取某一天的工作代码
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
						sWorkHours = "休息";
					}
					else
					{
						for (int index(0); index < busyHours.size(); index++)
						{
							sWorkHours += GetTools::IntToTime(busyHours[index].first, busyHours[index].second);
							if (index + 1 != busyHours.size())//如果不是最后一个，加个回车
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
	 * 以星期为主线的视图
	 * 星期 日期	班次	时间
	 *
	************************************************************************/
	string Html_Week(TIME_DATE _beginDay, TIME_DATE _endDay)
	{
		string sOutText("<html><head><style type=\"text/css\">td{ font-family:楷体; }</style></head><body>");

		auto allEmployees = m_Employees.QueryAll();
		for (auto everyEmployee : allEmployees)//遍历每一个员工
		{
			sOutText += "<table cellspacing=\"0\" border=\"1\" style=\"width:100%; font-family:幼圆\"><tr align=\"left\" style=\"background-color:yellowgreen;\"><th>星期</th><th>日期</th><th>班次</th><th>上班时间</th></tr>";

			map<int/*星期*/, vector<TIME_DATE>> data;

			//统计数据
			TIME_DATE curDay = _beginDay;
			while (curDay != _endDay + boost::gregorian::days(1))//遍历要导出的时间区间
			{
				data[curDay.day_of_week()==0?7:curDay.day_of_week()].push_back(curDay);//把星期天（0）改为（7）放于后面。
			
				curDay += boost::gregorian::days(1);
			}

			for (auto everyWeekDay:data)
			{//计算星期一到星期天的数据

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

				int iCount(everyWeekDay.second.size());	//这个星期*有多少条数据


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

	//导出为普通文本格式
	string Text_General(TIME_DATE _beginDay, TIME_DATE _endDay)
	{
		string sOutText;//所有的员工数据
		//导出当月的数据
		auto vctEmployee = m_Employees.QueryAll();	//取出所有的员工数据

		//提取所有员工的数据。每个员工的数据保存为一个TXT文档
		BOOST_FOREACH(CEmployeeManage2::TYPE_ALL_EMPLOYEES::value_type each, vctEmployee)//遍历所有的员工
		{
			sOutText += (boost::format("%s\r\n")%each->Name()).str();
			//取出该员工区间内数据
			string strText;
			{
				//设置开始时间
				boost::gregorian::date beginDate(_beginDay);

				//设置结束时间
				boost::gregorian::date endDate(_endDay);

				while (beginDate != (endDate + boost::gregorian::days(1)))
				{
					strText += (boost::format("%d月%02d号(%s):") % (int)beginDate.month() % beginDate.day() % GetTools::DayOfWeek(beginDate)).str();


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
		//保存数据
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
	CWorkTimeConfig m_workTimeConfig;				//工作代码管理对象
	
	//CEmployeeManage m_Employees;					//员工管理对象
	CEmployeeManage2 m_Employees;					//员工管理对象

	afx_msg void OnBnClickedBtnExport();			//导出数据
	CDateTimeCtrl m_timepick_export_from;
	CDateTimeCtrl m_timepick_export_to;
};
