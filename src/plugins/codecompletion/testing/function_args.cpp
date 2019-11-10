class AClass
{
  public:

           AClass() {};
  virtual ~AClass() {};

  AClass& operator=(const AClass &from)
  {
//    i_
//    f_

//    i_integer = from.i_;
//    f_float   = from.f_;

    return *this;
  };

  int   i_integer;
  float f_float;
};

void a_function(int i_integer, float f_float)
{
//  i_
//  f_
}

AClass* b_function(AClass c_class, const AClass& c_class_const)
{
  static AClass a_class;

//  c_
//  a_class.i_integer = c_

  return &a_class; // oh dear...
}

void /* comment before */ c_function(int /* type_comment */ i_integer, float f_float /* default_value_comment */) // trailing comment
{
//  i_
//  f_
}

int main (void)
{
        int     i = 0;
        float   f = 0.;
        AClass  c;
  const AClass  c_const;
        AClass* c_ret;

          a_function(i, f);
  c_ret = b_function(c, c_const);
          c_function(i, f);
}
