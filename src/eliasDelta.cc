#include "eliasDelta.h"
#include "p_assert.h"

  //
  // DeltaCode
  //
  void VLI_eliasDelta::EncodeInt(int64_t val, BitString *out) {
    out->Init(0);
    int length_val = CountBitLength(val + 1) - 1;
    int length_length_val = CountBitLength(length_val + 1) - 1;
    for (int i = 0; i < length_length_val; ++i) out->AppendBit(0);
    out->AppendBit(1);
    for (int i = length_length_val - 1; i >= 0; --i) {
      out->AppendBit((length_val + 1) >> i & 1);
    }
    for (int i = length_val - 1; i >= 0; --i) {
      out->AppendBit((val + 1) >> i & 1);
    }
  }

  //i is the bit position
  //returns the decoded int
  int64_t VLI_eliasDelta::DecodeInt(const BitString &in, uint64_t *i) {
    int length_length_val = 0;
    while (!in.GetBit(*i)){
      ++length_length_val;
      ++(*i);
    }
    ++(*i);
    int length_val = 1;
    for(int j = 0; j < length_length_val; ++j) {
      length_val = (length_val << 1) | in.GetBit(*i);
      ++(*i);
    }
    --length_val;
    int64_t val = 1;
    for(int j = 0; j < length_val; ++j) {
      val = (val << 1) | in.GetBit(*i);
      ++(*i);
    }
    ASSERT(val, >=, 0);
    return val - 1;
  }


