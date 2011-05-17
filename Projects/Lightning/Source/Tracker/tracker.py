#!/usr/bin/python2

import struct
import traceback
import sys

import socket
import select

import lightning
import tracker_pb2, lightning_pb2

class Tracker():
    """Class containing logic and main-loop for the tracker."""

    _sck_listen = None
    _scks = {} # {socket: {client}}
        # client: {buffer: }

    _db = None

    _tr_table = dict()

    _last_session_id = None
    _last_socket = None

    def __init__(self, address="127.0.0.1", port=50000):
        print("Tracker binding to {0}:{1}.".format(address, port))
        self._sck_listen = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._sck_listen.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self._sck_listen.bind((address, port))
        self._sck_listen.listen(10)

        self._db = TrackerDatabase()

    
    # --------------------------------------------------------------
    # Events: ------------------------------------------------------
    # args: message - parsed protobuf, or None
    # returns: either string, int or a tuple with
    #           (int message_type, <protobuf> response)
    def e_no_handle(self, message):
        return None
    _tr_table[lightning.PONG] = e_no_handle
    _tr_table[lightning.OK] = e_no_handle

    def e_ping(self, message):
        response = lightning_pb2.Pong()
        response.ping_data = message.ping_data
        return lightning.PONG, response
    _tr_table[lightning.PING] = e_ping
    
    def e_init_hello(self, message):
        new_session_id = self._db.new_init_client()
        print("new client id: {0}".format(new_session_id))
        response = tracker_pb2.HelloToClient()
        response.session_id = new_session_id
        self.set_session_id(None, new_session_id)
        return lightning.T_HELLO_CLIENT, response
    _tr_table[lightning.T_INIT] = e_init_hello

    def e_hello_to_tracker(self, message):
        # Delete the old connection.
        # TODO: Wait until we've successfully connected the new socket?
        #del self._scks[self._last_socket]
        self._db.del_client(self._last_session_id)
        # Connect to the client.
        family = socket.getaddrinfo(message.address, int(message.port))[0][0]
        c = socket.socket(socket.AF_INET if family == 2 else socket.AF_INET6)
        try:
            print("trying to connect to {0}:{1}".format(message.address, message.port))
            c.connect((message.address, message.port))
        except socket.error as e:
            # Couldn't connect. Ignore.
            print("unable to connect to client in return:".format(str(e)))
            # TODO: Raise an Exception?
            return None
        print("connected to client {0}:{1} on {2}.".format(message.address, message.port, c))
        self._scks[c] = dict([
            ('buffer', '')
            , ('pkt-length', 0)
            , ('session_id', message.session_id)
        ])
        self._db.add_client(message.session_id
            , message.address + ":" + str(message.client_port), message.available)
        return None
    _tr_table[lightning.T_HELLO_TRACKER] = e_hello_to_tracker

    def e_goodbye(self, message):
        self._db.del_client(message.session_id)
        return lightning.OK
    _tr_table[lightning.GOODBYE] = e_goodbye
    
    def e_nodesrequest(self, message):
        avail_cs = self._db.get_available_clients()
        possible = self._get_possible_nodes(self._last_session_id)
        wants = message.nodes
        print("client {0} wants {1} nodes.  available: {2} / possible: {3}.".format(
            self._last_session_id
            , wants
            , avail_cs
            , possible
        ))
        if possible == []:
            self._db.set_client_waiting(
                    message.session_id
                    , message.nodes
                )
            print("client couldn't get any, put on the waitlist.")
        response = tracker_pb2.NodesResponse()
        for session_id in possible:
            node = response.nodes.add()
            node.session_id = session_id
            client = self._db.get_client(session_id)
            node.address, port = client[0].split(':')
            node.port = int(port)
        return lightning.T_NODES_RESPONSE, response
    _tr_table[lightning.T_NODES_REQUEST] = e_nodesrequest
    
    def e_nodeavailable(self, message):
        # Set it available.
        self._db.set_client(message.session_id, available=message.available)
        # Send it to ALL NOODEEES.
        response = tracker_pb2.NodesResponse()
        node = response.nodes.add()
        node.session_id = message.session_id
        client = self._db.get_client(message.session_id)
        node.address, port = client[0].split(':')
        node.port = int(port)
        for c in self._db._clients.keys():
            self.send((lightning.T_NODES_RESPONSE, response), c)

        # Send it to the waiting nodes.
        new_wait = {}
        for node_sess, wait in self._db.get_waiting_clients():
            possible = self._get_possible_nodes(node_sess)
            if possible == []:
                # None avaialable.
                new_wait[node_sess] = wait
                continue
            response = tracker_pb2.NodesResponse()
            for session_id in possible:
                node = response.nodes.add()
                node.session_id = message.session_id
                client = self._db.get_client(message.session_id)
                node.address, port = client[0].split(':')
                node.port = int(port)
            self.send((lightning.T_NODES_RESPONSE, response), node_sess)
        self._db.waitlist = new_wait
        return
    _tr_table[lightning.T_NODE_AVAILABLE] = e_nodeavailable

    def e_nodeconnection(self, message):
        self._db.connect_clients(message.session_id, message.connected_to_id)
        #return lightning.OK # TODO: Should send an OK here?
        return
    _tr_table[lightning.T_NODE_CONNECTION] = e_nodeconnection

    # Events end.
    # --------------------------------------------------------------

    def _get_possible_nodes(self, _session_id):
        """_get_possible_nodes(session_id) -> [int nodes session id]."""
        # TODO: Respect num_avail.
        avail = self._db.get_available_clients()
        connections = []
        possible = []
        for k,v in self._db._connections.items():
            if k == _session_id:
                connections.append(v)
            if v == _session_id:
                connections.append(k)
        for session_id, num_avail in avail:
            if session_id in connections or session_id == _session_id:
                # Node already connected to this node.
                # OR it IS the node.
                continue
            else:
                possible.append(session_id)
        return possible

    # TODO: Make argument client optional.
    def set_session_id(self, client, new_id):
        """set_session_id(str/sck/none client, str new_id) -> bool success.
        
        Sets the session id for client to new_id, where client
        can be one of the following:
            str   session_id
            sck   client socket
            None  takes the client who sent the last packet
        """

        print("set_session_id from {0} to {1}.".format(client,  new_id))

        if type(client) == type(str()):
            # client = old_id
            old_id = self._scks[client]['session_id']
            sck = client
        elif client in self._scks:
            # client = sck
            old_id = client
            for s, si in self._sck.items():
                if si['session_id'] == old_id:
                    sck = s
        elif client is None:
            # client = last used.
            old_id = self._last_session_id
            sck = self._last_socket
        else:
            # none.
            return False

        if self._last_session_id == old_id:
            self._last_session_id = new_id
        
        self._scks[sck]['session_id'] = new_id

        return True

    def send(self, data, session_id=None):
        """send(str/int/protobuf data, int session_id=None) -> bool success.  

        Translates the data into protobuf-data with an enum at the beginning
        (if it's not already in that format), and sends it to the client
        with session id session_id. If no session_id is specified, the last
        client that sent data will be the receiver.
        
        Takes either an enum (int), a string (already in final format),
                or a tuple with enum and protobuf.
        Returns True on success, otherwise False.
        """

        if session_id is None:
            session_id = self._last_session_id

        # Find the client.
        client = None
        for sck, info in self._scks.items():
            if info['session_id'] == session_id:
                client = sck

        if client is None:
            # Couldn't find the client.
            print("unable to find client.")
            return False

        if type(data) == type(str()):
            msg = data
        elif type(data) == type(tuple()) and len(data) == 2:
            msg = lightning.pack(data[0], data[1])
        elif type(data) == type(int()):
            msg = lightning.pack(data)
        else:
            # Unknown format.
            print("unknown format.")
            return False

        # Add the length of the packet.
        msg = "{0}{1}".format(struct.pack('<I', len(msg)), msg)
        l = len(msg)

        tries = 20
        while msg != "":
            msg = msg[client.send(msg):]

            # To not end up in an infinite loop.
            tries -= 1
            if tries == 0:
                raise LightningException("Tried too many times.")

        print("successfully sent {1} bytes to socket {0}.".format(client, l))
        return True

    def recv(self):
        """recv() -> (int session_id, packet).
        
        Listens to all the connected sockets, even the bound socket.
        On new incomming connections, the function accepts the client and
        then tries to fetch another packet.

        The function returns a session_id along with the packet.
        """
        
        ls = [self._sck_listen] + list(self._scks.keys())
        rr, wr, er = select.select(ls, [], ls)
        
        for r in er:
            if r == self._sck_listen:
                print("error in the bound socket.  quitting.")
                exit(0)
            print("error in socket {0} with id {1}.".format(
                r, self._scks[r]['session_id']
            ))
            del self._scks[r]

        for r in rr:
            if r == self._sck_listen:
                # New client.
                client, addr = r.accept()
                self._scks[client] = dict([
                    ('buffer', '')
                    , ('pkt-length', 0)
                    , ('session_id', -1)
                ])
                # TODO: Do we want to return something here?
                print("client connected.")
                continue

            print("clients: {0}".format(", ".join([ str(s) for s in self._scks.keys() ])))

            client_data = self._scks[r]
            try:
                tmp = r.recv(1024)
            except socket.error as e:
                print("client socket error: {0}".format(str(e)))
                del self._scks[r]
                continue
            if tmp == '':
                print("client disconnected.")
                session_id = self._scks[r]['session_id']
                if len([ x for x in self._scks.values() \
                    if x['session_id'] == session_id ]) < 2:
                    self._db.del_client(session_id)
                del self._scks[r]
                continue
            client_data['buffer'] += tmp

            #print("data from client {0}: \"{1}\".".format(r, tmp))
            print("begin check.")
            if client_data['pkt-length'] == 0:
                if len(client_data['buffer']) >= 4:
                    # Packet length.
                    print("read packet length.")
                    client_data['pkt-length'] = struct.unpack('<I'
                                                , client_data['buffer'][:4])[0]
                    client_data['buffer'] = client_data['buffer'][4:]
                else:
                    print("not enough bytes for packet length.")
                    # Not enough bytes for a packet length.
                    continue
            if len(client_data['buffer']) < client_data['pkt-length']:
                # Not enough bytes for a packet.
                print("packet length known ({0}), not enough bytes for packet.".format(client_data['pkt-length']))
                continue

            # Alright, we have a packet. Take it from the buffer.
            length = client_data['pkt-length']
            packet = client_data['buffer'][:length]
            client_data['buffer'] = client_data['buffer'][length:]
            client_data['pkt-length'] = 0

            self._last_session_id = client_data['session_id']
            self._last_socket = r

            return (client_data["session_id"], packet)

        # Okey, we didn't find any this round.
        return self.recv()

    def run(self):
        """run() -> nothing.
        
        Main loop for Tracker.
        """

        print("Tracker.")


        while True:
            session_id, data = self.recv()
            message_type, message = lightning.unpack(data)

            print("Got message: {0}/{1}".format(message_type, message))
            if message_type not in self._tr_table:
                # TODO: Should be handled instead.
                #self._sck_in.send(lightning.pack(lightning.OK)) # TEMPORARY
                if not self.send(session_id, lightning.OK):
                    print("unable to send message.")

                print("Unhandled message. Sent OK back.")
                continue

            try:
                ret = self._tr_table[message_type](self, message)
                if ret is None:
                    continue

                if not self.send(ret):
                    print("unable to send message: {0}".format(str(ret)))
                else:
                    print("sent in return: {0}".format(str(ret)))
            except Exception as e:
                print("Function {0} raised an exception:".format(message_type))
                traceback.print_exc()


