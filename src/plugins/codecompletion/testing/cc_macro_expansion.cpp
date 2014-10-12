/****************************************/

#define A(x,y)  (x+y)
#define B 1
#define C 2


#if A(B,C) == 3

void f1234();

#endif

void f2345();

#if (defined B) && (defined ( C ))
void test_defined_expand();
#endif


A(B,C);



// f123      //f1234
// f234      //f2345
// test_defined_expand //test_defined_expand
