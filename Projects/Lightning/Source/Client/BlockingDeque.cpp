#include "BlockingDeque.h"

template <typename T>
void BlockingDeque<T>::Lock()
{
	m_Lock.acquire();
}

template <typename T>
void BlockingDeque<T>::Unlock()
{
	m_Lock.release();
}