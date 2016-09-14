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
			Year = std::stoi(strDate.substr(0, 4), nullptr, 0);
			Month = std::stoi(strDate.substr(4, 2), nullptr, 0);
			Day = std::stoi(strDate.substr(6, 2), nullptr, 0);
		}
		else if (len == 10)
		{
			Year = std::stoi(strDate.substr(0, 4), nullptr, 0);
			Month = std::stoi(strDate.substr(5, 2), nullptr, 0);
			Day = std::stoi(strDate.substr(8, 2), nullptr, 0);
		}
		else
			throw std::exception("Wrong date format, it should be yyyymmdd or yyyy-mm-dd");
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

	void Normalize(std::string& strDate)
	{
		strDate = std::to_string(Year) + "-" + std::to_string(Month) + "-" + std::to_string(Day);
	}

	int Year;
	int Month;
	int Day;
};

#endif