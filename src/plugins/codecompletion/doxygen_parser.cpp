/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#include <sdk.h>

#ifndef CB_PRECOMP
  #include <wx/string.h>

  #include <cbeditor.h>
  #include <editormanager.h>
  #include <sdk_events.h>
#endif

#include <wx/tokenzr.h>
#include <wx/html/htmlwin.h>

#include <cbstyledtextctrl.h>

#include "doxygen_parser.h"

#include "cbcolourmanager.h"
#include "codecompletion.h"

// Doxygen documents parser
namespace Doxygen
{
    /*  source: http://www.stack.nl/~dimitri/doxygen/commands.html
        Some commands have one or more arguments. Each argument has a
            certain range:

            If <sharp> braces are used the argument is a single word.

            If (round) braces are used the argument extends until the end of
                the line on which the command was found.

            If {curly} braces are used the argument extends until the next
                paragraph. Paragraphs are delimited by a blank line or by
                a section indicator.

        If in addition to the above argument specifiers [square] brackets
            are used the argument is optional.
    */
    const wxString DoxygenParser::Keywords[] = {
        _T(""),                     // no keyword
        _T("param"),                // \param [(dir)] <parameter-name> { parameter description }
        _T("return"), _T("result"), // \return { description of the return value }
        _T("brief"), _T("short"),   // \brief { brief description }
        _T("sa"), _T("see"),        // \sa { references }

        // structural commands:
        _T("class"), _T("struct"),  //
        _T("union"),                //
        _T("enum"),                 //
        _T("namespace"),            //

        _T("fn"),                   //
        _T("var"),                  //
        _T("def"),                  //

        _T("code"),                 //
        _T("endcode"),              //

        _T("b"),                    //

    };

    const int DoxygenParser::KwCount = sizeof(DoxygenParser::Keywords)/sizeof(DoxygenParser::Keywords[0]);

    const wxString DoxygenParser::NewLineReplacment = _T("\n");

    DoxygenParser::DoxygenParser() :
        m_FoundKw(-1),
        m_Pos(-1)
    {
        assert(KwCount == KEYWORDS_COUNT);
    }

    int DoxygenParser::FindNextKeyword(const wxString& doc)
    {
        ++m_Pos;
        if (m_Pos >= (int)doc.size())
            return KEYWORDS_COUNT;

        if ( IsKeywordBegin(doc) )
        {
            ++m_Pos;
            int fkw = CheckKeyword(doc);
            if (fkw != NO_KEYWORD)
                return fkw;
        }

        return NO_KEYWORD;
    }

    int DoxygenParser::GetArgument(const wxString& doc, int range, wxString& output)
    {
        SkipDecorations(doc);

        int nestedArgsCount = 0;
        switch (range)
        {
        case RANGE_PARAGRAPH:
            nestedArgsCount = GetParagraphArgument(doc, output);
            break;
        case RANGE_WORD:
            GetWordArgument(doc, output);
            break;
        case RANGE_BLOCK:
            GetBlockArgument(doc, output);
            break;
        case RANGE_LINE:
            nestedArgsCount = GetLineArgument(doc,output);
            break;
        default:
            break;
        }
        --m_Pos;
        return nestedArgsCount;
    }

    int DoxygenParser::GetPosition() const
    {
        return m_Pos;
    }

    void DoxygenParser::ReplaceInDoc(wxString& doc, size_t start, size_t count,
                      const wxString& str)
    {
        if (start < (size_t)m_Pos)
        {
            doc.replace(start, count, str);
            m_Pos += str.size() - count;
        }
        else
            doc.replace(start, count, str);
    }

    void DoxygenParser::ReplaceCurrentKeyword(wxString& doc, const wxString& str)
    {
        const wxString& kw = DoxygenParser::Keywords[m_FoundKw];
        int posBegin = m_Pos - kw.size();
        ReplaceInDoc(doc, posBegin - 1, kw.size()+1, str);
    }

    int DoxygenParser::CheckKeyword(const wxString& doc)
    {
        int kwLen = 0;
        int machingKwCount = KwCount;
        bool foundOne = false;
        bool isKw[KEYWORDS_COUNT] = {};
        for (int j = 0; j < KEYWORDS_COUNT; ++j)
            isKw[j] = true;

        while (m_Pos < (int)doc.size() && !foundOne)
        {
            for (int k = 0; k < KwCount; ++k)
            {
                if ( isKw[k] && (kwLen >= (int)Keywords[k].size() ||
                                   doc[m_Pos + kwLen] != Keywords[k][kwLen]) )
                {
                    isKw[k] = false;
                    --machingKwCount;
                    if (machingKwCount == 1)
                    {
                        foundOne = true;
                        //end loop:
                        k = KwCount;
                        --kwLen;
                    }
                }//if

            }// for (k)
            ++kwLen;
        }//while

        if (!foundOne)
            return NO_KEYWORD;

        int foundKw = 0;
        for (int l = 0; l < KwCount; ++l)
        {
            if (isKw[l])
            {
                foundKw = l;
                break;
            }
        }// for (l)

        if (doc.size() < m_Pos + Keywords[foundKw].size())
            return NO_KEYWORD;

        while (kwLen < (int)Keywords[foundKw].size())
        {
            if (isKw[foundKw])
                isKw[foundKw] = doc[m_Pos + kwLen] == Keywords[foundKw][kwLen];
            else
                return NO_KEYWORD;

            ++kwLen;
        }
        //now kwLen = Keywords[foundKw].size()

        if (m_Pos + kwLen < (int)doc.size())
        {
            if ( !IsOneOf(doc[m_Pos + kwLen], _T(" \t\n")))
                return NO_KEYWORD;
        }

        //got valid keyword
        m_FoundKw = foundKw;
        m_Pos = m_Pos + kwLen;
        return foundKw;
    }

