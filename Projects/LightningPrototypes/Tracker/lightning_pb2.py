# Generated by the protocol buffer compiler.  DO NOT EDIT!

from google.protobuf import descriptor
from google.protobuf import message
from google.protobuf import reflection
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)


DESCRIPTOR = descriptor.FileDescriptor(
  name='lightning.proto',
  package='lightning',
  serialized_pb='\n\x0flightning.proto\x12\tlightning\"Y\n\x07\x41\x64\x64ress\x12\x14\n\x06isipv4\x18\x01 \x02(\x08:\x04true\x12\x0c\n\x04ipv4\x18\x02 \x01(\x05\x12\r\n\x05ipv6a\x18\x03 \x01(\x03\x12\r\n\x05ipv6b\x18\x04 \x01(\x03\x12\x0c\n\x04port\x18\x05 \x02(\x05')




_ADDRESS = descriptor.Descriptor(
  name='Address',
  full_name='lightning.Address',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='isipv4', full_name='lightning.Address.isipv4', index=0,
      number=1, type=8, cpp_type=7, label=2,
      has_default_value=True, default_value=True,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='ipv4', full_name='lightning.Address.ipv4', index=1,
      number=2, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='ipv6a', full_name='lightning.Address.ipv6a', index=2,
      number=3, type=3, cpp_type=2, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='ipv6b', full_name='lightning.Address.ipv6b', index=3,
      number=4, type=3, cpp_type=2, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='port', full_name='lightning.Address.port', index=4,
      number=5, type=5, cpp_type=1, label=2,
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
  serialized_end=119,
)



class Address(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _ADDRESS
  
  # @@protoc_insertion_point(class_scope:lightning.Address)

# @@protoc_insertion_point(module_scope)
