/***********************************************************************
 * Module:  KeyAttributeDO.h
 * Author:  milk
 * Modified: 2014年10月8日 13:30:06
 * Purpose: Declaration of the class KeyAttributeDO
 ***********************************************************************/

#if !defined(__message_KeyAttributeDO_h)
#define __message_KeyAttributeDO_h

#include <memory>
#include <vector>
#include <map>
#include <set>
#include "AbstractDataObj.h"
#include "data_buffer.h"
#include "MultiRecordDO.h"

template <typename T>
class VectorDO : public std::vector<T>, public AbstractDataObj
{};
template <typename T> using VectorDO_Ptr = typename std::shared_ptr<VectorDO<T>>;

template <typename T>
class SetDO : public std::set<T>, public AbstractDataObj
{};
template <typename T> using SetDO_Ptr = typename std::shared_ptr<SetDO<T>>;


template <typename T>
class TemplateDO : public AbstractDataObj
{
public:
	TemplateDO(){};
	TemplateDO(const T& data) :Data(data){};
	T Data;
};

typedef TemplateDO<data_buffer> DataBufferDO;
typedef TemplateDO<MapIntVector> IntTableDO;
typedef TemplateDO<MapDoubleVector> DoubleTableDO;
typedef TemplateDO<MapStringVector> StringTableDO;

template<class T>
class TMultiRecordDO : public MultiRecordDO
{
public:
	T Data;

protected:
};


#endif