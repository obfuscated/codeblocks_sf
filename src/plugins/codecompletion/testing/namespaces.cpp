#include "namespaces.h"

namespace my_namespace
{
  namespace
  {
    // Place cursor here -> is the namespace correctly shown in the toolbar?
    const int ConstInt = 10;
  }

  bool NameSpaces::MethodA() const
  {
    // Place cursor here -> is the namespace correctly shown in the toolbar?
    return true;
  }

  void NameSpaces::MethodB()
  {
    // Place cursor here -> is the namespace correctly shown in the toolbar?
  }
}
