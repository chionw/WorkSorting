#ifndef WORK_TIME_CONFIG
#define WORK_TIME_CONFIG

#include<vector>
#include<string>
#include<boost/function.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/time_period.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include "CDataBase.hpp"

typedef boost::gregorian::date							TIME_DATE;

//一天中的工作时间
typedef struct
{
	int size()
	{
		return m_data.size();
	}

	std::pair<long, long> &operator[](int iPos)
	{
		return m_data[iPos];
	}

	void push_back(long _1, long _2)
	{
		push_back(std::make_pair(_1, _2));
	}
	void push_back(const std::pair<long, long>& _1)
	{
		m_data.push_back(_1);
		GetTools::SortPeriod(m_data);
	}
	void push_back(string _1, string _2)
	{
		push_back(GetTools::TimeToInt(_1), GetTools::TimeToInt(_2));
	}
	string str()
	{
		if (m_data.size() == 1 && m_data[0].first == m_data[0].second)
		{
			return "休息";
		}

		string sRet;

		for (auto it = m_data.begin();
			it != m_data.end();
			++it)
		{
			sRet += (boost::format("%02d:%02d:00~%02d:%02d:00 ")
				% (it->first / 60) % (it->first % 60)
				% (it->second / 60) % (it->second % 60)).str();
		}
		return sRet;
	}

	string HtmlTDStr()
	{
		if (m_data.size() == 1 && m_data[0].first == m_data[0].second)
		{
			return "<td>休息</td>";
		}

		string sRet("<td>");

		for (auto it = m_data.begin();
			it != m_data.end();
			++it)
		{
			sRet += (boost::format("%02d:%02d:00-%02d:%02d:00 ")
				% (it->first / 60) % (it->first % 60)
				% (it->second / 60) % (it->second % 60)).str();
			if ((it +1) != m_data.end())
			{
				sRet += "<br/>";
			}
		}
		return sRet + "</td>";
	}
	std::vector< std::pair<long/*开始的分钟数*/, long/*结束的分钟数*/> > m_data;
}									TYPE_BUSY_HOUR;

//记录一个需要工作的日子
typedef struct _STRUCT_WORK_TIME
{
	boost::gregorian::date date;
	TYPE_BUSY_HOUR hours;
}				TYPE_BUSY_DAY;


/************************
* 数据库的管理对象，单例模式
* 该类封装了对本系统所要使用到的数据文件的访问对象。使其为全局唯一的对象。
************************/
class DATABASE_OBJ
{

private:/*使构造、复制、赋值为私有*/
	DATABASE_OBJ(){};
	DATABASE_OBJ(DATABASE_OBJ&){};
	void operator=(DATABASE_OBJ&){}

private:/*数据库对象的指针*/

public:
	//返回数据库对象
	static shared_ptr<CSqliteDataBase> GetInstance()
	{
		static shared_ptr<CSqliteDataBase> m_pDataBase;		//数据库对象
		if (nullptr == m_pDataBase)
		{
			//获取可执行文件的路径
			TCHAR cPath[MAX_PATH];
			::GetModuleFileName(NULL, cPath, MAX_PATH);
			boost::filesystem::path clsPath(cPath);

			clsPath = (clsPath.parent_path() += "/Qifu.db");

			m_pDataBase = shared_ptr<CSqliteDataBase>(new CSqliteDataBase(clsPath.string()));
		}
		return m_pDataBase;
	}
};



/*上班时间的代码表*/
class CWorkTimeConfig
{
public:
	typedef std::map< std::string/*代码名称*/, TYPE_BUSY_HOUR > TYPE_ALL_ITEM;
	typedef shared_ptr<TYPE_ALL_ITEM> TYPE_SP_ALL_ITEM;
public:
	//构造函数将在数据库中创建（如果没有）一张代码表。然后加载表中的数据到内存中。
	CWorkTimeConfig()
		: m_strTableName("work_time_code")
		, m_strColumnCode("code")
		, m_strBeginTime("begin")
		, m_strEndTime("end")
		, m_spConfigBuf(new TYPE_ALL_ITEM)
	{
		//创建表
		string strSQL = \
			"create table if not exists " + m_strTableName +
			"(" +
			m_strColumnCode + " varchar(32)," +
			m_strBeginTime + " time," +
			m_strEndTime + " time" +
			");";

		string err;
		if (!DATABASE_OBJ::GetInstance()->Excute(strSQL, err))
		{
			MessageBoxA(NULL, err.c_str(), "", MB_OK);
			return;
		}

		//加载数据
		strSQL = "select " + m_strColumnCode + "," + m_strBeginTime + "," + m_strEndTime + " from " + m_strTableName + ";";
		SP_RESULT spResult;
		if (DATABASE_OBJ::GetInstance()->Excute(strSQL, spResult, err))
		{

			for (int index(0); index < spResult->GetRowCount(); ++index)
			{
				(*m_spConfigBuf)[*spResult->GetData(index, 0)].push_back(TimeToInt(*spResult->GetData(index, 1)), TimeToInt(*spResult->GetData(index, 2)));
			}
		}
		else
		{
			MessageBoxA(NULL, err.c_str(), "", MB_OK);
		}
	}

