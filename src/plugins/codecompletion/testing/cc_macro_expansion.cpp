/****************************************/

#define ADD(x,y)  (x+y)
#define ONE 1
#define TWO 2

#define ZERO 0


#if ADD(ONE,TWO) == 3

void f1234();

#endif

void f2345();

#if defined TWO && defined ( THREE )
void test_defined_expand_1();
#elif defined ONE
void test_defined_expand_2();
#endif

#if defined (ZERO)
int zero_defined;
#endif

A(B,C);

// see whether we correctly handle ## operator
#define _GLIBCXX_PSEUDO_VISIBILITY_default
#define _GLIBCXX_PSEUDO_VISIBILITY(V) _GLIBCXX_PSEUDO_VISIBILITY_ ## V
# define _GLIBCXX_VISIBILITY(V) _GLIBCXX_PSEUDO_VISIBILITY(V)
namespace std _GLIBCXX_VISIBILITY(default)
{
    class string { };
}



// std::        //string
// f123         //f1234
// f234         //f2345
// test_defined //test_defined_expand_2
// zero_        //zero_defined