import struct, tracker_pb2, lightning_pb2

tracker_address = "95.80.1.56"
tracker_port = 50000

# Common enumerators
OK, PING, PONG, GOODBYE = range(0,4)

# Tracker <-> Client
T_INIT, T_HELLO_CLIENT, T_HELLO_TRACKER, T_NEWBATCH, T_NODES_REQUEST \
    , T_NODES_RESPONSE, T_BATCH_DONE, T_TASK_DONE, T_HATE = range(4, 13)

# Clinet <-> Client
C_HELLO, C_ALLOCATE, C_ALLOC_RESP, C_DATA, C_TASKS = range(13, 18)



translate_message_type = {
    T_INIT: None,
    T_HELLO_CLIENT: tracker_pb2.HelloToClient,
    T_HELLO_TRACKER: tracker_pb2.HelloToTracker,
    GOODBYE: lightning_pb2.GoodBye,
    PING: lightning_pb2.Ping,
    PONG: lightning_pb2.Pong
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
