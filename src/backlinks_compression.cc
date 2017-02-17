// Copyright 2014, Kento Nikaido
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Kento Nikaido nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "backlinks_compression.h"
#include <stdint.h>
#include <algorithm>
#include <vector>
#include <queue>
#include <stack>
#include <fstream>
#include <iostream>
#include <string>
#include <math.h>

float g_scale = 1.2;
float g_gamma_r=0.6;
float g_gamma_meanmod=0.5;

//
// BitString
//
bool BitString::Input(const char *filename) {
  std::ifstream ifs(filename, std::ios::binary);
  if (!ifs) return false;
  data_.clear();
  uint64_t d;
  while (ifs.read((char *) &d, sizeof(uint64_t))) {
    data_.push_back(d);
  }
  length_ = (uint64_t) data_.size() * 64;
  return !ifs.bad();
}

bool BitString::Output(const char *filename) {
  std::ofstream ofs(filename, std::ios::binary);
  if (!ofs) return false;
  for (uint64_t i = 0; i < length_; i += 64) {
    ofs.write((char *) &data_[i >> 6], sizeof(uint64_t));
  }
  return ofs.good();
}
//
// BacklinksCompression
//
void BacklinksCompression
::TransformToAdj(const std::vector<std::pair<int, int> > &edges,
                 bool directed, std::vector<std::vector<int> > *adj) {
  int num_v = 0;
  for (uint64_t i = 0; i < edges.size(); ++i) {
    num_v = std::max(num_v, std::max(edges[i].first, edges[i].second) + 1);
  }
  adj->resize(num_v);
  for (uint64_t i = 0; i < edges.size(); ++i) {
    adj->at(edges[i].first).push_back(edges[i].second);
  }
  if (!directed) {
    for (uint64_t i = 0; i < edges.size(); ++i) {
      adj->at(edges[i].second).push_back(edges[i].first);
    }
  }
  for (int i = 0; i < num_v; ++i) {
    std::sort(adj->at(i).begin(), adj->at(i).end());
  }
}

void BacklinksCompression
::TransformToEdge(const std::vector<std::vector<int> > &adj,
                   std::vector<std::pair<int, int> > *edges) {
  for (int i = 0; i < adj.size(); ++i) {
    for (int j = 0; j < adj[i].size(); ++j) {
      edges->push_back(std::make_pair(i, adj[i][j]));
    }
  }
}

void BacklinksCompression
::Compress(std::vector<std::pair<int, int> > edges, ENCODING encoding, BitString *result) {
  std::vector<int> order;
  Order(edges, &order);
  for (uint64_t i = 0; i < edges.size(); ++i) {
    edges[i].first = order[edges[i].first];
    edges[i].second = order[edges[i].second];
  }
  std::vector<std::vector<int> > adj;
  TransformToAdj(edges, true, &adj);
  int num_v = adj.size();
  VLI_eliasDelta delta;
  BitString tmp;
  delta.EncodeInt(num_v, &tmp);
  result->AppendBitString(tmp);
  for (int i = 0; i < num_v; ++i) {
    delta.EncodeInt(order[i], &tmp);
    result->AppendBitString(tmp);
  }
  CompressVertexes(adj, encoding, result);
}

void BacklinksCompression
::Develop(const BitString &code, std::vector<std::pair<int, int> > *edges) {
  VLI_eliasDelta delta;
  uint64_t cur = 0;
  int num_v = delta.DecodeInt(code, &cur);
  std::vector<int> order(num_v);
  for (int i = 0; i < num_v; ++i) {
    order[delta.DecodeInt(code, &cur)] = i;
  }
  std::vector<std::vector<int> > adj(num_v);
  for (int i = 0; i < num_v; ++i) {
    int j = i - delta.DecodeInt(code, &cur);
    if (code.GetBit(cur++)) adj[i].push_back(i);
    // copying
    if (i != j) {
      for (int k = 0; k < adj[j].size(); ++k) {
        if (Exist(adj, adj[j][k], j) && j >= adj[j][k]) continue;
        if (code.GetBit(cur++)) {
          adj[i].push_back(adj[j][k]);
        }
      }
    }
    // residual
    int num_residual = delta.DecodeInt(code, &cur);
    if (num_residual != 0) {
      int sign = (code.GetBit(cur++) ? 1 : -1);
      int now = i + delta.DecodeInt(code, &cur) * sign;
      adj[i].push_back(now);
      for (int j = 1; j < num_residual; ++j) {
        now += delta.DecodeInt(code, &cur);
        adj[i].push_back(now);
      }
    }
    std::sort(adj[i].begin(), adj[i].end());
    // reciprocal
    for (int k = 0; k < adj[i].size(); ++k) {
      if (i >= adj[i][k]) continue;
      if (code.GetBit(cur++)) {
        adj[adj[i][k]].push_back(i);
      }
    }
  }
  TransformToEdge(adj, edges);
  for (uint64_t i = 0; i < edges->size(); ++i) {
    edges->at(i).first = order[edges->at(i).first];
    edges->at(i).second = order[edges->at(i).second];
  }
  std::sort(edges->begin(), edges->end());
}



