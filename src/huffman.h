/**
 * This is an implementation of Huffman coding.
 *
 * The core algorithm is taken from the CLR book (Introduction of Algorithms),
 * Chapter 16.3, and directly used to implement the 'build_tree()' routine.
 *
 * After the tree is built, a code table that maps a character to a binary
 * code is built from the tree, and used for encoding text. Decoding is done
 * by traversing the Huffman tree, as prescribed by the algorithm.
 *
 * Binary codes are represented by std::vector<bool>, which is a specialized
 * vector that optimizes space.
 */

//Jinho Lee 2017-Feb-11 modified freqtable to float type
#ifndef HUFFMAN_H_
#define HUFFMAN_H_

#include <vector>
#include <queue>
#include <map>
#include <algorithm>
#include <string>
#include <cassert>
#include <stdexcept>
#include <iostream>
#include "bitstring.h"

using namespace std;


namespace huffman {
  typedef vector<bool> code_t;
  typedef map<char, code_t> codetable;

  // A Huffman Tree Node
  struct HuffmanTree {
    char c; // character in an alphabet
    float cfreq; // frequency of c.
    struct HuffmanTree *left;
    struct HuffmanTree *right;
    HuffmanTree(char c, float cfreq, struct HuffmanTree *left=NULL,
        struct HuffmanTree *right=NULL) :
      c(c), cfreq(cfreq), left(left), right(right) {
      }
    ~HuffmanTree() {
      delete left, delete right;
    }
    // Compare two tree nodes
    class Compare {
      public:
        bool operator()(HuffmanTree *a, HuffmanTree *b) {
          return a->cfreq > b->cfreq;
        }
    };
  };


  HuffmanTree *build_tree(vector< pair<char, float> > &alph) ;

  void print_tree(HuffmanTree *t) ;

  map<char, code_t> build_lookup_table(HuffmanTree *htree) ;

  code_t encode(string input, codetable &lookup) ;
  code_t encode(char input, codetable &lookup) ;

  char code_lookup(code_t::iterator &biter, const code_t::iterator &biter_end,
      const HuffmanTree *htree) ;
  char code_lookup(const BitString &bStr,uint64_t* pos, const HuffmanTree *htree) ;



  string decode(code_t &compressed, const HuffmanTree *htree) ;


}







#endif
