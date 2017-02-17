/* Author Jinho Lee <leejinho@us.ibm.com> <iamjinholee@gmail.com>
 * Date: 2017-Feb-11
 * builds a VLI from gamma distribution for use in poisonG
 * I have not thought about licenses yet
 */

#ifndef POISON_H_
#define POISON_H_

#include "vli.h"
#include "huffman.h"
#include <vector>
#include <unordered_map>
#include <boost/math/distributions/gamma.hpp>
#include <math.h>
#include <iostream>

using namespace std;
const float avg_compensation = 0.5;
const float default_gamma_shape = 0.6;


class VLI_poison: public VLI
{
  private:
    vector<pair<char,float> > freq_table;
    huffman::HuffmanTree *htree;
    huffman::codetable ctbl;
    unordered_map<char,BitString> length_cache;
    unordered_map<char,uint64_t> freq_log;
    uint64_t count;
    uint64_t N;
    uint64_t degree;
  public:
    VLI_poison() 
    {
      init();
    }
    VLI_poison(uint64_t N, uint64_t degree, float shape) 
      : N(N), degree(degree)
    {
      init();
      generate_gamma_freqtable(N, degree, shape);
      generate_huffman_length_table();
    }
    void init()
    {
      count=0;
      for(int i=0;i<CountBitLength(N);i++)
      {
        freq_log[i]=0;
      }
    }

    void generate_gamma_freqtable(uint64_t N, uint64_t D, float shape=default_gamma_shape);

    void generate_huffman_length_table();
    virtual void print_stat();
    virtual void print_name() { cout<<"Poison"<<endl; }
    virtual void EncodeInt(int64_t val, BitString *out);
    virtual int64_t DecodeInt(const BitString &bStr, uint64_t *i);
 

  private:
    uint64_t decode_binary(const BitString &bStr, uint64_t* pos, char len);
 
};


#endif
