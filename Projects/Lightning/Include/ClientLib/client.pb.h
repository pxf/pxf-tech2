// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: client.proto

#ifndef PROTOBUF_client_2eproto__INCLUDED
#define PROTOBUF_client_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2003000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2003000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_message_reflection.h>
// @@protoc_insertion_point(includes)

namespace client {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_client_2eproto();
void protobuf_AssignDesc_client_2eproto();
void protobuf_ShutdownFile_client_2eproto();

class Hello;
class AllocateClient;
class AllocateResponse;
class Data;
class Tasks;
class Tasks_Task;

// ===================================================================

class Hello : public ::google::protobuf::Message {
 public:
  Hello();
  virtual ~Hello();
  
  Hello(const Hello& from);
  
  inline Hello& operator=(const Hello& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const Hello& default_instance();
  
  void Swap(Hello* other);
  
  // implements Message ----------------------------------------------
  
  Hello* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Hello& from);
  void MergeFrom(const Hello& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required string address = 1;
  inline bool has_address() const;
  inline void clear_address();
  static const int kAddressFieldNumber = 1;
  inline const ::std::string& address() const;
  inline void set_address(const ::std::string& value);
  inline void set_address(const char* value);
  inline void set_address(const char* value, size_t size);
  inline ::std::string* mutable_address();
  
  // required int32 port = 2;
  inline bool has_port() const;
  inline void clear_port();
  static const int kPortFieldNumber = 2;
  inline ::google::protobuf::int32 port() const;
  inline void set_port(::google::protobuf::int32 value);
  
  // required int32 session_id = 3;
  inline bool has_session_id() const;
  inline void clear_session_id();
  static const int kSessionIdFieldNumber = 3;
  inline ::google::protobuf::int32 session_id() const;
  inline void set_session_id(::google::protobuf::int32 value);
  
  // @@protoc_insertion_point(class_scope:client.Hello)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::std::string* address_;
  static const ::std::string _default_address_;
  ::google::protobuf::int32 port_;
  ::google::protobuf::int32 session_id_;
  friend void  protobuf_AddDesc_client_2eproto();
  friend void protobuf_AssignDesc_client_2eproto();
  friend void protobuf_ShutdownFile_client_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(3 + 31) / 32];
  
  // WHY DOES & HAVE LOWER PRECEDENCE THAN != !?
  inline bool _has_bit(int index) const {
    return (_has_bits_[index / 32] & (1u << (index % 32))) != 0;
  }
  inline void _set_bit(int index) {
    _has_bits_[index / 32] |= (1u << (index % 32));
  }
  inline void _clear_bit(int index) {
    _has_bits_[index / 32] &= ~(1u << (index % 32));
  }
  
  void InitAsDefaultInstance();
  static Hello* default_instance_;
};
// -------------------------------------------------------------------

class AllocateClient : public ::google::protobuf::Message {
 public:
  AllocateClient();
  virtual ~AllocateClient();
  
  AllocateClient(const AllocateClient& from);
  
  inline AllocateClient& operator=(const AllocateClient& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const AllocateClient& default_instance();
  
  void Swap(AllocateClient* other);
  
  // implements Message ----------------------------------------------
  
  AllocateClient* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const AllocateClient& from);
  void MergeFrom(const AllocateClient& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required int32 amount = 1;
  inline bool has_amount() const;
  inline void clear_amount();
  static const int kAmountFieldNumber = 1;
  inline ::google::protobuf::int32 amount() const;
  inline void set_amount(::google::protobuf::int32 value);
  
  // required string batchhash = 2;
  inline bool has_batchhash() const;
  inline void clear_batchhash();
  static const int kBatchhashFieldNumber = 2;
  inline const ::std::string& batchhash() const;
  inline void set_batchhash(const ::std::string& value);
  inline void set_batchhash(const char* value);
  inline void set_batchhash(const char* value, size_t size);
  inline ::std::string* mutable_batchhash();
  
  // @@protoc_insertion_point(class_scope:client.AllocateClient)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::google::protobuf::int32 amount_;
  ::std::string* batchhash_;
  static const ::std::string _default_batchhash_;
  friend void  protobuf_AddDesc_client_2eproto();
  friend void protobuf_AssignDesc_client_2eproto();
  friend void protobuf_ShutdownFile_client_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];
  
