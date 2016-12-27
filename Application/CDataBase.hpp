#pragma once
#ifndef DATABASE_201505
#define DATABASE_201505

#include "ToolsClass.hpp"
#include "stdafx.h"
#include "sqlite3.h"

#include <boost/filesystem.hpp>
#pragma comment(lib, "sqlite3.lib")

//定义了一个返回结果元素的类型
typedef shared_ptr<string> SP_RESULT_ELEMENT;

//定义了一个返回结果行的类型
typedef vector<SP_RESULT_ELEMENT> RESULT_ROW;

//数据库查询结果的保存结构体
class CDATABASE_RESULT
{
public:
	//获取行数
	unsigned int GetRowCount()
	{
		return m_data.empty() ? 0: m_data.size();
	}

	//获取列数
	unsigned int GetColumnCount()
	{
		return m_data.empty()? 0:m_data[0].size();
	}

	//获取数据
	SP_RESULT_ELEMENT GetData(unsigned int iRow, unsigned int iColumn)
	{
		if (iRow < m_data.size() && iColumn < m_data[iRow].size())
		{
			return m_data[iRow][iColumn];
		}
		else
		{
			return SP_RESULT_ELEMENT(nullptr);
		}
	}

	RESULT_ROW operator[](unsigned int iPos)
	{
		return m_data[iPos];
	}
private:
	friend class CSqliteDataBase;
	vector< RESULT_ROW > m_data;
};

//定义结构体指针的别名
typedef shared_ptr<CDATABASE_RESULT> SP_RESULT;

/*********************************************
 * sqlite数据库操作封装对象
 * 构造该对象时，传入一个路径+文件名。对象将在该路径下创建（如果已存在则是连接）数据库文件。
 * Excute可以执行对该数据库文件的操作。
 * 本类采用SP_RESULT结构来保存返回的查询结果
 ********************************************/
class CSqliteDataBase
{
public:
	CSqliteDataBase(const string &sDatabaseName)
		: m_pDb(NULL)
	{
		//建立数据库文件
		string sUTFDatabaseName = GetTools::GBKToUTF8(sDatabaseName);

		int iRet = sqlite3_open(sUTFDatabaseName.c_str(), &m_pDb);
		
		if (SQLITE_OK != iRet)
		{
			MessageBoxA(NULL, sqlite3_errmsg(m_pDb), nullptr, MB_OK);
			sqlite3_close(m_pDb);
			m_pDb = nullptr;
		}
	}

	~CSqliteDataBase()
	{
		if (m_pDb)
		{
			sqlite3_close(m_pDb);
		}
	}

	/************************************************************************
	 * 执行数据库命令
	 * sSQL:要执行的SQL命令
	 * spResult:返回的结果，这是一个二维数组，保存结果的列表信息。
	 * sErrMsg:失败时返回失败的原因。
	 * retun:成功返回true,失败返回false
	 *
	 ************************************************************************/
	bool Excute(const string & sSQL, SP_RESULT &spResult, string &sErrMsg)
	{
		//初始化参数
		spResult.reset();
		sErrMsg.clear();

		//检查数据库是否关联上了
		if (nullptr == m_pDb)
		{
			sErrMsg = "数据库没有被打开。";
			return false;
		}

		//开始准备执行
		sqlite3_stmt * structResult(nullptr);
		int iRet = sqlite3_prepare_v2(m_pDb, sSQL.c_str(), -1, &structResult, 0);
		
		//如果准备失败，返回失败原因
		if (SQLITE_OK != iRet)
		{
			sqlite3_finalize(structResult);
			sErrMsg.assign(sqlite3_errmsg(m_pDb));
			return false;
		}
		
		//执行SQL语句
		int iTimeOut(3000);			//超时查询时间
		spResult = SP_RESULT(new CDATABASE_RESULT);
		while (true)
		{
			iRet = sqlite3_step(structResult);

			if (SQLITE_BUSY == iRet)
			{
				Sleep(100);
				iTimeOut -= 100;
				if (iTimeOut > 0)
				{
					continue;
				}
				else
				{
					sErrMsg.assign("查询超时");
					sqlite3_finalize(structResult);
					return false;
				}
			}

			if (SQLITE_ROW == iRet)
			{
				RESULT_ROW row;
				int iCount = sqlite3_column_count(structResult);
				for (int index = 0; index < iCount; ++index)
				{
					shared_ptr<string> spStr(new string((char*)sqlite3_column_text(structResult, index)));
					*spStr = GetTools::UTF8ToGBK(*spStr);
					SP_RESULT_ELEMENT spElement( spStr );
					row.push_back(spElement);
				}
				spResult->m_data.push_back(row);
			}
			else
			{
				break;//数据取完
			}
		}
		sqlite3_finalize(structResult);
		return true;
	}


	/************************************************************************
	* 执行数据库命令
	* sSQL:要执行的SQL命令
	* sErrMsg:失败时返回失败的原因。
	* retun:成功返回true,失败返回false
	* 该版本没有返回结果的参数，对于有返回结果命令，结果将丢失。请使用Excute(const string & sSQL, SP_RESULT &spResult, string &sErrMsg)
	************************************************************************/
	bool Excute(const string & sSQL, string &sErrMsg)
	{
		SP_RESULT spResult(nullptr);
		return Excute(sSQL, spResult, sErrMsg);
	}
private:
	sqlite3						*m_pDb;			//数据库的句柄
};
#endif