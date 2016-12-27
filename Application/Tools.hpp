/********************************************
* 为数据库操作提供的工具
**********************************************/
#pragma once
#ifndef XD_IBOX_DATABASE_TOOLS_201411
#define XD_IBOX_DATABASE_TOOLS_201411
#define __FILE_CODE_GBK
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <queue>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/locale/encoding.hpp>
#include <boost/filesystem.hpp>
#include <boost/xpressive/xpressive_dynamic.hpp>
#include <fstream>
#include "Public/public.hpp"
#include "Public/md5.h"

using namespace std;

namespace IBox
{
	

	//支持线程安全操作的工具
	namespace Tools
	{
		/*线程安全的set容器，用法与set相同，但该类只实现了部分set常用的功能*/
		template<typename T>
		class set_s
		{
		public:
			
			typedef typename std::set<T>::iterator iterator;
			
			iterator begin()
			{
				boost::mutex::scoped_lock lock(m_lock);
				return m_data.begin();
			}

			iterator end()
			{
				boost::mutex::scoped_lock lock(m_lock);
				return m_data.end();
			}
			
			iterator find(T _value)
			{
				boost::mutex::scoped_lock lock(m_lock);
				return m_data.find(_value);
			}

			void insert(T _value)
			{
				boost::mutex::scoped_lock lock(m_lock);
				m_data.insert(_value);
			}
			
			void erase(T _value)
			{
				boost::mutex::scoped_lock lock(m_lock);
				m_data.erase(_value);
			}
			void erase(iterator _it)
			{
				m_data.erase(_it);
			}
			
			size_t size()
			{
				boost::mutex::scoped_lock lock(m_lock);
				return m_data.size();
			}

			void clear()
			{
				boost::mutex::scoped_lock lock(m_lock);
				m_data.clear();
			}
		private:
			boost::mutex m_lock;
			set<T> m_data;
		};


		template<typename T>
		class vector_s
		{
		public:

			typedef typename std::vector<T>::iterator iterator;

			iterator begin()
			{
				boost::mutex::scoped_lock lock(m_lock);
				return m_data.begin();
			}

			iterator end()
			{
				boost::mutex::scoped_lock lock(m_lock);
				return m_data.end();
			}
			T &operator [](size_t _index)
			{
				boost::mutex::scoped_lock lock(m_lock);
				return m_data[_index];
			}
			void push_back(T _value)
			{
				boost::mutex::scoped_lock lock(m_lock);
				m_data.push_back(_value);
			}
			
			void erase(iterator _it)
			{
				boost::mutex::scoped_lock lock(m_lock);
				m_data.erase(_it);
			}

			size_t size()
			{
				boost::mutex::scoped_lock lock(m_lock);
				return m_data.size();
			}

			void clear()
			{
				boost::mutex::scoped_lock lock(m_lock);
				m_data.clear();
			}
		private:
			boost::mutex m_lock;
			vector<T> m_data;

		};


		/*线程安全的map容器，用法与map相同，但该类只实现了部分set常用的功能*/
		template<typename _KEY, typename _VALUE>
		class map_s
		{
		public:
			typename typedef std::map<_KEY, _VALUE>::iterator				iterator;
			typename typedef std::map<_KEY, _VALUE>::key_type				key_type;
			typename typedef std::map<_KEY, _VALUE>::value_type				value_type;
			typename typedef std::map<_KEY, _VALUE>::const_iterator			const_iterator;
			typename typedef std::map<_KEY, _VALUE>::reverse_iterator		reverse_iterator;
			typename typedef std::map<_KEY, _VALUE>::const_reverse_iterator const_reverse_iterator;
			typename typedef std::map<_KEY, _VALUE>::size_type				size_type;
			typename typedef std::map<_KEY, _VALUE>::mapped_type			mapped_type;
			typename typedef std::map<_KEY, _VALUE>::key_type				key_type;
			reverse_iterator rbegin()
			{
				boost::mutex::scoped_lock lock(m_muLock);
				return m_map.rbegin();
			}
			const_reverse_iterator crbegin()
			{
				boost::mutex::scoped_lock lock(m_muLock);
				return m_map.crbegin();
			}
			const_iterator cbegin()
			{
				boost::mutex::scoped_lock lock(m_muLock);
				return m_map.cbegin();
			}
			iterator begin()
			{
				boost::mutex::scoped_lock lock(m_muLock);
				return m_map.begin();
			}


