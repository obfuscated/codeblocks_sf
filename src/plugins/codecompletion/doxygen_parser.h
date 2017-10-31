/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef DOXYGENPARSER_H
#define DOXYGENPARSER_H

#include <set>

#if wxUSE_POPUPWIN
    #include <wx/popupwin.h>
#else
    #include <wx/frame.h>
#endif // wxUSE_POPUPWIN

#include "parser/token.h" // TokenIdxSet

class CodeCompletion;
class TokenTree;

class cbEditor;
class ConfigManager;

class wxHtmlLinkEvent;

namespace Doxygen
{
    enum KeywordsIds
    {
        NO_KEYWORD,

        PARAM,
        RETURN, RESULT,
        BRIEF, SHORT,
        SA, SEE,

        // structural keywords:
        CLASS, STRUCT,
        UNION,
        ENUM,
        NAMESPACE,

        FN,
        VAR,
        DEF,

        // some other keywords:
        CODE,
        ENDCODE,

        NESTED_KEYWORDS_BEGIN,

        B = NESTED_KEYWORDS_BEGIN,

        KEYWORDS_COUNT // Always at end
    };

    enum KeywordsRanges
    {
        RANGE_UNKNOWN,
        RANGE_PARAGRAPH,
        RANGE_BLOCK,
        RANGE_LINE,
        RANGE_WORD
    };


    struct DoxygenParser
    {
        static const wxString Keywords[];
        static const int KwCount;
        static const wxString NewLineReplacment;

        DoxygenParser();

        int FindNextKeyword(const wxString& doc);
        int GetArgument(const wxString& doc, int range, wxString& output);
        int GetPosition() const;
        void ReplaceInDoc(wxString& doc, size_t start, size_t count, const wxString& str);
        void ReplaceCurrentKeyword(wxString& doc, const wxString& str);

    protected:
        int CheckKeyword(const wxString& doc);
        int GetParagraphArgument(const wxString& doc, wxString& output);
        void GetWordArgument(const wxString& doc, wxString& output);
        void GetBlockArgument(const wxString& doc, wxString& output);
        int GetLineArgument(const wxString& doc, wxString& output);
        bool IsKeywordBegin(const wxString& doc) const;
        bool IsOneOf(wxChar c, const wxChar* chars) const;

    protected:
        bool IsEnd(const wxString& doc) const;
        int GetEndLine(const wxString& doc) const;
        //! \return true if m_Pos has changed
        bool SkipDecorations(const wxString& doc);
        //! \return true if m_Pos has changed
        bool HandleNewLine(const wxString& doc, wxString& output, const wxString& replaceWith = NewLineReplacment);

        int m_FoundKw;   // index to Keywors array
        int m_Pos;          // index to doc
    };

}//namespace Doxygen


class DocumentationHelper
{

public:
    enum Command
    {
        cmdNone,
        cmdDisplayToken,    // args: token index
        cmdSearch,          // args: token name
        cmdSearchAll,       // args: token name
        cmdOpenDecl,        // args: token index
        cmdOpenImpl,        // args: token index
        cmdClose            // args: -----
    };

    //helper functions:
    static wxString DoxygenToHTML(const wxString& doc);
    static wxString ConvertTypeToAnchor(wxString fullType);
    static wxString ConvertArgsToAnchors(wxString args);
    /*! returns argument base type */
    static wxString ExtractTypeAndName(wxString type, wxString* outName = 0);
    static wxString CommandToAnchor(Command cmd, const wxString& name, const wxString* args = 0);
    static wxString CommandToAnchorInt(Command cmd, const wxString& name, int arg0);
    static Command HrefToCommand(const wxString& href, wxString& args);

    static const wxChar   separatorTag;
    static const wxString commandTag;

    DocumentationHelper(CodeCompletion* cc);
    ~DocumentationHelper();

    void OnAttach();
    void OnRelease();
    wxString GenerateHTML(int tokenIdx, TokenTree* tree);
    wxString GenerateHTML(const TokenIdxSet& tokensIdx, TokenTree* tree);
    void RereadOptions(ConfigManager* cfg);
    void WriteOptions(ConfigManager* cfg);

    bool IsEnabled()              { return m_Enabled;    }
    void SetEnabled(bool enabled) { m_Enabled = enabled; }

protected:
    void SaveTokenIdx();
public:
    wxString OnDocumentationLink(wxHtmlLinkEvent& event, bool& dismissPopup);

    /*Members:*/
protected:
    /** Pointer to CodeComplete object */
    CodeCompletion* m_CC;
    /** Documentation of which token was previously displayed */
    int m_CurrentTokenIdx, m_LastTokenIdx;
    // User options
    bool m_Enabled;
};

#endif //DOXYGENPARSER_H
