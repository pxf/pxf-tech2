#!/usr/bin/python2

import zmq,tracker_pb2

def main():

    context = zmq.Context()
    socket = context.socket(zmq.REP)
    socket.bind("tcp://*:34567")
     
    while True:
        #  Wait for next request from client
        message = socket.recv()
        print "Received message, parsing... "
        hello = tracker_pb2.HelloToTracker()
        hello.ParseFromString(message)
        print str(hello)
     
     
        #  Send reply back to client
        socket.send("World")
        print "Tracker."

if __name__ == "__main__":
	main()

