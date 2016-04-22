/* 
 * File:   data_commondef.h
 * Author: milk
 *
 * Created on 2014年10月5日, 上午1:44
 */

#ifndef __data_COMMONDEF_H
#define	__data_COMMONDEF_H
#include <memory>
#include <vector>

typedef std::shared_ptr<void> shared_void_ptr;
typedef shared_void_ptr Buffer_Ptr;
typedef shared_void_ptr Attrib_Ptr;
typedef std::vector<const void*> ParamVector;
typedef std::shared_ptr<unsigned char> byte_ptr;

#endif	/* data_commondef.h */

