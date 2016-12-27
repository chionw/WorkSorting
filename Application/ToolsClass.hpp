#ifndef TOOLS_CLASS
#define TOOLS_CLASS
#include"stdafx.h"
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/locale/encoding.hpp>
#include <boost/filesystem.hpp>
#include <boost/xpressive/xpressive_dynamic.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
using namespace std;

class GetTools
{
public:
	static string GBKToUTF8(const  string& strGBK)
	{
		return boost::locale::conv::to_utf<char>(strGBK, "gbk");
	}

	static string UTF8ToGBK(const  string& strUTF8)
	{
		return boost::locale::conv::from_utf<char>(strUTF8, "gbk");
	}

	static string UnicodeToUTF8(const  wstring& strUnicode)
	{
		return boost::locale::conv::utf_to_utf<char>(strUnicode);
	}

	static wstring UTF8ToUnicode(const  string& strUTF8)
	{
		return boost::locale::conv::utf_to_utf<wchar_t>(strUTF8);
	}

	static string UnicodeToGBK(const wstring& strUnicode)
	{
		return boost::locale::conv::from_utf<char>(boost::locale::conv::utf_to_utf<char>(strUnicode), "gbk");
	}

	static wstring GBKToUnicode(const string& strGBK)
	{
		return boost::locale::conv::utf_to_utf<wchar_t>(boost::locale::conv::to_utf<char>(strGBK, "gbk"));
	}

	//���ַ���ת��Ϊ����������16:13:12�������������������16*60+13=973������ַ�����ʽ����ȷ��������-1
	static long TimeToInt(const string &Time)
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
	static string IntToTime(const unsigned long &Int)
	{
		std::string sRet("");
		if (Int > 1440)
		{
			return sRet;
		}
		return (boost::format("%02d:%02d:00") % (Int / 60) % (Int % 60)).str();
	}

	static string IntToTime(const unsigned long &Int1, const unsigned long &Int2)
	{
		std::string sRet("");
		if (Int1 > 1440 || Int2 > 1440)
		{
			return sRet;
		}
		return (boost::format("%02d:%02d - %02d:%02d") % (Int1 / 60) % (Int1 % 60) % (Int2 / 60) % (Int2 % 60)).str();
	}

	static string IntToTime(const std::pair<long, long> _time)
	{
		return IntToTime(_time.first, _time.second);
	}
/*�������*/
	//�������������ÿ��������һ����������[long, long]�����򰴴�С����������������������ཻ����ϲ�֮
	static void SortPeriod(vector<std::pair<long, long>> &_periods)
	{
		vector<std::pair<long, long>> tempPeriods;

		//�Ƚ��ܹ��ϲ���������кϲ���ֱ�����е����䶼����
		while (_periods.size() > 0)
		{
			std::pair<long, long> temp = _periods[0];
			int i = 1;
			for (; i < _periods.size(); ++i)
			{
				if (ComparePeriod(temp, _periods[i]) == 0)
				{
					_periods[i] = MeragePeriod(temp, _periods[i]);
					break;
					
				}
			}
			if (i == _periods.size())
			{
				tempPeriods.push_back(_periods[0]);
			}
			_periods.erase(_periods.begin());
		}

		//��ʱtempPeriods������䶼�ǻ����
		for (int i = 0; i < tempPeriods.size(); ++i)
		{
			for (int j = i + 1; j < tempPeriods.size(); ++j)
			{
				if (ComparePeriod(tempPeriods[i], tempPeriods[j]) == 1)
				{
					tempPeriods[i].first ^= tempPeriods[j].first;
					tempPeriods[j].first ^= tempPeriods[i].first;
					tempPeriods[i].first ^= tempPeriods[j].first;

					tempPeriods[i].second ^= tempPeriods[j].second;
					tempPeriods[j].second ^= tempPeriods[i].second;
					tempPeriods[i].second ^= tempPeriods[j].second;
				}
			}
		}

		//��ֵ���
		_periods = tempPeriods;
	}

	//�Ƚ�����1������2.�������1>����2�򷵻�1��С���򷵻�-1.��Ȼ��ཻ�򷵻�0
	static int ComparePeriod(std::pair<long, long> _1, std::pair<long, long> _2)
	{
		if (_1.first < _1.second)
		{
			_1.first ^= _1.second;
			_1.second ^= _1.first;
			_1.first ^= _1.second;
		}
		if (_2.first < _2.second)
		{
			_2.first ^= _2.second;
			_2.second ^= _2.first;
			_2.first ^= _2.second;
		}

		if (_1.second < _2.first)
		{
			return -1;
		}

		if (_2.second < _1.first)
		{
			return 1;
		}

		return 0;
	}

	//�ϲ��������䣬����ϲ�
	static std::pair<long, long> MeragePeriod(std::pair<long, long> _1, std::pair<long, long> _2)
	{
		long lMin(_1.first);
		long lMax(_1.first);

		if (lMin > _1.second)
		{
			lMin = _1.second;
		}
		if (lMin > _2.first)
		{
			lMin = _2.first;
		}
		if (lMin > _2.second)
		{
			lMin = _2.second;
		}

		if (lMax < _1.second)
		{
			lMax = _1.second;
		}
		if (lMax < _2.first)
		{
			lMax = _2.first;
		}
		if (lMax < _2.second)
		{
			lMax = _2.second;
		}
		
		return std::pair<long, long>(lMin, lMax);
	}

	static std::string DayOfWeek(boost::gregorian::date _date)
	{
		if (_date.is_special())
		{
			return "δ����";
		}
		switch (_date.day_of_week())
		{
		case 0:
			return "������";
		case 1:
			return "����һ";
		case 2:
			return "���ڶ�";
		case 3:
			return "������";
		case 4:
			return "������";
		case 5:
			return "������";
		case 6:
			return "������";
		default:
			return "δ����";
		}
	}

	typedef enum 
	{
		enMorning	= 0,	//���ϣ�00:00-12:00
		enAfternoon = 1,	//���磬12:00-18:00
		enNight		= 2,	//���ϣ�18:00-24:00
		enInvalid	= 3,	//��Ч
	}EnumAreaOfDay;

	//�ж�һ��ʱ���������ĸ�ʱ�Ρ��������ʱ�����ĸ�ʱ�εĿ�����������
	static EnumAreaOfDay AreaOfDay(const std::pair<long, long>& area)
	{
		if (area.first >= area.second
			||
			area.second > 24*60)
		{
			return enInvalid;
		}
		//�ȼ�������ÿ������Ľ��������С��Ȼ��ȡ���ķ���
		int iInclude1, iInclude2, iInclude3;

		iInclude1 = (area.first > 720 || area.second < 0) ? 0 : min(area.second, 720) - max(area.first, 0);
		iInclude2 = (area.first > 1080 || area.second < 720) ? 0 : min(area.second, 1080) - max(area.first, 720);
		iInclude3 = (area.first > 1440 || area.second < 1080) ? 0 : min(area.second, 1440) - max(area.first, 1080);
		
		if (iInclude1 >= iInclude2
			&&
			iInclude1 >= iInclude3)
		{
			return enMorning;
		}

		if (iInclude2 >= iInclude1
			&&
			iInclude2 >= iInclude3)
		{
			return enAfternoon;
		}
		
		if (iInclude3 >= iInclude2
			&&
			iInclude3 >= iInclude1)
		{
			return enNight;
		}
		return enInvalid;
	}
};

#endif