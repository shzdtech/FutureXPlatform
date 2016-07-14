// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: errormsg.proto

#ifndef PROTOBUF_errormsg_2eproto__INCLUDED
#define PROTOBUF_errormsg_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3000000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3000000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace Micro {
namespace Future {
namespace Message {

// Internal implementation detail -- do not call these.
void protobuf_AddDesc_errormsg_2eproto();
void protobuf_AssignDesc_errormsg_2eproto();
void protobuf_ShutdownFile_errormsg_2eproto();

class ExceptionMessage;
class RawData;
class Result;

// ===================================================================

class ExceptionMessage : public ::google::protobuf::Message {
 public:
  ExceptionMessage();
  virtual ~ExceptionMessage();

  ExceptionMessage(const ExceptionMessage& from);

  inline ExceptionMessage& operator=(const ExceptionMessage& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const ExceptionMessage& default_instance();

  void Swap(ExceptionMessage* other);

  // implements Message ----------------------------------------------

  inline ExceptionMessage* New() const { return New(NULL); }

  ExceptionMessage* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ExceptionMessage& from);
  void MergeFrom(const ExceptionMessage& from);
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
  void InternalSwap(ExceptionMessage* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const {
    return _internal_metadata_.raw_arena_ptr();
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional uint32 serialId = 1;
  void clear_serialid();
  static const int kSerialIdFieldNumber = 1;
  ::google::protobuf::uint32 serialid() const;
  void set_serialid(::google::protobuf::uint32 value);

  // optional uint32 messageId = 2;
  void clear_messageid();
  static const int kMessageIdFieldNumber = 2;
  ::google::protobuf::uint32 messageid() const;
  void set_messageid(::google::protobuf::uint32 value);

  // optional int32 errortype = 3;
  void clear_errortype();
  static const int kErrortypeFieldNumber = 3;
  ::google::protobuf::int32 errortype() const;
  void set_errortype(::google::protobuf::int32 value);

  // optional int32 errorcode = 4;
  void clear_errorcode();
  static const int kErrorcodeFieldNumber = 4;
  ::google::protobuf::int32 errorcode() const;
  void set_errorcode(::google::protobuf::int32 value);

  // optional bytes description = 5;
  void clear_description();
  static const int kDescriptionFieldNumber = 5;
  const ::std::string& description() const;
  void set_description(const ::std::string& value);
  void set_description(const char* value);
  void set_description(const void* value, size_t size);
  ::std::string* mutable_description();
  ::std::string* release_description();
  void set_allocated_description(::std::string* description);

  // @@protoc_insertion_point(class_scope:Micro.Future.Message.ExceptionMessage)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool _is_default_instance_;
  ::google::protobuf::uint32 serialid_;
  ::google::protobuf::uint32 messageid_;
  ::google::protobuf::int32 errortype_;
  ::google::protobuf::int32 errorcode_;
  ::google::protobuf::internal::ArenaStringPtr description_;
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_errormsg_2eproto();
  friend void protobuf_AssignDesc_errormsg_2eproto();
  friend void protobuf_ShutdownFile_errormsg_2eproto();

  void InitAsDefaultInstance();
  static ExceptionMessage* default_instance_;
};
// -------------------------------------------------------------------

class Result : public ::google::protobuf::Message {
 public:
  Result();
  virtual ~Result();

  Result(const Result& from);

  inline Result& operator=(const Result& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Result& default_instance();

  void Swap(Result* other);

  // implements Message ----------------------------------------------

  inline Result* New() const { return New(NULL); }

  Result* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Result& from);
  void MergeFrom(const Result& from);
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
  void InternalSwap(Result* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const {
    return _internal_metadata_.raw_arena_ptr();
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional int32 code = 1;
  void clear_code();
  static const int kCodeFieldNumber = 1;
  ::google::protobuf::int32 code() const;
  void set_code(::google::protobuf::int32 value);

  // optional uint32 serialId = 2;
  void clear_serialid();
  static const int kSerialIdFieldNumber = 2;
  ::google::protobuf::uint32 serialid() const;
  void set_serialid(::google::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:Micro.Future.Message.Result)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool _is_default_instance_;
  ::google::protobuf::int32 code_;
  ::google::protobuf::uint32 serialid_;
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_errormsg_2eproto();
  friend void protobuf_AssignDesc_errormsg_2eproto();
  friend void protobuf_ShutdownFile_errormsg_2eproto();

  void InitAsDefaultInstance();
  static Result* default_instance_;
};
// -------------------------------------------------------------------

class RawData : public ::google::protobuf::Message {
 public:
  RawData();
  virtual ~RawData();

  RawData(const RawData& from);

  inline RawData& operator=(const RawData& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const RawData& default_instance();

  void Swap(RawData* other);

  // implements Message ----------------------------------------------

  inline RawData* New() const { return New(NULL); }

  RawData* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RawData& from);
  void MergeFrom(const RawData& from);
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
  void InternalSwap(RawData* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const {
    return _internal_metadata_.raw_arena_ptr();
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional bytes data = 1;
  void clear_data();
  static const int kDataFieldNumber = 1;
  const ::std::string& data() const;
  void set_data(const ::std::string& value);
  void set_data(const char* value);
  void set_data(const void* value, size_t size);
  ::std::string* mutable_data();
  ::std::string* release_data();
  void set_allocated_data(::std::string* data);

  // @@protoc_insertion_point(class_scope:Micro.Future.Message.RawData)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool _is_default_instance_;
  ::google::protobuf::internal::ArenaStringPtr data_;
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_errormsg_2eproto();
  friend void protobuf_AssignDesc_errormsg_2eproto();
  friend void protobuf_ShutdownFile_errormsg_2eproto();

  void InitAsDefaultInstance();
  static RawData* default_instance_;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// ExceptionMessage

// optional uint32 serialId = 1;
inline void ExceptionMessage::clear_serialid() {
  serialid_ = 0u;
}
inline ::google::protobuf::uint32 ExceptionMessage::serialid() const {
  // @@protoc_insertion_point(field_get:Micro.Future.Message.ExceptionMessage.serialId)
  return serialid_;
}
inline void ExceptionMessage::set_serialid(::google::protobuf::uint32 value) {
  
  serialid_ = value;
  // @@protoc_insertion_point(field_set:Micro.Future.Message.ExceptionMessage.serialId)
}

// optional uint32 messageId = 2;
inline void ExceptionMessage::clear_messageid() {
  messageid_ = 0u;
}
inline ::google::protobuf::uint32 ExceptionMessage::messageid() const {
  // @@protoc_insertion_point(field_get:Micro.Future.Message.ExceptionMessage.messageId)
  return messageid_;
}
inline void ExceptionMessage::set_messageid(::google::protobuf::uint32 value) {
  
  messageid_ = value;
  // @@protoc_insertion_point(field_set:Micro.Future.Message.ExceptionMessage.messageId)
}

// optional int32 errortype = 3;
inline void ExceptionMessage::clear_errortype() {
  errortype_ = 0;
}
inline ::google::protobuf::int32 ExceptionMessage::errortype() const {
  // @@protoc_insertion_point(field_get:Micro.Future.Message.ExceptionMessage.errortype)
  return errortype_;
}
inline void ExceptionMessage::set_errortype(::google::protobuf::int32 value) {
  
  errortype_ = value;
  // @@protoc_insertion_point(field_set:Micro.Future.Message.ExceptionMessage.errortype)
}

// optional int32 errorcode = 4;
inline void ExceptionMessage::clear_errorcode() {
  errorcode_ = 0;
}
inline ::google::protobuf::int32 ExceptionMessage::errorcode() const {
  // @@protoc_insertion_point(field_get:Micro.Future.Message.ExceptionMessage.errorcode)
  return errorcode_;
}
inline void ExceptionMessage::set_errorcode(::google::protobuf::int32 value) {
  
  errorcode_ = value;
  // @@protoc_insertion_point(field_set:Micro.Future.Message.ExceptionMessage.errorcode)
}

// optional bytes description = 5;
inline void ExceptionMessage::clear_description() {
  description_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& ExceptionMessage::description() const {
  // @@protoc_insertion_point(field_get:Micro.Future.Message.ExceptionMessage.description)
  return description_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void ExceptionMessage::set_description(const ::std::string& value) {
  
  description_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:Micro.Future.Message.ExceptionMessage.description)
}
inline void ExceptionMessage::set_description(const char* value) {
  
  description_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:Micro.Future.Message.ExceptionMessage.description)
}
inline void ExceptionMessage::set_description(const void* value, size_t size) {
  
  description_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:Micro.Future.Message.ExceptionMessage.description)
}
inline ::std::string* ExceptionMessage::mutable_description() {
  
  // @@protoc_insertion_point(field_mutable:Micro.Future.Message.ExceptionMessage.description)
  return description_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* ExceptionMessage::release_description() {
  // @@protoc_insertion_point(field_release:Micro.Future.Message.ExceptionMessage.description)
  
  return description_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void ExceptionMessage::set_allocated_description(::std::string* description) {
  if (description != NULL) {
    
  } else {
    
  }
  description_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), description);
  // @@protoc_insertion_point(field_set_allocated:Micro.Future.Message.ExceptionMessage.description)
}

// -------------------------------------------------------------------

// Result

// optional int32 code = 1;
inline void Result::clear_code() {
  code_ = 0;
}
inline ::google::protobuf::int32 Result::code() const {
  // @@protoc_insertion_point(field_get:Micro.Future.Message.Result.code)
  return code_;
}
inline void Result::set_code(::google::protobuf::int32 value) {
  
  code_ = value;
  // @@protoc_insertion_point(field_set:Micro.Future.Message.Result.code)
}

// optional uint32 serialId = 2;
inline void Result::clear_serialid() {
  serialid_ = 0u;
}
inline ::google::protobuf::uint32 Result::serialid() const {
  // @@protoc_insertion_point(field_get:Micro.Future.Message.Result.serialId)
  return serialid_;
}
inline void Result::set_serialid(::google::protobuf::uint32 value) {
  
  serialid_ = value;
  // @@protoc_insertion_point(field_set:Micro.Future.Message.Result.serialId)
}

// -------------------------------------------------------------------

// RawData

// optional bytes data = 1;
inline void RawData::clear_data() {
  data_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& RawData::data() const {
  // @@protoc_insertion_point(field_get:Micro.Future.Message.RawData.data)
  return data_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void RawData::set_data(const ::std::string& value) {
  
  data_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:Micro.Future.Message.RawData.data)
}
inline void RawData::set_data(const char* value) {
  
  data_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:Micro.Future.Message.RawData.data)
}
inline void RawData::set_data(const void* value, size_t size) {
  
  data_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:Micro.Future.Message.RawData.data)
}
inline ::std::string* RawData::mutable_data() {
  
  // @@protoc_insertion_point(field_mutable:Micro.Future.Message.RawData.data)
  return data_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* RawData::release_data() {
  // @@protoc_insertion_point(field_release:Micro.Future.Message.RawData.data)
  
  return data_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void RawData::set_allocated_data(::std::string* data) {
  if (data != NULL) {
    
  } else {
    
  }
  data_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), data);
  // @@protoc_insertion_point(field_set_allocated:Micro.Future.Message.RawData.data)
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS
// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace Message
}  // namespace Future
}  // namespace Micro

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_errormsg_2eproto__INCLUDED
