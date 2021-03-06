# Generated by the protocol buffer compiler.  DO NOT EDIT!

from google.protobuf import descriptor
from google.protobuf import message
from google.protobuf import reflection
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)


DESCRIPTOR = descriptor.FileDescriptor(
  name='lightning.proto',
  package='lightning',
  serialized_pb='\n\x0flightning.proto\x12\tlightning\"\x19\n\x04Ping\x12\x11\n\tping_data\x18\x01 \x02(\x05\"\x19\n\x04Pong\x12\x11\n\tping_data\x18\x01 \x02(\x05\"\x1d\n\x07GoodBye\x12\x12\n\nsession_id\x18\x01 \x02(\x05')




_PING = descriptor.Descriptor(
  name='Ping',
  full_name='lightning.Ping',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='ping_data', full_name='lightning.Ping.ping_data', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=30,
  serialized_end=55,
)


_PONG = descriptor.Descriptor(
  name='Pong',
  full_name='lightning.Pong',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='ping_data', full_name='lightning.Pong.ping_data', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=57,
  serialized_end=82,
)


_GOODBYE = descriptor.Descriptor(
  name='GoodBye',
  full_name='lightning.GoodBye',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='session_id', full_name='lightning.GoodBye.session_id', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=84,
  serialized_end=113,
)



class Ping(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _PING
  
  # @@protoc_insertion_point(class_scope:lightning.Ping)

class Pong(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _PONG
  
  # @@protoc_insertion_point(class_scope:lightning.Pong)

class GoodBye(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _GOODBYE
  
  # @@protoc_insertion_point(class_scope:lightning.GoodBye)

# @@protoc_insertion_point(module_scope)
