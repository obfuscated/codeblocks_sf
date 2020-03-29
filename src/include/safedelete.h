/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef DELETEFUN
#define DELETEFUN

#include <map>
#include <vector>
#include <list>
#include <deque>
#include <algorithm>

struct SeqDelete
{
    template<typename T>inline void operator()(T& p){Delete(p);}
    template<typename T, typename U>inline void operator()(std::pair<T,U>& p){Delete(p.second);}
};
template<typename T>inline void Delete(std::vector<T>& s){for_each(s.begin(), s.end(), SeqDelete()); s.clear();}
template<typename T>inline void Delete(std::list<T>& s){for_each(s.begin(), s.end(), SeqDelete()); s.clear();}
template<typename T>inline void Delete(std::deque<T>& s){for_each(s.begin(), s.end(), SeqDelete()); s.clear();}
template<typename T, typename U>inline void Delete(std::map<T, U>& s){for_each(s.begin(), s.end(), SeqDelete()); s.clear();}

#endif
