// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_CHANNEL_OPENEPHYSFLATBUFFER_H_
#define FLATBUFFERS_GENERATED_CHANNEL_OPENEPHYSFLATBUFFER_H_

#include "flatbuffers/flatbuffers.h"

namespace openephysflatbuffer {

struct ContinuousData;
struct ContinuousDataBuilder;

struct ContinuousData FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef ContinuousDataBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_SAMPLES = 4,
    VT_EVENT_CODES = 6,
    VT_STREAM = 8,
    VT_N_CHANNELS = 10,
    VT_N_SAMPLES = 12,
    VT_SAMPLE_NUM = 14,
    VT_TIMESTAMP = 16,
    VT_MESSAGE_ID = 18,
    VT_SAMPLE_RATE = 20
  };
  const flatbuffers::Vector<float> *samples() const {
    return GetPointer<const flatbuffers::Vector<float> *>(VT_SAMPLES);
  }
  const flatbuffers::Vector<uint16_t> *event_codes() const {
    return GetPointer<const flatbuffers::Vector<uint16_t> *>(VT_EVENT_CODES);
  }
  const flatbuffers::String *stream() const {
    return GetPointer<const flatbuffers::String *>(VT_STREAM);
  }
  uint32_t n_channels() const {
    return GetField<uint32_t>(VT_N_CHANNELS, 0);
  }
  uint32_t n_samples() const {
    return GetField<uint32_t>(VT_N_SAMPLES, 0);
  }
  uint64_t sample_num() const {
    return GetField<uint64_t>(VT_SAMPLE_NUM, 0);
  }
  double timestamp() const {
    return GetField<double>(VT_TIMESTAMP, 0.0);
  }
  uint64_t message_id() const {
    return GetField<uint64_t>(VT_MESSAGE_ID, 0);
  }
  uint32_t sample_rate() const {
    return GetField<uint32_t>(VT_SAMPLE_RATE, 0);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_SAMPLES) &&
           verifier.VerifyVector(samples()) &&
           VerifyOffset(verifier, VT_EVENT_CODES) &&
           verifier.VerifyVector(event_codes()) &&
           VerifyOffset(verifier, VT_STREAM) &&
           verifier.VerifyString(stream()) &&
           VerifyField<uint32_t>(verifier, VT_N_CHANNELS) &&
           VerifyField<uint32_t>(verifier, VT_N_SAMPLES) &&
           VerifyField<uint64_t>(verifier, VT_SAMPLE_NUM) &&
           VerifyField<double>(verifier, VT_TIMESTAMP) &&
           VerifyField<uint64_t>(verifier, VT_MESSAGE_ID) &&
           VerifyField<uint32_t>(verifier, VT_SAMPLE_RATE) &&
           verifier.EndTable();
  }
};

struct ContinuousDataBuilder {
  typedef ContinuousData Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_samples(flatbuffers::Offset<flatbuffers::Vector<float>> samples) {
    fbb_.AddOffset(ContinuousData::VT_SAMPLES, samples);
  }
  void add_event_codes(flatbuffers::Offset<flatbuffers::Vector<uint16_t>> event_codes) {
    fbb_.AddOffset(ContinuousData::VT_EVENT_CODES, event_codes);
  }
  void add_stream(flatbuffers::Offset<flatbuffers::String> stream) {
    fbb_.AddOffset(ContinuousData::VT_STREAM, stream);
  }
  void add_n_channels(uint32_t n_channels) {
    fbb_.AddElement<uint32_t>(ContinuousData::VT_N_CHANNELS, n_channels, 0);
  }
  void add_n_samples(uint32_t n_samples) {
    fbb_.AddElement<uint32_t>(ContinuousData::VT_N_SAMPLES, n_samples, 0);
  }
  void add_sample_num(uint64_t sample_num) {
    fbb_.AddElement<uint64_t>(ContinuousData::VT_SAMPLE_NUM, sample_num, 0);
  }
  void add_timestamp(double timestamp) {
    fbb_.AddElement<double>(ContinuousData::VT_TIMESTAMP, timestamp, 0.0);
  }
  void add_message_id(uint64_t message_id) {
    fbb_.AddElement<uint64_t>(ContinuousData::VT_MESSAGE_ID, message_id, 0);
  }
  void add_sample_rate(uint32_t sample_rate) {
    fbb_.AddElement<uint32_t>(ContinuousData::VT_SAMPLE_RATE, sample_rate, 0);
  }
  explicit ContinuousDataBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<ContinuousData> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<ContinuousData>(end);
    return o;
  }
};

inline flatbuffers::Offset<ContinuousData> CreateContinuousData(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<float>> samples = 0,
    flatbuffers::Offset<flatbuffers::Vector<uint16_t>> event_codes = 0,
    flatbuffers::Offset<flatbuffers::String> stream = 0,
    uint32_t n_channels = 0,
    uint32_t n_samples = 0,
    uint64_t sample_num = 0,
    double timestamp = 0.0,
    uint64_t message_id = 0,
    uint32_t sample_rate = 0) {
  ContinuousDataBuilder builder_(_fbb);
  builder_.add_message_id(message_id);
  builder_.add_timestamp(timestamp);
  builder_.add_sample_num(sample_num);
  builder_.add_sample_rate(sample_rate);
  builder_.add_n_samples(n_samples);
  builder_.add_n_channels(n_channels);
  builder_.add_stream(stream);
  builder_.add_event_codes(event_codes);
  builder_.add_samples(samples);
  return builder_.Finish();
}

inline flatbuffers::Offset<ContinuousData> CreateContinuousDataDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<float> *samples = nullptr,
    const std::vector<uint16_t> *event_codes = nullptr,
    const char *stream = nullptr,
    uint32_t n_channels = 0,
    uint32_t n_samples = 0,
    uint64_t sample_num = 0,
    double timestamp = 0.0,
    uint64_t message_id = 0,
    uint32_t sample_rate = 0) {
  auto samples__ = samples ? _fbb.CreateVector<float>(*samples) : 0;
  auto event_codes__ = event_codes ? _fbb.CreateVector<uint16_t>(*event_codes) : 0;
  auto stream__ = stream ? _fbb.CreateString(stream) : 0;
  return openephysflatbuffer::CreateContinuousData(
      _fbb,
      samples__,
      event_codes__,
      stream__,
      n_channels,
      n_samples,
      sample_num,
      timestamp,
      message_id,
      sample_rate);
}

inline const openephysflatbuffer::ContinuousData *GetContinuousData(const void *buf) {
  return flatbuffers::GetRoot<openephysflatbuffer::ContinuousData>(buf);
}

inline const openephysflatbuffer::ContinuousData *GetSizePrefixedContinuousData(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<openephysflatbuffer::ContinuousData>(buf);
}

inline bool VerifyContinuousDataBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<openephysflatbuffer::ContinuousData>(nullptr);
}

inline bool VerifySizePrefixedContinuousDataBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<openephysflatbuffer::ContinuousData>(nullptr);
}

inline void FinishContinuousDataBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<openephysflatbuffer::ContinuousData> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedContinuousDataBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<openephysflatbuffer::ContinuousData> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace openephysflatbuffer

#endif  // FLATBUFFERS_GENERATED_CHANNEL_OPENEPHYSFLATBUFFER_H_
