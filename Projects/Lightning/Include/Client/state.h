#ifndef _STATE_H_
#define _STATE_H_

#include <BlockingDeque.h>

enum ClientState {
	WOK = 2,
	WALLOC = 4,
	W_HELLO = 8
};

struct client_state
{
	ClientState state;
	int send_tasks;
	Batch* batch;
};

class State
{
	public:
		Pxf::Util::Map<Connection*, client_state*> m_States;
		Pxf::Util::Array<Connection*> m_Clients;
		BlockingDeque* m_OutQueue; // TODO: Change this to blockingdeque!
		
		State() { m_OutQueue = new BlockingDeque(); }
};

#endif

