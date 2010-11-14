#!/usr/bin/python2

import tracker_pb2,zmq,socket,struct,hashlib,lightning

def main():
    print "Client."

    zmq_context = zmq.Context()
    zmq_socket = zmq_context.socket(zmq.REQ)
    zmq_socket_in = zmq_context.socket(zmq.PUB)

    # Find an open port to listen to.
    listen_port = 34568 # Start at 34568
    for i in range(500): # Max 500 tries. 
        try:
            zmq_socket_in.bind("tcp://{0}:{1}".format("users.mkeyd.net", str(listen_port)))
        except:
            listen_port += 1
        break
    print("Listening to {0}".format("tcp://{0}:{1}".format("users.mkeyd.net", str(listen_port))))
	
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
            hello.address = "tcp://localhost:34568"
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