void BacklinksCompression
::BFSOrder(const std::vector<std::vector<int> > &adj, std::vector<int> *order) {
  int num_v = adj.size();
  std::queue<int> q;
  int k = 0;
  for (int i = 0; i < num_v; ++i) {
    if (order->at(i) != -1) continue;
    order->at(i) = k++;
    q.push(i);
    while (!q.empty()) {
      int v = q.front();
      q.pop();
      for (int j = 0; j < adj[v].size(); ++j) {
        int u = adj[v][j];
        if (order->at(u) != -1) continue;
        order->at(u) = k++;
        q.push(u);
      }
    }
  }
}

void BacklinksCompression
::Order(const std::vector<std::pair<int, int> > &edges,
        std::vector<int> *order) {
  std::vector<std::vector<int> > adj;
  TransformToAdj(edges, false, &adj);
  int num_v = adj.size();
  order->resize(num_v);
  std::fill(order->begin(), order->end(), -1);
  switch (kORDERING) {
    case BFS: {
      BFSOrder(adj, order);
      break;
    }
    default: {
      for (int i = 0; i < num_v; ++i) order->at(i) = i;
    }
  }
}

void BacklinksCompression
::CompressVertexes(const std::vector<std::vector<int> > &adj,
    ENCODING encoding,
                   BitString *result) {
  int num_v = adj.size();
  VLI_eliasDelta delta;
  BitString tmp;
  for (int i = 0; i < num_v; ++i) {
    BitString best;
    best.Init(0);
    for (int j = i; j > i - kWINDOW_WIDTH && j >= 0; --j) {
      BitString now;
      now.Init(0);
      delta.EncodeInt(i - j, &tmp);
      now.AppendBitString(tmp);
      if (Exist(adj, i, i)) { // self-loop
        now.AppendBit(1);
      } else {
        now.AppendBit(0);
      }
      std::vector<int> residual;
      // copying
      int cur = 0;
      if (i != j) {
        for (int k = 0; k < adj[j].size(); ++k) {
          if (Exist(adj, adj[j][k], j) && j >= adj[j][k]) continue;
          ProceedCopying(adj, adj[j][k], i, &cur, &residual);
          if (cur < adj[i].size() && adj[i][cur] == adj[j][k]) {
            now.AppendBit(1);
          } else {
            now.AppendBit(0);
          }
        }
      }
      ProceedCopying(adj, num_v, i, &cur, &residual);
      // residual
      delta.EncodeInt(residual.size(), &tmp);

      VLI* vli;
      uint64_t coarse_degree = pow(g_scale, (uint64_t)(log(residual.size())/log(g_scale)));
      switch(encoding)
      {
        case DELTA:
          vli = new VLI_eliasDelta;
          break;
        case FIX6:
          vli = new VLI_fix6;
          break;
        case POISON:
          if(vli_cache.count(coarse_degree)) 
          {
            vli = vli_cache[coarse_degree];
          }
          else
          {
            cout<<"vli poison : "<<num_v<<" d "<<coarse_degree<<" r "<<g_gamma_r<<endl;
            vli = new VLI_poison(num_v, coarse_degree, g_gamma_r);
            vli_cache[coarse_degree]=vli;
          }
          break;
      }

      if(bitlength_acc.count(coarse_degree)==0)
      {
        bitlength_acc[coarse_degree]=0;
        bitlength_count[coarse_degree]=0;
      }


      now.AppendBitString(tmp);
      if (residual.size() != 0) {
        if (residual[0] > i) {
          now.AppendBit(1);
        } else {
          now.AppendBit(0);
        }
        vli->EncodeInt(std::abs(residual[0] - i), &tmp);
        now.AppendBitString(tmp);
        bitlength_count[coarse_degree]++;
        bitlength_acc[coarse_degree]+=tmp.get_length();
        for (int k = 1; k < residual.size(); ++k) {
          vli->EncodeInt(residual[k] - residual[k - 1], &tmp);
          now.AppendBitString(tmp);
          bitlength_count[coarse_degree]++;
          bitlength_acc[coarse_degree]+=tmp.get_length();
        }
      }
      // reciprocal
      for (int k = 0; k < adj[i].size(); ++k) {
        if (i >= adj[i][k]) continue;
        if (Exist(adj, adj[i][k], i)) {
          now.AppendBit(1);
        } else {
          now.AppendBit(0);
        }
      }
      // update best
      if (i == j || best.get_length() > now.get_length()) best = now;
    }
    result->AppendBitString(best);
  }
  this->print_stat();
}

void BacklinksCompression::print_stat()
{
  cout<<"print stat bl"<<endl;

  //(*vli_cache.begin()).second->print_name();
  for(int i=0;i<64;i++)
  {
    uint64_t coarse_degree = (uint64_t)pow(g_scale, i);
    if(vli_cache.count(coarse_degree))
    {
      vli_cache[coarse_degree]->print_stat();

      cout<<"\navg encoding, "<<coarse_degree<<" , "<<bitlength_acc[coarse_degree]/(double)bitlength_count[coarse_degree]<<" / "<<bitlength_count[coarse_degree]<<endl<<endl<<endl;

    }
  }
}

