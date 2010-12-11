#ifndef _STATE_H_
#define _STATE_H_

#include <deque>

enum ClientState {
	WOK, // Why? HELLO
	WALLOC
};

//enum WaitFor {
//	HELLO
//};

struct client_state
{
	ClientState state;
	//WaitFor	waitfor;
};

class State
{
	public:
		Pxf::Util::Map<Connection*, client_state*> m_States;
		Pxf::Util::Array<Connection*> m_Allocatees;
		Pxf::Util::Array<Connection*> m_Allocated;
		std::deque<client::Tasks*> m_OutQueue;
};

#endif

