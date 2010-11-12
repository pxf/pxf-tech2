#!/usr/bin/python2

import tracker_pb2,zmq,socket,struct,hashlib,lightning

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
			if zmq_socket.closed
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

