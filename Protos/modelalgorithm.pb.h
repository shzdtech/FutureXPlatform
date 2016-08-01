// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: modelalgorithm.proto

#ifndef PROTOBUF_modelalgorithm_2eproto__INCLUDED
#define PROTOBUF_modelalgorithm_2eproto__INCLUDED

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

// Internal implementation detail -- do not call these.
void protobuf_AddDesc_modelalgorithm_2eproto();
void protobuf_AssignDesc_modelalgorithm_2eproto();
void protobuf_ShutdownFile_modelalgorithm_2eproto();

class ModelParams;
class ModelParams_double_vector;

// ===================================================================

class ModelParams_double_vector : public ::google::protobuf::Message {
 public:
  ModelParams_double_vector();
  virtual ~ModelParams_double_vector();

  ModelParams_double_vector(const ModelParams_double_vector& from);

  inline ModelParams_double_vector& operator=(const ModelParams_double_vector& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const ModelParams_double_vector& default_instance();

  void Swap(ModelParams_double_vector* other);

  // implements Message ----------------------------------------------

  inline ModelParams_double_vector* New() const { return New(NULL); }

  ModelParams_double_vector* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ModelParams_double_vector& from);
  void MergeFrom(const ModelParams_double_vector& from);
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
  void InternalSwap(ModelParams_double_vector* other);
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

  // repeated double entry = 1;
  int entry_size() const;
  void clear_entry();
  static const int kEntryFieldNumber = 1;
  double entry(int index) const;
  void set_entry(int index, double value);
  void add_entry(double value);
  const ::google::protobuf::RepeatedField< double >&
      entry() const;
  ::google::protobuf::RepeatedField< double >*
      mutable_entry();

  // @@protoc_insertion_point(class_scope:Micro.Future.Message.ModelParams.double_vector)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool _is_default_instance_;
  ::google::protobuf::RepeatedField< double > entry_;
  mutable int _entry_cached_byte_size_;
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_modelalgorithm_2eproto();
  friend void protobuf_AssignDesc_modelalgorithm_2eproto();
  friend void protobuf_ShutdownFile_modelalgorithm_2eproto();

  void InitAsDefaultInstance();
  static ModelParams_double_vector* default_instance_;
};
// -------------------------------------------------------------------

class ModelParams : public ::google::protobuf::Message {
 public:
  ModelParams();
  virtual ~ModelParams();

  ModelParams(const ModelParams& from);

  inline ModelParams& operator=(const ModelParams& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const ModelParams& default_instance();

  void Swap(ModelParams* other);

  // implements Message ----------------------------------------------

  inline ModelParams* New() const { return New(NULL); }

  ModelParams* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ModelParams& from);
  void MergeFrom(const ModelParams& from);
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
  void InternalSwap(ModelParams* other);
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

  typedef ModelParams_double_vector double_vector;

  // accessors -------------------------------------------------------

  // optional uint32 serialId = 1;
  void clear_serialid();
  static const int kSerialIdFieldNumber = 1;
  ::google::protobuf::uint32 serialid() const;
  void set_serialid(::google::protobuf::uint32 value);

  // repeated double values = 2;
  int values_size() const;
  void clear_values();
  static const int kValuesFieldNumber = 2;
  double values(int index) const;
  void set_values(int index, double value);
  void add_values(double value);
  const ::google::protobuf::RepeatedField< double >&
      values() const;
  ::google::protobuf::RepeatedField< double >*
      mutable_values();

  // optional string modelName = 3;
  void clear_modelname();
  static const int kModelNameFieldNumber = 3;
  const ::std::string& modelname() const;
  void set_modelname(const ::std::string& value);
  void set_modelname(const char* value);
  void set_modelname(const char* value, size_t size);
  ::std::string* mutable_modelname();
  ::std::string* release_modelname();
  void set_allocated_modelname(::std::string* modelname);

  // map<string, double> scalaParams = 4;
  int scalaparams_size() const;
  void clear_scalaparams();
  static const int kScalaParamsFieldNumber = 4;
  const ::google::protobuf::Map< ::std::string, double >&
      scalaparams() const;
  ::google::protobuf::Map< ::std::string, double >*
      mutable_scalaparams();

