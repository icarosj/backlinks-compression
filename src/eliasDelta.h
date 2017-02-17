
#ifndef ELIASDELTA_H_
#define ELIASDELTA_H_

#include "vli.h"

class VLI_eliasDelta : public VLI
{
  public:
  virtual void EncodeInt(int64_t val, BitString *out);
  virtual int64_t DecodeInt(const BitString &in, uint64_t *i);

    virtual void print_stat(){}
    virtual void print_name() { cout<<"elias delta"<<endl; }
};


#endif
