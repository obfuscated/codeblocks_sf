#ifndef CLASSBROWSERBUILDERTHREAD_H
#define CLASSBROWSERBUILDERTHREAD_H

#include <wx/thread.h>
#include <wx/treectrl.h>

#include "parser/token.h"
#include "parser/parser.h"

class ClassBrowserBuilderThread : public wxThread
{
    public:
        ClassBrowserBuilderThread(Parser* parser,
                                wxTreeCtrl& tree,
                                const wxString& active_filename,
                                BrowserOptions options,
                                TokensTree* pTokens);
        virtual ~ClassBrowserBuilderThread();
    protected:
        virtual void* Entry();
        void BuildTree();
        void BuildTreeNamespace(const wxTreeItemId& parentNode, Token* parent, const TokenFilesSet& currset);
        void AddTreeNamespace(const wxTreeItemId& parentNode, Token* parent,const TokenFilesSet& currset);
        void AddTreeNode(const wxTreeItemId& parentNode, Token* token, bool childrenOnly = false);

        Parser* m_pParser;
        wxTreeCtrl& m_Tree;
        wxString m_ActiveFilename;
        BrowserOptions m_Options;
        TokensTree* m_pTokens;
    private:
};

#endif // CLASSBROWSERBUILDERTHREAD_H
