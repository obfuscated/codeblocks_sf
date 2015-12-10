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

  namespace nested_namespace
  {
    bool variable;
  }
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
};

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

// test the bug reported here:
// http://forums.codeblocks.org/index.php/topic,20729.msg141341.html#msg141341
// where, if the _GLIBCXX_PSEUDO_VISIBILITY is undefined, we still need to parse the body
// of the namespace std
# define _GLIBCXX_VISIBILITY(V) _GLIBCXX_PSEUDO_VISIBILITY(V)
namespace std _GLIBCXX_VISIBILITY(default)
{
    class vector
    {
        size_t size();
    }
}


//temp_        //temp_string
//newline_     //newline_string
//Logger.      //caption,info,warning,success,error
//my_namespace.  //ConstInt
//my_namespace.NameSpaces.  //MethodA,MethodB
//my_namespace.nested_namespace. //variable
//std::vector.size  //size
