#ifndef EDITORLEXERLOADER_H
#define EDITORLEXERLOADER_H

#include <wx/string.h>

class EditorColourSet;
class LoaderBase;
class TiXmlElement;

class EditorLexerLoader
{
	public:
		EditorLexerLoader(EditorColourSet* target);
		virtual ~EditorLexerLoader();

		void Load(LoaderBase *loader);
	protected:
        void DoLexer(TiXmlElement* node);
        void DoStyles(HighlightLanguage language, TiXmlElement* node);
        void DoKeywords(HighlightLanguage language, TiXmlElement* node);
        void DoSingleKeywordNode(HighlightLanguage language, TiXmlElement* node, const wxString& nodename);
        void DoSampleCode(HighlightLanguage language, TiXmlElement* node);
        EditorColourSet* m_pTarget;
	private:
};

#endif // EDITORLEXERLOADER_H
