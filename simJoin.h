#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sys/time.h>
#include <map>
#include <climits>
#include <cmath>
#include <set>
using namespace std;

class Partition
{
public:
  map<string, vector<int>> segments = map<string, vector<int>>();
  void toString();
};

template <typename T1, typename T2, typename T3>
struct triple
{
  T1 id1;
  T2 id2;
  T3 ed;
};

class simJoin
{
public:
  simJoin(const string &filename)
  {
    readData(filename);
  };

  ~simJoin(){};

  bool SimilarityJoin(unsigned ed_threshold, vector<triple<unsigned, unsigned, unsigned>> &results);
  bool getString(int id, string &out) const;
  int getDataNum() const;
  void verification(vector<triple<unsigned, unsigned, unsigned>> &results, vector<int> candidates, unsigned threshold, string s, int id, set<int> &visited);

private:
  vector<string> data;
  bool readData(const string &filename);
};
