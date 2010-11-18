#!/usr/bin/python2

import struct
import traceback

import zmq

import lightning
import tracker_pb2

class Tracker():
    """Class containing logic and main-loop for the tracker."""

    _context = None
    _sck_out = None
    _sck_in = None

    _db = None

    _tr_table = dict()

    def __init__(self):
        self._context = zmq.Context()
        self._sck_in = self._context.socket(zmq.REP)
        try:
            self._sck_in.bind("tcp://{0}:{1}".format(lightning.tracker_address
                                , lightning.tracker_port))
        except Exception as e:
            print("Unable to bind socket for incoming (sending) connections.")
            traceback.print_exc()
            return
        self._sck_out = self._context.socket(zmq.REQ)
        self._sck_out.setsockopt(zmq.IDENTITY, "0")

        self._db = TrackerDatabase()
    
    # Events:
    # args: message - parsed protobuf, or None
    # returns: either string or a tuple with
    #           (int message_type, <protobuf> response)
    def e_ping(self, message):
        response = tracker_pb2.Pong()
        response.ping_data = message.ping_data

        return lightning.pack(lightning.PONG, response)
    _tr_table[lightning.PING] = e_ping

# TODO: Do it.
#    def e_pong(self, message):
#        return lightning.pack(lightning.OK)
#    _tr_table[lightning.PONG] = e_pong

    def e_init_hello(self, message):
        new_session_id = self._db.new_init_client()

        response = tracker_pb2.HelloToClient()
        response.session_id = new_session_id
        return lightning.pack(lightning.HELLO_TO_CLIENT, response)
    _tr_table[lightning.INIT_HELLO] = e_init_hello

    def e_hello_to_tracker(self, message):
        self._db.set_client(
            message.session_id
            , address = message.address
            , available = message.available
        )

        self._sck_out.connect(message.address)
        return lightning.pack(lightning.OK)
    _tr_table[lightning.HELLO_TO_TRACKER] = e_hello_to_tracker

    def e_goodbye(self, message):
        self._db.del_client(message.session_id)
        return lightning.pack(lightning.OK)
    _tr_table[lightning.GOODBYE] = e_goodbye

    def run(self):
        """run() -> nothing.
        
        Main loop for Tracker.
        """

        print("Tracker.")

        while True:
            data = self._sck_in.recv()

            message_type, message = lightning.unpack(data)

            print("Got message: {0}/{1}".format(message_type, message))
            if message_type not in self._tr_table:
                # TODO: Should be handled here instead.
                self._sck_in.send(lightning.pack(lightning.OK)) # TEMPORARY
                print("Unhandled message. Sent OK back.")
                continue

            try:
                ret = self._tr_table[message_type](self, message)
                if type(ret) == type(str()):
                    self._sck_in.send(ret)
                elif type(ret) == type(list()) and len(ret) == 2:
                    self._sck_in.send(lightning.pack[ret[0], ret[1]])
                else:
                    print("Function {0} returned invalid data: {1}:\"{2}\"".format(
                        self._tr_table[message_type]
                        , type(ret)
                        , ret
                    ))
                    self._sck_in.send(lightning.pack(lightning.OK))
            except Exception as e:
                print("Function {0} raised an exception:")
                traceback.print_exc()


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
        self.add_client(session_id, "", 0)

        return session_id

    def new_client(self, address, available):
        """new_client(str address, int available) -> int session_id.

        Adds a new client to the client database, returning the new session_id.
        """

        session_id = str(self.next_id())
        self.add_client(session_id, address, available)    

        return session_id

def main():
    tracker = Tracker()
    tracker.run()


if __name__ == "__main__":
	main()