			reverse_iterator rend()
			{
				boost::mutex::scoped_lock lock(m_muLock);
				return m_map.rend();
			}
			const_reverse_iterator crend()
			{
				boost::mutex::scoped_lock lock(m_muLock);
				return m_map.crend();
			}
			const_iterator cend()
			{
				boost::mutex::scoped_lock lock(m_muLock);
				return m_map.cend();
			}
			iterator end()
			{
				boost::mutex::scoped_lock lock(m_muLock);
				return m_map.end();
			}

			mapped_type &at(key_type _key)
			{
				boost::mutex::scoped_lock lock(m_muLock);
				if (m_map.end() == m_map.find(_key))
				{
					return m_invalidValue;
				}
				else
				{
					return m_map.at(_key);
				}
			}
			mapped_type &operator[](key_type _key)
			{
				boost::mutex::scoped_lock lock(m_muLock);
				return m_map[_key];
			}

			void clear()
			{
				boost::mutex::scoped_lock lock(m_muLock);
				m_map.clear();
			}


			bool erase(key_type _key)
			{
				boost::mutex::scoped_lock lock(m_muLock);
				return m_map.erase(_key);
			}
			iterator erase(iterator _where)
			{
				boost::mutex::scoped_lock lock(m_muLock);
				return m_map.erase(_where);
			}

			iterator find(key_type _key)
			{
				boost::mutex::scoped_lock lock(m_muLock);
				return m_map.find(_key);
			}

			iterator insert(iterator _where, value_type _val)
			{
				boost::mutex::scoped_lock lock(m_muLock);
				return m_map.insert(_where, _val);
			}


			void insert(const std::pair<_KEY, _VALUE> &_val)
			{
				boost::mutex::scoped_lock lock(m_muLock);
				m_map.insert(_val);
			}

			size_type size()
			{
				boost::mutex::scoped_lock lock(m_muLock);
				return m_map.size();
			}
		private:
			boost::mutex m_muLock;
			std::map<_KEY, _VALUE> m_map;
			_VALUE m_invalidValue;			//无效值
		};



		/*信号灯工具
		* 这是一个轻量级的信号灯工具
		*/
		class semaphore
		{
			unsigned int count_;
			boost::mutex mutex_;	//该变量用于在修改count时进行锁定
			boost::condition_variable condition_;

		public:
			explicit semaphore(unsigned int initial) : count_(initial){}

			void signal()
			{
				{
					boost::lock_guard<boost::mutex> lock(mutex_);
					++count_;
				}
				condition_.notify_one(); 
			}

			boost::cv_status wait(long long _llMS) 
			{
				boost::unique_lock<boost::mutex> lock(mutex_);
				while (count_ == 0)
				{
					if(boost::cv_status::timeout 
						== 
						condition_.wait_for(lock, boost::chrono::milliseconds(_llMS)))
					{
						return boost::cv_status::timeout;
					}
					
				}
				--count_;
				return boost::cv_status::no_timeout;
			}
			
			void signalAll()
			{
				while(count_ < 0)
				{
					signal();
				}
			}
		};

		/*消息队列
		* 可以定义任何数据结构的消息队列。该队列先进先出，支持挂起等待。
		*/
		template <typename T>
		class MessageQueue
		{
		public:
			MessageQueue()
				:m_sm(0)
			{

			}

			/*从消息队列取出一个消息。如果消息队列为空，则等待_lMuilitiTime的时间。默认为永久等待。
			  超时情况下，或者用户刻意释放所有等待，但是队列中没有消息。返回false*/
			bool GetElement(T &_TMsg, long long _llMuilitiTime = 0xFFFFFFFFFF)
			{
				if (boost::cv_status::timeout == m_sm.wait(_llMuilitiTime))
				{
					return false;
				}

				if( 0 != m_queue.size() )
				{
					
					m_muQueue.lock();
					_TMsg = m_queue.front();
					m_queue.pop();
					m_muQueue.unlock();
					return true;
				}
				else
				{
					return false;
				}
				
			}
			
