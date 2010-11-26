#ifndef _BLOCKINGARRAY_H_
#define _BLOCKINGARRAY_H_
#include <vector>

#include <zthread/Guard.h>
#include <zthread/Condition.h>
#include <zthread/FastMutex.h>

template <typename T>
class BlockingArray
{
protected:
	struct Entry_t
	{
		unsigned type;
		T& value;
	};

	FastMutex m_Lock;
	Condition m_NotEmpty;
	std::vector<T> m_InternalArray;
public:
	BlockingArray()
		: m_NotEmpty(m_Lock)
	{
	
	}

	~BlockingArray()
	{}

	T Get(unsigned int _Index)
	{
		ZThread::Guard<FastMutex> g(m_Lock);
		return m_InternalArray.at(_Index);
	}

};

#endif // _BLOCKINGARRAY_H_