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

//һ���еĹ���ʱ��
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
			return "��Ϣ";
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
			return "<td>��Ϣ</td>";
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
	std::vector< std::pair<long/*��ʼ�ķ�����*/, long/*�����ķ�����*/> > m_data;
}									TYPE_BUSY_HOUR;

//��¼һ����Ҫ����������
typedef struct _STRUCT_WORK_TIME
{
	boost::gregorian::date date;
	TYPE_BUSY_HOUR hours;
}				TYPE_BUSY_DAY;


/************************
* ���ݿ�Ĺ�����󣬵���ģʽ
* �����װ�˶Ա�ϵͳ��Ҫʹ�õ��������ļ��ķ��ʶ���ʹ��Ϊȫ��Ψһ�Ķ���
************************/
class DATABASE_OBJ
{

private:/*ʹ���졢���ơ���ֵΪ˽��*/
	DATABASE_OBJ(){};
	DATABASE_OBJ(DATABASE_OBJ&){};
	void operator=(DATABASE_OBJ&){}

private:/*���ݿ�����ָ��*/

public:
	//�������ݿ����
	static shared_ptr<CSqliteDataBase> GetInstance()
	{
		static shared_ptr<CSqliteDataBase> m_pDataBase;		//���ݿ����
		if (nullptr == m_pDataBase)
		{
			//��ȡ��ִ���ļ���·��
			TCHAR cPath[MAX_PATH];
			::GetModuleFileName(NULL, cPath, MAX_PATH);
			boost::filesystem::path clsPath(cPath);

			clsPath = (clsPath.parent_path() += "/Qifu.db");

			m_pDataBase = shared_ptr<CSqliteDataBase>(new CSqliteDataBase(clsPath.string()));
		}
		return m_pDataBase;
	}
};



/*�ϰ�ʱ��Ĵ����*/
class CWorkTimeConfig
{
public:
	typedef std::map< std::string/*��������*/, TYPE_BUSY_HOUR > TYPE_ALL_ITEM;
	typedef shared_ptr<TYPE_ALL_ITEM> TYPE_SP_ALL_ITEM;
public:
	//���캯���������ݿ��д��������û�У�һ�Ŵ����Ȼ����ر��е����ݵ��ڴ��С�
	CWorkTimeConfig()
		: m_strTableName("work_time_code")
		, m_strColumnCode("code")
		, m_strBeginTime("begin")
		, m_strEndTime("end")
		, m_spConfigBuf(new TYPE_ALL_ITEM)
	{
		//������
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

		//��������
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
	//���ݿ������ݵĻ�����
	shared_ptr<TYPE_ALL_ITEM> m_spConfigBuf;

	//���������ݿ�����ƺ��ֶ���
	const string \
		m_strTableName,
		m_strColumnCode,
		m_strBeginTime,
		m_strEndTime;

private:
	//���ַ���ת��Ϊ����������16:13:12�������������������16*60+13=973������ַ�����ʽ����ȷ��������-1
	long TimeToInt(const string &Time)
	{
		long lRet(-1);

		string sHours, sMinutes, sSeconds;
		int iPos(0);

		//������Сʱ
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

		//����������
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

		//����������
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

	//��һ������(���������)ת��Ϊ�ַ�����ʽ��ʱ�䡣��100��ת��Ϊ1:40:00��������Ϊ��ֵ�����ȡֵΪ1440
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