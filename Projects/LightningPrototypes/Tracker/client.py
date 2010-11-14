#!/usr/bin/python2

import struct, socket, hashlib

import zmq
import lightning, tracker_pb2

def main():
    print "Client."

    zmq_context = zmq.Context()
    zmq_socket = zmq_context.socket(zmq.REQ)
    zmq_socket_in = zmq_context.socket(zmq.PUB)

    # Find an open port to listen to, by testing.
    listen_port = 34568 # Start at 34568
    listen_address = ""
    for i in range(500): # Max 500 tries. 
        try:
            listen_address = "tcp://{0}:{1}".format("*", listen_port)
            zmq_socket_in.bind(listen_address)
        except Exception as e:
            print("ERROR BINDING {0}: {1}".format(listen_address, str(e)))
            listen_port += 1
            continue
        break
    print("Listening to {0}".format(listen_address))
	
    while True:
        print "1: HelloToTracker\n" +\
            "2: NewBatch\n" +\
            "3: NodeRequest\n" +\
            "0: Quit\n"

        i = raw_input("Message to send: ")

        try:
            i = int(i)
        except ValueError:
            break

        sessionid = 0
        batchhash = hashlib.sha1("aoeu").hexdigest()
        batchtype = tracker_pb2.NewBatch.RAYTRACE
        tasks = 12000

        if i == 0: break
        elif i == 1:
            # HelloToTracker message
            print "Creating protocol buffer data..."
            hello = tracker_pb2.HelloToTracker()
            hello.address = listen_address
            print "protobuf data:\n" + str(hello)

            print "Connecting to tracker..."
            zmq_socket.connect("tcp://" + lightning.tracker_address + ":" + lightning.tracker_port)
            print "Sending protobuf data..."
            zmq_socket.send(struct.pack('<I', lightning.HELLO) + hello.SerializeToString())
            print hello.SerializeToString()

        elif i == 2:
            print "Sending NewBatch..."
            new = tracker_pb2.NewBatch()
            new.sessionid = sessionid
            new.batchhash = batchhash
            new.tasks = tasks
            new.batchtype = batchtype

            print "protobuf data:\n" + str(new)




if __name__ == "__main__":
	main()

