#define AA
#ifdef AA
void fly() {}
#endif

#define BB a::b::c
#define CC unsigned int

#define DD \
    ABC

#define EE(x)     void   hello(int x)
#define FF /*comment*/ (x, y) \
    void test(int x, int y)

#define GG /*comment*/ \
    (x, y) \
    void line(int x, int y)
