#!/usr/bin/python2

import struct, socket, hashlib

import zmq
import lightning, tracker_pb2

def main():
    print "Client."

    zmq_context = zmq.Context()
    zmq_socket = zmq_context.socket(zmq.REQ)
    zmq_socket_in = zmq_context.socket(zmq.REP)

	
    session_id = 0
    batchhash = hashlib.sha1("aoeu").hexdigest()
    batchtype = tracker_pb2.NewBatch.RAYTRACE
    tasks = 12000
    available = 4

    while True:
        print "1: Connect to tracker\n" +\
            "2: NewBatch\n" +\
            "3: NodeRequest\n" +\
            "4: GoodBye\n" +\
            "0: Quit\n"

        i = raw_input("Message to send: ")

        try:
            i = int(i)
        except ValueError:
            break

        if i == 0: break
        elif i == 1:
            # INIT_HELLO
            print "Connecting to tracker..."
            zmq_socket.connect("tcp://" + lightning.tracker_address + ":" + lightning.tracker_port)
            zmq_socket.send(struct.pack('<I', lightning.INIT_HELLO))

            # Wait for HelloToClient()
            print "..waiting for HelloToClient..."
            hello_message = zmq_socket.recv()
            hello_to_client = tracker_pb2.HelloToClient()
            hello_to_client.ParseFromString(hello_message[4:])
            session_id = hello_to_client.session_id
            print "DATA: {0}".format(hello_to_client)
            print "...got session_id {0}".format(session_id)

            # Set socket identity to session_id
            zmq_socket_in.setsockopt_unicode(zmq.IDENTITY, session_id)

            # Find an open port to listen to, by testing.
            listen_address = "tcp://*"
            listen_port = zmq_socket_in.bind_to_random_port(listen_address, 23458, 24000, 500)
            listen_address = "{0}:{1}".format("tcp://*", listen_port)
            print("Listening to {0}".format(listen_address))

            # HelloToTracker message
            hello = tracker_pb2.HelloToTracker()
            hello.address = listen_address 
            hello.available = available
            hello.session_id = session_id
            
            print("Sending new address information..")
            zmq_socket.send(struct.pack('<I', lightning.HELLO) + hello.SerializeToString())

            ok = zmq_socket.recv()
            if struct.unpack('<I', ok)[0] != lightning.OK:
                print ok
                break

            zmq_socket.close()
            print("Socket closed..")


        elif i == 2:
            print "Sending NewBatch..."
            new = tracker_pb2.NewBatch()
            new.session_id = session_id
            new.batchhash = batchhash
            new.tasks = tasks
            new.batchtype = batchtype

            print "protobuf data:\n" + str(new)
        elif i == 3:
            pass
        elif i == 4:
            print "Sending GoodBye..."
            bye = tracker_pb2.GoodBye()
            bye.session_id = session_id


            



if __name__ == "__main__":
	main()

