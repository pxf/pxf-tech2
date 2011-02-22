#ifndef _BLOCKINGARRAY_H_
#define _BLOCKINGARRAY_H_

#include <deque>
#include <map>

#include <zthread/Guard.h>
#include <zthread/Condition.h>
#include <zthread/FastMutex.h>

#include <Pxf/Base/Debug.h>

template <typename T>
class BlockingTaskQueue
{
public:
	struct Entry_t
	{
		unsigned int type;
		T value;
		Entry_t(unsigned int _Type, T _Value)
		{
			type = _Type;
			value = _Value;
		}
	};

protected:
	std::map<unsigned int, ZThread::Condition*> m_Conditions;
	std::deque<Entry_t> m_InternalArray;
	ZThread::FastMutex m_Lock;
	int m_capacity;
	bool m_Canceled;

	int count(unsigned int _Type)
	{
		int c = 0;
		for (int i = 0; i < m_InternalArray.size(); i++)
		{
			if (m_InternalArray[i].type == _Type)
				c++;
		}
		return c;
	}

public:
	BlockingTaskQueue()
		: m_Canceled(false), m_capacity(10)
	{
	
	}

	BlockingTaskQueue(const T& crap)
	{
		// blah blah blah
		PXF_ASSERT(0, "Not implemented.");
	}

	~BlockingTaskQueue()
	{}

	bool register_type(unsigned int _Type)
	{
		ZThread::Guard<ZThread::FastMutex> g(m_Lock);
		if (m_Conditions.find(_Type) == m_Conditions.end())
			m_Conditions[_Type] = new ZThread::Condition(m_Lock);
		return true;
	}

	bool push(unsigned int _Type, const T _Task)
	{
		ZThread::Guard<ZThread::FastMutex> g(m_Lock);
		if (get_available())
		{
			Entry_t e(_Type, _Task);
			m_InternalArray.push_back(e);

			if (m_Conditions.find(_Type) == m_Conditions.end())
				register_type(_Type);

			m_Conditions[_Type]->signal();

			return true;
		}

		return false;
	}

	int get_available()
	{
		return (m_capacity - m_InternalArray.size());
	}

	void set_capacity(int _c)
	{
		m_capacity = _c;
	}

	T pop(unsigned int _Type)
	{
		ZThread::Guard<ZThread::FastMutex> g(m_Lock);

		if (m_Conditions.find(_Type) == m_Conditions.end())
			register_type(_Type);

		while(count(_Type) == 0 && !m_Canceled)
			m_Conditions[_Type]->wait();

		if (m_Canceled)
			throw ZThread::Cancellation_Exception();

		T ret = 0;
		typename std::deque<Entry_t>::iterator iter = m_InternalArray.begin();
		for(; iter != m_InternalArray.end(); ++iter)
		{
			if (iter->type == _Type)
			{
				ret = iter->value;
				m_InternalArray.erase(iter);
				break;
			}
		}
		
		return ret;
	}

	void acquire()
	{
		m_Lock.acquire();
	}

	void release()
	{
		m_Lock.release();
	}

	std::deque<Entry_t>& get_array()
	{
		return m_InternalArray;
	}

	void cancel()
	{
		m_Canceled = true;
		typename std::map<unsigned int, ZThread::Condition*>::iterator iter = m_Conditions.begin();
		for(; iter != m_Conditions.end(); ++iter)
			iter->second->broadcast();
	}

};

#endif // _BLOCKINGARRAY_H_
