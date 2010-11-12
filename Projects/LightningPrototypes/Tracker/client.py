#!/usr/bin/python2

import tracker_pb2,zmq,socket,struct,hashlib

tracker_address = "127.0.0.1"
tracker_port = "34567"

def main():
	print "Client."

	zmq_context = zmq.Context()
	zmq_socket = zmq_context.socket(zmq.REQ)
	
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
			print "Creating protocol buffer data..."
			hello = tracker_pb2.HelloToTracker()
			hello.address.port = 34568
			hello.address.isipv4 = True
			hello.address.ipv4 = struct.unpack('L',socket.inet_aton("127.0.0.1"))[0]
			print "protobuf data:\n" + str(hello)

			print "Connecting to tracker..."
			zmq_socket.connect("tcp://" + tracker_address + ":" + tracker_port)
			print "Sending protobuf data..."
			zmq_socket.send(hello.SerializeToString())

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

