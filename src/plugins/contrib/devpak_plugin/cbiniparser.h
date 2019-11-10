#ifndef CB_INIPARSER_H
#define CB_INIPARSER_H

#include <wx/string.h>
#include <wx/dynarray.h>
#include <wx/textctrl.h>

struct IniKeyValuePair
{
    wxString key;
    wxString value;
};
WX_DECLARE_OBJARRAY(IniKeyValuePair, IniKeyValuePairArray);

struct IniGroup
{
    wxString name;
    IniKeyValuePairArray pairs;
};
WX_DECLARE_OBJARRAY(IniGroup, IniGroupArray);

class IniParser
{
	public:
		IniParser();
		~IniParser();
		
		bool ParseFile(const wxString& filename);
		bool ParseBuffer(wxString& buffer);
		
		int GetGroupsCount() const;
		const wxString& GetGroupName(int idx) const;
		int FindGroupByName(const wxString& name, bool caseSensitive = false) const;
		int GetKeysCount(int group) const;
		const wxString& GetKeyName(int group, int idx) const;
		const wxString& GetKeyValue(int group, int idx) const;
		const wxString& GetKeyValue(int group, const wxString& key) const;
		int FindKeyByName(int groupIdx, const wxString& name, bool caseSensitive = false) const;
		const wxString& GetValue(const wxString& group, const wxString& key, bool caseSensitive = false) const;
		void Sort(bool caseSensitive = false);
	protected:
        wxString ReadLineFromBuffer(wxString& buffer);
		IniGroupArray m_Array;
	private:
		
};


#endif // CB_INIPARSER_H
