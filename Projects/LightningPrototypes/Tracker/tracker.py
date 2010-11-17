#!/usr/bin/python2

import struct

import zmq

import lightning
import tracker_pb2

def main():
    print("Tracker.")

    context = zmq.Context()
    socket = context.socket(zmq.REP)
    #socket.bind("tcp://" + lightning.tracker_address + ":" + lightning.tracker_port)
    socket.bind("tcp://*:" + lightning.tracker_port)

    zmq_socket_clients = context.socket(zmq.REQ)
    zmq_socket_clients.setsockopt(zmq.IDENTITY, "0")
    
    tracker_database = TrackerDatabase()

    while True:
        #  Wait for next request from client
        message = socket.recv()
        print("Received message, parsing... ")

        # Determening message type
        #message_type = struct.unpack('<I',message[:4])[0]
        message_type, data = lightning.unpack(message)

        if message_type == lightning.INIT_HELLO:
            print("InitHelloToTracker.")
            session_id = tracker_database.new_init_client()

            response = tracker_pb2.HelloToClient()
            response.session_id = session_id
            socket.send(struct.pack('<I', lightning.HELLO_TO_CLIENT)+response.SerializeToString())

        elif message_type == lightning.PING:
            print("PING.")

            response = tracker_pb.Pong()
            response.ping_data = data.ping_data

            socket.send(struct.pack('<I', lightning.PONG)+response.SerializeToString())

        elif message_type == lightning.HELLO_TO_TRACKER:
            print("HelloToTracker.")
            hello = tracker_pb2.HelloToTracker()
            hello.ParseFromString(message[4:])
            print("hello: "+str(hello))

            tracker_database.set_client(
                session_id
                , address=hello.address
                , available=hello.available
            )

            zmq_socket_clients.connect(hello.address)
            socket.send(struct.pack('<I', lightning.OK))

        elif message_type == lightning.GOODBYE:
            print("GoodBye message:")
            goodbye = tracker_pb2.GoodBye()
            goodbye.ParseFromString(message)
            print(str(goodbye))
            print("Removing client with session_id {0} from database.".format(goodbye.session_id))
            trackerdatabase.del_client(goodbye.session_id)


class TrackerDatabaseException(Exception):
    """Exception in TrackerDatabase."""
    pass
     
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

    def get_client(self, session_id):
        """get_client(int session_id) -> (address, available)."""
        
        if session_id not in self._clients:
            raise TrackerDatabaseException(
                "Unable to find tracker with session_id {0}.".format(session_id)
            )

        return self._clients[session_id]

    def set_client(self, session_id, address=None, available=None):
        """set_client(session_id, address=None, available=None) -> Nothing.
        
        Sets whatever argument is specified, None means untouched.
        """
        
        if address is None and available is None:
            raise TrackerDatabaseException("Can't set nothing.")

        if session_id not in self._clients:
            raise TrackerDatabaseException(
                "Unable to find tracker with session_id {0}.".format(session_id)
            )

        caddress, cavailable = self._clients[session_id]
        self._clients[session_id] = (
            caddress if address is None else address
            , cavailable if available is None else available
        )

    def next_id(self):
        """next_id() -> int session_id.
        
        Find the next available session id for a client.
        """
        
        while str(self._next_id) in self._clients:
            if self._next_id >= pow(2,31):
                self._next_id = 1
                return self.next_id()
            self._next_id += 1

        return self._next_id

    def add_client(self, session_id, address, available):
        """add_client(int session_id, str address, int available) -> bool success.

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

    def new_init_client(self):
        """new_init_client() -> int session_id.

        Adds a new init_client to the client database, returning the new session_id.
        """

        session_id = str(self.next_id())
        print("add_client " + str(session_id))
        self.add_client(session_id, "", 0)

        return session_id

    def new_client(self, address, available):
        """new_client(str address, int available) -> int session_id.

        Adds a new client to the client database, returning the new session_id.
        """

        session_id = str(self.next_id())
        self.add_client(session_id, address, available)    

        return session_id


if __name__ == "__main__":
	main()

