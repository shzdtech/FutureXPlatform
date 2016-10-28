/***********************************************************************
 * Module:  data_buffer.h
 * Author:  milk
 * Modified: 2014年10月6日 12:49:50
 * Purpose: Declaration of the class data_buffer
 ***********************************************************************/

#if !defined(__dataobject_data_buffer_h)
#define __dataobject_data_buffer_h

#include "../common/typedefs.h"

struct data_buffer : std::shared_ptr<byte>
{
public:
	data_buffer() {}
	data_buffer(byte* buf, std::size_t bufsz) : std::shared_ptr<byte>(buf, std::default_delete<byte[]>()), _buffer_size(bufsz) {}
	std::size_t size() const { return _buffer_size; }
	void resize(std::size_t newSz) const { _buffer_size = newSz; }

private:
	mutable std::size_t _buffer_size;
};

#endif