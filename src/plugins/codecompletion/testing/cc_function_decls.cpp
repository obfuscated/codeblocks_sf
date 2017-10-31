#include <string>
#include <vector>

extern "C"
{
  double ExternallyC(double* my_double, std::vector<int> int_vec)
  {
    // nothing
    return 1.0;
  }
}

class TheClass
{
public:
                  TheClass  ()                   { ; }
  virtual        ~TheClass  ()                   { ; }
  int             GetInt    (size_t idx) const   { int i=0; return i; }
  const TheClass* GetClass  (const TheClass& tc) { return this; }
  static void     StaticVoid()                   { ; }
  void            Void      (void* pVoid)        { ; }
};

extern int ExternInt(TheClass* the_class, int my_int);

int InternInt(TheClass* the_class, int my_int)
{
  // nothing
  return 42;
}

void VoidFunction(void* my_void_pointer, int my_int)
{
  // nothing
}

std::string GetString(TheClass* the_class, const std::string& in_str)
{
  std::string str(in_str);
  return str;
}

const float& ConstFloatRef(void)
{
  // nothing
  static float f = 0.0;
  return f;
}

static int StaticInt(const int& in_int)
{
  // nothing
  int i = 1;
  return i;
}

struct S
{
  int i;
};
S g_S;


TheClass tc;
std::string str;
std::vector<TheClass> vt;


// str.   		//size,length
// tc.GetI  	//GetInt
// tc.GetC  	//GetClass
// tc.St    	//StaticVoid
// tc.Vo    	//Void
// g_       	//g_S
// g_S.     	//i
// vt[1]. 		//GetInt