  // WHY DOES & HAVE LOWER PRECEDENCE THAN != !?
  inline bool _has_bit(int index) const {
    return (_has_bits_[index / 32] & (1u << (index % 32))) != 0;
  }
  inline void _set_bit(int index) {
    _has_bits_[index / 32] |= (1u << (index % 32));
  }
  inline void _clear_bit(int index) {
    _has_bits_[index / 32] &= ~(1u << (index % 32));
  }
  
  void InitAsDefaultInstance();
  static AllocateClient* default_instance_;
};
// -------------------------------------------------------------------

class AllocateResponse : public ::google::protobuf::Message {
 public:
  AllocateResponse();
  virtual ~AllocateResponse();
  
  AllocateResponse(const AllocateResponse& from);
  
  inline AllocateResponse& operator=(const AllocateResponse& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const AllocateResponse& default_instance();
  
  void Swap(AllocateResponse* other);
  
  // implements Message ----------------------------------------------
  
  AllocateResponse* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const AllocateResponse& from);
  void MergeFrom(const AllocateResponse& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required bool hasdata = 1;
  inline bool has_hasdata() const;
  inline void clear_hasdata();
  static const int kHasdataFieldNumber = 1;
  inline bool hasdata() const;
  inline void set_hasdata(bool value);
  
  // required bool isavailable = 2;
  inline bool has_isavailable() const;
  inline void clear_isavailable();
  static const int kIsavailableFieldNumber = 2;
  inline bool isavailable() const;
  inline void set_isavailable(bool value);
  
  // @@protoc_insertion_point(class_scope:client.AllocateResponse)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  bool hasdata_;
  bool isavailable_;
  friend void  protobuf_AddDesc_client_2eproto();
  friend void protobuf_AssignDesc_client_2eproto();
  friend void protobuf_ShutdownFile_client_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];
  
  // WHY DOES & HAVE LOWER PRECEDENCE THAN != !?
  inline bool _has_bit(int index) const {
    return (_has_bits_[index / 32] & (1u << (index % 32))) != 0;
  }
  inline void _set_bit(int index) {
    _has_bits_[index / 32] |= (1u << (index % 32));
  }
  inline void _clear_bit(int index) {
    _has_bits_[index / 32] &= ~(1u << (index % 32));
  }
  
  void InitAsDefaultInstance();
  static AllocateResponse* default_instance_;
};
// -------------------------------------------------------------------

class Data : public ::google::protobuf::Message {
 public:
  Data();
  virtual ~Data();
  
  Data(const Data& from);
  
  inline Data& operator=(const Data& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const Data& default_instance();
  
  void Swap(Data* other);
  
  // implements Message ----------------------------------------------
  
  Data* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Data& from);
  void MergeFrom(const Data& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required string batchhash = 1;
  inline bool has_batchhash() const;
  inline void clear_batchhash();
  static const int kBatchhashFieldNumber = 1;
  inline const ::std::string& batchhash() const;
  inline void set_batchhash(const ::std::string& value);
  inline void set_batchhash(const char* value);
  inline void set_batchhash(const char* value, size_t size);
  inline ::std::string* mutable_batchhash();
  
  // required int32 datasize = 2;
  inline bool has_datasize() const;
  inline void clear_datasize();
  static const int kDatasizeFieldNumber = 2;
  inline ::google::protobuf::int32 datasize() const;
  inline void set_datasize(::google::protobuf::int32 value);
  
  // required int32 datatype = 3;
  inline bool has_datatype() const;
  inline void clear_datatype();
  static const int kDatatypeFieldNumber = 3;
  inline ::google::protobuf::int32 datatype() const;
  inline void set_datatype(::google::protobuf::int32 value);
  
  // required string data = 4;
  inline bool has_data() const;
  inline void clear_data();
  static const int kDataFieldNumber = 4;
  inline const ::std::string& data() const;
  inline void set_data(const ::std::string& value);
  inline void set_data(const char* value);
  inline void set_data(const char* value, size_t size);
  inline ::std::string* mutable_data();
  
  // required string returnaddress = 5;
  inline bool has_returnaddress() const;
  inline void clear_returnaddress();
  static const int kReturnaddressFieldNumber = 5;
  inline const ::std::string& returnaddress() const;
  inline void set_returnaddress(const ::std::string& value);
  inline void set_returnaddress(const char* value);
  inline void set_returnaddress(const char* value, size_t size);
  inline ::std::string* mutable_returnaddress();
  
  // @@protoc_insertion_point(class_scope:client.Data)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::std::string* batchhash_;
  static const ::std::string _default_batchhash_;
  ::google::protobuf::int32 datasize_;
  ::google::protobuf::int32 datatype_;
  ::std::string* data_;
  static const ::std::string _default_data_;
  ::std::string* returnaddress_;
  static const ::std::string _default_returnaddress_;
  friend void  protobuf_AddDesc_client_2eproto();
  friend void protobuf_AssignDesc_client_2eproto();
  friend void protobuf_ShutdownFile_client_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(5 + 31) / 32];
  
