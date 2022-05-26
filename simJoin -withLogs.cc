#include "simJoin.h"

/* Print out Inverted List */
void Partition::toString()
{
  for (map<string, vector<int>>::iterator it = segments.begin(); it != segments.end(); it++)
  {
    cout << "  \t" << it->first // string (key)
         << "==> ";
    vector<int> arr = it->second;
    for (int x = 0; x < arr.size(); x++)
    {
      cout << arr[x] << " ";
    }
    cout << endl;
  }
}

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

/* Add segment to Partition */
void addPartition(int partNum, vector<Partition> &partitions, string segment, int id)
{
  if (partitions[partNum].segments.count(segment) == 1)
  {
    partitions[partNum].segments[segment].push_back(id);
  }
  else
  {
    vector<int> arr;
    arr.push_back(id);
    partitions[partNum].segments[segment] = arr;
  }
}

/* Select substrings using multi-match aware */
vector<string> substringSelection(string s, Partition &inverted_index, int partitionNum, unsigned threshold, int length, int position)
{
  int i = partitionNum + 1;
  int delta = (int)abs((double)length - s.length());
  int lower = max(position - (i - 1), position + delta - ((int)threshold + 1 - i));
  int upper = min(position + (i - 1), position + delta + ((int)threshold + 1 - i));
  vector<string> substrings;
  int sub_length = (inverted_index.segments.begin()->first).length();
  for (int y = lower - 1; y < upper; y++)
  {
    substrings.push_back(s.substr(y, sub_length));
  }
  // cout << "Substrings: [" << lower - 1 << " " << upper - 1 << "] ";
  // for (int r = 0; r < substrings.size(); r++)
  // {
  //   cout << substrings[r] << '\t';
  // }
  // cout << endl;
  return substrings;
}

/* Get position numbers for strings in partition */
vector<int> getPositions(vector<Partition> &partitions)
{
  vector<int> positions;
  int sum = 1;
  for (int x = 0; x < partitions.size(); x++)
  {
    string val = partitions[x].segments.begin()->first;
    positions.push_back(sum);
    sum = sum + val.length();
  }
  return positions;
}

/* Split the String and add to inverted Index */
void partitionString(string r, unsigned threshold, vector<Partition> &partitions, int id)
{
  double length = r.length() / (double)(threshold + 1);
  // cout << length << endl;
  int minLen = floor(length); // minimum length of seg
  int maxLen = ceil(length);  // maximum length of seg
  int maxNum = r.length() - minLen * (threshold + 1);
  if (partitions.empty())
  {
    for (int x = 0; x < threshold + 1; x++)
    {
      partitions.push_back(Partition());
    }
  }
  int num = 0;
  for (int x = 0; x < (threshold + 1 - maxNum) * minLen; x += minLen)
  {
    // cout << r.substr(x, minLen) << endl;
    addPartition(num, partitions, r.substr(x, minLen), id);
    num++;
  }
  for (int y = (threshold + 1 - maxNum) * minLen; y < r.length(); y += maxLen)
  {
    addPartition(num, partitions, r.substr(y, maxLen), id);
    // cout << r.substr(y, maxLen) << "\t";
    num++;
  }
  // cout << endl;
}

/* Remove inverted indexes that are beyond threshold */
void purge_old_indexes(map<int, vector<Partition>> &invert_indexes, int current, unsigned threshold)
{
  for (int x = current - threshold - 1; x >= 0; x--)
  {
    if (invert_indexes.count(x) == 1)
    {
      invert_indexes.erase(x);
    }
  }
}

/* Check if substring is in partition */
bool check_in_partition(string s, Partition partition)
{
  for (map<string, vector<int>>::iterator it = partition.segments.begin(); it != partition.segments.end(); it++)
  {
    if (it->first == s)
    {
      return true;
    }
  }
  return false;
}

void logTimeMessage1(timeval &t1, timeval &t2, string message)
{
  cout << message << t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) / 1e6 << endl;
}

