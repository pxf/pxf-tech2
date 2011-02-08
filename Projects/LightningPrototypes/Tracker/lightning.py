import struct, tracker_pb2

tracker_address = "127.0.0.1"
tracker_port = 50000

#Enumerators
INIT_HELLO, HELLO_TO_CLIENT, HELLO_TO_TRACKER, GOODBYE, NEWBATCH, OK, NODES_REQUEST, NODES_RESPONSE, BATCH_DONE, \
	TASK_DONE, HATE, PING, PONG = range(13)

translate_message_type = {
    INIT_HELLO: None,
    HELLO_TO_CLIENT: tracker_pb2.HelloToClient,
    HELLO_TO_TRACKER: tracker_pb2.HelloToTracker,
    GOODBYE: tracker_pb2.GoodBye,
    PING: tracker_pb2.Ping,
    PONG: tracker_pb2.Pong
}

def unpack_raw(message):
    """unpack_raw(str message) -> (int message_type, string data)

    Wrapper for unpacking data sent between the tracker and client.
    """

    # The first 4 bytes of the message is an identifier
    type_data = struct.unpack('<I', message[:4])[0]
    return (type_data, message[4:])

def unpack(message):
    """unpack(str message) -> (int message_type, <protobuf> data)

    Returns a tuple containing the type of data, and parsed protocol buffers
    data.
    """

    (type_data, data) = unpack_raw(message)

    if type_data not in translate_message_type:
        raise PackingException("Enum {0} could not be found in translate dict".format(type_data))

    if data == "":
        return (type_data, None)
    else:
        if translate_message_type[type_data] is not None:
            proto_data = translate_message_type[type_data]()
        else:
            return (type_data, None)
        proto_data.ParseFromString(data)
        return (type_data, proto_data)


def pack(message_type, data=None):
    """pack(int message_type, <protobuf> data=None) -> str message.

    Packs a message type and data and returns a message string.
    If no data is to be sent, set it to NULL.
    """

    ret = struct.pack('<I', message_type)
    if data is not None:
        ret += data.SerializeToString()

    return ret

class LightningException(Exception):
    """Main class for lightnings exceptions, if there are any..."""
    pass

class PackingException(LightningException):
    pass