  // WHY DOES & HAVE LOWER PRECEDENCE THAN != !?
  inline bool _has_bit(int index) const {
    return (_has_bits_[index / 32] & (1u << (index % 32))) != 0;
  }
  inline void _set_bit(int index) {
    _has_bits_[index / 32] |= (1u << (index % 32));
  }
  inline void _clear_bit(int index) {
    _has_bits_[index / 32] &= ~(1u << (index % 32));
  }
  
  void InitAsDefaultInstance();
  static Data* default_instance_;
};
// -------------------------------------------------------------------

class Tasks_Task : public ::google::protobuf::Message {
 public:
  Tasks_Task();
  virtual ~Tasks_Task();
  
  Tasks_Task(const Tasks_Task& from);
  
  inline Tasks_Task& operator=(const Tasks_Task& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const Tasks_Task& default_instance();
  
  void Swap(Tasks_Task* other);
  
  // implements Message ----------------------------------------------
  
  Tasks_Task* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Tasks_Task& from);
  void MergeFrom(const Tasks_Task& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required int32 tasksize = 1;
  inline bool has_tasksize() const;
  inline void clear_tasksize();
  static const int kTasksizeFieldNumber = 1;
  inline ::google::protobuf::int32 tasksize() const;
  inline void set_tasksize(::google::protobuf::int32 value);
  
  // required string task = 2;
  inline bool has_task() const;
  inline void clear_task();
  static const int kTaskFieldNumber = 2;
  inline const ::std::string& task() const;
  inline void set_task(const ::std::string& value);
  inline void set_task(const char* value);
  inline void set_task(const char* value, size_t size);
  inline ::std::string* mutable_task();
  
  // @@protoc_insertion_point(class_scope:client.Tasks.Task)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::google::protobuf::int32 tasksize_;
  ::std::string* task_;
  static const ::std::string _default_task_;
  friend void  protobuf_AddDesc_client_2eproto();
  friend void protobuf_AssignDesc_client_2eproto();
  friend void protobuf_ShutdownFile_client_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];
  
  // WHY DOES & HAVE LOWER PRECEDENCE THAN != !?
  inline bool _has_bit(int index) const {
    return (_has_bits_[index / 32] & (1u << (index % 32))) != 0;
  }
  inline void _set_bit(int index) {
    _has_bits_[index / 32] |= (1u << (index % 32));
  }
  inline void _clear_bit(int index) {
    _has_bits_[index / 32] &= ~(1u << (index % 32));
  }
  
  void InitAsDefaultInstance();
  static Tasks_Task* default_instance_;
};
// -------------------------------------------------------------------

class Tasks : public ::google::protobuf::Message {
 public:
  Tasks();
  virtual ~Tasks();
  
  Tasks(const Tasks& from);
  
  inline Tasks& operator=(const Tasks& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const Tasks& default_instance();
  
  void Swap(Tasks* other);
  
  // implements Message ----------------------------------------------
  
  Tasks* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Tasks& from);
  void MergeFrom(const Tasks& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  typedef Tasks_Task Task;
  
  // accessors -------------------------------------------------------
  
  // required string batchhash = 1;
  inline bool has_batchhash() const;
  inline void clear_batchhash();
  static const int kBatchhashFieldNumber = 1;
  inline const ::std::string& batchhash() const;
  inline void set_batchhash(const ::std::string& value);
  inline void set_batchhash(const char* value);
  inline void set_batchhash(const char* value, size_t size);
  inline ::std::string* mutable_batchhash();
  
  // repeated .client.Tasks.Task task = 2;
  inline int task_size() const;
  inline void clear_task();
  static const int kTaskFieldNumber = 2;
  inline const ::client::Tasks_Task& task(int index) const;
  inline ::client::Tasks_Task* mutable_task(int index);
  inline ::client::Tasks_Task* add_task();
  inline const ::google::protobuf::RepeatedPtrField< ::client::Tasks_Task >&
      task() const;
  inline ::google::protobuf::RepeatedPtrField< ::client::Tasks_Task >*
      mutable_task();
  
  // @@protoc_insertion_point(class_scope:client.Tasks)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::std::string* batchhash_;
  static const ::std::string _default_batchhash_;
  ::google::protobuf::RepeatedPtrField< ::client::Tasks_Task > task_;
  friend void  protobuf_AddDesc_client_2eproto();
  friend void protobuf_AssignDesc_client_2eproto();
  friend void protobuf_ShutdownFile_client_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];
  
