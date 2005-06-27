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
        void DoStyles(int language, TiXmlElement* node);
        void DoKeywords(int language, TiXmlElement* node);
        void DoSingleKeywordNode(int language, TiXmlElement* node, const wxString& nodename);
        void DoSampleCode(int language, TiXmlElement* node);
        EditorColorSet* m_pTarget;
	private:
};

#endif // EDITORLEXERLOADER_H
