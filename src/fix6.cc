#include "fix6.h"
#include <iostream>
#include "p_assert.h"


using std::cout;



//
// VLI_fix6
//
void VLI_fix6::EncodeEOS(BitString *out) {
  out->Init(0);
  out->AppendBit(1);
  out->AppendBit(1);
  out->AppendBit(1);
  out->AppendBit(1);
  out->AppendBit(1);
  out->AppendBit(1);
  //F format : b'111111' means EOS

}

void VLI_fix6::EncodeCONT(BitString *out) {
  out->Init(0);
  out->AppendBit(1);
  out->AppendBit(1);
  out->AppendBit(1);
  out->AppendBit(1);
  out->AppendBit(1);
  out->AppendBit(0);
  //G format : b'111110' means CONT follows 

}
void VLI_fix6::EncodeInt(int64_t val, BitString *out) {
  out->Init(0);
  int length_val = CountBitLength(val+1)-1;//if we +1, the first bit of the val is always 1 (0~max to 1~max), so we can save one bit
  int length_length_val = 6;
  //cout<<"encoding val: "<<val<<" len(-1): "<<length_val<<endl;
  for (int i = length_length_val-1;i>=0;i--){
    //cout<<"\tput len "<<((length_val) >> i & 1)<<endl;
    out->AppendBit((length_val) >> i & 1);
  }
  if(length_val>=62)
  {
    cout<<"WARNING! legnth_val limit reached. encoding will be a junk "<<val<<endl;
  }
  ASSERT(length_val, <, 62);
  val++;
  for (int i = length_val-1;i>=0;i--) {
  //  cout<<"\tput val "<<((val) >> i & 1)<<endl;
    out->AppendBit((val) >> i & 1);
  }
}

//i is the bit position
//returns the decoded int
int64_t VLI_fix6::DecodeInt(const BitString &in, uint64_t *i) {
  //'cout<<"at "<<*i;
    int length_length_val = 6;
    int length_val = 0;
    for(int j = 0; j < length_length_val; ++j) {
      length_val = (length_val << 1) | in.GetBit(*i);
   //   cout<<"\t get len "<<in.GetBit(*i)<<endl;
      ++(*i);
    }
    //--length_val;
    if(length_val==63)
    {
      //end-of-stream
      return -1;
    }
    if(length_val==62)
    {
      //cont-sig
      return -2;
    }
    int64_t val = 1;
    for(int j = 0; j < length_val; ++j) {
      val = (val << 1) | in.GetBit(*i);
     // cout<<"\t get val "<<in.GetBit(*i)<<endl;
      ++(*i);
    }
//  cout<<"decoded val: "<<val<<" len(-1): "<<length_val<<dec<<endl;
    ASSERT(val, >=, 0);
    //cout<<" int : "<<val-1<<endl;
    return val - 1;
}


