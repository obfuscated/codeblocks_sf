// bug report here:
// Code::Blocks / Tickets / #175 CC patch for typedef declarations in class templates - https://sourceforge.net/p/codeblocks/tickets/175/

class string
{
public:
    int m_aaa;
};

class c1
{
public:
    typedef string s1;
};


template<typename _Tp>
class c2
{
public:
    typedef string s2;
};


//c1.s1.    //m_aaa
//c2.s2.    //m_aaa