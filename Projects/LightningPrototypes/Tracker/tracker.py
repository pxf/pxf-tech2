#!/usr/bin/python2

import zmq,tracker_pb2,lightning,struct

def main():
    print("Tracker.")

    context = zmq.Context()
    socket = context.socket(zmq.REP)
    #socket.bind("tcp://" + lightning.tracker_address + ":" + lightning.tracker_port)
    socket.bind("tcp://*:" + lightning.tracker_port)
    
    trackerdatabase = TrackerDatabase()

    while True:
        next_id = 1

        #  Wait for next request from client
        message = socket.recv()
        print("Received message, parsing... ")

        # Determening message type
        message_type = struct.unpack('<I',message[:4])[0]

        if message_type == lightning.HELLO:
            print("HelloToTracker message:")
            hello = tracker_pb2.HelloToTracker()
            hello.ParseFromString(message)
            print(str(hello))

            sessionid = trackerdatabase.new_client(hello.address)
            response = tracker_pb2.HelloToClient()
            response.sessionid = sessionid
            socket.send(response.SerializeToString())
     
class TrackerDatabase:
    """Class containing the current connected nodes, connections within
    the network and a blacklist."""
    
    _clients = dict()
    _connections = dict()
    _blacklist = dict()

    _next_id = 1

    def __init__(self):
        pass

    def __del__(self):
        pass

    def next_id(self):
        """next_id() -> int sessionid.
        
        Find the next available session id for a client.
        """
        
        while self._next_id in self._clients.keys():
            if self._next_id >= pow(2,31):
                self._next_id = 1
                return self.next_id()
            self._next_id += 1

        return self._next_id

    def add_client(self, sessionid, address, available):
        """add_client(int sessionid, <ipy> address, int available) -> bool success.

        Adds a client to the client database. Throws AssertionError if 
        sessionid is already in use.
        """

        assert(sessionid not in self._clients.keys())
        
        self._clients[sessionid] = (address, available)

        return True

    def new_client(self, address, available):
        """new_client(<ipy> address, int available) -> int sessionid.

        Adds a new client to the client database, returning the sessionid.
        """

        sessionid = self.next_id()
        self.add_client(sessionid, address, available)    

        return sessionid


if __name__ == "__main__":
	main()

