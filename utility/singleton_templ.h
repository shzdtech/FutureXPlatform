#ifndef _singleton_templ_h
#define _singleton_templ_h

#include <memory>

template <typename T>
class singleton_ptr
{
public:
	static std::shared_ptr<T>& Instance() {
		static std::shared_ptr<T> instace(new T);
		return instace;
	}

protected:
	singleton_ptr(){}
};

#endif