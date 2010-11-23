#ifndef _CLIENT_H_
#define _CLIENT_H_

class Client
{
	public:
		int available;
		char *session_id;
		char *tracker_address;
		char *local_address;

		Client(const char *tracker_address, const char *local_address);
		~Client();

		int connect_tracker(const char *address);
	private:
		void *context;
		void *out_socket;
		void *in_socket;
	
};

#endif

