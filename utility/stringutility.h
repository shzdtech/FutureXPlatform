#if !defined(__utility_stringutility_h)
#define __utility_stringutility_h

#include <string>

static const char DEFAULT_DEMILITER = ':';

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
			for (size_t i = 0;;i++)
			{
				TChar c1 = str1[i];
				TChar c2 = str2[i];
				if (c1 >= 'A' && c1 <= 'Z')
					c1 += 32;
				if (c2 >= 'A' && c2 <= 'Z')
					c2 += 32;
				ret = c1 - c2;

				if (ret || !c1 || !c2)
					break;
			}
		}

		return ret;
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
};

#endif