			/*向消息队列添加一个消息*/
			void PutElement(T _TMsg)
			{
				m_muQueue.lock();
				m_queue.push(_TMsg);
				m_muQueue.unlock();
				m_sm.signal();
			}
			
			/*释放所有的等待，使其返回失败。但如果在释放期间，有线程调用PutMessage，则所有等待中还是会有成功的情况出现。*/
			void Release()
			{
				m_sm.signalAll();
			}


		private:
			semaphore m_sm;
			boost::mutex	m_muQueue;
			std::queue<T>	m_queue;
		};

		/*线程池管理工具
		* 支持对线程进行统一管理。
		*/
		typedef struct _THREAD_POOL
		{
			~_THREAD_POOL(){RemoveAllThread();}

			vector<boost::thread*>	 m_vctThread;

			void AddThread(boost::thread *pThread)
			{
				m_vctThread.push_back(pThread);
			}

			void RemoveAllThread()
			{
				for (std::vector<boost::thread *>::iterator it = m_vctThread.begin();
					it != m_vctThread.end();
					++it)
				{
					if (*it)
					{
						delete (*it);
					}

				}
				m_vctThread.clear();

			}

			long GetCount()
			{
				return m_vctThread.size();
			}

			void interrupt_all()
			{
				for (std::vector<boost::thread *>::iterator it = m_vctThread.begin();
					it != m_vctThread.end();
					++it)
				{
					(*it)->interrupt();
				}
			}

		}THREAD_POOL;

	}

	//字符串格式化，字符编码的工具
	namespace Tools
	{
		void Split(vector<string> & vctResult, string strScource, char cCahr)
		{
			vctResult.clear();
			string strTemp;
			for (size_t index(0); index < strScource.size(); ++index)
			{
				if (strScource[index] == cCahr)
				{
					if(!strTemp.empty())vctResult.push_back(strTemp);
					strTemp.clear();
				}
				else
				{
					strTemp += strScource[index];
				}
				
			}
			if(!strTemp.empty())vctResult.push_back(strTemp);
		}

		template<typename T1>
		string ToString(T1 t1)
		{
			ostringstream t;
			t<<t1;
			return t.str();
		}

		template<typename T1,typename T2>
		string ToString(T1 t1,T2 t2)
		{
			ostringstream t;
			t<<t1<<t2;
			return t.str();
		}

		template<typename T1,typename T2, typename T3>
		string ToString(T1 t1,T2 t2, T3 t3)
		{
			ostringstream t;
			t<<t1<<t2<<t3;
			return t.str();
		}

		template<typename T1,typename T2, typename T3, typename T4>
		string ToString(T1 t1,T2 t2, T3 t3, T4 t4)
		{
			ostringstream t;
			t<<t1<<t2<<t3<<t4;
			return t.str();
		}

		template<typename T1, typename T2, typename T3, typename T4, typename T5>
		string ToString(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5)
		{
			ostringstream t;
			t << t1 << t2 << t3 << t4 << t5;
			return t.str();
		}

		template<typename T1,typename T2, typename T3, typename T4, typename T5, typename T6>
		string ToString(T1 t1,T2 t2, T3 t3, T4 t4, T5 t5, T6 t6)
		{
			ostringstream t;
			t<<t1<<t2<<t3<<t4<<t5<<t6;
			return t.str();
		}

		template<typename T1,typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
		string ToString(T1 t1,T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7)
		{
			ostringstream t;
			t<<t1<<t2<<t3<<t4<<t5<<t6<<t7;
			return t.str();
		}

		
		string GBKToUTF8(const  string& strGBK)  
		{  
			return boost::locale::conv::to_utf<char>(strGBK, "gbk");
		}  

		string UTF8ToGBK(const  string& strUTF8)  
		{  
			return boost::locale::conv::from_utf<char>(strUTF8, "gbk");
		}

		string UnicodeToUTF8(const  wstring& strUnicode)
		{
			return boost::locale::conv::utf_to_utf<char>(strUnicode);
		}

		wstring UTF8ToUnicode(const  string& strUTF8)
		{
			return boost::locale::conv::utf_to_utf<wchar_t>(strUTF8);
		}