class TrackerDatabaseException(Exception):
    """Exception in TrackerDatabase."""
    pass
     
class TrackerDatabase:
    """Class containing the current connected nodes, connections within
    the network and a blacklist."""

    # TODO: Document these better.
    _clients = dict()
    _connections = dict()
    _blacklist = dict()
    _waitlist = dict()
    
    _batches = dict()
    
    # Testing something out here! /sven
    #_html_path = "index.html" #"/home/sweetfish/www.md5/tracker/index.html"
    _html_path = "/home/ethex/www/tmp/lightning.html"

    _next_id = 1

    def connect_clients(self, id_1, id_2):
        self._connections[id_1] = id_2
        self.update_html()
    
    def update_html(self):
        if self._html_path:
            html_head = """<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
            	"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">

            <html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
            <head>
            	<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
            	<style type="text/css" media="screen">
                    * {
                        padding: 0;
                        margin: 0;
                    }
                    
                    html {
                        padding: 20px;
                        background-color: #eee;
                        font-family: Arial, "MS Trebuchet", sans-serif;
                        font-size: 0.9em;
                        width: 600px;
                        margin: auto;
                    }
                    
                    ul {
                        list-style: none;
                    }
                    
                    li {
                        padding: 10px;
                        background-color: #0f0;
                        width: 100%;
                        color: #000;
                        margin-bottom: 2px;
                    }
                    
                    .session_id {
                        color: #080;
                    }
                    
                    .address {
                        font-weight: bold;
                    }
                    
                </style>
            	<title>Lightning - Tracker</title>
            </head>

            <body>
            """
            
            html_footer = """
            </body>
            </html>
            """
            
            html_final = html_head
            
            html_final += """
            <h1>Connected clients</h1>
            <ul>
            """
            for k,c in self._clients.items():
                html_final += "<li><span class=\"session_id\">(#" + str(k) + ")</span> <span class=\"address\">" + str(c[0]) + "</span></li>"

            html_final += """
            </ul>
            <br />
            <h1>Node connections</h1>
            <ul>
            """
            for k,c in self._connections.items():
                fr = self._clients[k]
                to = self._clients[c]
                html_final += "<li><span>{0} -> {1}</span></li>".format(fr[0], to[0])

            html_final += """
            </ul>
            <br />
            <h1>Wait list</h1>
            <ul>
            """
            for k,n in self._waitlist.items():
                if k in self._clients:
                    client = self._clients[k]
                    html_final += "<li><span>{0} waits for {1} nodes.</span></li>".format(client[0], n)

            html_final += "</ul>" + html_footer
            
            # TODO: Save html to file!
            #print html_final
            f = open(self._html_path, 'w')
            f.write(html_final)
            f.close()
            

    def new_batch(self, batch_hash, batch_type):
        """new_batch() -> ."""
        pass

    def get_waiting_clients(self):
        """get_waiting_clients() -> [(int session_id, int wants)]."""

        return [ (c, w) for (c, w) in self._waitlist.items() ]

    def set_client_waiting(self, session_id, wants=1):
        """set_client_waiting(int session_id, int wants=1) -> nothing."""

        self._waitlist[session_id] = wants
        self.update_html()

    def get_available_clients(self, min_available=1):
        """get_available_clients(int min_available=1) -> [(int session_id, int available)]."""

        return [ (s, i[1]) \
                 for s, i in self._clients.items() \
                 if i[1] >= min_available ]

    def get_client(self, session_id):
        """get_client(int session_id) -> (address, available)."""
        
        if session_id not in self._clients:
            raise TrackerDatabaseException(
                "Unable to find client with session_id {0}.".format(session_id)
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
                "Unable to find client with session_id {0}.".format(session_id)
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
        
        while self._next_id in self._clients:
            if self._next_id >= pow(2,31):
                self._next_id = 1
                #return self.next_id()
            self._next_id += 1

        return self._next_id

    def add_client(self, session_id, address, available):
        """add_client(int session_id, str address, int available) -> bool success.

        Adds a client to the client database. Throws AssertionError if 
        session_id is already in use.
        """

        assert(session_id not in self._clients.keys())
        
        self._clients[session_id] = (address, available)
        
        self.update_html()

        return True

    def del_client(self, session_id):
        """del_client(int session_id) -> bool succes

        Removes the client with the corresponding session_id.
        """

        # Clean up the connections.
        new_connections = dict([
                (ns, nt) for (ns, nt) in self._connections.items() \
                if ns != session_id and nt != session_id
            ])
        self._connections = new_connections
        
        if session_id in self._clients.keys():
            del self._clients[session_id]
            self.update_html()
            return True
        else:
            return False

    def new_init_client(self):
        """new_init_client() -> int session_id.

        Adds a new init_client to the client database, returning the new session_id.
        """

        session_id = self.next_id()
        self.add_client(session_id, "", 0)

        return session_id

    def new_client(self, address, available):
        """new_client(str address, int available) -> int session_id.

        Adds a new client to the client database, returning the new session_id.
        """

        session_id = self.next_id()
        self.add_client(session_id, address, available)    

        return session_id

def main():
    if len(sys.argv) >= 2 and sys.argv[1] in ['h', 'help']:
        print("usage: {0} h[elp]|[port=50000] [address=0.0.0.0]".format(sys.argv[0]))
        return

    try:
        address = "0.0.0.0" if len(sys.argv) < 3 else sys.argv[2]
        port = 50000 if len(sys.argv) < 2 else int(sys.argv[1])
    except:
        print("usage: {0} h[elp]|[port=50000] [address=0.0.0.0]".format(sys.argv[0]))
        return
    tracker = Tracker(address, port)
    tracker.run()


if __name__ == "__main__":
	main()

