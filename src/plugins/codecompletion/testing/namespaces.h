#include <wx/string>

namespace my_namespace
{
  class NameSpaces
  {
  public:
    bool MethodA() const;
    void MethodB();

  private:
    int* m_Int1;
    int* m_Int2;
  };
}

#define DLLIMPORT __declspec (dllexport)

namespace
{
    static wxString temp_string(_T('\0'), 250);
    static wxString newline_string(_T("\n"));
}

class DLLIMPORT Logger
{
public:
    enum level { caption, info, warning, success, error };
}