		string UnicodeToGBK(const wstring& strUnicode)
		{
			return boost::locale::conv::from_utf<char>(boost::locale::conv::utf_to_utf<char>(strUnicode), "gbk");
		}

		wstring GBKToUnicode(const string& strGBK)
		{
			return boost::locale::conv::utf_to_utf<wchar_t>( boost::locale::conv::to_utf<char>(strGBK, "gbk") );
		}


		/*
		* 这个函数保证不管在UNICODE编码环境下还是多字节编码环境
		* CString::GetString()的值总能转换为string类型
		*/
#ifdef _UNICODE
		template<typename T = wchar_t*>
#else
		template<typename T = char*>
#endif
		string _TO_STRING(const T &str)
		{
#ifdef _UNICODE
			return UnicodeToGBK(wstring(str));
#else
			return string(str);
#endif
		}

		/*
		* 这个函数保证不管在UNICODE编码环境下还是多字节编码环境
		* CString::GetString()的值总能转换为wstring类型
		*/
#ifdef _UNICODE
		template<typename T = wchar_t*>
#else
		template<typename T = char*>
#endif
		wstring _TO_WSTRING(const T &str)
		{
#ifdef _UNICODE
			return wstring(str);
#else
			return GBKToUnicode(string(str));
#endif
		}

		//这两个函数用于保证string和wstring的字符串
		//不管在UNICODE编码环境下还是多字节编码环境
		//下都能做为参数传递给MFC的函数
#ifdef _UNICODE
template<typename T = wstring>
#else
template<typename T = string>
#endif
		T _TO_MFC(const string &str)
		{
#ifdef _UNICODE
			return GBKToUnicode(str);
#else
			return str;
#endif
		}

#ifdef _UNICODE
		template<typename T = wstring>
#else
		template<typename T = string>
#endif
		T _TO_MFC(const wstring &wstr)
		{
#ifdef _UNICODE
			return wstr;
#else
			return UnicodeToGBK(wstr);
#endif
		}

		
	}

	//文件操作工具
	namespace Tools
	{
		string GetFileMD5Hash(string sFilename)
		{
			// Declare variables
			char szBuf[64] = { 0 };
			string sResult = "";
			FILE * fileToHash = NULL;

			// MD5 object to work with
			MD5_CTX md5;

			// Check if file exists
			
			
			if (!boost::filesystem::exists(boost::filesystem::path(sFilename)))
				return "";

			// Hash the file
			if (md5file((char*)sFilename.c_str(), 0, &md5))
			{
				char buffer[1024] = {0};
				sprintf_s(buffer, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
					md5.digest[0], md5.digest[1], md5.digest[2], md5.digest[3],
					md5.digest[4], md5.digest[5], md5.digest[6], md5.digest[7],
					md5.digest[8], md5.digest[9], md5.digest[10], md5.digest[11],
					md5.digest[12], md5.digest[13], md5.digest[14], md5.digest[15]);
				/*sResult.Format(_T("%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X"),
					md5.digest[0], md5.digest[1], md5.digest[2], md5.digest[3],
					md5.digest[4], md5.digest[5], md5.digest[6], md5.digest[7],
					md5.digest[8], md5.digest[9], md5.digest[10], md5.digest[11],
					md5.digest[12], md5.digest[13], md5.digest[14], md5.digest[15]);*/
				sResult.assign(buffer);
			}

			// Set result and return it
			return sResult;
		}

		size_t GetFileLength(string _sFileName)
		{
			return boost::filesystem::file_size(_sFileName);
		}

		bool GetFileBlock(string strFileName, int iIndex, vector<byte> &vctBuf)
		{

			if (false == boost::filesystem::exists(strFileName))
			{
				return false;
			}

			#define BLOCK_SIZE 1024

			ifstream fFile;
			fFile.open(strFileName, ios::binary);

			if (fFile.fail())
			{
				return false;
			}
			
			vctBuf.resize(BLOCK_SIZE);
			fFile.seekg(BLOCK_SIZE * iIndex);

			vctBuf.resize (fFile.readsome((char*)&vctBuf[0], BLOCK_SIZE));

			return true;

		}

		//文件读取工具，单例模式
		class CFileRead
		{

