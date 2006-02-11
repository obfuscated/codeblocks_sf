#ifndef EDITORLEXERLOADER_H
#define EDITORLEXERLOADER_H

#include <wx/string.h>
#include "tinyxml/tinyxml.h"

class EditorColorSet;

class EditorLexerLoader
{
	public:
		EditorLexerLoader(EditorColorSet* target);
		virtual ~EditorLexerLoader();

		void Load(const wxString& filename);
	protected:
        void DoLexer(TiXmlElement* node);
        void DoStyles(HighlightLanguage language, TiXmlElement* node);
        void DoKeywords(HighlightLanguage language, TiXmlElement* node);
        void DoSingleKeywordNode(HighlightLanguage language, TiXmlElement* node, const wxString& nodename);
        void DoSampleCode(HighlightLanguage language, TiXmlElement* node);
        EditorColorSet* m_pTarget;
	private:
};

#endif // EDITORLEXERLOADER_H
