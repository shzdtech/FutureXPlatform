// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: commondefine.proto

#ifndef PROTOBUF_commondefine_2eproto__INCLUDED
#define PROTOBUF_commondefine_2eproto__INCLUDED

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
#include <google/protobuf/map.h>
#include <google/protobuf/map_field_inl.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace Micro {
namespace Future {
namespace Message {
namespace Business {

// Internal implementation detail -- do not call these.
void protobuf_AddDesc_commondefine_2eproto();
void protobuf_AssignDesc_commondefine_2eproto();
void protobuf_ShutdownFile_commondefine_2eproto();

class DataHeader;
class StringMap;

// ===================================================================

class DataHeader : public ::google::protobuf::Message {
 public:
  DataHeader();
  virtual ~DataHeader();

  DataHeader(const DataHeader& from);

  inline DataHeader& operator=(const DataHeader& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const DataHeader& default_instance();

  void Swap(DataHeader* other);

  // implements Message ----------------------------------------------

  inline DataHeader* New() const { return New(NULL); }

  DataHeader* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const DataHeader& from);
  void MergeFrom(const DataHeader& from);
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
  void InternalSwap(DataHeader* other);
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

  // optional bool hasMore = 2;
  void clear_hasmore();
  static const int kHasMoreFieldNumber = 2;
  bool hasmore() const;
  void set_hasmore(bool value);

  // @@protoc_insertion_point(class_scope:Micro.Future.Message.Business.DataHeader)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool _is_default_instance_;
  ::google::protobuf::uint32 serialid_;
  bool hasmore_;
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_commondefine_2eproto();
  friend void protobuf_AssignDesc_commondefine_2eproto();
  friend void protobuf_ShutdownFile_commondefine_2eproto();

  void InitAsDefaultInstance();
  static DataHeader* default_instance_;
};
// -------------------------------------------------------------------

class StringMap : public ::google::protobuf::Message {
 public:
  StringMap();
  virtual ~StringMap();

  StringMap(const StringMap& from);

  inline StringMap& operator=(const StringMap& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const StringMap& default_instance();

  void Swap(StringMap* other);

  // implements Message ----------------------------------------------

  inline StringMap* New() const { return New(NULL); }

  StringMap* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const StringMap& from);
  void MergeFrom(const StringMap& from);
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
  void InternalSwap(StringMap* other);
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

  // optional .Micro.Future.Message.Business.DataHeader header = 1;
  bool has_header() const;
  void clear_header();
  static const int kHeaderFieldNumber = 1;
  const ::Micro::Future::Message::Business::DataHeader& header() const;
  ::Micro::Future::Message::Business::DataHeader* mutable_header();
  ::Micro::Future::Message::Business::DataHeader* release_header();
  void set_allocated_header(::Micro::Future::Message::Business::DataHeader* header);

  // map<string, string> entry = 2;
  int entry_size() const;
  void clear_entry();
  static const int kEntryFieldNumber = 2;
  const ::google::protobuf::Map< ::std::string, ::std::string >&
      entry() const;
  ::google::protobuf::Map< ::std::string, ::std::string >*
      mutable_entry();

  // @@protoc_insertion_point(class_scope:Micro.Future.Message.Business.StringMap)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool _is_default_instance_;
  ::Micro::Future::Message::Business::DataHeader* header_;
  typedef ::google::protobuf::internal::MapEntryLite<
      ::std::string, ::std::string,
      ::google::protobuf::internal::WireFormatLite::TYPE_STRING,
      ::google::protobuf::internal::WireFormatLite::TYPE_STRING,
      0 >
      StringMap_EntryEntry;
  ::google::protobuf::internal::MapField<
      ::std::string, ::std::string,
      ::google::protobuf::internal::WireFormatLite::TYPE_STRING,
      ::google::protobuf::internal::WireFormatLite::TYPE_STRING,
      0 > entry_;
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_commondefine_2eproto();
  friend void protobuf_AssignDesc_commondefine_2eproto();
  friend void protobuf_ShutdownFile_commondefine_2eproto();

  void InitAsDefaultInstance();
  static StringMap* default_instance_;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// DataHeader

// optional uint32 serialId = 1;
inline void DataHeader::clear_serialid() {
  serialid_ = 0u;
}
inline ::google::protobuf::uint32 DataHeader::serialid() const {
  // @@protoc_insertion_point(field_get:Micro.Future.Message.Business.DataHeader.serialId)
  return serialid_;
}
inline void DataHeader::set_serialid(::google::protobuf::uint32 value) {
  
  serialid_ = value;
  // @@protoc_insertion_point(field_set:Micro.Future.Message.Business.DataHeader.serialId)
}

// optional bool hasMore = 2;
inline void DataHeader::clear_hasmore() {
  hasmore_ = false;
}
inline bool DataHeader::hasmore() const {
  // @@protoc_insertion_point(field_get:Micro.Future.Message.Business.DataHeader.hasMore)
  return hasmore_;
}
inline void DataHeader::set_hasmore(bool value) {
  
  hasmore_ = value;
  // @@protoc_insertion_point(field_set:Micro.Future.Message.Business.DataHeader.hasMore)
}

// -------------------------------------------------------------------

// StringMap

// optional .Micro.Future.Message.Business.DataHeader header = 1;
inline bool StringMap::has_header() const {
  return !_is_default_instance_ && header_ != NULL;
}
inline void StringMap::clear_header() {
  if (GetArenaNoVirtual() == NULL && header_ != NULL) delete header_;
  header_ = NULL;
}
inline const ::Micro::Future::Message::Business::DataHeader& StringMap::header() const {
  // @@protoc_insertion_point(field_get:Micro.Future.Message.Business.StringMap.header)
  return header_ != NULL ? *header_ : *default_instance_->header_;
}
inline ::Micro::Future::Message::Business::DataHeader* StringMap::mutable_header() {
  
  if (header_ == NULL) {
    header_ = new ::Micro::Future::Message::Business::DataHeader;
  }
  // @@protoc_insertion_point(field_mutable:Micro.Future.Message.Business.StringMap.header)
  return header_;
}
inline ::Micro::Future::Message::Business::DataHeader* StringMap::release_header() {
  // @@protoc_insertion_point(field_release:Micro.Future.Message.Business.StringMap.header)
  
  ::Micro::Future::Message::Business::DataHeader* temp = header_;
  header_ = NULL;
  return temp;
}
inline void StringMap::set_allocated_header(::Micro::Future::Message::Business::DataHeader* header) {
  delete header_;
  header_ = header;
  if (header) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_set_allocated:Micro.Future.Message.Business.StringMap.header)
}

// map<string, string> entry = 2;
inline int StringMap::entry_size() const {
  return entry_.size();
}
inline void StringMap::clear_entry() {
  entry_.Clear();
}
inline const ::google::protobuf::Map< ::std::string, ::std::string >&
StringMap::entry() const {
  // @@protoc_insertion_point(field_map:Micro.Future.Message.Business.StringMap.entry)
  return entry_.GetMap();
}
inline ::google::protobuf::Map< ::std::string, ::std::string >*
StringMap::mutable_entry() {
  // @@protoc_insertion_point(field_mutable_map:Micro.Future.Message.Business.StringMap.entry)
  return entry_.MutableMap();
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace Business
}  // namespace Message
}  // namespace Future
}  // namespace Micro

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_commondefine_2eproto__INCLUDED
