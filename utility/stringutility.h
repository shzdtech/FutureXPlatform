#if !defined(__utility_stringutility_h)
#define __utility_stringutility_h

#include <string>
#include <vector>

static const char DEFAULT_DEMILITER = ';';

class stringutility
{
public:
	template<class TChar>
	inline static int compare(const TChar* str1, const TChar* str2)
	{
		int ret;

		if (!str1 || !str2)
			ret = str1 - str2;
		else
		{
			for (size_t i = 0; ; i++)
			{
				TChar c1 = str1[i];
				TChar c2 = str2[i];
				if (c1 >= 'a' && c1 <= 'z')
					c1 -= 32;
				if (c2 >= 'a' && c2 <= 'z')
					c2 -= 32;
				ret = c1 - c2;

				if (ret || !c1 || !c2)
					break;
			}
		}

		return ret;
	}

	template<class TChar>
	inline static bool startwith(const TChar* str, const TChar* head)
	{
		if (!str || !head)
			return false;

		for (size_t i = 0; ; i++)
		{
			TChar c1 = str[i];
			TChar c2 = head[i];
			if (c1 >= 'a' && c1 <= 'z')
				c1 -= 32;
			if (c2 >= 'a' && c2 <= 'z')
				c2 -= 32;

			if (c2 != c1)
			{
				if (!c2)
					return true;
				else
					return false;
			}

			if (!c2 || !c1)
				break;
		}

		return true;
	}

	inline static int compare(const std::string& str1, const std::string& str2)
	{
		return compare(str1.data(), str2.data());
	}

	inline static size_t split(const std::string& input, std::string& first, std::string& second, const char demiliter = DEFAULT_DEMILITER)
	{
		auto pos = input.find(demiliter);
		if (pos != std::string::npos)
		{
			first = input.substr(0, pos);
			second = input.substr(pos + 1);
		}
		else
		{
			first = input;
			second.clear();
		}

		return pos;
	}

	static void split(const std::string& input, std::vector<std::string>& splitVec, const char demiliter = DEFAULT_DEMILITER)
	{
		std::string first, remain;
		size_t pos = stringutility::split(input, first, remain);
		splitVec.push_back(std::move(first));

		while (pos != std::string::npos)
		{
			pos = stringutility::split(remain, first, remain);
			splitVec.push_back(std::move(first));
		}
	}
};

#endif