  // map<string, .Micro.Future.Message.ModelParams.double_vector> vectorParams = 5;
  int vectorparams_size() const;
  void clear_vectorparams();
  static const int kVectorParamsFieldNumber = 5;
  const ::google::protobuf::Map< ::std::string, ::Micro::Future::Message::ModelParams_double_vector >&
      vectorparams() const;
  ::google::protobuf::Map< ::std::string, ::Micro::Future::Message::ModelParams_double_vector >*
      mutable_vectorparams();

  // @@protoc_insertion_point(class_scope:Micro.Future.Message.ModelParams)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool _is_default_instance_;
  ::google::protobuf::RepeatedField< double > values_;
  mutable int _values_cached_byte_size_;
  ::google::protobuf::internal::ArenaStringPtr modelname_;
  typedef ::google::protobuf::internal::MapEntryLite<
      ::std::string, double,
      ::google::protobuf::internal::WireFormatLite::TYPE_STRING,
      ::google::protobuf::internal::WireFormatLite::TYPE_DOUBLE,
      0 >
      ModelParams_ScalaParamsEntry;
  ::google::protobuf::internal::MapField<
      ::std::string, double,
      ::google::protobuf::internal::WireFormatLite::TYPE_STRING,
      ::google::protobuf::internal::WireFormatLite::TYPE_DOUBLE,
      0 > scalaparams_;
  typedef ::google::protobuf::internal::MapEntryLite<
      ::std::string, ::Micro::Future::Message::ModelParams_double_vector,
      ::google::protobuf::internal::WireFormatLite::TYPE_STRING,
      ::google::protobuf::internal::WireFormatLite::TYPE_MESSAGE,
      0 >
      ModelParams_VectorParamsEntry;
  ::google::protobuf::internal::MapField<
      ::std::string, ::Micro::Future::Message::ModelParams_double_vector,
      ::google::protobuf::internal::WireFormatLite::TYPE_STRING,
      ::google::protobuf::internal::WireFormatLite::TYPE_MESSAGE,
      0 > vectorparams_;
  ::google::protobuf::uint32 serialid_;
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_modelalgorithm_2eproto();
  friend void protobuf_AssignDesc_modelalgorithm_2eproto();
  friend void protobuf_ShutdownFile_modelalgorithm_2eproto();

