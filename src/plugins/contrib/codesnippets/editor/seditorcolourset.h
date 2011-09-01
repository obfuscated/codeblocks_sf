/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef SEDITORCOLORSET_H
#define SEDITORCOLORSET_H

#include <wx/dynarray.h>
#include <wx/hashmap.h>
#include <wx/intl.h>
#include <wx/wxscintilla.h> // wxSCI_KEYWORDSET_MAX
#include "settings.h"

#ifndef CB_PRECOMP
    #include "globals.h" // HighlightLanguage
#endif

// forward decls
class ScbEditor;
class cbStyledTextCtrl;

#define COLORSET_DEFAULT	_T("default")

struct SOptionColour
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
WX_DEFINE_ARRAY(SOptionColour*, SOptionColours);

struct SOptionSet
{
    wxString m_Langs;
    SOptionColours m_Colours;
    wxString m_Keywords[wxSCI_KEYWORDSET_MAX + 1]; // wxSCI_KEYWORDSET_MAX+1 keyword sets
    wxArrayString m_FileMasks;
    int m_Lexers;
    wxString m_SampleCode;
    int m_BreakLine;
    int m_DebugLine;
    int m_ErrorLine;

    wxString m_originalKeywords[wxSCI_KEYWORDSET_MAX + 1]; // wxSCI_KEYWORDSET_MAX+1 keyword sets
    wxArrayString m_originalFileMasks;
};
WX_DECLARE_STRING_HASH_MAP(SOptionSet, SOptionSetsMap);

class SEditorColourSet
{
	public:
		SEditorColourSet(const wxString& setName = COLORSET_DEFAULT);
		SEditorColourSet(const SEditorColourSet& other); // copy ctor
		~SEditorColourSet();

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
		bool AddOption(HighlightLanguage lang, SOptionColour* option, bool checkIfExists = true);
		SOptionColour* GetOptionByName(HighlightLanguage lang, const wxString& name);
		SOptionColour* GetOptionByValue(HighlightLanguage lang, int value);
		SOptionColour* GetOptionByIndex(HighlightLanguage lang, int index);
		void UpdateOptionsWithSameName(HighlightLanguage lang, SOptionColour* base);
		int GetOptionCount(HighlightLanguage lang);
		HighlightLanguage GetLanguageForFilename(const wxString& filename);
		wxString GetLanguageName(HighlightLanguage lang);
		wxString GetName(){ return m_Name; }
		void SetName(const wxString& name){ m_Name = name; }
		HighlightLanguage Apply(ScbEditor* editor, HighlightLanguage lang=HL_AUTO);
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
		void DoApplyStyle(cbStyledTextCtrl* control, int value, SOptionColour* option);
		void LoadAvailableSets();
		void Load();
		void ClearAllOptionColours();

		wxString m_Name;
		SOptionSetsMap m_Sets;
};

#endif // SEDITORCOLORSET_H