    int DoxygenParser::GetParagraphArgument(const wxString& doc, wxString& output)
    {
        int nestedArgsCount = 0;
        while (m_Pos < (int)doc.size())
        {
            int tmpPos = m_Pos;
            nestedArgsCount += GetLineArgument(doc, output);
            HandleNewLine(doc,output,_T(' '));
            if (doc[m_Pos] == _T('\n') || m_Pos == tmpPos)
            {
                //++i;
                break;
            }

        }
        return nestedArgsCount;
    }

    void DoxygenParser::GetWordArgument(const wxString& doc, wxString& output)
    {
        bool gotWord = false;
        while (m_Pos < (int)doc.size())
        {
            wxChar c = doc[m_Pos];
            switch (c)
            {
            case _T('\n'):
            case _T(' '):
            case _T('\t'):
                if (gotWord)
                    return;

                ++m_Pos;
                break;
            default:
                output += doc[m_Pos];
                ++m_Pos;
                gotWord = true;
            }
        }
    }

    void DoxygenParser::GetBlockArgument(const wxString& doc, wxString& output)
    {
        //TODO: add implementation
        (void)doc;
        (void)output;
    }

    int DoxygenParser::GetLineArgument(const wxString& doc, wxString& output)
    {
        int nestedArgsCount = 0;
        while (m_Pos < (int)doc.size())
        {
            wxChar c = doc[m_Pos];
            switch (c)
            {
            case _T('\n'):
                //SkipDecorations(doc);
                return nestedArgsCount;
                break;

            case _T('@'):
            case _T('\\'):
                if ( IsKeywordBegin(doc) )
                {
                    int tmpI = m_Pos;
                    ++m_Pos;
                    int kw = CheckKeyword(doc);
                    m_Pos = tmpI;
                    if (kw < NESTED_KEYWORDS_BEGIN && kw != NO_KEYWORD)
                        return nestedArgsCount;
                    else
                    {
                        output += doc[m_Pos];
                        ++nestedArgsCount;
                    }
                }
                ++m_Pos;
                break;

            default:
                output += doc[m_Pos];
                ++m_Pos;
            }// switch

        }// while
        return nestedArgsCount;
    }


    bool DoxygenParser::IsKeywordBegin(const wxString& doc) const
    {
        bool isSpecial = doc[m_Pos] == _T('@') || doc[m_Pos] == _T('\\');

        if (!isSpecial)
            return false;

        if (m_Pos > 0)
        {
            bool isPrevWhitespace = doc[m_Pos - 1] == _T(' ') ||
                doc[m_Pos - 1] == _T('\n') || doc[m_Pos - 1] == _T('\t');

            return isPrevWhitespace;
        }

        if (m_Pos == 0)
            return true;

        return false;
    }

    bool DoxygenParser::IsOneOf(wxChar c, const wxChar* chars) const
    {
        while (*chars)
        {
            if (c == *chars)
                return true;
            ++chars;
        }
        return false;
    }

    bool DoxygenParser::IsEnd(const wxString& doc) const
    {
        return m_Pos >= (int)doc.size();
    }

    int DoxygenParser::GetEndLine(const wxString& doc) const
    {
        size_t endLine = doc.find(_T('\n'), m_Pos);
        if (endLine == wxString::npos)
            endLine = doc.size();
        return endLine;
    }

    bool DoxygenParser::SkipDecorations(const wxString& doc)
    {
        //ignore everything from beginig of line to first word
        if (doc[m_Pos] != _T('\n'))
            return false;
        ++m_Pos;

        while (!IsEnd(doc) && IsOneOf(doc[m_Pos], _T(" \t*/")))
            ++m_Pos;

        return true;
    }

    bool DoxygenParser::HandleNewLine(const wxString& doc, wxString& output,
                                      const wxString& replaceWith)
    {
        if ( SkipDecorations(doc) )
        {
            output += replaceWith;
            return true;
        }
        return false;
    }

}// end of Doxygen namespace


/* Unfocusable popup */


