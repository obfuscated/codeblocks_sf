// test case for SF #178 CC doesn't work for element access functions/operators belonging to STL containers. 
// https://sourceforge.net/p/codeblocks/tickets/178/

#include <vector>
#include <deque>
#include <forward_list>
#include <list>
#include <stack>
#include <queue>
#include <map>


using namespace std;


class info
{
    public:
        int m_aaa;
};


class mycomp
{
public:
    bool operator() (const info& lhs, const info&rhs) const
    {
        return (lhs.m_aaa<rhs.m_aaa);
    }
};


std::vector<info> vec;
std::vector<info*> vec_ptr;
std::deque<info> deq;
std::forward_list<info> frw;
std::list<info> lis;
std::stack<info> sta;
std::queue<info> que;
std::priority_queue<info, vector<info>, mycomp> pri;
std::map<info, info> mpp;


//vec.at().         //m_aaa
//vec[].            //m_aaa
//vec_ptr.at()->    //m_aaa
//vec_ptr[]->       //m_aaa
//deq.front().      //m_aaa
//frw.front().      //m_aaa
//lis.front().      //m_aaa
//sta.top().        //m_aaa
//que.front().      //m_aaa
//pri.top().        //m_aaa
//mpp.at().         //m_aaa
//mpp[].            //m_aaa