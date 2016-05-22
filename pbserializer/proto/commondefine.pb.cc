// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: commondefine.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "commondefine.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace Micro {
namespace Future {
namespace Message {
namespace Business {

namespace {

const ::google::protobuf::Descriptor* DataHeader_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  DataHeader_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_commondefine_2eproto() {
  protobuf_AddDesc_commondefine_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "commondefine.proto");
  GOOGLE_CHECK(file != NULL);
  DataHeader_descriptor_ = file->message_type(0);
  static const int DataHeader_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DataHeader, serialid_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DataHeader, hasmore_),
  };
  DataHeader_reflection_ =
    ::google::protobuf::internal::GeneratedMessageReflection::NewGeneratedMessageReflection(
      DataHeader_descriptor_,
      DataHeader::default_instance_,
      DataHeader_offsets_,
      -1,
      -1,
      -1,
      sizeof(DataHeader),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DataHeader, _internal_metadata_),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DataHeader, _is_default_instance_));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_commondefine_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
      DataHeader_descriptor_, &DataHeader::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_commondefine_2eproto() {
  delete DataHeader::default_instance_;
  delete DataHeader_reflection_;
}

void protobuf_AddDesc_commondefine_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\022commondefine.proto\022\035Micro.Future.Messa"
    "ge.Business\"/\n\nDataHeader\022\020\n\010serialId\030\001 "
    "\001(\r\022\017\n\007hasMore\030\002 \001(\010b\006proto3", 108);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "commondefine.proto", &protobuf_RegisterTypes);
  DataHeader::default_instance_ = new DataHeader();
  DataHeader::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_commondefine_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_commondefine_2eproto {
  StaticDescriptorInitializer_commondefine_2eproto() {
    protobuf_AddDesc_commondefine_2eproto();
  }
} static_descriptor_initializer_commondefine_2eproto_;

namespace {

static void MergeFromFail(int line) GOOGLE_ATTRIBUTE_COLD;
static void MergeFromFail(int line) {
  GOOGLE_CHECK(false) << __FILE__ << ":" << line;
}

}  // namespace


// ===================================================================

#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int DataHeader::kSerialIdFieldNumber;
const int DataHeader::kHasMoreFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

DataHeader::DataHeader()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Micro.Future.Message.Business.DataHeader)
}

void DataHeader::InitAsDefaultInstance() {
  _is_default_instance_ = true;
}

DataHeader::DataHeader(const DataHeader& from)
  : ::google::protobuf::Message(),
    _internal_metadata_(NULL) {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:Micro.Future.Message.Business.DataHeader)
}

void DataHeader::SharedCtor() {
    _is_default_instance_ = false;
  _cached_size_ = 0;
  serialid_ = 0u;
  hasmore_ = false;
}

DataHeader::~DataHeader() {
  // @@protoc_insertion_point(destructor:Micro.Future.Message.Business.DataHeader)
  SharedDtor();
}

void DataHeader::SharedDtor() {
  if (this != default_instance_) {
  }
}

void DataHeader::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* DataHeader::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return DataHeader_descriptor_;
}

const DataHeader& DataHeader::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_commondefine_2eproto();
  return *default_instance_;
}

DataHeader* DataHeader::default_instance_ = NULL;

DataHeader* DataHeader::New(::google::protobuf::Arena* arena) const {
  DataHeader* n = new DataHeader;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void DataHeader::Clear() {
// @@protoc_insertion_point(message_clear_start:Micro.Future.Message.Business.DataHeader)
#if defined(__clang__)
#define ZR_HELPER_(f) \
  _Pragma("clang diagnostic push") \
  _Pragma("clang diagnostic ignored \"-Winvalid-offsetof\"") \
  __builtin_offsetof(DataHeader, f) \
  _Pragma("clang diagnostic pop")
#else
#define ZR_HELPER_(f) reinterpret_cast<char*>(\
  &reinterpret_cast<DataHeader*>(16)->f)
#endif

#define ZR_(first, last) do {\
  ::memset(&first, 0,\
           ZR_HELPER_(last) - ZR_HELPER_(first) + sizeof(last));\
} while (0)

  ZR_(serialid_, hasmore_);

#undef ZR_HELPER_
#undef ZR_

}

