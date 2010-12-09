# Generated by the protocol buffer compiler.  DO NOT EDIT!

from google.protobuf import descriptor
from google.protobuf import message
from google.protobuf import reflection
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)


DESCRIPTOR = descriptor.FileDescriptor(
  name='client.proto',
  package='client',
  serialized_pb='\n\x0c\x63lient.proto\x12\x06\x63lient\":\n\x05Hello\x12\x0f\n\x07\x61\x64\x64ress\x18\x01 \x02(\t\x12\x0c\n\x04port\x18\x02 \x02(\x05\x12\x12\n\nsession_id\x18\x03 \x02(\x05\"3\n\x0e\x41llocateClient\x12\x0e\n\x06\x61mount\x18\x01 \x02(\x05\x12\x11\n\tbatchhash\x18\x02 \x02(\t\"8\n\x10\x41llocateResponse\x12\x0f\n\x07hasdata\x18\x01 \x02(\x08\x12\x13\n\x0bisavailable\x18\x02 \x02(\x08\"v\n\x04\x44\x61ta\x12\x11\n\tbatchhash\x18\x01 \x02(\t\x12\x10\n\x08\x64\x61tasize\x18\x02 \x02(\x05\x12\x10\n\x08\x64\x61tatype\x18\x03 \x02(\x05\x12\x0c\n\x04\x64\x61ta\x18\x04 \x02(\x0c\x12\x15\n\rreturnaddress\x18\x05 \x02(\t\x12\x12\n\nreturnport\x18\x06 \x02(\x05\"d\n\x05Tasks\x12\x11\n\tbatchhash\x18\x01 \x02(\t\x12 \n\x04task\x18\x02 \x03(\x0b\x32\x12.client.Tasks.Task\x1a&\n\x04Task\x12\x10\n\x08tasksize\x18\x01 \x02(\x05\x12\x0c\n\x04task\x18\x02 \x02(\x0c')




_HELLO = descriptor.Descriptor(
  name='Hello',
  full_name='client.Hello',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='address', full_name='client.Hello.address', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='port', full_name='client.Hello.port', index=1,
      number=2, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='session_id', full_name='client.Hello.session_id', index=2,
      number=3, type=5, cpp_type=1, label=2,
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
  serialized_start=24,
  serialized_end=82,
)


_ALLOCATECLIENT = descriptor.Descriptor(
  name='AllocateClient',
  full_name='client.AllocateClient',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='amount', full_name='client.AllocateClient.amount', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='batchhash', full_name='client.AllocateClient.batchhash', index=1,
      number=2, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
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
  serialized_end=135,
)


_ALLOCATERESPONSE = descriptor.Descriptor(
  name='AllocateResponse',
  full_name='client.AllocateResponse',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='hasdata', full_name='client.AllocateResponse.hasdata', index=0,
      number=1, type=8, cpp_type=7, label=2,
      has_default_value=False, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='isavailable', full_name='client.AllocateResponse.isavailable', index=1,
      number=2, type=8, cpp_type=7, label=2,
      has_default_value=False, default_value=False,
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
  serialized_start=137,
  serialized_end=193,
)


_DATA = descriptor.Descriptor(
  name='Data',
  full_name='client.Data',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='batchhash', full_name='client.Data.batchhash', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='datasize', full_name='client.Data.datasize', index=1,
      number=2, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='datatype', full_name='client.Data.datatype', index=2,
      number=3, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='data', full_name='client.Data.data', index=3,
      number=4, type=12, cpp_type=9, label=2,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='returnaddress', full_name='client.Data.returnaddress', index=4,
      number=5, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='returnport', full_name='client.Data.returnport', index=5,
      number=6, type=5, cpp_type=1, label=2,
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
  serialized_start=195,
  serialized_end=313,
)


_TASKS_TASK = descriptor.Descriptor(
  name='Task',
  full_name='client.Tasks.Task',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='tasksize', full_name='client.Tasks.Task.tasksize', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='task', full_name='client.Tasks.Task.task', index=1,
      number=2, type=12, cpp_type=9, label=2,
      has_default_value=False, default_value="",
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
  serialized_start=377,
  serialized_end=415,
)

_TASKS = descriptor.Descriptor(
  name='Tasks',
  full_name='client.Tasks',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='batchhash', full_name='client.Tasks.batchhash', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='task', full_name='client.Tasks.task', index=1,
      number=2, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[_TASKS_TASK, ],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=315,
  serialized_end=415,
)


_TASKS_TASK.containing_type = _TASKS;
_TASKS.fields_by_name['task'].message_type = _TASKS_TASK

class Hello(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _HELLO
  
  # @@protoc_insertion_point(class_scope:client.Hello)

class AllocateClient(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _ALLOCATECLIENT
  
  # @@protoc_insertion_point(class_scope:client.AllocateClient)

class AllocateResponse(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _ALLOCATERESPONSE
  
  # @@protoc_insertion_point(class_scope:client.AllocateResponse)

class Data(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _DATA
  
  # @@protoc_insertion_point(class_scope:client.Data)

class Tasks(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  
  class Task(message.Message):
    __metaclass__ = reflection.GeneratedProtocolMessageType
    DESCRIPTOR = _TASKS_TASK
    
    # @@protoc_insertion_point(class_scope:client.Tasks.Task)
  DESCRIPTOR = _TASKS
  
  # @@protoc_insertion_point(class_scope:client.Tasks)

# @@protoc_insertion_point(module_scope)