		public:
			~CFileRead()
			{
				for (map<string, pair<HANDLE, size_t>>::iterator it= mFiles.begin();
					it != mFiles.end();
					++it)
				{
					CloseHandle(it->second.first);
				}
				mFiles.clear();
			}

			static void Release()
			{
				if (m_stcInstance)
				{
					delete m_stcInstance;
					m_stcInstance = nullptr;
				}
			}

			static CFileRead *GetInstance()
			{
				if (!m_stcInstance)
				{
					m_stcInstance = new CFileRead();
				}
				return m_stcInstance;
			}
			
			bool GetFileBlock(string _sFileName, size_t _pointer, size_t _block_size, vector<byte> &_vctBuffer)
			{
				_vctBuffer.clear();

				//在内存中查找该文件
				if (mFiles.find(_sFileName) == mFiles.end())
				{
					//找不到，则从硬盘上读入内存
					HANDLE _hFile = ::CreateFile(_TO_MFC(_sFileName).c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					if (INVALID_HANDLE_VALUE == _hFile)
					{
						m_errDesc = ToString("创建文件句柄失败。err=",::GetLastError());
						return false;
					}

					//获取文件大小
					DWORD dwSizeLow, dwSizeHigh;
					dwSizeLow = ::GetFileSize(_hFile, &dwSizeHigh);
					if (INVALID_FILE_SIZE == dwSizeLow)
					{
						CloseHandle(_hFile);
						m_errDesc = ToString("计算文件大小失败。err=",::GetLastError());
						return false;
					}

					//创建内存映射
					HANDLE _hMapFile = ::CreateFileMapping(
						_hFile,
						NULL,
						PAGE_READONLY,  //对映射文件进行读写
						dwSizeHigh,
						dwSizeLow,   //这两个参数共64位，所以支持的最大文件长度为16EB
						NULL);
					if (_hMapFile == INVALID_HANDLE_VALUE)
					{
						::CloseHandle(_hFile);
						m_errDesc = ToString("创建内存映射失败。err=", ::GetLastError());
						return false;
					}
					//保存映射的句柄，并关闭文件句柄。
					::CloseHandle(_hFile);
					mFiles.insert(make_pair(_sFileName, make_pair(_hMapFile, dwSizeLow)));
				}

				if (mFiles[_sFileName].second <= _pointer)
				{
					m_errDesc = "要获取的数据大于文件范围。";
					return false;
				}


				//把文件数据映射到进程的地址空间
				void* pvFile = ::MapViewOfFile(
					mFiles[_sFileName].first,
					FILE_MAP_READ,
					0,
					0,
					0);
				unsigned char *p = (unsigned char*)pvFile;

				_vctBuffer.resize(_pointer + _block_size <= mFiles[_sFileName].second ? _block_size : mFiles[_sFileName].second - _pointer);
				memcpy_s(&_vctBuffer[0], _vctBuffer.size(), p + _pointer, _vctBuffer.size());

				//结束
				::UnmapViewOfFile(pvFile); //撤销映射
				return true;
			}

			string GetErrorDesc()
			{
				return m_errDesc;
			}

			size_t GetFileSize(string _sFileName)
			{


				if (mFiles.find(_sFileName) == mFiles.end())
				{
					HANDLE _hFile = ::CreateFile(_TO_MFC(_sFileName).c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					if (INVALID_HANDLE_VALUE == _hFile)
					{
						return 0;
					}

					//获取文件大小
					DWORD dwSizeLow, dwSizeHigh;
						dwSizeLow = ::GetFileSize(_hFile, &dwSizeHigh);
					if (INVALID_FILE_SIZE == dwSizeLow)
					{
						::CloseHandle(_hFile);
						return 0;
					}
					::CloseHandle(_hFile);
					return dwSizeLow;
				}
				else
				{
					return mFiles[_sFileName].second;
				}
			}

		protected:
		private:
			static CFileRead* m_stcInstance;			//单例的对象指针

			map_s<string/*文件路径*/, pair<HANDLE/*文件内存映射句柄*/, size_t/*文件大小*/>> mFiles;	//管理的文件对象

			CFileRead(){}
			CFileRead(const CFileRead &){}
			string m_errDesc;
		};
		CFileRead* CFileRead::m_stcInstance = nullptr;
	}
}


#endif


