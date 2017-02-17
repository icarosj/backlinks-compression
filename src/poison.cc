#include "poison.h"
#include "huffman.h"
#include "backlinks_compression.h"


void VLI_poison::generate_gamma_freqtable(uint64_t N, uint64_t D, float shape )
{

  float mean = (float)N/(float)(D) * g_gamma_meanmod;
  float scale = mean / shape;
  if(D==0) 
  {
    mean=1;
    scale=1;//probably means nothing cause it means 
  }
  cout<<"mean : "<<mean<<" shape : "<<shape <<" scale : "<<scale<<endl;
  boost::math::gamma_distribution<> my_gamma(shape, scale);

  //int N_limit = ceil(log2(N));
  int N_limit = CountBitLength(N);

  //cout<<"N: "<<N<<" ceil : "<<N_limit<<" count : "<<CountBitLength(N)<<endl;

  int max_index = -1;
  float maxpdf = 0.0;

  //We need to +1 (<=) because we want to put +1 for encoding 0
  for( int i=0;i<=N_limit;i++)
  {
    float logpdf = cdf(my_gamma, pow(2, i+1)) - cdf(my_gamma, pow(2, i));
    this->freq_table.push_back(make_pair(i,logpdf));
    //cout<<i<<" : "<<logpdf<<endl;
    if(logpdf > maxpdf)
    {
      maxpdf = logpdf;
      max_index = i;
    }
  }

  //cout<<"generation done. maxpdf: "<<max_index<<" with pdf = "<<maxpdf<<endl; 
}


void VLI_poison::generate_huffman_length_table()
{
  this->htree = huffman::build_tree(freq_table);
  this->ctbl  = huffman::build_lookup_table(htree);

}


void VLI_poison::EncodeInt(int64_t val, BitString *out)
{
  out->Init(0);
//  cout<<"encode "<<val<<endl;
  uint64_t val_inc = val+1;//our scheme encodes from 1. but the numbers are from 0
  char length = CountBitLength(val_inc)-1;
 // cout<<"length : "<<(int)length<<endl;
  ASSERT(int(length), < ,this->freq_table.size()); 
  ASSERT(length, >= ,0); 


  freq_log[length]++;
  count++;

  //cout<<"length : "<<(int)length<<endl;

  BitString bStr;
  if(length_cache.count(length))
  {
    //cout<<"len cache"<<endl;
    bStr = length_cache[length];
  }
  else
  {

    huffman::code_t len_code = huffman::encode(length, this->ctbl);
    //cout<<"built huffman table of size "<<len_code.size()<<endl;
    bStr=BitString(len_code);
    length_cache[length] = bStr;
    //cout<<"length part: "<<endl;
    //bStr.Print();
  }
  bStr.AppendBinary(val_inc);

  //cout<<"final encoding (number part in reverse): len "<<bStr.get_length()<<endl;
  //bStr.Print();
  out->AppendBitString(bStr);



}

uint64_t VLI_poison::decode_binary(const BitString &bStr, uint64_t* pos, char len)
{
  uint64_t res=0;
  
  for(int i=0;i<len;i++)
  {
    res += bStr.GetBit((*pos)++) << i;
    //cout<<"res: "<<res<<endl;
  }
  res += 1<<len;
  return res;

}


int64_t VLI_poison::DecodeInt(const BitString &bStr, uint64_t *pos)
{
  char length = huffman::code_lookup(bStr, pos, this->htree);
  cout<<"len : "<<(int)length<<endl;;
  int64_t ret = this->decode_binary(bStr, pos, length)-1;
  return ret;

}

void VLI_poison::print_stat()
{
  cout<<"N, "<<N<<" , Deg , "<<degree<<", theomean , "<<N/(float)degree<<" , count , "<<count<<", R ,"<<g_gamma_r<<" , M ,"<<g_gamma_meanmod<<endl;
  cout<<"len, freq, pdf, pdf(gamma) "<<endl;
  float acc=0;
  for(int i=0;i<CountBitLength(N);i++)
  {
    acc+=i*(double)freq_log[i]/count;
    cout<<i<<" , "<<freq_log[i]<<" , "<<(double)freq_log[i]/count<<" , "<<(int)freq_table[i].first<<" , "<<freq_table[i].second<<endl;
  }
  cout<<"avg len , "<<acc<<endl;
}

#ifdef MAIN_POISON
int main()
{ 
  cout<<"poison"<<endl;
  VLI_poison vli;
  vli.generate_gamma_freqtable(100000000, 1000);
  vli.generate_huffman_length_table();
  BitString bStr;
  cout<<"\n\n=======12========="<<endl;
  vli.EncodeInt(12, &bStr);
  cout<<"\n\n=======2========="<<endl;
  vli.EncodeInt(2, &bStr);
  cout<<"\n\n=======1========="<<endl;
  vli.EncodeInt(1, &bStr);
  cout<<"\n\n=======1024========="<<endl;
  vli.EncodeInt(1024, &bStr);
  cout<<"\n\n=======4095========="<<endl;
  vli.EncodeInt(4095, &bStr);
  cout<<"\n\n=======10240========="<<endl;
  vli.EncodeInt(10240, &bStr);

  int64_t pos=0;
  uint64_t decoded;
  while(pos != bStr.get_length())
  {
    decoded = vli.DecodeInt(bStr,&pos);
    cout<<"decode "<<decoded<<endl;

  }
  return 0;
}

#endif