  // WHY DOES & HAVE LOWER PRECEDENCE THAN != !?
  inline bool _has_bit(int index) const {
    return (_has_bits_[index / 32] & (1u << (index % 32))) != 0;
  }
  inline void _set_bit(int index) {
    _has_bits_[index / 32] |= (1u << (index % 32));
  }
  inline void _clear_bit(int index) {
    _has_bits_[index / 32] &= ~(1u << (index % 32));
  }
  
  void InitAsDefaultInstance();
  static Tasks* default_instance_;
};
// ===================================================================


// ===================================================================

// Hello

// required string address = 1;
inline bool Hello::has_address() const {
  return _has_bit(0);
}
inline void Hello::clear_address() {
  if (address_ != &_default_address_) {
    address_->clear();
  }
  _clear_bit(0);
}
inline const ::std::string& Hello::address() const {
  return *address_;
}
inline void Hello::set_address(const ::std::string& value) {
  _set_bit(0);
  if (address_ == &_default_address_) {
    address_ = new ::std::string;
  }
  address_->assign(value);
}
inline void Hello::set_address(const char* value) {
  _set_bit(0);
  if (address_ == &_default_address_) {
    address_ = new ::std::string;
  }
  address_->assign(value);
}
inline void Hello::set_address(const char* value, size_t size) {
  _set_bit(0);
  if (address_ == &_default_address_) {
    address_ = new ::std::string;
  }
  address_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Hello::mutable_address() {
  _set_bit(0);
  if (address_ == &_default_address_) {
    address_ = new ::std::string;
  }
  return address_;
}

// required int32 port = 2;
inline bool Hello::has_port() const {
  return _has_bit(1);
}
inline void Hello::clear_port() {
  port_ = 0;
  _clear_bit(1);
}
inline ::google::protobuf::int32 Hello::port() const {
  return port_;
}
inline void Hello::set_port(::google::protobuf::int32 value) {
  _set_bit(1);
  port_ = value;
}

// required int32 session_id = 3;
inline bool Hello::has_session_id() const {
  return _has_bit(2);
}
inline void Hello::clear_session_id() {
  session_id_ = 0;
  _clear_bit(2);
}
inline ::google::protobuf::int32 Hello::session_id() const {
  return session_id_;
}
inline void Hello::set_session_id(::google::protobuf::int32 value) {
  _set_bit(2);
  session_id_ = value;
}

// -------------------------------------------------------------------

// AllocateClient

// required int32 amount = 1;
inline bool AllocateClient::has_amount() const {
  return _has_bit(0);
}
inline void AllocateClient::clear_amount() {
  amount_ = 0;
  _clear_bit(0);
}
inline ::google::protobuf::int32 AllocateClient::amount() const {
  return amount_;
}
inline void AllocateClient::set_amount(::google::protobuf::int32 value) {
  _set_bit(0);
  amount_ = value;
}

// required string batchhash = 2;
inline bool AllocateClient::has_batchhash() const {
  return _has_bit(1);
}
inline void AllocateClient::clear_batchhash() {
  if (batchhash_ != &_default_batchhash_) {
    batchhash_->clear();
  }
  _clear_bit(1);
}
inline const ::std::string& AllocateClient::batchhash() const {
  return *batchhash_;
}
inline void AllocateClient::set_batchhash(const ::std::string& value) {
  _set_bit(1);
  if (batchhash_ == &_default_batchhash_) {
    batchhash_ = new ::std::string;
  }
  batchhash_->assign(value);
}
inline void AllocateClient::set_batchhash(const char* value) {
  _set_bit(1);
  if (batchhash_ == &_default_batchhash_) {
    batchhash_ = new ::std::string;
  }
  batchhash_->assign(value);
}
inline void AllocateClient::set_batchhash(const char* value, size_t size) {
  _set_bit(1);
  if (batchhash_ == &_default_batchhash_) {
    batchhash_ = new ::std::string;
  }
  batchhash_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* AllocateClient::mutable_batchhash() {
  _set_bit(1);
  if (batchhash_ == &_default_batchhash_) {
    batchhash_ = new ::std::string;
  }
  return batchhash_;
}

// -------------------------------------------------------------------

// AllocateResponse

// required bool hasdata = 1;
inline bool AllocateResponse::has_hasdata() const {
  return _has_bit(0);
}
inline void AllocateResponse::clear_hasdata() {
  hasdata_ = false;
  _clear_bit(0);
}
inline bool AllocateResponse::hasdata() const {
  return hasdata_;
}
inline void AllocateResponse::set_hasdata(bool value) {
  _set_bit(0);
  hasdata_ = value;
}

// required bool isavailable = 2;
inline bool AllocateResponse::has_isavailable() const {
  return _has_bit(1);
}
inline void AllocateResponse::clear_isavailable() {
  isavailable_ = false;
  _clear_bit(1);
}
inline bool AllocateResponse::isavailable() const {
  return isavailable_;
}
inline void AllocateResponse::set_isavailable(bool value) {
  _set_bit(1);
  isavailable_ = value;
}

// -------------------------------------------------------------------

// Data

// required string batchhash = 1;
inline bool Data::has_batchhash() const {
  return _has_bit(0);
}
inline void Data::clear_batchhash() {
  if (batchhash_ != &_default_batchhash_) {
    batchhash_->clear();
  }
  _clear_bit(0);
}
inline const ::std::string& Data::batchhash() const {
  return *batchhash_;
}
inline void Data::set_batchhash(const ::std::string& value) {
  _set_bit(0);
  if (batchhash_ == &_default_batchhash_) {
    batchhash_ = new ::std::string;
  }
  batchhash_->assign(value);
}
inline void Data::set_batchhash(const char* value) {
  _set_bit(0);
  if (batchhash_ == &_default_batchhash_) {
    batchhash_ = new ::std::string;
  }
  batchhash_->assign(value);
}
inline void Data::set_batchhash(const char* value, size_t size) {
  _set_bit(0);
  if (batchhash_ == &_default_batchhash_) {
    batchhash_ = new ::std::string;
  }
  batchhash_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Data::mutable_batchhash() {
  _set_bit(0);
  if (batchhash_ == &_default_batchhash_) {
    batchhash_ = new ::std::string;
  }
  return batchhash_;
}

// required int32 datasize = 2;
inline bool Data::has_datasize() const {
  return _has_bit(1);
}
inline void Data::clear_datasize() {
  datasize_ = 0;
  _clear_bit(1);
}
inline ::google::protobuf::int32 Data::datasize() const {
  return datasize_;
}
inline void Data::set_datasize(::google::protobuf::int32 value) {
  _set_bit(1);
  datasize_ = value;
}

// required int32 datatype = 3;
inline bool Data::has_datatype() const {
  return _has_bit(2);
}
inline void Data::clear_datatype() {
  datatype_ = 0;
  _clear_bit(2);
}
inline ::google::protobuf::int32 Data::datatype() const {
  return datatype_;
}
inline void Data::set_datatype(::google::protobuf::int32 value) {
  _set_bit(2);
  datatype_ = value;
}

// required string data = 4;
inline bool Data::has_data() const {
  return _has_bit(3);
}
inline void Data::clear_data() {
  if (data_ != &_default_data_) {
    data_->clear();
  }
  _clear_bit(3);
}
inline const ::std::string& Data::data() const {
  return *data_;
}
inline void Data::set_data(const ::std::string& value) {
  _set_bit(3);
  if (data_ == &_default_data_) {
    data_ = new ::std::string;
  }
  data_->assign(value);
}
inline void Data::set_data(const char* value) {
  _set_bit(3);
  if (data_ == &_default_data_) {
    data_ = new ::std::string;
  }
  data_->assign(value);
}
inline void Data::set_data(const char* value, size_t size) {
  _set_bit(3);
  if (data_ == &_default_data_) {
    data_ = new ::std::string;
  }
  data_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Data::mutable_data() {
  _set_bit(3);
  if (data_ == &_default_data_) {
    data_ = new ::std::string;
  }
  return data_;
}

// required string returnaddress = 5;
inline bool Data::has_returnaddress() const {
  return _has_bit(4);
}
inline void Data::clear_returnaddress() {
  if (returnaddress_ != &_default_returnaddress_) {
    returnaddress_->clear();
  }
  _clear_bit(4);
}
inline const ::std::string& Data::returnaddress() const {
  return *returnaddress_;
}
inline void Data::set_returnaddress(const ::std::string& value) {
  _set_bit(4);
  if (returnaddress_ == &_default_returnaddress_) {
    returnaddress_ = new ::std::string;
  }
  returnaddress_->assign(value);
}
inline void Data::set_returnaddress(const char* value) {
  _set_bit(4);
  if (returnaddress_ == &_default_returnaddress_) {
    returnaddress_ = new ::std::string;
  }
  returnaddress_->assign(value);
}
inline void Data::set_returnaddress(const char* value, size_t size) {
  _set_bit(4);
  if (returnaddress_ == &_default_returnaddress_) {
    returnaddress_ = new ::std::string;
  }
  returnaddress_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Data::mutable_returnaddress() {
  _set_bit(4);
  if (returnaddress_ == &_default_returnaddress_) {
    returnaddress_ = new ::std::string;
  }
  return returnaddress_;
}

// -------------------------------------------------------------------

// Tasks_Task

// required int32 tasksize = 1;
inline bool Tasks_Task::has_tasksize() const {
  return _has_bit(0);
}
inline void Tasks_Task::clear_tasksize() {
  tasksize_ = 0;
  _clear_bit(0);
}
inline ::google::protobuf::int32 Tasks_Task::tasksize() const {
  return tasksize_;
}
inline void Tasks_Task::set_tasksize(::google::protobuf::int32 value) {
  _set_bit(0);
  tasksize_ = value;
}

// required string task = 2;
inline bool Tasks_Task::has_task() const {
  return _has_bit(1);
}
inline void Tasks_Task::clear_task() {
  if (task_ != &_default_task_) {
    task_->clear();
  }
  _clear_bit(1);
}
inline const ::std::string& Tasks_Task::task() const {
  return *task_;
}
inline void Tasks_Task::set_task(const ::std::string& value) {
  _set_bit(1);
  if (task_ == &_default_task_) {
    task_ = new ::std::string;
  }
  task_->assign(value);
}
inline void Tasks_Task::set_task(const char* value) {
  _set_bit(1);
  if (task_ == &_default_task_) {
    task_ = new ::std::string;
  }
  task_->assign(value);
}
inline void Tasks_Task::set_task(const char* value, size_t size) {
  _set_bit(1);
  if (task_ == &_default_task_) {
    task_ = new ::std::string;
  }
  task_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Tasks_Task::mutable_task() {
  _set_bit(1);
  if (task_ == &_default_task_) {
    task_ = new ::std::string;
  }
  return task_;
}

// -------------------------------------------------------------------

// Tasks

// required string batchhash = 1;
inline bool Tasks::has_batchhash() const {
  return _has_bit(0);
}
inline void Tasks::clear_batchhash() {
  if (batchhash_ != &_default_batchhash_) {
    batchhash_->clear();
  }
  _clear_bit(0);
}
inline const ::std::string& Tasks::batchhash() const {
  return *batchhash_;
}
inline void Tasks::set_batchhash(const ::std::string& value) {
  _set_bit(0);
  if (batchhash_ == &_default_batchhash_) {
    batchhash_ = new ::std::string;
  }
  batchhash_->assign(value);
}
inline void Tasks::set_batchhash(const char* value) {
  _set_bit(0);
  if (batchhash_ == &_default_batchhash_) {
    batchhash_ = new ::std::string;
  }
  batchhash_->assign(value);
}
inline void Tasks::set_batchhash(const char* value, size_t size) {
  _set_bit(0);
  if (batchhash_ == &_default_batchhash_) {
    batchhash_ = new ::std::string;
  }
  batchhash_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Tasks::mutable_batchhash() {
  _set_bit(0);
  if (batchhash_ == &_default_batchhash_) {
    batchhash_ = new ::std::string;
  }
  return batchhash_;
}

// repeated .client.Tasks.Task task = 2;
inline int Tasks::task_size() const {
  return task_.size();
}
inline void Tasks::clear_task() {
  task_.Clear();
}
inline const ::client::Tasks_Task& Tasks::task(int index) const {
  return task_.Get(index);
}
inline ::client::Tasks_Task* Tasks::mutable_task(int index) {
  return task_.Mutable(index);
}
inline ::client::Tasks_Task* Tasks::add_task() {
  return task_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::client::Tasks_Task >&
Tasks::task() const {
  return task_;
}
inline ::google::protobuf::RepeatedPtrField< ::client::Tasks_Task >*
Tasks::mutable_task() {
  return &task_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace client

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_client_2eproto__INCLUDED
