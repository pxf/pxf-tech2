#ifndef _BLOCKINGDEQUE_H_
#define _BLOCKINGDEQUE_H_

#include <deque>
#include <zthread/Guard.h>
#include <zthread/FastMutex.h>

template <typename T>
class BlockingDeque : public std::deque<T>
{
private:
	ZThread::FastMutex* m_Lock;

public:
	BlockingDeque()
	{
		//m_Lock = new ZThread::FastMutex();
	}

	virtual ~BlockingDeque()
	{
		//delete m_Lock;
	}

	BlockingDeque(const T& crap)
	{
	}
	
	bool TryLock()
	{
		return m_Lock->tryAcquire(0);
	}

	void Lock()
	{
		m_Lock->acquire();
	}

	void Unlock()
	{
		m_Lock->release();
	}
};

#endif /* _BLOCKINGDEQUE_H_ */
