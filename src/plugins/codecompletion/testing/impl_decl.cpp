#include "impl_decl.h"

bool              ImplDecl::MethodA()                      { return true; }
void              ImplDecl::MethodB() const                { ;            }
const void*       ImplDecl::MethodC() const                { return 0;    }
bool              ImplDecl::MethodD(const    int      i  ) { return true; }
bool              ImplDecl::MethodE(const    int&     i  ) { return true; }
ImplDecl::int_foo ImplDecl::MethodF(const    int_foo  i  ) { int_foo l = 0; return l; }
bool              ImplDecl::MethodG(volatile float    f  ) { return true; }
bool              ImplDecl::MethodH(         float    FF ) { return true; } // different name of variable
bool              ImplDecl::MethodI(         float /*FF*/) { return true; }
bool              ImplDecl::MethodJ(         float UN(FF)) { return true; }

bool GlobalExt()    { return true; }
bool GlobalNonExt() { return true; }