bool DataHeader::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:Micro.Future.Message.Business.DataHeader)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional uint32 serialId = 1;
      case 1: {
        if (tag == 8) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &serialid_)));

        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(16)) goto parse_hasMore;
        break;
      }

      // optional bool hasMore = 2;
      case 2: {
        if (tag == 16) {
         parse_hasMore:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   bool, ::google::protobuf::internal::WireFormatLite::TYPE_BOOL>(
                 input, &hasmore_)));

        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormatLite::SkipField(input, tag));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:Micro.Future.Message.Business.DataHeader)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:Micro.Future.Message.Business.DataHeader)
  return false;
#undef DO_
}

void DataHeader::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:Micro.Future.Message.Business.DataHeader)
  // optional uint32 serialId = 1;
  if (this->serialid() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(1, this->serialid(), output);
  }

  // optional bool hasMore = 2;
  if (this->hasmore() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteBool(2, this->hasmore(), output);
  }

  // @@protoc_insertion_point(serialize_end:Micro.Future.Message.Business.DataHeader)
}

::google::protobuf::uint8* DataHeader::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:Micro.Future.Message.Business.DataHeader)
  // optional uint32 serialId = 1;
  if (this->serialid() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(1, this->serialid(), target);
  }

  // optional bool hasMore = 2;
  if (this->hasmore() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteBoolToArray(2, this->hasmore(), target);
  }

  // @@protoc_insertion_point(serialize_to_array_end:Micro.Future.Message.Business.DataHeader)
  return target;
}

int DataHeader::ByteSize() const {
// @@protoc_insertion_point(message_byte_size_start:Micro.Future.Message.Business.DataHeader)
  int total_size = 0;

  // optional uint32 serialId = 1;
  if (this->serialid() != 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::UInt32Size(
        this->serialid());
  }

  // optional bool hasMore = 2;
  if (this->hasmore() != 0) {
    total_size += 1 + 1;
  }

  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void DataHeader::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:Micro.Future.Message.Business.DataHeader)
  if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
  const DataHeader* source = 
      ::google::protobuf::internal::DynamicCastToGenerated<const DataHeader>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:Micro.Future.Message.Business.DataHeader)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:Micro.Future.Message.Business.DataHeader)
    MergeFrom(*source);
  }
}

void DataHeader::MergeFrom(const DataHeader& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:Micro.Future.Message.Business.DataHeader)
  if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
  if (from.serialid() != 0) {
    set_serialid(from.serialid());
  }
  if (from.hasmore() != 0) {
    set_hasmore(from.hasmore());
  }
}

void DataHeader::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:Micro.Future.Message.Business.DataHeader)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void DataHeader::CopyFrom(const DataHeader& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Micro.Future.Message.Business.DataHeader)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool DataHeader::IsInitialized() const {

  return true;
}

void DataHeader::Swap(DataHeader* other) {
  if (other == this) return;
  InternalSwap(other);
}
void DataHeader::InternalSwap(DataHeader* other) {
  std::swap(serialid_, other->serialid_);
  std::swap(hasmore_, other->hasmore_);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  std::swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata DataHeader::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = DataHeader_descriptor_;
  metadata.reflection = DataHeader_reflection_;
  return metadata;
}

#if PROTOBUF_INLINE_NOT_IN_HEADERS
// DataHeader

// optional uint32 serialId = 1;
void DataHeader::clear_serialid() {
  serialid_ = 0u;
}
 ::google::protobuf::uint32 DataHeader::serialid() const {
  // @@protoc_insertion_point(field_get:Micro.Future.Message.Business.DataHeader.serialId)
  return serialid_;
}
 void DataHeader::set_serialid(::google::protobuf::uint32 value) {
  
  serialid_ = value;
  // @@protoc_insertion_point(field_set:Micro.Future.Message.Business.DataHeader.serialId)
}

// optional bool hasMore = 2;
void DataHeader::clear_hasmore() {
  hasmore_ = false;
}
 bool DataHeader::hasmore() const {
  // @@protoc_insertion_point(field_get:Micro.Future.Message.Business.DataHeader.hasMore)
  return hasmore_;
}
 void DataHeader::set_hasmore(bool value) {
  
  hasmore_ = value;
  // @@protoc_insertion_point(field_set:Micro.Future.Message.Business.DataHeader.hasMore)
}

#endif  // PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

}  // namespace Business
}  // namespace Message
}  // namespace Future
}  // namespace Micro

// @@protoc_insertion_point(global_scope)