  void InitAsDefaultInstance();
  static ModelParams* default_instance_;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// ModelParams_double_vector

// repeated double entry = 1;
inline int ModelParams_double_vector::entry_size() const {
  return entry_.size();
}
inline void ModelParams_double_vector::clear_entry() {
  entry_.Clear();
}
inline double ModelParams_double_vector::entry(int index) const {
  // @@protoc_insertion_point(field_get:Micro.Future.Message.ModelParams.double_vector.entry)
  return entry_.Get(index);
}
inline void ModelParams_double_vector::set_entry(int index, double value) {
  entry_.Set(index, value);
  // @@protoc_insertion_point(field_set:Micro.Future.Message.ModelParams.double_vector.entry)
}
inline void ModelParams_double_vector::add_entry(double value) {
  entry_.Add(value);
  // @@protoc_insertion_point(field_add:Micro.Future.Message.ModelParams.double_vector.entry)
}
inline const ::google::protobuf::RepeatedField< double >&
ModelParams_double_vector::entry() const {
  // @@protoc_insertion_point(field_list:Micro.Future.Message.ModelParams.double_vector.entry)
  return entry_;
}
inline ::google::protobuf::RepeatedField< double >*
ModelParams_double_vector::mutable_entry() {
  // @@protoc_insertion_point(field_mutable_list:Micro.Future.Message.ModelParams.double_vector.entry)
  return &entry_;
}

// -------------------------------------------------------------------

// ModelParams

// optional uint32 serialId = 1;
inline void ModelParams::clear_serialid() {
  serialid_ = 0u;
}
inline ::google::protobuf::uint32 ModelParams::serialid() const {
  // @@protoc_insertion_point(field_get:Micro.Future.Message.ModelParams.serialId)
  return serialid_;
}
inline void ModelParams::set_serialid(::google::protobuf::uint32 value) {
  
  serialid_ = value;
  // @@protoc_insertion_point(field_set:Micro.Future.Message.ModelParams.serialId)
}

// repeated double values = 2;
inline int ModelParams::values_size() const {
  return values_.size();
}
inline void ModelParams::clear_values() {
  values_.Clear();
}
inline double ModelParams::values(int index) const {
  // @@protoc_insertion_point(field_get:Micro.Future.Message.ModelParams.values)
  return values_.Get(index);
}
inline void ModelParams::set_values(int index, double value) {
  values_.Set(index, value);
  // @@protoc_insertion_point(field_set:Micro.Future.Message.ModelParams.values)
}
inline void ModelParams::add_values(double value) {
  values_.Add(value);
  // @@protoc_insertion_point(field_add:Micro.Future.Message.ModelParams.values)
}
inline const ::google::protobuf::RepeatedField< double >&
ModelParams::values() const {
  // @@protoc_insertion_point(field_list:Micro.Future.Message.ModelParams.values)
  return values_;
}
inline ::google::protobuf::RepeatedField< double >*
ModelParams::mutable_values() {
  // @@protoc_insertion_point(field_mutable_list:Micro.Future.Message.ModelParams.values)
  return &values_;
}

// optional string modelName = 3;
inline void ModelParams::clear_modelname() {
  modelname_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& ModelParams::modelname() const {
  // @@protoc_insertion_point(field_get:Micro.Future.Message.ModelParams.modelName)
  return modelname_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void ModelParams::set_modelname(const ::std::string& value) {
  
  modelname_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:Micro.Future.Message.ModelParams.modelName)
}
inline void ModelParams::set_modelname(const char* value) {
  
  modelname_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:Micro.Future.Message.ModelParams.modelName)
}
inline void ModelParams::set_modelname(const char* value, size_t size) {
  
  modelname_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:Micro.Future.Message.ModelParams.modelName)
}
inline ::std::string* ModelParams::mutable_modelname() {
  
  // @@protoc_insertion_point(field_mutable:Micro.Future.Message.ModelParams.modelName)
  return modelname_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* ModelParams::release_modelname() {
  // @@protoc_insertion_point(field_release:Micro.Future.Message.ModelParams.modelName)
  
  return modelname_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void ModelParams::set_allocated_modelname(::std::string* modelname) {
  if (modelname != NULL) {
    
  } else {
    
  }
  modelname_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), modelname);
  // @@protoc_insertion_point(field_set_allocated:Micro.Future.Message.ModelParams.modelName)
}

// map<string, double> scalaParams = 4;
inline int ModelParams::scalaparams_size() const {
  return scalaparams_.size();
}
inline void ModelParams::clear_scalaparams() {
  scalaparams_.Clear();
}
inline const ::google::protobuf::Map< ::std::string, double >&
ModelParams::scalaparams() const {
  // @@protoc_insertion_point(field_map:Micro.Future.Message.ModelParams.scalaParams)
  return scalaparams_.GetMap();
}
inline ::google::protobuf::Map< ::std::string, double >*
ModelParams::mutable_scalaparams() {
  // @@protoc_insertion_point(field_mutable_map:Micro.Future.Message.ModelParams.scalaParams)
  return scalaparams_.MutableMap();
}

// map<string, .Micro.Future.Message.ModelParams.double_vector> vectorParams = 5;
inline int ModelParams::vectorparams_size() const {
  return vectorparams_.size();
}
inline void ModelParams::clear_vectorparams() {
  vectorparams_.Clear();
}
inline const ::google::protobuf::Map< ::std::string, ::Micro::Future::Message::ModelParams_double_vector >&
ModelParams::vectorparams() const {
  // @@protoc_insertion_point(field_map:Micro.Future.Message.ModelParams.vectorParams)
  return vectorparams_.GetMap();
}
inline ::google::protobuf::Map< ::std::string, ::Micro::Future::Message::ModelParams_double_vector >*
ModelParams::mutable_vectorparams() {
  // @@protoc_insertion_point(field_mutable_map:Micro.Future.Message.ModelParams.vectorParams)
  return vectorparams_.MutableMap();
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace Message
}  // namespace Future
}  // namespace Micro

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_modelalgorithm_2eproto__INCLUDED
