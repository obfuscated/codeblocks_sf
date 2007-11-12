#ifndef HELP_COMMON_H
#define HELP_COMMON_H

#include <wx/string.h>
#include <vector>
#include <utility>

using std::vector;
using std::pair;

class HelpCommon
{
  public:
    struct HelpFileAttrib
    {
      wxString name;
      bool isExecutable;
      bool openEmbeddedViewer;

      HelpFileAttrib() : isExecutable(false), openEmbeddedViewer(false) { }
    };

    typedef pair<wxString, HelpFileAttrib> wxStringPair;
    typedef vector<wxStringPair> HelpFilesVector;

  private:
    static int m_DefaultHelpIndex;

  public:
    static int getDefaultHelpIndex();
    static void setDefaultHelpIndex(int index);
    static void LoadHelpFilesVector(HelpFilesVector &vect);
    static void SaveHelpFilesVector(HelpFilesVector &vect);

	private:
	  // Block instantiation
    HelpCommon();
    ~HelpCommon();
};

// Inline member functions

inline int HelpCommon::getDefaultHelpIndex()
{
	return m_DefaultHelpIndex;
}

inline void HelpCommon::setDefaultHelpIndex(int index)
{
	m_DefaultHelpIndex = index;
}

inline bool operator == (const HelpCommon::wxStringPair &str_pair, const wxString &value)
{
	return str_pair.first.CmpNoCase(value) == 0;
}

#endif // HELP_COMMON_H
