#include <iostream>

// Note: Also don't forget to hover over #defines!

// CC for include statements:
//#include "nam

// ifdef / elif
#ifdef HAVE_STDIO
  #include <cstdio>
#elif HAVE_DUMMY
  #include <dummy>
#endif

// defines
#define MyConst   1
#define MyComm    /* a c-style comment */ 1
#define MyStr1    "this is a string"
#define MyStr2    "this is an \" escaped string"
#define MyStr3    "this is a string" /* short C style comment */
#define MyStr4    "this is a string" /* long multi-line
                                        C style comment */
#define MyComplex if (true) while (true) { break; } // a comment
#define MyMultiline while (true) \
                    { \
                      if (true) break; \
                    }

#if defined(__WXMOTIF__) || defined(__WXGTK__)
  #define wxSPECIAL1 wxID_OK | wxID_CANCEL
#else
  #define wxSPECIAL1
#endif

// nested
#define CONDITION_B   1
#define CONDITION_B_2 2

#if   CONDITION_A
  #if   CONDITION_A_1
    #define COND IS_A_1
  #elif CONDITION_A_2
    #define COND IS_A_2
  #endif
#elif CONDITION_B
  #if   CONDITION_B_1
    #define COND IS_B_1
  #elif CONDITION_B_2
    #define COND IS_B_2
  #endif
#endif

class MyClass1
{
  public:
           MyClass1() {};
  virtual ~MyClass1() {};
};

class MyClass2
{
  public:
           MyClass2() {};
  virtual ~MyClass2() {};
};

class AClass
#ifdef FOO
    : public MyClass1
    , public MyClass2
#endif
{
  public:

           AClass() {};
  virtual ~AClass() {};

  int   i_integer;
  float f_float;
};

#if 0
void a_function(int i_integer, float f_float)
{
}
#endif

#if defined(B_FUNCTION_ENABLED)
void b_function(int i_integer, float f_float)
{
}
#endif

int main (void)
{
//  HAVE
//  MyCon
//  MyCom
//  MyComp
//  MyMu
//  wxS
//  CON
}
