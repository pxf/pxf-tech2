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

            session_id = trackerdatabase.new_client(hello.address, hello.available)
            print("Adding client to database, session_id = {0}.".format(session_id))
            response = tracker_pb2.HelloToClient()
            response.session_id = session_id
            socket.send(response.SerializeToString())
        elif message_type == lightning.GOODBYE:
            print("GoodBye message:")
            goodbye = tracker_pb2.GoodBye()
            goodbye.ParseFromString(message)
            print(str(goodbye))
            print("Removing client with session_id {0} from database.".format(goodbye.session_id))
            trackerdatabase.del_client(goodbye.session_id)


            
     
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
        """next_id() -> int session_id.
        
        Find the next available session id for a client.
        """
        
        while self._next_id in self._clients.keys():
            if self._next_id >= pow(2,31):
                self._next_id = 1
                return self.next_id()
            self._next_id += 1

        return self._next_id

    def add_client(self, session_id, address, available):
        """add_client(int session_id, <ipy> address, int available) -> bool success.

        Adds a client to the client database. Throws AssertionError if 
        session_id is already in use.
        """

        assert(session_id not in self._clients.keys())
        
        self._clients[session_id] = (address, available)

        return True

    def del_client(self, session_id):
        """del_client(int session_id) -> bool succes

        Removes the client with the corresponding session_id.
        """

        if session_id in self._clients.keys():
            del self._clients[session_id]
            return True
        else:
            return False

    def new_client(self, address, available):
        """new_client(<ipy> address, int available) -> int session_id.

        Adds a new client to the client database, returning the new session_id.
        """

        session_id = self.next_id()
        self.add_client(session_id, address, available)    

        return session_id


if __name__ == "__main__":
	main()

