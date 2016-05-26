#if !defined(__utility_epsvaluetype_h)
#define __utility_epsvaluetype_h

template <class T>
class epsvaluetype
{
public:
	static T EPSILON;
	typedef epsvaluetype<T> _MyValueType;

public:
	epsvaluetype() {}
	epsvaluetype(T value) : _value(value) {};

	inline _MyValueType& operator=(T value) {
		_value = value;
		return *this;
	}

	inline _MyValueType& operator+=(T value) {
		_value += value;
		return *this;
	}

	inline _MyValueType& operator+=(const _MyValueType& epsdbl) {
		_value += epsdbl._value;
		return *this;
	}

	inline bool operator==(T value) const {
		double delta = _value - value;
		return delta < EPSILON || -delta < EPSILON;
	}

	inline bool operator==(const _MyValueType& epsdbl) const {
		double delta = _value - epsdbl._value;
		return delta < EPSILON || EPSILON;
	}

	inline bool operator<(T value) const {
		return value - _value > EPSILON;
	}

	inline bool operator<(const _MyValueType& epsdbl) const {
		return epsdbl._value - _value > EPSILON;
	}

	inline bool operator>(T value) const {
		return _value - value > EPSILON;
	}

	inline bool operator>(const _MyValueType& epsdbl) const {
		return _value - epsdbl._value > EPSILON;
	}

	inline T value() {
		return _value;
	}

	inline T value() const {
		return _value;
	}

private:
	T _value;
};


#endif
