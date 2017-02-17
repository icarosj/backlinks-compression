#ifndef VLI_H_
#define VLI_H_

#include <inttypes.h>
#include <vector>
#include <stdio.h>
#include "p_assert.h"
#include "bitstring.h"
using namespace std;



class VLI {
 public:
  virtual int CountBitLength(int64_t val);
  virtual void EncodeInt(int64_t val, BitString *out)=0;
  virtual int64_t DecodeInt(const BitString &in, uint64_t *i)=0;
  virtual void print_stat()=0;
  virtual void print_name()=0;
};

#endif
