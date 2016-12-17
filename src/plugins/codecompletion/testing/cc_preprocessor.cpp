// this is mainly modified from preprocessor.cpp

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

AClass obj;

// macro definition contains handle string literals
// the string literal also contains a c++ comments //
// see SF ticket report: Code::Blocks / Tickets / #278 parser error: string literal inside macro definition - https://sourceforge.net/p/codeblocks/tickets/278/
// and Code::Blocks / Tickets / #393 CC: sizeof("") define with later use is killing parser - https://sourceforge.net/p/codeblocks/tickets/393/
#define TEST "http://www.test.com"

const char szStr[] = TEST;

void Test( void )
{
    printf( "%s\n", szStr );
}

int main()
{
    Test();
    return 0;
}

//Test      //Test
//MyCon     //MyConst
//MyCom     //MyComm,MyComplex
//MyComp    //MyComplex
//MyMu      //MyMultiline
//MyS       //MyStr1,MyStr2,MyStr3,MyStr4
//CON       //CONDITION_B,CONDITION_B_2,COND
//obj.      //i_integer,f_float