#define UN(identifier) /* identifier */

class ImplDecl
{
  typedef int int_foo;
  public:
    bool        MethodA();
    void        MethodB() const;
    const void* MethodC() const;
    bool        MethodD(const    int     i = 4 );
    bool        MethodE(const    int&    i = 5 );
    int_foo     MethodF(const    int_foo i = 6 );
    bool        MethodG(volatile float   f = 7.);
    bool        MethodH(         float   f = 8.);
    bool        MethodI(         float   /*FF*/);
    bool        MethodJ(         float   FF    );
    bool        MethodK(         float   f = 9.) { return true; };
};

bool        GlobalNonExt();
extern bool GlobalExt();

template <class T> T GetMax(T a, T b) { return (a>b?a:b); };
