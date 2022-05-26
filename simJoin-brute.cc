#include "simJoin.h"

int simJoin::getDataNum() const
{
  return data.size();
}

bool simJoin::getString(int id, string &out) const
{
  if (id < 0 || id >= data.size())
    return false;
  out = data[id];
  return true;
}

bool simJoin::readData(const string &filename)
{
  string str;
  ifstream datafile(filename, ios::in);
  while (getline(datafile, str))
    data.emplace_back(str);

  return true;
}

unsigned editDistance(string s, string r, unsigned threshold)
{
  int dp[r.length() + 1][s.length() + 1];
  for (int i = 0; i <= r.length(); i++)
  {
    for (int j = 0; j <= s.length(); j++)
    {
      if (i == 0)
        dp[i][j] = j;
      else if (j == 0)
        dp[i][j] = i;
      else if (r[i - 1] == s[j - 1])
        dp[i][j] = dp[i - 1][j - 1];
      else
      {
        dp[i][j] = 1 + min({dp[i][j - 1], dp[i - 1][j], dp[i - 1][j - 1]});
      }
    }
  }
  return dp[r.length()][s.length()];
}

bool simJoin::SimilarityJoin(unsigned threshold, vector<triple<unsigned, unsigned, unsigned>> &results)
{

  // map<int, vector<Partition>> invert_indexes;
  for (int i = 0; i < data.size(); i++)
  {
    if (i % 10000 == 0 && i != 0)
    {
      cerr << "Proccessed " << i << " data" << endl;
    }
    for (int y = i + 1; y < data.size(); y++)
    {
      int distance = editDistance(data[i], data[y], threshold);
      if (distance <= threshold)
      {
        triple<unsigned, unsigned, unsigned> t;
        t.id1 = (unsigned)i;
        t.id2 = (unsigned)y;
        t.ed = (unsigned)distance;
        results.push_back(t);
      }
    }
  }
  return true;
}
