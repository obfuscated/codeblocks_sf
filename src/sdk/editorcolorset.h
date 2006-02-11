#ifndef EDITORCOLORSET_H
#define EDITORCOLORSET_H

#include <wx/dynarray.h>
#include <wx/hashmap.h>
#include <wx/intl.h>
#include "settings.h"

// forward decls
class cbEditor;
class cbStyledTextCtrl;

#define COLORSET_DEFAULT	_T("default")

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

	wxColour originalfore;
	wxColour originalback;
	bool originalbold;
	bool originalitalics;
	bool originalunderlined;
	bool originalisStyle;
};
WX_DEFINE_ARRAY(OptionColor*, OptionColors);

struct OptionSet
{
    wxString m_Langs;
    OptionColors m_Colors;
    wxString m_Keywords[3]; // 3 keyword sets
    wxArrayString m_FileMasks;
    int m_Lexers;
    wxString m_SampleCode;
    int m_BreakLine;
    int m_DebugLine;
    int m_ErrorLine;

    wxString m_originalKeywords[3]; // 3 keyword sets
    wxArrayString m_originalFileMasks;
};
WX_DECLARE_STRING_HASH_MAP(OptionSet, OptionSetsMap);

typedef wxString HighlightLanguage;
#define HL_AUTO _T(" ")
#define HL_NONE _T("  ")

class EditorColorSet
{
	public:
		EditorColorSet(const wxString& setName = COLORSET_DEFAULT);
		EditorColorSet(const EditorColorSet& other); // copy ctor
		~EditorColorSet();

		HighlightLanguage AddHighlightLanguage(int lexer, const wxString& name);
		HighlightLanguage GetHighlightLanguage(int lexer); // from scintilla lexer (wxSCI_LEX_*)
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
		int GetOptionCount(HighlightLanguage lang);
		HighlightLanguage GetLanguageForFilename(const wxString& filename);
		wxString GetLanguageName(HighlightLanguage lang);
		wxString GetName(){ return m_Name; }
		void SetName(const wxString& name){ m_Name = name; }
		HighlightLanguage Apply(cbEditor* editor, HighlightLanguage lang=HL_AUTO);
		void Apply(HighlightLanguage lang, cbStyledTextCtrl* control);
		void Save();
		void Reset(HighlightLanguage lang);
		wxString& GetKeywords(HighlightLanguage lang, int idx);
		void SetKeywords(HighlightLanguage lang, int idx, const wxString& keywords);
		const wxArrayString& GetFileMasks(HighlightLanguage lang);
		void SetFileMasks(HighlightLanguage lang, const wxString& masks, const wxString& = _(","));
		wxString GetSampleCode(HighlightLanguage lang, int* breakLine, int* debugLine, int* errorLine);
		void SetSampleCode(HighlightLanguage lang, const wxString& sample, int breakLine, int debugLine, int errorLine);
	protected:
	private:
		void DoApplyStyle(cbStyledTextCtrl* control, int value, OptionColor* option);
		void LoadAvailableSets();
		void Load();
		void ClearAllOptionColors();

		wxString m_Name;
		OptionSetsMap m_Sets;
};

#endif // EDITORCOLORSET_H

