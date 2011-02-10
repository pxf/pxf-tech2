// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "lightning.pb.h"
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace lightning {

namespace {

const ::google::protobuf::Descriptor* Ping_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Ping_reflection_ = NULL;
const ::google::protobuf::Descriptor* Pong_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Pong_reflection_ = NULL;
const ::google::protobuf::Descriptor* GoodBye_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  GoodBye_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_lightning_2eproto() {
  protobuf_AddDesc_lightning_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "lightning.proto");
  GOOGLE_CHECK(file != NULL);
  Ping_descriptor_ = file->message_type(0);
  static const int Ping_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Ping, ping_data_),
  };
  Ping_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Ping_descriptor_,
      Ping::default_instance_,
      Ping_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Ping, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Ping, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Ping));
  Pong_descriptor_ = file->message_type(1);
  static const int Pong_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Pong, ping_data_),
  };
  Pong_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Pong_descriptor_,
      Pong::default_instance_,
      Pong_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Pong, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Pong, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Pong));
  GoodBye_descriptor_ = file->message_type(2);
  static const int GoodBye_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GoodBye, session_id_),
  };
  GoodBye_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      GoodBye_descriptor_,
      GoodBye::default_instance_,
      GoodBye_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GoodBye, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GoodBye, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(GoodBye));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_lightning_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    Ping_descriptor_, &Ping::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    Pong_descriptor_, &Pong::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    GoodBye_descriptor_, &GoodBye::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_lightning_2eproto() {
  delete Ping::default_instance_;
  delete Ping_reflection_;
  delete Pong::default_instance_;
  delete Pong_reflection_;
  delete GoodBye::default_instance_;
  delete GoodBye_reflection_;
}

void protobuf_AddDesc_lightning_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\017lightning.proto\022\tlightning\"\031\n\004Ping\022\021\n\t"
    "ping_data\030\001 \002(\005\"\031\n\004Pong\022\021\n\tping_data\030\001 \002"
    "(\005\"\035\n\007GoodBye\022\022\n\nsession_id\030\001 \002(\005", 113);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "lightning.proto", &protobuf_RegisterTypes);
  Ping::default_instance_ = new Ping();
  Pong::default_instance_ = new Pong();
  GoodBye::default_instance_ = new GoodBye();
  Ping::default_instance_->InitAsDefaultInstance();
  Pong::default_instance_->InitAsDefaultInstance();
  GoodBye::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_lightning_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_lightning_2eproto {
  StaticDescriptorInitializer_lightning_2eproto() {
    protobuf_AddDesc_lightning_2eproto();
  }
} static_descriptor_initializer_lightning_2eproto_;


// ===================================================================

#ifndef _MSC_VER
const int Ping::kPingDataFieldNumber;
#endif  // !_MSC_VER

Ping::Ping()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void Ping::InitAsDefaultInstance() {
}

Ping::Ping(const Ping& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void Ping::SharedCtor() {
  _cached_size_ = 0;
  ping_data_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Ping::~Ping() {
  SharedDtor();
}

void Ping::SharedDtor() {
  if (this != default_instance_) {
  }
}

void Ping::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Ping::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Ping_descriptor_;
}

const Ping& Ping::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_lightning_2eproto();  return *default_instance_;
}

Ping* Ping::default_instance_ = NULL;

Ping* Ping::New() const {
  return new Ping;
}

void Ping::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    ping_data_ = 0;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool Ping::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required int32 ping_data = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &ping_data_)));
          _set_bit(0);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void Ping::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required int32 ping_data = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->ping_data(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* Ping::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required int32 ping_data = 1;
  if (_has_bit(0)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->ping_data(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int Ping::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required int32 ping_data = 1;
    if (has_ping_data()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->ping_data());
    }
    
  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Ping::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const Ping* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const Ping*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void Ping::MergeFrom(const Ping& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from._has_bit(0)) {
      set_ping_data(from.ping_data());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void Ping::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Ping::CopyFrom(const Ping& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Ping::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;
  
  return true;
}

void Ping::Swap(Ping* other) {
  if (other != this) {
    std::swap(ping_data_, other->ping_data_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata Ping::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Ping_descriptor_;
  metadata.reflection = Ping_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int Pong::kPingDataFieldNumber;
#endif  // !_MSC_VER

Pong::Pong()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void Pong::InitAsDefaultInstance() {
}

Pong::Pong(const Pong& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void Pong::SharedCtor() {
  _cached_size_ = 0;
  ping_data_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Pong::~Pong() {
  SharedDtor();
}

void Pong::SharedDtor() {
  if (this != default_instance_) {
  }
}

void Pong::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Pong::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Pong_descriptor_;
}

const Pong& Pong::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_lightning_2eproto();  return *default_instance_;
}

Pong* Pong::default_instance_ = NULL;

Pong* Pong::New() const {
  return new Pong;
}

void Pong::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    ping_data_ = 0;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool Pong::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required int32 ping_data = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &ping_data_)));
          _set_bit(0);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void Pong::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required int32 ping_data = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->ping_data(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* Pong::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required int32 ping_data = 1;
  if (_has_bit(0)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->ping_data(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int Pong::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required int32 ping_data = 1;
    if (has_ping_data()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->ping_data());
    }
    
  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Pong::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const Pong* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const Pong*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void Pong::MergeFrom(const Pong& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from._has_bit(0)) {
      set_ping_data(from.ping_data());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void Pong::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Pong::CopyFrom(const Pong& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Pong::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;
  
  return true;
}

void Pong::Swap(Pong* other) {
  if (other != this) {
    std::swap(ping_data_, other->ping_data_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata Pong::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Pong_descriptor_;
  metadata.reflection = Pong_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int GoodBye::kSessionIdFieldNumber;
#endif  // !_MSC_VER

GoodBye::GoodBye()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void GoodBye::InitAsDefaultInstance() {
}

GoodBye::GoodBye(const GoodBye& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void GoodBye::SharedCtor() {
  _cached_size_ = 0;
  session_id_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

GoodBye::~GoodBye() {
  SharedDtor();
}

void GoodBye::SharedDtor() {
  if (this != default_instance_) {
  }
}

void GoodBye::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* GoodBye::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return GoodBye_descriptor_;
}

const GoodBye& GoodBye::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_lightning_2eproto();  return *default_instance_;
}

GoodBye* GoodBye::default_instance_ = NULL;

GoodBye* GoodBye::New() const {
  return new GoodBye;
}

void GoodBye::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    session_id_ = 0;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool GoodBye::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required int32 session_id = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &session_id_)));
          _set_bit(0);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void GoodBye::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required int32 session_id = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->session_id(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* GoodBye::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required int32 session_id = 1;
  if (_has_bit(0)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->session_id(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int GoodBye::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required int32 session_id = 1;
    if (has_session_id()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->session_id());
    }
    
  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void GoodBye::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const GoodBye* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const GoodBye*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void GoodBye::MergeFrom(const GoodBye& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from._has_bit(0)) {
      set_session_id(from.session_id());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void GoodBye::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void GoodBye::CopyFrom(const GoodBye& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool GoodBye::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;
  
  return true;
}

void GoodBye::Swap(GoodBye* other) {
  if (other != this) {
    std::swap(session_id_, other->session_id_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata GoodBye::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = GoodBye_descriptor_;
  metadata.reflection = GoodBye_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace lightning

// @@protoc_insertion_point(global_scope)
