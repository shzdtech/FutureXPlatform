#if !defined(__utility_epsdouble_h)
#define __utility_epsdouble_h

template <unsigned int precision = 8>
class epsdouble_t
{
public:
	typedef epsdouble_t<precision> _myespdouble;

public:
	epsdouble_t() {}
	epsdouble_t(double value) : _value(value) {};

	static constexpr uint64_t ipow(uint64_t num, unsigned int pow)
	{
		return pow == 0 ? 1 : num * ipow(num, pow - 1);
	}

	static constexpr double EPSILON = 1.0 / ipow(10, precision);

	inline _myespdouble& operator=(double value) {
		_value = value;
		return *this;
	}

	inline _myespdouble& operator+=(double value) {
		_value += value;
		return *this;
	}

	inline _myespdouble& operator+=(const _myespdouble& epsdbl) {
		_value += epsdbl._value;
		return *this;
	}

	inline bool operator==(double value) const {
		double delta = _value - value;
		return delta < EPSILON || -delta < EPSILON;
	}

	inline bool operator==(const _myespdouble& epsdbl) const {
		double delta = _value - epsdbl._value;
		return delta < EPSILON || EPSILON;
	}

	inline bool operator<(double value) const {
		return value - _value > EPSILON;
	}

	inline bool operator<(const _myespdouble& epsdbl) const {
		return epsdbl._value - _value > EPSILON;
	}

	inline bool operator>(double value) const {
		return _value - value > EPSILON;
	}

	inline bool operator>(const _myespdouble& epsdbl) const {
		return _value - epsdbl._value > EPSILON;
	}

	inline double value() {
		return _value;
	}

	inline double value() const {
		return _value;
	}

private:
	double _value;
};

typedef epsdouble_t<> epsdouble;

#endif
