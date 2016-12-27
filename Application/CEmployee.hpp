/*�������һ���ϰ��ʱ�����*/
#pragma once
#ifndef DEFINE_CEMPLOYEE
#define DEFINE_CEMPLOYEE
#include "CWorkTimeConfig.hpp"

/*********************************************************
 * Ա������������
 * �����Ӧ��һ��Ա���Ĺ������š�
 * �������������Ҫ����һ��Ա����ID�š����󽫴����ݿ��м�������������ص���Ϣ�����ء�
 * ���ö���Ľӿڽ������صĲ�����ͬʱ����Ὣ����ͬ�������ݿ��С�
 ********************************************************/
class CEmployee
{
public:
	typedef map<TIME_DATE, TYPE_BUSY_HOUR> TYPE_ALL_BUSY_DAY;
	typedef shared_ptr< TYPE_ALL_BUSY_DAY > TYPE_SP_ALL_BUSY_DAY;
public:
	CEmployee(std::string _sID, std::string _sName)
		: m_ID(_sID)
		, m_Name(_sName)
		, m_strTableName("work_time")
		, m_strID("id")
		, m_strName("name")
		, m_strDate("work_date")
		, m_strBeginTime("begin_time")
		, m_strEndTime("end_time")
	{
		m_spBusyDay = TYPE_SP_ALL_BUSY_DAY(new TYPE_ALL_BUSY_DAY);

		//�������ݿ������������򱻴���
		string sqlCreateTable = "create table if not exists  " + m_strTableName + "(" +
			m_strID + " varchar(32)," +
			m_strName + " varchar(256)," +
			m_strDate + " date," +
			m_strBeginTime + " time," +
			m_strEndTime + " time);";

		string err;
		if (false == DATABASE_OBJ::GetInstance()->Excute(sqlCreateTable, err))
		{
			MessageBoxA(NULL, err.c_str(), "", MB_OK);
		}

		//�ӱ��м�������������ص�����
		string sqlSelect = "select " + m_strID + "," + m_strName + "," + m_strDate + "," + m_strBeginTime + "," + m_strEndTime + " from " + m_strTableName + " where " + m_strID + "='" + m_ID + "';";

		SP_RESULT spResult;
		if (DATABASE_OBJ::GetInstance()->Excute(sqlSelect, spResult, err))
		{

			for (int index(0); index < spResult->GetRowCount(); ++index)
			{
				TIME_DATE date(boost::gregorian::from_string(*(spResult->GetData(index, 2))));
				if (false == date.is_special())
				{
					(*m_spBusyDay)[date].push_back(*(spResult->GetData(index, 3)), *(spResult->GetData(index, 4)));
				}
			}
		}
		else
		{
			MessageBoxA(NULL, err.c_str(), "", MB_OK);
		}
	}
public:
	string ID(){ return m_ID; }

	string Name(){ return m_Name; }

	bool Add(TYPE_BUSY_DAY _day)
	{
		m_spBusyDay->insert(std::make_pair(_day.date, _day.hours));
		return true;
	}
	
	bool Add(TIME_DATE date, TYPE_BUSY_HOUR busyHour)
	{
		int iYear = date.year();
		int iMonth = date.month();
		int iDay = date.day();
		string strDate = (boost::format("%04d-%02d-%02d")%iYear %iMonth %iDay).str();

		for (int index(0); index < busyHour.size(); ++index)
		{
			string strSQL = "insert into " + m_strTableName +
				"(" +
				m_strID + "," +
				m_strName + "," +
				m_strDate + "," +
				m_strBeginTime + "," +
				m_strEndTime +
				") values('" +
				m_ID + "','" +
				m_Name + "','" +
				strDate + "','" +
				GetTools::IntToTime(busyHour[index].first) + "','" + 
				GetTools::IntToTime(busyHour[index].second) + "');";

			string err;
			if (false == DATABASE_OBJ::GetInstance()->Excute(strSQL, err))
			{
				MessageBoxA(NULL, err.c_str(), "���ݿ����", MB_OK);
				return false;
			}
		}
		(*m_spBusyDay)[date] = busyHour;
		return true;
	}

	bool Add(TIME_DATE, unsigned long, unsigned long)
	{
		return false;
	}

	bool Remove(TIME_DATE)
	{
		return false;
	}

	bool Remove(TIME_DATE, unsigned long, unsigned long)
	{
		return false;
	}
	
	TYPE_BUSY_HOUR GetBusyHours(TIME_DATE _day)
	{
		TYPE_BUSY_HOUR temp;
		if (m_spBusyDay->find(_day) != m_spBusyDay->end())
		{
			temp = m_spBusyDay->at(_day);
		}
		return temp;
	}
	
	TYPE_SP_ALL_BUSY_DAY GetAllBusyDays()
	{
		return m_spBusyDay;
	}
private:
	std::string m_ID;
	std::string m_Name;
	TYPE_SP_ALL_BUSY_DAY m_spBusyDay;

	string \
		m_strTableName,
		m_strID,
		m_strName,
		m_strDate,
		m_strBeginTime,
		m_strEndTime;
};

typedef shared_ptr<CEmployee> SP_CEMPLOYEE;

class CEmployeeManage
{
public:
	typedef vector<shared_ptr<CEmployee>>	TYPE_ALL_EMPLOYEES;
public:
	CEmployeeManage()
		: m_strTableName("employees")
		, m_strID("id")
		, m_strName("name")
	{
		//��ȡ���ݿ�����ݣ�����Ա���б�
		//������
		string strSQL = \
			"create table if not exists " + m_strTableName +
			"(" +
			m_strID + " varchar(32) PRIMARY KEY," +
			m_strName + " varchar(32));";

		string err;
		if (!DATABASE_OBJ::GetInstance()->Excute(strSQL, err))
		{
			MessageBoxA(NULL, err.c_str(), "", MB_OK);
			return;
		}

		//��������
		strSQL = "select " + m_strID + "," + m_strName + " from " + m_strTableName + ";";
		SP_RESULT spResult;
		if (DATABASE_OBJ::GetInstance()->Excute(strSQL, spResult, err))
		{
			for (int index(0); index < spResult->GetRowCount(); ++index)
			{
				SP_CEMPLOYEE spEmployee(new CEmployee(*(*spResult)[index][0], *(*spResult)[index][1]));
				m_Employees.push_back(spEmployee);
			}
		}
		else
		{
			MessageBoxA(NULL, err.c_str(), "", MB_OK);
		}
	}
	bool AddEmployee(string _sID, string _sName);

	bool DeleteEmployee(string _sID);

	TYPE_ALL_EMPLOYEES QueryAll()
	{
		return m_Employees;
	}

	SP_CEMPLOYEE Query(string _sID)
	{
		BOOST_FOREACH(const SP_CEMPLOYEE &spEmployee, m_Employees)
		{
			if (spEmployee->ID() == _sID)
			{
				return spEmployee;
			}
		}
		return SP_CEMPLOYEE(nullptr);
	}
private:
	TYPE_ALL_EMPLOYEES m_Employees;

	//���ݿ���ֶζ���
	string\
		m_strTableName,
		m_strID,
		m_strName;

	
};
#endif