#ifndef EDITORCOLORSET_H
#define EDITORCOLORSET_H

#include <wx/dynarray.h>
#include <wx/hashmap.h>
#include <wx/intl.h>
#include "settings.h"

// forward decls
class cbEditor;
class wxStyledTextCtrl;

#define COLORSET_DEFAULT	_("default")

enum HighlightLanguage
{
	hlNone = 0,
	hlCpp,
	hlLua
};

struct OptionColor
{
	wxString name;
	int value;
	wxColour fore;
	wxColour back;
	bool bold;
	bool italics;
	bool underlined;
	bool isStyle;
};

WX_DEFINE_ARRAY(OptionColor*, OptionColors);
WX_DECLARE_HASH_MAP(HighlightLanguage, OptionColors, wxIntegerHash, wxIntegerEqual, ColorsMap);
WX_DECLARE_HASH_MAP(HighlightLanguage, wxString, wxIntegerHash, wxIntegerEqual, KeywordsMap);

class EditorColorSet
{
	public:
		EditorColorSet(const wxString& setName = COLORSET_DEFAULT);
		EditorColorSet(const EditorColorSet& other); // copy ctor
		~EditorColorSet();
		
		void AddOption(HighlightLanguage lang,
						const wxString& name,
						int value,
						wxColour fore = wxNullColour,
						wxColour back = wxNullColour,
						bool bold = false,
						bool italics = false,
						bool underlined = false,
						bool isStyle = true);
		bool AddOption(HighlightLanguage lang, OptionColor* option, bool checkIfExists = true);
		OptionColor* GetOptionByName(HighlightLanguage lang, const wxString& name);
		OptionColor* GetOptionByValue(HighlightLanguage lang, int value);
		OptionColor* GetOptionByIndex(HighlightLanguage lang, int index);
		int GetOptionCount(HighlightLanguage lang){ return m_Colors[lang].GetCount(); }
		HighlightLanguage GetLanguageForFilename(const wxString& filename);
		wxString GetLanguageName(HighlightLanguage lang);
		void Apply(cbEditor* editor);
		void Apply(HighlightLanguage lang, wxStyledTextCtrl* control);
		void Save();
		wxString& GetKeywords(HighlightLanguage lang){ return m_Keywords[lang]; }
		void SetKeywords(HighlightLanguage lang, const wxString& keywords){ if (lang != hlNone) m_Keywords[lang] = keywords; }
	protected:
	private:
		void DoApplyStyle(wxStyledTextCtrl* control, int value, OptionColor* option);
		void LoadBuiltInSet(HighlightLanguage lang);
		void Load();
		void ClearAllOptionColors();
		
		ColorsMap m_Colors;
		KeywordsMap m_Keywords;
		wxString m_Name;
};

#endif // EDITORCOLORSET_H

