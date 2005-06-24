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

typedef short int HighlightLanguage;
#define HL_NONE     0
#define HL_LAST     45

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

class EditorColorSet
{
	public:
		EditorColorSet(const wxString& setName = COLORSET_DEFAULT);
		EditorColorSet(const EditorColorSet& other); // copy ctor
		~EditorColorSet();
		
		void AddHighlightLanguage(HighlightLanguage lang, const wxString& name);
		HighlightLanguage GetHighlightLanguage(const wxString& name);
		wxArrayString GetAllHighlightLanguages();

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
		void UpdateOptionsWithSameName(HighlightLanguage lang, OptionColor* base);
		int GetOptionCount(HighlightLanguage lang){ return m_Colors[lang].GetCount(); }
		HighlightLanguage GetLanguageForFilename(const wxString& filename);
		wxString GetLanguageName(HighlightLanguage lang);
		wxString GetName(){ return m_Name; }
		void SetName(const wxString& name){ m_Name = name; }
		void Apply(cbEditor* editor);
		void Apply(HighlightLanguage lang, wxStyledTextCtrl* control);
		void Save();
		void Reset(HighlightLanguage lang);
		wxString& GetKeywords(HighlightLanguage lang);
		void SetKeywords(HighlightLanguage lang, const wxString& keywords);
	protected:
	private:
		void DoApplyStyle(wxStyledTextCtrl* control, int value, OptionColor* option);
		void LoadAvailableSets();
		void Load();
		void ClearAllOptionColors();
		
		wxString m_Langs[HL_LAST];
		OptionColors m_Colors[HL_LAST];
		wxString m_Keywords[HL_LAST];
		wxString m_Name;
};

#endif // EDITORCOLORSET_H

