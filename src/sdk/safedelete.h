#ifndef DELETEFUN
#define DELETEFUN

#include <map>
#include <vector>
#include <list>
#include <deque>
#include <algorithm>

template<typename T>inline void Delete(T*& p){delete p; p = 0;};

struct SeqDelete
{
    template<typename T>inline void operator()(T& p){Delete(p);};
    template<typename T, typename U>inline void operator()(std::pair<T,U>& p){Delete(p.second);};
};
template<typename T>inline void Delete(std::vector<T>& s){for_each(s.begin(), s.end(), SeqDelete()); s.clear();};
template<typename T>inline void Delete(std::list<T>& s){for_each(s.begin(), s.end(), SeqDelete()); s.clear();};
template<typename T>inline void Delete(std::deque<T>& s){for_each(s.begin(), s.end(), SeqDelete()); s.clear();};
template<typename T, typename U>inline void Delete(std::map<T, U>& s){for_each(s.begin(), s.end(), SeqDelete()); s.clear();};

#endif
