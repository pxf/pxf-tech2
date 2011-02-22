#ifndef _BLOCKINGDEQUE_H_
#define _BLOCKINGDEQUE_H_

#include <deque>
#include <zthread/Guard.h>
#include <zthread/FastMutex.h>

template <typename T>
class BlockingDeque : public std::deque<T>
{
public:
	BlockingDeque();
	virtual ~BlockingDeque();
	
	void Lock();
	void Unlock();

private:
	ZThread::FastMutex m_Lock;
};

#endif /* _BLOCKINGDEQUE_H_ */
