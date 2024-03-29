#if !defined(__dataobject_DateType_h)
#define __dataobject_DateType_h
#include <exception>
#include <string>

struct DateType
{
	DateType() = default;
	DateType(const std::string& strDate)
	{
		auto len = strDate.length();
		if (len == 8)
		{
			Year = std::stoi(strDate.substr(0, 4));
			Month = std::stoi(strDate.substr(4, 2));
			Day = std::stoi(strDate.substr(6, 2));
		}
		else if (len == 10)
		{
			Year = std::stoi(strDate.substr(0, 4));
			Month = std::stoi(strDate.substr(5, 2));
			Day = std::stoi(strDate.substr(8, 2));
		}
		else
			throw std::exception("Wrong date format, it should be yyyymmdd or yyyy-mm-dd");
	}

	DateType(int yyyymmdd)
	{
		Year = yyyymmdd / 10000;
		Month = yyyymmdd / 100 % 100;
		Day = yyyymmdd % 100;
	}

	static bool YYYYMMDD2YYYY_MM_DD(const std::string& strDate, std::string& desDate)
	{
		bool ret = false;
		auto len = strDate.length();
		if (len == 8)
		{
			desDate = strDate.substr(0, 4) + "-" + strDate.substr(4, 2) + "-" + strDate.substr(6, 2);
			ret = true;
		}
		else if (len == 10)
		{
			desDate = strDate;
			ret = true;
		}
		
		return ret;
	}

	static bool YYYY_MM_DD2YYYYMMDD(const std::string& strDate, std::string& desDate)
	{
		bool ret = false;
		auto len = strDate.length();
		if (len == 10)
		{
			desDate = strDate.substr(0, 4) + strDate.substr(5, 2) + strDate.substr(8, 2);
			ret = true;
		}
		else if (len == 8)
		{
			desDate = strDate;
			ret = true;
		}

		return ret;
	}

	int YYYYMMDD()
	{
		return Year * 10000 + Month * 100 + Day;
	}

	void Normalize(std::string& strDate)
	{
		strDate = std::to_string(Year) + "-" + std::to_string(Month) + "-" + std::to_string(Day);
	}

	int Year = 1970;
	int Month = 1;
	int Day = 1;
};

#endif