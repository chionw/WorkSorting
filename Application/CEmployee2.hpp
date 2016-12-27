#ifndef CEMPLOYEE2
#define CEMPLOYEE2
#include "CWorkTimeConfig.hpp"

class CEmployee2
{
public:
	typedef map<TIME_DATE, std::string> TYPE_ALL_BUSY_DAY;
	typedef shared_ptr< TYPE_ALL_BUSY_DAY > TYPE_SP_ALL_BUSY_DAY;
public:
	CEmployee2(std::string _sID, std::string _sName)
		: m_ID(_sID)
		, m_Name(_sName)
		, m_strTableName("work_time2")
		, m_strID("id")
		, m_strName("name")
		, m_strDate("work_date")
		, m_strWorkTimeCode("work_time_code")
	{
		m_spBusyDay = TYPE_SP_ALL_BUSY_DAY(new TYPE_ALL_BUSY_DAY);
		//创建数据库表，如果表不存在则被创建
		
		string sqlCreateTable = (boost::format("CREATE TABLE IF NOT EXISTS %s(%s varchar(32), %s varchar(256), %s date, %s varchar(32), primary key(%s,%s));") % m_strTableName % m_strID %m_strName %m_strDate %m_strWorkTimeCode %m_strID %m_strDate).str();

		string err;
		if (false == DATABASE_OBJ::GetInstance()->Excute(sqlCreateTable, err))
		{
			MessageBoxA(NULL, err.c_str(), "", MB_OK);
		}

		//从表中检索出与自身相关的数据
		string sqlSelect = "select " + m_strID + "," + m_strName + "," + m_strDate + "," + m_strWorkTimeCode  + " from " + m_strTableName + " where " + m_strID + "='" + m_ID + "';";

		SP_RESULT spResult;
		if (DATABASE_OBJ::GetInstance()->Excute(sqlSelect, spResult, err))
		{
			for (int index(0); index < spResult->GetRowCount(); ++index)
			{
				TIME_DATE date(boost::gregorian::from_string(*(spResult->GetData(index, 2))));
				if (false == date.is_special())
				{
					(*m_spBusyDay)[date] = *(spResult->GetData(index, 3));					
				}
			}
		}
		else
		{
			MessageBoxA(NULL, err.c_str(), "", MB_OK);
		}
	}

	bool Add(const TIME_DATE &_date, const std::string &_code)
	{
		int iYear = _date.year();
		int iMonth = _date.month();
		int iDay = _date.day();
		string strDate = (boost::format("%04d-%02d-%02d") % iYear %iMonth %iDay).str();

		string strSQL = (boost::format("REPLACE INTO %s(%s, %s, %s, %s) VALUES('%s','%s','%s',\"%s\");") % m_strTableName %m_strID %m_strName %m_strDate %m_strWorkTimeCode %m_ID %m_Name %strDate %_code).str();

		string err;
		if (false == DATABASE_OBJ::GetInstance()->Excute(strSQL, err))
		{
			MessageBoxA(NULL, err.c_str(), "数据库错误", MB_OK);
			return false;
		}

		(*m_spBusyDay)[_date] = _code;
		return true;
	}

	bool Remove(TIME_DATE _date)
	{
		return false;
	}

	std::string GetBusyHours(TIME_DATE _day)
	{
		if (m_spBusyDay->find(_day) != m_spBusyDay->end())
		{
			return m_spBusyDay->at(_day);
		}
		return "";
	}

	TYPE_SP_ALL_BUSY_DAY GetAllBusyDays()
	{
		return m_spBusyDay;
	}

	string ID(){ return m_ID; }

	string Name(){ return m_Name; }




private:
	std::string m_ID;
	std::string m_Name;
	TYPE_SP_ALL_BUSY_DAY m_spBusyDay;

	string \
		m_strTableName,
		m_strID,
		m_strName,
		m_strDate,
		m_strWorkTimeCode;
};

typedef shared_ptr<CEmployee2> SP_CEMPLOYEE2;

class CEmployeeManage2
{
public:
	typedef vector<SP_CEMPLOYEE2>	TYPE_ALL_EMPLOYEES;
public:
	CEmployeeManage2()
		: m_strTableName("employees")
		, m_strID("id")
		, m_strName("name")
	{
		//读取数据库的数据，创建员工列表
		//创建表
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

		//加载数据
		strSQL = "select " + m_strID + "," + m_strName + " from " + m_strTableName + ";";
		SP_RESULT spResult;
		if (DATABASE_OBJ::GetInstance()->Excute(strSQL, spResult, err))
		{
			for (int index(0); index < spResult->GetRowCount(); ++index)
			{
				SP_CEMPLOYEE2 spEmployee(new CEmployee2(*(*spResult)[index][0], *(*spResult)[index][1]));
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

	SP_CEMPLOYEE2 Query(string _sID)
	{
		BOOST_FOREACH(const SP_CEMPLOYEE2 &spEmployee, m_Employees)
		{
			if (spEmployee->ID() == _sID)
			{
				return spEmployee;
			}
		}
		return SP_CEMPLOYEE2(nullptr);
	}
private:
	TYPE_ALL_EMPLOYEES m_Employees;

	//数据库的字段定义
	string\
		m_strTableName,
		m_strID,
		m_strName;


};

#endif