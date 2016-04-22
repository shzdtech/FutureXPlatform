/***********************************************************************
 * Module:  data_buffer.h
 * Author:  milk
 * Modified: 2014年10月6日 12:49:50
 * Purpose: Declaration of the class data_buffer
 ***********************************************************************/

#if !defined(__message_data_buffer_h)
#define __message_data_buffer_h

#include "data_commondef.h"

class data_buffer
{
public:
   data_buffer(Buffer_Ptr buf, std::size_t bufsz):_buffer_ptr(buf),_buffer_size(bufsz){}
   data_buffer(void* buf, std::size_t bufsz) :_buffer_ptr(buf), _buffer_size(bufsz){}
   Buffer_Ptr get_buffer() const {return _buffer_ptr;}
   void* get() const {return _buffer_ptr.get();}
   int size() const {return _buffer_size;}
   operator bool() const
    { return _buffer_size > 0 && _buffer_ptr; }
   
private:
   Buffer_Ptr _buffer_ptr;
   std::size_t _buffer_size;
};

#endif