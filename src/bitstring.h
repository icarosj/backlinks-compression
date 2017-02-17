#ifndef BITSTRING_H_
#define BITSTRING_H_
#include <vector>
#include <inttypes.h>
#include <stdio.h>
using namespace std;
//
// BitString
//
class BitString {
 public:
  BitString() : data_(0), length_(0) {}
  BitString(const BitString &b) : data_(b.data_), length_(b.length_) {}
  BitString(vector<bool> boolVec)
    : data_(0), length_(0)
  {
    for(auto b:boolVec)
    {
      this->AppendBit(b);
    }
  }
  inline void Init(uint64_t length);
  inline void AppendBit(uint64_t val);
  inline void AppendBitString(const BitString &bitstr);
  inline void AppendBinary(const uint64_t val) ;
  inline void SetBit(uint64_t pos, uint64_t val);
  inline int GetBit(uint64_t pos) const;
  inline uint64_t get_length() const { return length_;}
  inline void Print() const;
  bool Input(const char *filename);
  bool Output(const char *filename);
 private:
  static const int kBITMASK63 = (1 << 6) - 1;
  std::vector<uint64_t> data_;
  uint64_t length_;
};



//
// BitString
//
inline void BitString::Init(uint64_t length) {
  data_.clear();
  data_.resize((length + 63) >> 6, 0);
  length_ = length;
}

inline void BitString::AppendBit(uint64_t val) {
  uint64_t index = length_ >> 6;
  uint64_t bit_pos = length_ & kBITMASK63;
  if (!bit_pos) {
    data_.push_back(val);
  } else {
    data_[index] |= val << bit_pos;
  }
  ++length_;
}

inline void BitString::AppendBitString(const BitString &bitstr) {
  for (uint64_t i = 0; i < bitstr.get_length(); ++i) {
    AppendBit(bitstr.GetBit(i));
  }
}

inline void BitString::AppendBinary(const uint64_t val) {
  int val_cp=val;
  for (uint64_t i = 0; val_cp!=0, val_cp!=1; ++i) {
    AppendBit(val_cp & 1);
    val_cp = val_cp >> 1 ;
  }
}


inline void BitString::SetBit(uint64_t pos, uint64_t val) {
  uint64_t index = pos >> 6;
  uint64_t bit_pos = pos & kBITMASK63;
  data_[index] ^= ((data_[index] >> bit_pos & 1) ^ val) << bit_pos;
}

inline int BitString::GetBit(uint64_t pos) const {
  uint64_t index = pos >> 6;
  uint64_t bit_pos = pos & kBITMASK63;
  return (data_[index] >> bit_pos & 1);
}

inline void BitString::Print() const {
  for (uint64_t i = 0; i < length_; ++i) printf("%d", GetBit(i));
}







#endif
