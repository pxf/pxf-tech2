#ifndef _BLOCKINGARRAY_H_
#define _BLOCKINGARRAY_H_

#include <vector>
#include <map>

#include <zthread/Guard.h>
#include <zthread/Condition.h>
#include <zthread/Mutex.h>

template <typename T>
class BlockingTaskQueue
{
protected:
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

	std::map<unsigned int, ZThread::Condition*> m_Conditions;
	std::vector<Entry_t> m_InternalArray;
	ZThread::Mutex m_Lock;
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
		: m_Canceled(false)
	{
	
	}

	~BlockingTaskQueue()
	{}

	bool register_type(unsigned int _Type)
	{
		ZThread::Guard<ZThread::Mutex> g(m_Lock);
		if (m_Conditions.find(_Type) == m_Conditions.end())
			m_Conditions[_Type] = new ZThread::Condition(m_Lock);
		return true;
	}

	void push(unsigned int _Type, const T _Task)
	{
		ZThread::Guard<ZThread::Mutex> g(m_Lock);
		Entry_t e(_Type, _Task);
		m_InternalArray.push_back(e);
		m_Conditions[_Type]->signal();
	}

	T pop(unsigned int _Type)
	{
		ZThread::Guard<ZThread::Mutex> g(m_Lock);
		while(count(_Type) == 0 && !m_Canceled)
			m_Conditions[_Type]->wait();

		if (m_Canceled)
			throw ZThread::Cancellation_Exception();

		T ret = 0;
		typename std::vector<Entry_t>::iterator iter = m_InternalArray.begin();
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

	void cancel()
	{
		m_Canceled = true;
		typename std::map<unsigned int, ZThread::Condition*>::iterator iter = m_Conditions.begin();
		for(; iter != m_Conditions.end(); ++iter)
			iter->second->broadcast();
	}

};

#endif // _BLOCKINGARRAY_H_