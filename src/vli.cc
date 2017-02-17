#include "vli.h"
#include "p_assert.h"
#include <iostream>


int VLI::CountBitLength(int64_t val) {
  int count = 0;
  while (val) {
    val >>= 1;
    ++count;
  }
  return count;
}


//void BacklinksCompression
//::test()
#ifdef DEBUG_COMP 
int main()
{
  BitString now;
  BitString code;
//  code.Init((int64_t*)malloc(100000));
  int64_t pos=0;
  VLI delta;
  delta.EncodeInt(0,&now);
  code.AppendBitString(now);
  delta.EncodeInt(1,&now);
  code.AppendBitString(now);
  delta.EncodeInt(2,&now);
  code.AppendBitString(now);
  delta.EncodeInt(3,&now);
  code.AppendBitString(now);
  delta.EncodeInt(4,&now);
  code.AppendBitString(now);
  delta.EncodeInt(5,&now);
  code.AppendBitString(now);
  delta.EncodeInt(100,&now);
  code.AppendBitString(now);
  delta.EncodeInt(32768,&now);
  code.AppendBitString(now);
  delta.EncodeInt(123456,&now);
  code.AppendBitString(now);
  delta.EncodeInt(987654321,&now);
  code.AppendBitString(now);

  cout<<"return0 "<<delta.DecodeInt(code, &pos)<<endl;
  cout<<"return1 "<<delta.DecodeInt(code, &pos)<<endl;
  cout<<"return2 "<<delta.DecodeInt(code, &pos)<<endl;
  cout<<"return3 "<<delta.DecodeInt(code, &pos)<<endl;
  cout<<"return4 "<<delta.DecodeInt(code, &pos)<<endl;
  cout<<"return5 "<<delta.DecodeInt(code, &pos)<<endl;
  cout<<"return100 "<<delta.DecodeInt(code, &pos)<<endl;
  cout<<"return32768 "<<delta.DecodeInt(code, &pos)<<endl;
  cout<<"return123456 "<<delta.DecodeInt(code, &pos)<<endl;
  cout<<"return987654321 "<<delta.DecodeInt(code, &pos)<<endl;

  

  
  return 0;
}
#endif