//imported with small changes from PlatWX.cpp
class UnfocusablePopupWindow :
#if wxUSE_POPUPWIN
    public wxPopupWindow
{
public:
    typedef wxPopupWindow BaseClass;

    UnfocusablePopupWindow(wxWindow* parent, int style = wxBORDER_NONE) :
        wxPopupWindow(parent, style)
#else
     public wxFrame
{
public:
    typedef wxFrame BaseClass;

    UnfocusablePopupWindow(wxWindow* parent, int style = 0) :
        wxFrame(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                style | wxFRAME_NO_TASKBAR | wxFRAME_FLOAT_ON_PARENT | wxNO_BORDER | wxFRAME_SHAPED
#ifdef __WXMAC__
                | wxPOPUP_WINDOW
#endif
            )
#endif // wxUSE_POPUPWIN
    {
        Hide();
    }

    bool Destroy();
    void OnFocus(wxFocusEvent& event);
    void ActivateParent();

    virtual void DoSetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
    virtual bool Show(bool show = true);

private:
    DECLARE_EVENT_TABLE()
};

// On OSX and (possibly others) there can still be pending
// messages/events for the list control when Scintilla wants to
// close it, so do a pending delete of it instead of destroying
// immediately.
bool UnfocusablePopupWindow::Destroy()
{
#ifdef __WXMAC__
    // The bottom edge of this window is not getting properly
    // refreshed upon deletion, so help it out...
    wxWindow* p = GetParent();
    wxRect r(GetPosition(), GetSize());
    r.SetHeight(r.GetHeight()+1);
    p->Refresh(false, &r);
#endif
    if ( !wxPendingDelete.Member(this) )
        wxPendingDelete.Append(this);
    return true;
}

void UnfocusablePopupWindow::OnFocus(wxFocusEvent& event)
{
    ActivateParent();
    GetParent()->SetFocus();
    event.Skip();
}

void UnfocusablePopupWindow::ActivateParent()
{
    // Although we're a frame, we always want the parent to be active, so
    // raise it whenever we get shown, focused, etc.
    wxTopLevelWindow *frame = wxDynamicCast(
        wxGetTopLevelParent(GetParent()), wxTopLevelWindow);
    if (frame)
        frame->Raise();
}

void UnfocusablePopupWindow::DoSetSize(int x, int y,
                       int width, int height,
                       int sizeFlags)
{
    // convert coords to screen coords since we're a top-level window
    if (x != wxDefaultCoord)
        GetParent()->ClientToScreen(&x, NULL);

    if (y != wxDefaultCoord)
        GetParent()->ClientToScreen(NULL, &y);

    BaseClass::DoSetSize(x, y, width, height, sizeFlags);
}

bool UnfocusablePopupWindow::Show(bool show)
{
    bool rv = BaseClass::Show(show);
    if (rv && show)
        ActivateParent();
#ifdef __WXMAC__
    GetParent()->Refresh(false);
#endif
    return rv;
}

BEGIN_EVENT_TABLE(UnfocusablePopupWindow, UnfocusablePopupWindow::BaseClass)
    EVT_SET_FOCUS(UnfocusablePopupWindow::OnFocus)
END_EVENT_TABLE()

/* end of Unfocusable popup */


// (shamelessly stolen from mime handler plugin ;) )
// build all HTML font sizes (1..7) from the given base size
static void wxBuildFontSizes(int *sizes, int size)
{
    // using a fixed factor (1.2, from CSS2) is a bad idea as explained at
    // http://www.w3.org/TR/CSS21/fonts.html#font-size-props but this is by far
    // simplest thing to do so still do it like this for now
    sizes[0] = int(size * 0.75); // exception to 1.2 rule, otherwise too small
    sizes[1] = int(size * 0.83);
    sizes[2] = size;
    sizes[3] = int(size * 1.2);
    sizes[4] = int(size * 1.44);
    sizes[5] = int(size * 1.73);
    sizes[6] = int(size * 2);
}

// (shamelessly stolen from mime handler plugin ;) )
static int wxGetDefaultHTMLFontSize()
{
    // base the default font size on the size of the default system font but
    // also ensure that we have a font of reasonable size, otherwise small HTML
    // fonts are unreadable
    int size = wxNORMAL_FONT->GetPointSize();
    if ( size < 9 )
        size = 9;

    return size;
}

/*Documentation Popup*/

BEGIN_EVENT_TABLE(DocumentationHelper,DocumentationHelper::BaseClass)
    EVT_HTML_LINK_CLICKED(wxID_ANY, DocumentationHelper::OnLink)
END_EVENT_TABLE()

namespace HTMLTags
{
    static const wxString br = _T("<br>");
    static const wxString sep = _T(" ");
    static const wxString b1 = _T("<b>");
    static const wxString b0 = _T("</b>");

    static const wxString a1 = _T("<a>");
    static const wxString a0 = _T("</a>");

    static const wxString i1 = _T("<i>");
    static const wxString i0 = _T("</i>");

    static const wxString pre1 = _T("<pre>");
    static const wxString pre0 = _T("</pre>");

    static const wxString nbsp(_T("&nbsp;"));
    static const wxString tab = nbsp + nbsp + nbsp;
}

/* DocumentationPopup static member functions implementation: */

wxString DocumentationHelper::DoxygenToHTML(const wxString& doc)
{
    using namespace HTMLTags;

    wxString arguments[5];
    wxString &plainText = arguments[0], &brief = arguments[1], &params = arguments[2],
        &seeAlso = arguments[3], &returns = arguments[4];

    Doxygen::DoxygenParser parser;
    int keyword = parser.FindNextKeyword(doc);
    while (keyword < Doxygen::KEYWORDS_COUNT)
    {
        using namespace Doxygen;
        switch (keyword)
        {
        case NO_KEYWORD:
            parser.GetArgument(doc, RANGE_PARAGRAPH, plainText);
            break;
        case PARAM:
            params += tab;
            parser.GetArgument(doc, RANGE_PARAGRAPH, params);
            params += br;
            break;
        case BRIEF:
        case Doxygen::SHORT:
            parser.GetArgument(doc, RANGE_PARAGRAPH, brief);
            break;
        case RETURN:
        case RESULT:
            parser.GetArgument(doc, RANGE_PARAGRAPH, returns);
            break;
        case SEE:
        case SA:
            parser.GetArgument(doc, RANGE_PARAGRAPH, seeAlso);
            break;
        case CODE:
            plainText += pre1;
            break;
        case ENDCODE:
            plainText += pre0;
            break;
        default:
            break;
        }
        keyword = parser.FindNextKeyword(doc);
    }
    // process nested keywords:
    for (size_t i = 0; i < (size_t)(sizeof(arguments)/sizeof(arguments[0])); ++i)
    {
        arguments[i].Trim(true).Trim(false);

        Doxygen::DoxygenParser dox_parser;
        int dox_keyword = dox_parser.FindNextKeyword(arguments[i]);
        while (dox_keyword < Doxygen::KEYWORDS_COUNT)
        {
            using namespace Doxygen;
            switch (dox_keyword)
            {
            case B:
                {
                    dox_parser.ReplaceCurrentKeyword(arguments[i], b1);
                    wxString arg0;
                    dox_parser.GetArgument(arguments[i], RANGE_WORD, arg0);
                    arguments[i].insert(dox_parser.GetPosition() + 1, b0);
                }
                break;
            default:
                break;
            }
            dox_keyword = dox_parser.FindNextKeyword(arguments[i]);
        }
    }// for (i)

    wxString html;
    html.reserve(doc.size());

    if (brief.size() > 0)
        html += b1 + brief + b0 + br;

    if (params.size() > 0)
        html += b1 + _T("Parameters:") + b0 + br + params;

    if (returns.size() > 0)
        html += b1 + _T("Returns:") + b0 + br + tab + returns + br;

    if (plainText.size()>0)
    {
        plainText.Trim(false);
        plainText.Trim(true);
        html += b1 + _T("Description:") + b0 + br + tab;
        plainText.Replace(_T("\n"), br + tab);
        html += plainText + br;
    }

    if (seeAlso.size() > 0)
    {
        html += b1 + _T("See also: ") + b0;
        wxStringTokenizer tokenizer(seeAlso, _T(" \n\t,;"));
        while ( tokenizer.HasMoreTokens() )
        {
            const wxString& tok = tokenizer.GetNextToken();
            if (tok.size() > 0)
                html += CommandToAnchor(cmdSearchAll, tok, &tok) + _T(" ");
        }
    }

    return html;
}

wxString DocumentationHelper::ConvertTypeToAnchor(wxString fullType)
{
    static Token ancestorChecker(_T(""), 0, 0, 0); // Because IsValidAncestor isn't static
    const wxString& argType = ExtractTypeAndName(fullType);
    if (!ancestorChecker.IsValidAncestor(argType))
        return fullType;

    size_t found = fullType.find(argType);
    fullType.replace(found, argType.size(), CommandToAnchor(cmdSearch, argType, &argType) );
    return fullType;
}

wxString DocumentationHelper::ConvertArgsToAnchors(wxString args)
{
    if (args.size() == 0)
        return args;

    //removes '(' and ')'
    wxStringTokenizer tokenizer(args.SubString(1,args.find_last_of(_T(')'))-1), _T(","));
    args.clear();
    while ( tokenizer.HasMoreTokens() )
    {
        wxString tok = tokenizer.GetNextToken();
        args += ConvertTypeToAnchor(tok);
        if (tokenizer.HasMoreTokens())
            args += _T(", ");
    }
    return _T('(') + args +_T(')');
}

//! \return type
wxString DocumentationHelper::ExtractTypeAndName(wxString tok, wxString* outName)
{
    // remove default argument
    size_t eqPos = tok.Find(_T('='));
    if (eqPos != wxString::npos)
        tok.resize(eqPos);

    tok.Replace(_T("*"), _T(" "),true); //remove all '*'
    tok.Replace(_T("&"), _T(" "),true); //remove all '&'
    if (tok.GetChar(0) != _T(' '))
        tok.insert(0u, _T(" ")); //it will be easer to find " const " and " volatile "

    //remove cv:
    tok.Replace(_T(" const "), _T(" "),true);
    tok.Replace(_T(" volatile "), _T(" "),true);
    tok.Trim(true);
    //tok.Trim(false);

    wxString _outName;
    if (!outName)
        outName = &_outName;

    static const wxString whitespace = _T(" \n\t");
    size_t found = tok.find_last_of(whitespace);
    if (found != wxString::npos)
    {
        // Argument have name _or_ type, if space was found
        *outName = tok.SubString(found+1,tok.size());
        tok.resize(found);  //remove name
        tok.Trim(true);
    }

    found = tok.find_last_of(whitespace);
    if (found != wxString::npos)
    {
        // Argument have name _and_ type
        tok = tok.SubString(found+1,tok.size());
        //tok.resize(found);
        tok.Trim(true);
    }
    else
    {
        // Argument have only type
        // so outName already have argument type.
        tok.swap(*outName);
        outName->clear();
    }

    tok.Trim(false);
    return tok;
}

wxString DocumentationHelper::CommandToAnchor(Command cmd, const wxString& name,
                                              const wxString* args)
{
    if (args)
    {
        return _T("<a href=\"") + commandTag + wxString::Format(_T("%i"), (int)cmd) +
               separatorTag + *args + _T("\">") + name + _T("</a>");
    }

    return _T("<a href=\"") + commandTag + wxString::Format(_T("%i"), (int)cmd) +
           _T("\">") + name + _T("</a>");
}

wxString DocumentationHelper::CommandToAnchorInt(Command cmd, const wxString& name, int arg0)
{
    const wxString& tmp = wxString::Format(_T("%i"),arg0);
    return CommandToAnchor(cmd, name, &tmp);
}

DocumentationHelper::Command DocumentationHelper::HrefToCommand(const wxString& href, wxString& args)
{
    if (!href.StartsWith(commandTag, &args))
        return cmdNone;

    size_t separator = args.rfind(separatorTag);
    if (separator == wxString::npos)
        separator = args.size() + 1;

    long int command;
    bool gotCommand = args.SubString(0,separator-1).ToLong(&command);
    if (!gotCommand)
        return cmdNone;

    if (separator + 1 < args.size())
        args = args.SubString(separator+1, args.size());
    else
        args.clear();

    return (Command)(command);
}

const wxChar   DocumentationHelper::separatorTag = _T('+');
const wxString DocumentationHelper::commandTag = _T("cmd=");

DocumentationHelper::DocumentationHelper(CodeCompletion* cc) :
    BaseClass(),
    m_Popup(0),
    m_Html(0),
    m_CC(cc),
    m_CurrentTokenIdx(-1),
    m_LastTokenIdx(-1),
    m_Pos(),
    m_Size(),
    m_Enabled(false)
{
    ColourManager *colours = Manager::Get()->GetColourManager();
    colours->RegisterColour(_("Code completion"), _("Documentation popup background"), wxT("cc_docs_back"), *wxWHITE);
    colours->RegisterColour(_("Code completion"), _("Documentation popup text"), wxT("cc_docs_fore"), *wxBLACK);
    colours->RegisterColour(_("Code completion"), _("Documentation popup link"), wxT("cc_docs_link"), *wxBLUE);
}

DocumentationHelper::~DocumentationHelper()
{
}

void DocumentationHelper::Hide()
{
    if (m_Popup && m_Popup->IsShown())
        m_Popup->Hide();
}

void DocumentationHelper::OnAttach()
{
    // Dont attach if user dont want to use documentation helper and its already attached
    if (!m_Enabled || IsAttached())
        return;

    // register event sinks
    Manager* pm = Manager::Get();

    typedef cbEventFunctor<DocumentationHelper, CodeBlocksEvent> MyFunctor;
    void (DocumentationHelper::* onEvent)(CodeBlocksEvent&) =
        &DocumentationHelper::OnCbEventHide;

    pm->RegisterEventSink(cbEVT_WORKSPACE_CHANGED,    new MyFunctor(this, onEvent));

    pm->RegisterEventSink(cbEVT_PROJECT_ACTIVATE,     new MyFunctor(this, onEvent));
    pm->RegisterEventSink(cbEVT_PROJECT_CLOSE,        new MyFunctor(this, onEvent));


    pm->RegisterEventSink(cbEVT_EDITOR_ACTIVATED,     new MyFunctor(this, onEvent));
    pm->RegisterEventSink(cbEVT_EDITOR_TOOLTIP,       new MyFunctor(this, onEvent));

    m_Popup = new UnfocusablePopupWindow(pm->GetAppFrame());
    m_Html = new wxHtmlWindow(m_Popup, wxID_ANY, wxDefaultPosition,
                            wxSize(210,320),
                            wxHW_SCROLLBAR_AUTO | wxBORDER_SIMPLE);
    int sizes[7] = {};
    wxBuildFontSizes(sizes, wxGetDefaultHTMLFontSize());
    m_Html->SetFonts(wxEmptyString, wxEmptyString, &sizes[0]);

    m_Html->Connect(wxEVT_COMMAND_HTML_LINK_CLICKED,
                        (wxObjectEventFunction)&DocumentationHelper::OnLink,
                         NULL, this);
    Hide();

    m_Popup->Layout();
    m_Popup->Fit();
}

void DocumentationHelper::OnRelease()
{
    if ( !IsAttached() )
        return;

    Manager::Get()->RemoveAllEventSinksFor(this);

    if (m_Html)
        m_Html->Destroy();
    m_Html = 0;

    if (m_Popup)
        m_Popup->Destroy();
    m_Popup = 0;
}

bool DocumentationHelper::ShowDocumentation(const wxString& html)
{
    if (!m_Enabled || !IsAttached() || html.size() == 0)
    {
        Hide();
        return false;
    }

    m_Popup->Freeze();
    ResetSize(m_Size);

    m_Html->SetPage(html);

    FitToContent();

    m_Popup->SetPosition(m_Pos);
    m_Popup->Thaw();
    m_Popup->Show();

    return true;
}

wxString DocumentationHelper::GenerateHTML(int tokenIdx, TokenTree* tree)
{
    //http://docs.wxwidgets.org/2.8/wx_wxhtml.html#htmltagssupported

    using namespace HTMLTags;

    if (tokenIdx == -1)
        return wxEmptyString;
    ColourManager *colours = Manager::Get()->GetColourManager();

    wxString html = _T("<html><body bgcolor=\"");
    html += colours->GetColour(wxT("cc_docs_back")).GetAsString(wxC2S_HTML_SYNTAX) + _T("\" text=\"");
    html += colours->GetColour(wxT("cc_docs_fore")).GetAsString(wxC2S_HTML_SYNTAX) + _T("\" link=\"");
    html += colours->GetColour(wxT("cc_docs_link")).GetAsString(wxC2S_HTML_SYNTAX) + _T("\">");

    html += _T("<a name=\"top\"></a>");

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    Token* token = tree->at(tokenIdx);
    if (!token || token->m_Name.IsEmpty())
    {
        CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

        return wxEmptyString;
    }

    wxString doxyDoc = tree->GetDocumentation(tokenIdx);

    m_CurrentTokenIdx = token->m_Index;

    // add parent:
    wxString tokenNs = token->GetNamespace();
    if (tokenNs.size() > 0)
        html += b1 + CommandToAnchorInt(cmdDisplayToken, tokenNs.RemoveLast(2), token->m_ParentIndex) + b0 + br;

    html += br;

    //add scope and name:
    switch (token->m_TokenKind)
    {
    case tkFunction:
        if (token->m_Scope != tsUndefined)
            html += i1 + token->GetTokenScopeString() + i0 + sep;
        html += ConvertTypeToAnchor(token->m_FullType) + sep + b1 + token->m_Name + b0;
        html += ConvertArgsToAnchors(token->GetFormattedArgs());
        if (token->m_IsConst)
            html += _T(" const");
        if (token->m_IsNoExcept)
            html += _T(" noexcept");
        html += br;
        break;
    case tkPreprocessor:
        html += b1 + token->m_Name + b0 + br + token->m_FullType + br;
        break;

    case tkVariable:
        if (token->m_Scope != tsUndefined)
            html += i1 + token->GetTokenScopeString() + i0 + sep;
        html += ConvertTypeToAnchor(token->m_FullType) + sep + b1 + token->m_Name + b0 + br;
        break;

    case tkEnumerator:
        if (token->m_Scope != tsUndefined)
            html += i1 + token->GetTokenScopeString() + i0 + sep;
        html += token->m_FullType + sep + b1 + token->m_Name + b0;
        if (!token->m_Args.IsEmpty())
            html += wxT(" = ") + token->GetFormattedArgs();
        html += br;
        break;

    case tkNamespace:    // fall-through
    case tkClass:        // fall-through
    case tkEnum:         // fall-through
    case tkTypedef:      // fall-through
    case tkConstructor:  // fall-through
    case tkDestructor:   // fall-through
    case tkMacro:        // fall-through
    case tkAnyContainer: // fall-through
    case tkAnyFunction:  // fall-through
    case tkUndefined:    // fall-through
    default:
        if (token->m_Scope != tsUndefined)
            html += i1 + token->GetTokenScopeString() + i0 + sep;
        html += token->m_FullType + sep + b1 + token->m_Name + b0 + br;
    }

    //add kind:
    if (token->m_TokenKind != tkUndefined)
        html += i1 + _T("<font color=\"green\" size=3>") + _T("(") +
            token->GetTokenKindString() +_T(")") + _T("</font>") + i0 + br;

    html += DoxygenToHTML(doxyDoc);

    //add go to declaration / implementation
    {
        const wxString& arg0 = wxString::Format(_T("%i"), token->m_Index);

        html += br + br + CommandToAnchor(cmdOpenDecl, _T("Open declaration"), &arg0);
        if ((token->m_TokenKind & tkAnyFunction) && token->m_ImplLine > 0)
            html += br + CommandToAnchor(cmdOpenImpl, _T("Open implementation"), &arg0);
    }

    //add details:
    switch (token->m_TokenKind)
    {
        case tkClass:
            html += br + b1 + _T("Members:") + b0;
            for (TokenIdxSet::iterator it = token->m_Children.begin(); it != token->m_Children.end(); ++it)
            {
                const Token* t2 = tree->at(*it);
                if (t2 && !t2->m_Name.IsEmpty())
                {
                    html += br + sep + CommandToAnchorInt(cmdDisplayToken, t2->m_Name, *it) +
                        t2->GetStrippedArgs() + _T(": ") + t2->m_FullType;
                }
            }
            break;

        case tkEnum:
            html += br + b1 + _T("Values:") + b0;
            for (TokenIdxSet::iterator it = token->m_Children.begin(); it != token->m_Children.end(); ++it)
            {
                const Token* t2 = tree->at(*it);
                if (t2 && !t2->m_Name.IsEmpty())
                    html += br + sep + CommandToAnchorInt(cmdDisplayToken, t2->m_Name, *it);
            }
            break;

        case tkNamespace:    // fall-through
        case tkTypedef:      // fall-through
        case tkConstructor:  // fall-through
        case tkDestructor:   // fall-through
        case tkFunction:     // fall-through
        case tkVariable:     // fall-through
        case tkEnumerator:   // fall-through
        case tkPreprocessor: // fall-through
        case tkMacro:        // fall-through
        case tkAnyContainer: // fall-through
        case tkAnyFunction:  // fall-through
        case tkUndefined:    // fall-through
        default:
            break;
    }

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    html += br + br;

    // Append 'back' link:
    if (m_LastTokenIdx >= 0)
        html += CommandToAnchorInt(cmdDisplayToken, _T("Back"), m_LastTokenIdx);

    // Append 'close' link:
    html += _T(" ") + CommandToAnchor(cmdClose, _T("Close"));
    html += _T(" <a href=\"#top\">Top</a> ");

    html += _T("</body></html>");

    return html;
}

wxString DocumentationHelper::GenerateHTML(const TokenIdxSet& tokensIdx, TokenTree* tree)
{
    using namespace HTMLTags;

    if (tokensIdx.size() == 0)
    {
        Hide();
        return wxEmptyString;
    }

    if (tokensIdx.size() == 1)
        return GenerateHTML(*tokensIdx.begin(),tree);
    ColourManager *colours = Manager::Get()->GetColourManager();
    wxString html = _T("<html><body bgcolor=\"");
    html += colours->GetColour(wxT("cc_docs_back")).GetAsString(wxC2S_HTML_SYNTAX) + _T("\" text=\"");
    html += colours->GetColour(wxT("cc_docs_fore")).GetAsString(wxC2S_HTML_SYNTAX) + _T("\" link=\"");
    html += colours->GetColour(wxT("cc_docs_link")).GetAsString(wxC2S_HTML_SYNTAX) + _T("\">");

    html += _T("<a name=\"top\"></a>");

    html += _T("Multiple matches, please select one:<br>");
    TokenIdxSet::const_iterator it = tokensIdx.begin();

    CC_LOCKER_TRACK_TT_MTX_LOCK(s_TokenTreeMutex)

    while (it != tokensIdx.end())
    {
        const Token* token = tree->at(*it);

        html += token->GetNamespace() + CommandToAnchorInt(cmdDisplayToken, token->m_Name, token->m_Index);
        html += nbsp + nbsp + token->GetTokenKindString();
        html += _T("<br>");

        ++it;
    }

    CC_LOCKER_TRACK_TT_MTX_UNLOCK(s_TokenTreeMutex)

    html += _T("<br>");

    //Append 'back' link:
    if (m_LastTokenIdx >= 0)
        html += CommandToAnchorInt(cmdDisplayToken, _T("Back"), m_LastTokenIdx);


    //Append 'close' link:
    html += _T(" ") + CommandToAnchor(cmdClose, _T("Close"));
    html += _T(" <a href=\"#top\">Top</a> ");

    html += _T("</body></html>");

    return html;
}

void DocumentationHelper::OnSelectionChange(wxListEvent& event)
{
    event.Skip();
    if (!m_Enabled)
        return;

    wxObject* evtObj = event.GetEventObject();

    if (evtObj && ((wxWindow*)evtObj)->GetParent())
    {
        wxWindow* evtWin = ((wxWindow*)evtObj)->GetParent();
        if (!m_Popup->IsShown())
        {
            evtWin->Connect(wxID_ANY, wxEVT_SHOW,
                            (wxObjectEventFunction)&DocumentationHelper::OnWxEventHide,
                            NULL, this);

            m_Pos = m_Popup->GetParent()->ScreenToClient(evtWin->GetScreenPosition());
            m_Pos.x += evtWin->GetSize().x;

            cbEditor* editor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
            cbStyledTextCtrl* control = editor->GetControl();
            int acMaxHeight = control->AutoCompGetMaxHeight()+1;
            int textHeight = control->TextHeight(control->GetCurrentLine());
            wxRect edRect = editor->GetRect();

            m_Size.x = edRect.width * 5/12;
            m_Size.y = acMaxHeight*textHeight;
        }

        int tokenIdx = m_CC->GetAutocompTokenIdx();
        TokenTree* tree = m_CC->m_NativeParser.GetParser().GetTokenTree();

        ShowDocumentation(GenerateHTML(tokenIdx, tree));
    }
}

void DocumentationHelper::ResetSize(const wxSize& size)
{
    const wxSize& cliSize = size;
    m_Html->SetMaxSize(cliSize);
    m_Html->SetClientSize(cliSize);
}

bool DocumentationHelper::IsAttached() const
{
    return m_Popup && m_Html;
}

bool DocumentationHelper::IsVisible() const
{
    return IsAttached() && m_Popup->IsShown();
}

void DocumentationHelper::RereadOptions(ConfigManager* cfg)
{
    if (!cfg)
        cfg = Manager::Get()->GetConfigManager(_T("code_completion"));

    m_Enabled = cfg->ReadBool(_T("/use_documentation_helper"), false);

    // Apply changes
    if (m_Enabled)
        OnAttach();
    else
        OnRelease();
}

void DocumentationHelper::WriteOptions(ConfigManager* cfg)
{
    if (!cfg)
        cfg = Manager::Get()->GetConfigManager(_T("code_completion"));

    cfg->Write(_T("/use_documentation_helper"), m_Enabled);
}

void DocumentationHelper::FitToContent()
{
    m_Popup->Layout();
    m_Popup->Fit();
}

void DocumentationHelper::SaveTokenIdx()
{
    m_LastTokenIdx = m_CurrentTokenIdx;
}

//events:
void DocumentationHelper::OnCbEventHide(CodeBlocksEvent& event)
{
    event.Skip();
    m_Popup->Hide();
}

void DocumentationHelper::OnWxEventHide(wxEvent& event)
{
    event.Skip();
    Hide();

    wxObject* evtObj = event.GetEventObject();
    if (evtObj)
    {
        wxWindow* evtWin = (wxWindow*)(evtObj);
        evtWin->Disconnect(wxID_ANY, wxEVT_SHOW,
                        (wxObjectEventFunction)&DocumentationHelper::OnWxEventHide,
                        NULL, this);
    }
}

void DocumentationHelper::OnLink(wxHtmlLinkEvent& event)
{
    TokenTree* tree = m_CC->m_NativeParser.GetParser().GetTokenTree();

    const wxString& href = event.GetLinkInfo().GetHref();
    wxString args;
    long int  tokenIdx;

    Command command = HrefToCommand(href,args);
    switch (command)
    {
    case cmdDisplayToken:
        if(args.ToLong(&tokenIdx))
        {
            SaveTokenIdx();
            ShowDocumentation(GenerateHTML(tokenIdx, tree));
        }
        break;
    case cmdSearch:
    case cmdSearchAll:
        {
            size_t opb = args.find_last_of(_T('('));
            size_t clb = args.find_last_of(_T(')'));
            int kindToSearch = tkUndefined;
            if (opb != wxString::npos && clb != wxString::npos)
            {
                args = args.Truncate(opb);
                kindToSearch = tkAnyFunction|tkPreprocessor;
            }

            TokenIdxSet result;
            size_t scpOp = args.rfind(_T("::"));
            if (scpOp != wxString::npos)
            {
                //it may be function
                tree->FindMatches(args.SubString(scpOp+2,args.size()), result,
                                   true, false, TokenKind(kindToSearch));
            }
            else if (command == cmdSearchAll)
                tree->FindMatches(args, result, true, false, TokenKind(kindToSearch));
            else
                tree->FindMatches(args, result, true, false, TokenKind(tkAnyContainer|tkEnum));

            if (result.size() > 0)
            {
                SaveTokenIdx();
                ShowDocumentation(GenerateHTML(result, tree));
            }
        }
        break;
    case cmdOpenDecl:
        if (args.ToLong(&tokenIdx))
        {
            EditorManager* edMan = Manager::Get()->GetEditorManager();
            const Token* token = tree->at(tokenIdx);
            cbEditor* targetEditor = edMan->Open(token->GetFilename());
            if (targetEditor)
                targetEditor->GotoTokenPosition(token->m_Line - 1, token->m_Name);
        }
        break;
    case cmdOpenImpl:
        if (args.ToLong(&tokenIdx))
        {
            EditorManager* edMan = Manager::Get()->GetEditorManager();
            const Token* token = tree->at(tokenIdx);
            cbEditor* targetEditor = edMan->Open(token->GetImplFilename());
            if (targetEditor)
                targetEditor->GotoTokenPosition(token->m_ImplLine - 1, token->m_Name);
        }
        break;
    case cmdClose:
        m_Popup->Hide();
        break;

    case cmdNone:
    default:
        if (href.size()>1 && href[0] == _T('#'))
            event.Skip(); //go to anchor
        else if (href.StartsWith(_T("www.")) || href.StartsWith(_T("http://")))
            wxLaunchDefaultBrowser(href);
    }

    // Dont skip this event
}

//end of Documentation popup functions