	void AddItem(std::string _sCode, unsigned long _ulFrom, unsigned long _ulTo)
	{
		string strSQL
			= "insert into " + m_strTableName +
			"(" + m_strColumnCode + "," + m_strBeginTime + "," + m_strEndTime +
			") values(" +
			_sCode + "," +
			IntToTime(_ulFrom) + "," +
			IntToTime(_ulTo) +
			");";
		string err;

		if (DATABASE_OBJ::GetInstance()->Excute(strSQL, err))
		{
			(*m_spConfigBuf)[_sCode].push_back(_ulFrom, _ulTo);
		}
	}

	TYPE_BUSY_HOUR GetItem(const std::string &_sCode)
	{
		TYPE_BUSY_HOUR temp;
		return m_spConfigBuf->find(_sCode) == m_spConfigBuf->end() ? temp : (*m_spConfigBuf)[_sCode];

	}

	void RemoveItem(const std::string &_sCode)
	{
		string strSQL = "delete from " + m_strTableName + " where " + m_strColumnCode + " = '" + _sCode + "';";

		string err;
		if (DATABASE_OBJ::GetInstance()->Excute(strSQL, err))
		{
			m_spConfigBuf->erase(_sCode);
		}
	}

	TYPE_SP_ALL_ITEM & GetAllItems()
	{
		return m_spConfigBuf;
	}
private:
	//数据库中数据的缓冲区
	shared_ptr<TYPE_ALL_ITEM> m_spConfigBuf;

	//定义了数据库的名称和字段名
	const string \
		m_strTableName,
		m_strColumnCode,
		m_strBeginTime,
		m_strEndTime;

private:
	//将字符串转换为分钟数，如16:13:12，将忽略秒数，计算出16*60+13=973。如果字符串格式不正确，将返回-1
	long TimeToInt(const string &Time)
	{
		long lRet(-1);

		string sHours, sMinutes, sSeconds;
		int iPos(0);

		//解析出小时
		for (; iPos < Time.size(); ++iPos)
		{
			if (Time[iPos] >= '0' && Time[iPos] <= '9')
			{
				sHours += Time[iPos];
			}
			else if (Time[iPos] == ':')
			{
				++iPos;
				break;
			}
			else
			{
				return lRet;
			}
		}

		//解析出分钟
		for (; iPos < Time.size(); ++iPos)
		{
			if (Time[iPos] >= '0' && Time[iPos] <= '9')
			{
				sMinutes += Time[iPos];
			}
			else if (Time[iPos] == ':')
			{
				++iPos;
				break;
			}
			else
			{
				return lRet;
			}
		}

		//解析出秒数
		for (; iPos < Time.size(); ++iPos)
		{
			if (Time[iPos] >= '0' && Time[iPos] <= '9')
			{
				sSeconds += Time[iPos];
			}
			else
			{
				return lRet;
			}
		}

		if (sHours.empty() || sMinutes.empty() || sHours.size() > 2 || sMinutes.size() > 2 || sSeconds.size() > 2)
		{
			return lRet;
		}

		lRet = atoi(sHours.c_str()) * 60 + atoi(sMinutes.c_str());
		return lRet;
	}

	//将一个整数(代表分钟数)转换为字符串形式的时间。如100将转换为1:40:00。参数不为负值。最大取值为1440
	string IntToTime(const unsigned long &Int)
	{
		std::string sRet("");
		if (Int > 1440)
		{
			return sRet;
		}
		return (boost::format("%02d:%02d:00") % (Int / 60) % (Int % 60)).str();
	}
};

#endif