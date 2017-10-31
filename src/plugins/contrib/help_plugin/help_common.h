#ifndef HELP_COMMON_H
#define HELP_COMMON_H

#include <vector>
#include <utility>

#include <wx/string.h>

using std::vector;
using std::pair;

class HelpCommon
{
  public:

    // Patch by Yorgos Pagles: Add keyword case choises
    enum StringCase {Preserve=0,UpperCase,LowerCase};

    struct HelpFileAttrib
    {
      wxString name;
      bool isExecutable;
      bool openEmbeddedViewer;
      bool readFromIni;
      // Patch by Yorgos Pagles: Add keyword case and default
      // keyword substitution attributes in Help file attributes
      StringCase keywordCase;
      wxString defaultKeyword;

      HelpFileAttrib() : isExecutable(false), openEmbeddedViewer(false), readFromIni(false) { }
    };

    typedef pair<wxString, HelpFileAttrib> wxStringPair;
    typedef vector<wxStringPair> HelpFilesVector;

  private:
    static int m_DefaultHelpIndex;
    static int m_NumReadFromIni;

  public:
    static int getDefaultHelpIndex();
    static void setDefaultHelpIndex(int index);
    static int getNumReadFromIni();
    static void setNumReadFromIni(int num);
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

inline int HelpCommon::getNumReadFromIni()
{
	return m_NumReadFromIni;
}

inline void HelpCommon::setNumReadFromIni(int num)
{
	m_NumReadFromIni = num;
}

inline bool operator == (const HelpCommon::wxStringPair &str_pair, const wxString &value)
{
	return str_pair.first.CmpNoCase(value) == 0;
}

#endif // HELP_COMMON_H