/* Length aware + early termination verification */
unsigned verifyStringPair(string s, string r, unsigned threshold)
{

  int delta = (int)abs((double)s.length() - r.length());
  int dp[r.length() + 1][s.length() + 1];
  int minDiff = floor(0.5 * (threshold - delta));
  int maxDiff = floor(0.5 * (threshold + delta));
  for (int i = 0; i <= r.length(); i++)
  {
    int jMin = i - minDiff;
    int jMax = i + maxDiff;
    if (jMin < 0)
      jMin = 0;
    if (jMax > s.length())
      jMax = s.length();
    int earlyCount = 0;
    for (int j = jMin; j <= jMax; j++)
    {
      if (i == 0)
        dp[i][j] = j;
      else if (j == 0)
        dp[i][j] = i;
      else if (r[i - 1] == s[j - 1])
        dp[i][j] = dp[i - 1][j - 1];
      else
      {
        int min_pos = INT_MAX;
        if (j - 1 >= i - minDiff && j - 1 <= i + maxDiff && dp[i][j - 1] < min_pos)
          min_pos = dp[i][j - 1];
        if (j >= i - 1 - minDiff && j <= i - 1 + maxDiff && dp[i - 1][j] < min_pos)
          min_pos = dp[i - 1][j];
        if (j - 1 >= i - 1 - minDiff && j - 1 <= i - 1 + maxDiff && dp[i - 1][j - 1] < min_pos)
          min_pos = dp[i - 1][j - 1];
        dp[i][j] = 1 + min_pos;
      }
      if (dp[i][j] + abs((double)((s.length() - j) - (r.length() - i))) > threshold)
      {
        earlyCount = earlyCount + 1;
      }
    }
    if (earlyCount == (jMax - jMin + 1))
    {
      return threshold + 1;
    }
  }
  return dp[r.length()][s.length()];
}

/* Verify String and add to results */
void simJoin::verification(vector<triple<unsigned, unsigned, unsigned>> &results, vector<int> candidates, unsigned threshold, string s, int id, set<int> &visited)
{
  for (int x = 0; x < candidates.size(); x++)
  {
    int distance = verifyStringPair(s, data[candidates[x]], threshold);
    if (distance <= threshold)
    {
      triple<unsigned, unsigned, unsigned> t;
      t.id1 = (unsigned)id;
      t.id2 = (unsigned)candidates[x];
      t.ed = (unsigned)distance;
      results.push_back(t);
      visited.insert(candidates[x]);
    }
  }
}

/* Main function for finding similar strings within threshold */
bool simJoin::SimilarityJoin(unsigned threshold, vector<triple<unsigned, unsigned, unsigned>> &results)
{

  map<int, vector<Partition>> invert_indexes;
  for (int i = 0; i < data.size(); i++)
  {
    string val = data[i];
    if (i % 10000 == 0 && i != 0)
    {
      cerr << "Proccessed " << i << " data" << endl;
    }
    // cout << val << endl;
    purge_old_indexes(invert_indexes, val.length(), threshold);
    set<int> candidates;
    for (map<int, vector<Partition>>::iterator it = invert_indexes.begin(); it != invert_indexes.end(); it++)
    { // iterate through lengths
      int length = it->first;
      // vector<Partition> partitions = it->second;
      vector<int> positions = getPositions(it->second);
      for (int p = 0; p < (it->second).size(); p++)
      { // iterate through segs of each length
        vector<string> substrings = substringSelection(val, it->second[p], p, threshold, length, positions[p]);
        for (int y = 0; y < substrings.size(); y++)
        {
          // check if it is in Li and then verify
          if ((it->second)[p].segments.count(substrings[y]) == 1)
          {
            vector<int> c = (it->second)[p].segments[substrings[y]];
            vector<int> filtered;
            for (int j = 0; j < c.size(); j++)
            {
              bool notFound = candidates.find(c[j]) == candidates.end();
              if (notFound)
              {
                filtered.push_back(c[j]);
              }
            }
            verification(results, filtered, threshold, val, i, candidates);
          }
          //
          //  verification(results, filtered, threshold, val, i);
        }
      }
    }

    if (invert_indexes.count(val.length()) != 1)
    {
      invert_indexes[val.length()] = vector<Partition>();
    }

    partitionString(val, threshold, invert_indexes[val.length()], i);
    // Print invert_indexes
    // for (map<int, vector<Partition>>::iterator it = invert_indexes.begin(); it != invert_indexes.end(); it++)
    // {
    //   cout << "Length " << it->first << endl;
    //   vector<Partition> partitions = it->second;
    //   for (int x = 0; x < partitions.size(); x++)
    //   {
    //     cout << x << ":";
    //     partitions[x].toString();
    //   }
    // }
    // cout << "==============" << endl;
  }
  return true;
}
