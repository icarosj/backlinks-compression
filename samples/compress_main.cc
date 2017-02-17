#include <cstdlib>
#include <iostream>
#include <fstream>
#include <getopt.h>
#include "backlinks_compression.h"

using namespace std;

bool Input(const char *filename, vector<pair<int, int> > *edges);

int main(int argc, char **argv) {
/*  if (argc != 3) {
    cerr << "usage: compress GRAPH OUTPUT" << endl;
    exit(EXIT_FAILURE);
  }
  
  vector<pair<int, int> > edges;
  if (!Input(argv[1], &edges)) {
    cerr << "error: Load failed" << endl;
    exit(EXIT_FAILURE);
  }*/

int iarg = 0;
vector<pair<int, int> > edges;
string outfile="outputfile";
string encoding_str;
ENCODING encoding;
while(iarg != -1)
{
  iarg = getopt(argc, argv, "i:o:r:m:e:");
  switch(iarg)
  {
    case 'i':
      if (!Input(optarg, &edges)) {
        cerr << "error: Load failed" << endl;
        exit(EXIT_FAILURE);
      }
      break;
    case 'o':
      outfile = optarg;
      break;
    case 'r':
      g_gamma_r = atof(optarg);
      break;
    case 'm':
      g_gamma_meanmod = atof(optarg);
      break;
    case 'e':
      encoding_str = optarg;
      if(encoding_str=="fix6")
      {
        encoding = FIX6;
      }
      else if(encoding_str=="delta")
      {
        encoding = DELTA;
      }
      else if(encoding_str=="poison")
      {
        encoding = POISON;
      }
      else
      {
        assert(0);
      }
      break;
  }
}

  cout << "edges      : " << edges.size() << endl;
  cout<<"r: "<<g_gamma_r<<"  m: "<<g_gamma_meanmod<<" e: "<<encoding_str<<" : "<<encoding<<endl;
  
  BacklinksCompression bl;
  BitString result;
  bl.Compress(edges, encoding, &result);
  cout << "bit length : " << result.get_length() << endl;
  cout << "bits/edge  : " << ((double)result.get_length()/edges.size()) << endl;
  
  if (!result.Output(outfile.c_str())) {
    cerr << "error: Output failed" << endl;
    exit(EXIT_FAILURE);
  }
  exit(EXIT_SUCCESS);
}

bool Input(const char *filename, vector<pair<int, int> > *edges) {
  ifstream ifs(filename);
  if (!ifs) return false;
  for (int v, w; ifs >> v >> w; ) edges->push_back(make_pair(v, w));
  return !ifs.bad();
}
