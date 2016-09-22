/***********************************************************************
 * Module:  data_buffer.h
 * Author:  milk
 * Modified: 2014年10月6日 12:49:50
 * Purpose: Declaration of the class data_buffer
 ***********************************************************************/

#if !defined(__dataobject_data_buffer_h)
#define __dataobject_data_buffer_h

#include "../common/typedefs.h"

class buffer_ptr : public std::shared_ptr<byte>
{
public:
	buffer_ptr() = default;
	buffer_ptr(byte* buffer) : std::shared_ptr<byte>(buffer, std::default_delete<byte[]>()) {}
};

class data_buffer : public buffer_ptr
{
public:
	data_buffer() = default;
	data_buffer(const buffer_ptr& buf, std::size_t bufsz) : buffer_ptr(buf), _buffer_size(bufsz) {}
	data_buffer(byte* buf, std::size_t bufsz) : buffer_ptr(buf), _buffer_size(bufsz) {}
	std::size_t size() const { return _buffer_size; }

private:
	std::size_t _buffer_size;
};

#endif