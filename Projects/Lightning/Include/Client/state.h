#ifndef _STATE_H_
#define _STATE_H_

enum ClientState {
	WOK,
	WALLOC
};

struct client_state
{
	ClientState state;
};

class State
{
	public:
		Pxf::Util::Map<Connection*, client_state*> m_States;
		Pxf::Util::Array<Connection*> m_Allocatees;
		Pxf::Util::Array<Connection*> m_Allocated;
};

#endif

