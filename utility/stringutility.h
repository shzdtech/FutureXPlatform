#if !defined(__utility_stringutility_h)
#define __utility_stringutility_h

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
};

#endif