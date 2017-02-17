#ifndef FIX6_H_
#define FIX6_H_
#include "vli.h"
#include <inttypes.h>

class VLI_fix6:public VLI {
 public:
  virtual void EncodeEOS(BitString *out);
  virtual void EncodeCONT(BitString *out) ;
  virtual void EncodeInt(int64_t val, BitString *out);
  virtual int64_t DecodeInt(const BitString &in, uint64_t *i);
  virtual void print_stat(){}
  virtual void print_name() { cout<<"fix6"<<endl; }
};



#endif
