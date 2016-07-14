#ifndef _singleton_templ_mt_h
#define _singleton_templ_mt_h

#include <memory>
#include <mutex>

template <typename T>
class singleton_mt_ptr
{
public:
	static std::shared_ptr<T>& Instance() {
		if (!_instance) {
			std::call_once(_instance_flag, []{
				_instance.reset(new T);
				_instance->InitializeInstance();
			});
		}
		return _instance;
	}

	virtual void InitializeInstance() = 0;

protected:
	singleton_mt_ptr(){}

private:
	static std::once_flag _instance_flag;
	static std::shared_ptr<T> _instance;
};

#endif