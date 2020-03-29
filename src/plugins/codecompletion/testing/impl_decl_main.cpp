#include "impl_decl.h"

bool Global() { return true; }

int main (void)
{
    ImplDecl id;
    int i = 4; int j = 6;
    int k = GetMax(i, j);
    return (GlobalExt() ? k - id.MethodF(42) : 42);
}
