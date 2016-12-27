#pragma once
#ifndef DATABASE_201505
#define DATABASE_201505

#include "ToolsClass.hpp"
#include "stdafx.h"
#include "sqlite3.h"

#include <boost/filesystem.hpp>
#pragma comment(lib, "sqlite3.lib")

//������һ�����ؽ��Ԫ�ص�����
typedef shared_ptr<string> SP_RESULT_ELEMENT;

//������һ�����ؽ���е�����
typedef vector<SP_RESULT_ELEMENT> RESULT_ROW;

//���ݿ��ѯ����ı���ṹ��
class CDATABASE_RESULT
{
public:
	//��ȡ����
	unsigned int GetRowCount()
	{
		return m_data.empty() ? 0: m_data.size();
	}

	//��ȡ����
	unsigned int GetColumnCount()
	{
		return m_data.empty()? 0:m_data[0].size();
	}

	//��ȡ����
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

//����ṹ��ָ��ı���
typedef shared_ptr<CDATABASE_RESULT> SP_RESULT;

/*********************************************
 * sqlite���ݿ������װ����
 * ����ö���ʱ������һ��·��+�ļ����������ڸ�·���´���������Ѵ����������ӣ����ݿ��ļ���
 * Excute����ִ�жԸ����ݿ��ļ��Ĳ�����
 * �������SP_RESULT�ṹ�����淵�صĲ�ѯ���
 ********************************************/
class CSqliteDataBase
{
public:
	CSqliteDataBase(const string &sDatabaseName)
		: m_pDb(NULL)
	{
		//�������ݿ��ļ�
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
	 * ִ�����ݿ�����
	 * sSQL:Ҫִ�е�SQL����
	 * spResult:���صĽ��������һ����ά���飬���������б���Ϣ��
	 * sErrMsg:ʧ��ʱ����ʧ�ܵ�ԭ��
	 * retun:�ɹ�����true,ʧ�ܷ���false
	 *
	 ************************************************************************/
	bool Excute(const string & sSQL, SP_RESULT &spResult, string &sErrMsg)
	{
		//��ʼ������
		spResult.reset();
		sErrMsg.clear();

		//������ݿ��Ƿ��������
		if (nullptr == m_pDb)
		{
			sErrMsg = "���ݿ�û�б��򿪡�";
			return false;
		}

		//��ʼ׼��ִ��
		sqlite3_stmt * structResult(nullptr);
		int iRet = sqlite3_prepare_v2(m_pDb, sSQL.c_str(), -1, &structResult, 0);
		
		//���׼��ʧ�ܣ�����ʧ��ԭ��
		if (SQLITE_OK != iRet)
		{
			sqlite3_finalize(structResult);
			sErrMsg.assign(sqlite3_errmsg(m_pDb));
			return false;
		}
		
		//ִ��SQL���
		int iTimeOut(3000);			//��ʱ��ѯʱ��
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
					sErrMsg.assign("��ѯ��ʱ");
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
				break;//����ȡ��
			}
		}
		sqlite3_finalize(structResult);
		return true;
	}


	/************************************************************************
	* ִ�����ݿ�����
	* sSQL:Ҫִ�е�SQL����
	* sErrMsg:ʧ��ʱ����ʧ�ܵ�ԭ��
	* retun:�ɹ�����true,ʧ�ܷ���false
	* �ð汾û�з��ؽ���Ĳ����������з��ؽ������������ʧ����ʹ��Excute(const string & sSQL, SP_RESULT &spResult, string &sErrMsg)
	************************************************************************/
	bool Excute(const string & sSQL, string &sErrMsg)
	{
		SP_RESULT spResult(nullptr);
		return Excute(sSQL, spResult, sErrMsg);
	}
private:
	sqlite3						*m_pDb;			//���ݿ�ľ��
};
#endif