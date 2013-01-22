#include <cstring>
#include <map>
#include <set>
#include <string>
#include <vector>

struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

int main (void)
{
//  std::

  std::string ss;
  std::basic_string<char> bs;

//  ss.
//  bs.

  std::vector<int> v;
//  v.

  std::map<const char*, int, ltstr> m;
//  m.
//  m["foo"].;

  std::set<const char*, int> st;
//  st.
}
