#include "simJoin.h"

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

void addPartition(map<string, vector<int>> &partitions, string segment, int id)
{
  if (partitions.count(segment) == 1)
  {
    partitions[segment].push_back(id);
  }
  else
  {
    vector<int> arr;
    arr.push_back(id);
    partitions[segment] = arr;
  }
}

void purge_old_indexes(map<int, vector<map<string, vector<int>>>> &invert_indexes, int current, unsigned threshold)
{
  for (int x = current - threshold - 1; x >= 0; x--)
  {
    if (invert_indexes.count(x) == 1)
    {
      invert_indexes.erase(x);
    }
  }
}

void partitionString(string r, unsigned threshold, vector<map<string, vector<int>>> &partitions, int id)
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
      partitions.push_back(map<string, vector<int>>());
    }
  }
  int num = 0;

  for (int x = 0; x < (threshold + 1 - maxNum) * minLen; x += minLen)
  {
    // cout << r.substr(x, minLen) << endl;
    addPartition(partitions[num], r.substr(x, minLen), id);
    num++;
  }
  for (int y = (threshold + 1 - maxNum) * minLen; y < r.length(); y += maxLen)
  {
    addPartition(partitions[num], r.substr(y, maxLen), id);
    // cout << r.substr(y, maxLen) << "\t";
    num++;
  }
  // cout << endl;
}

void logTimeMessage1(timeval &t1, timeval &t2, string message)
{
  cout << message << t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) / 1e6 << endl;
}

vector<int> getPositions(vector<map<string, vector<int>>> &partitions)
{
  vector<int> positions;
  int sum = 1;
  for (int x = 0; x < partitions.size(); x++)
  {
    string val = partitions[x].begin()->first;
    positions.push_back(sum);
    sum = sum + val.length();
  }
  return positions;
}

vector<string> substringSelection(string s, map<string, vector<int>> &inverted_index, int partitionNum, unsigned threshold, int length, int position)
{
  int i = partitionNum + 1;
  int delta = (int)abs((double)length - s.length());
  int lower = max(position - (i - 1), position + delta - ((int)threshold + 1 - i));
  int upper = min(position + (i - 1), position + delta + ((int)threshold + 1 - i));
  vector<string> substrings;
  int sub_length = (inverted_index.begin()->first).length();
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

bool simJoin::SimilarityJoin(unsigned threshold, vector<triple<unsigned, unsigned, unsigned>> &results)
{

  map<int, vector<map<string, vector<int>>>> invert_indexes;
  int candidateCount = 0;
  timeval t1;
  gettimeofday(&t1, NULL);
  double time = 0;
  for (int i = 0; i < data.size(); i++)
  {
    string val = data[i];
    if (i % 10000 == 0 && i != 0)
    {
      timeval t2;
      gettimeofday(&t2, NULL);
      cerr << "Proccessed " << i << " data" << endl;
      logTimeMessage1(t1, t2, "");
      t1 = t2;
      cout << "Partition Time: " << time << endl;
      cout << "Invert Index Size: " << invert_indexes.size() << endl;
      // for (map<int, vector<map<string, vector<int>> >>::iterator it = invert_indexes.begin(); it != invert_indexes.end(); it++)
      // {
      //   cout << "Length " << it->first << endl;
      //   vector<map<string, vector<int>> > partitions = it->second;
      //   for (int x = 0; x < partitions.size(); x++)
      //   {
      //     cout << x << ":";
      //     partitions[x].toString();
      //   }
      // }
      // cout << "==============" << endl;
      // exit(1);
    }
    // // cout << val << endl;
    purge_old_indexes(invert_indexes, val.length(), threshold);
    // set<int> candidates;
    for (map<int, vector<map<string, vector<int>>>>::iterator it = invert_indexes.begin(); it != invert_indexes.end(); it++)
    { // iterate through lengths
      int length = it->first;
      // vector<map<string, vector<int>>> partitions = *it->second;
      // time = time + t4.tv_sec - t3.tv_sec + (t4.tv_usec - t3.tv_usec) / 1e6;
      vector<int> positions = getPositions(it->second);
      for (int p = 0; p < it->second.size(); p++)
      { // iterate through segs of each length
        vector<string> substrings = substringSelection(val, it->second[p], p, threshold, length, positions[p]);
        for (int y = 0; y < substrings.size(); y++)
        {
          // hello
        }
      }
    }

    if (invert_indexes.count(val.length()) != 1)
    {
      invert_indexes[val.length()] = vector<map<string, vector<int>>>();
    }

    partitionString(val, threshold, invert_indexes[val.length()], i);
  }
  return true;
}
