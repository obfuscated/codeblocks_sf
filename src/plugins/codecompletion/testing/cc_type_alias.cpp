// test case for SF #158 Code completion fails with the "using Alias = Type" syntax in C++11
// https://sourceforge.net/p/codeblocks/tickets/158/

class info
{
    public:
        int m_aaa;
};

namespace ns1
{
    namespace ns2
    {
        class cs
        {
        public:
            int m_bbb;
        };
    }
}

namespace ns3
{
    template<typename T> class vec
    {
    public:
        T& at();
    };
}

typedef info AAA;
using BBB = info;
using CCC = ns1::ns2;
CCC::cs DDD;

template <class T>
using EEE = ns3::vec<T>;

EEE<info> FFF;

//AAA.          //m_aaa
//BBB.          //m_aaa
//CCC::cs.      //m_bbb
//DDD.          //m_bbb
//FFF.at().     //m_aaa