/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>

#ifndef CB_PRECOMP
    #include <algorithm>
    #include <sstream>
    #include <string>
    #include <vector>

    #include <wx/msgdlg.h>
    #include <wx/xrc/xmlres.h>
    #include <wx/fs_zip.h>
    #include <wx/strconv.h>

    #include <cbeditor.h>
    #include <cbexception.h>
    #include <cbproject.h>
    #include <configmanager.h>
    #include <editormanager.h>
    #include <manager.h>
    #include <projectmanager.h>
#endif

#include <wx/progdlg.h>
#include <cbstyledtextctrl.h>

#include "astyleplugin.h"
#include "astyleconfigdlg.h"
#include "formattersettings.h"
#include "asstreamiterator.h"

using std::istringstream;
using std::string;

namespace
{
    const int idCodeFormatterFile = wxNewId();
    const int idCodeFormatterActiveFile = wxNewId();
    const int idCodeFormatterProject = wxNewId();
}

BEGIN_EVENT_TABLE( AStylePlugin, cbPlugin )
    EVT_MENU( idCodeFormatterActiveFile, AStylePlugin::OnFormatActiveFile )
    EVT_MENU( idCodeFormatterProject, AStylePlugin::OnFormatProject )
END_EVENT_TABLE()

// this auto-registers the plugin

namespace
{
    PluginRegistrant<AStylePlugin> reg(_T("AStylePlugin"));
}

AStylePlugin::AStylePlugin()
{
    //ctor

    if (!Manager::LoadResource(_T("astyle.zip")))
        NotifyMissingFile(_T("astyle.zip"));
}

AStylePlugin::~AStylePlugin()
{
    //dtor
}


cbConfigurationPanel* AStylePlugin::GetConfigurationPanel(wxWindow* parent)
{
    AstyleConfigDlg* dlg = new AstyleConfigDlg(parent);
    // deleted by the caller

    return dlg;
}

void AStylePlugin::BuildModuleMenu( const ModuleType type, wxMenu* menu, const FileTreeData* data )
{
    if ( !menu || !IsAttached() )
        return;

    switch ( type )
    {
        case mtEditorManager:
            menu->AppendSeparator();
            menu->Append( idCodeFormatterActiveFile, _( "Format use AStyle" ), _( "Format the selected source code (selected line) in the current file" ) );
            break;

        case mtProjectManager:
            if ( data ) switch ( data->GetKind() )
                {
                    case FileTreeData::ftdkProject:
                        menu->AppendSeparator();
                        menu->Append( idCodeFormatterProject, _( "Format this project (AStyle)" ), _( "Format the source code in this project" ) );
                        break;

                    case FileTreeData::ftdkFile:
                        menu->AppendSeparator();
                        menu->Append( idCodeFormatterProject, _( "Format this file (AStyle)" ), _( "Format the source code in this file" ) );
                        break;

                    default:
                        // Do nothing.
                        break;
                }
            break;

        default:
            break;
    }
}

void AStylePlugin::OnFormatProject( wxCommandEvent& /*event*/ )
{
    ProjectManager* manager = Manager::Get()->GetProjectManager();
    wxTreeCtrl *tree = manager->GetUI().GetTree();

    if ( !tree )
        return;

    wxTreeItemId treeItem =  manager->GetUI().GetTreeSelection();

    if ( !treeItem.IsOk() )
        return;

    const FileTreeData *data = static_cast<FileTreeData*>( tree->GetItemData( treeItem ) );

    if ( !data )
        return;

    switch ( data->GetKind() )
    {
        case FileTreeData::ftdkProject:
            {
                cbProject* prj = data->GetProject();
                wxProgressDialog progressDlg(_("Please wait"), _("Formatting..."), prj->GetFilesCount(), 0, wxPD_CAN_ABORT|wxPD_AUTO_HIDE|wxPD_SMOOTH );
                progressDlg.Show();
                int i = 0;
                for (FilesList::iterator it = prj->GetFilesList().begin(); it != prj->GetFilesList().end(); ++it)
                {
                    ProjectFile* pf = *it;
                    wxString filename = pf->file.GetFullPath();

                    FileType fileType = FileTypeOf( filename );
                    if ( fileType == ftSource || fileType == ftHeader )
                    {
                        FormatFile( filename );
                        if ( false == progressDlg.Update( i++, wxString(_("Formatting ")) + pf->relativeFilename ) )
                            break;
                    }
                }
            }
            break;

        case FileTreeData::ftdkFile:
            {
                ProjectFile* f = data->GetProjectFile();
                if ( f )
                    FormatFile( f->file.GetFullPath() );
            }
            break;

        default:
            break;
    }
}

void AStylePlugin::OnFormatActiveFile( wxCommandEvent& /*event*/ )
{
        Execute();
}

int AStylePlugin::Execute()
{
    if (!IsAttached())
        return -1;

    cbEditor *ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();

    if (!ed)
        return 0;

    FormatEditor( ed );

    return 0;
}

void AStylePlugin::FormatFile( const wxString &filename )
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->IsBuiltinOpen( filename );

    if ( ed ) // File is already open
        FormatEditor( ed );
    else
    {
        // File is not open.  We must open it.
        ed = Manager::Get()->GetEditorManager()->Open( filename );

        if ( ed )
        {
            bool changed = FormatEditor( ed );

            if ( !changed ) // We opened a file and it didn't change.  Close it.
                Manager::Get()->GetEditorManager()->Close( filename );
        }
    }
}

// Special code to compare strings which doesn't care
// about spaces leading up to the EOL.
static bool BuffersDiffer( const wxString &a, const wxString &b )
{
    const wxChar *aCurrent = a.c_str();
    const wxChar *bCurrent = b.c_str();
    const wxChar * const aEnd = aCurrent + a.Len();
    const wxChar * const bEnd = bCurrent + b.Len();

    while ( aCurrent != aEnd && bCurrent != bEnd )
    {
        if ( *aCurrent != *bCurrent )
        {
            // Check for varying space at EOL
            while ( *aCurrent == ' ' || *aCurrent == '\t' )
            {
                if ( ++aCurrent == aEnd )
                    break;
            }
            while ( *bCurrent == ' ' || *bCurrent == '\t' )
            {
                if ( ++bCurrent == bEnd )
                    break;
            }

            // Make sure it was at EOL
            if ( ( *aCurrent != '\r' && *aCurrent != '\n' ) || ( *bCurrent != '\r' && *bCurrent != '\n' ) )
                return true;
        }

        ++aCurrent;
        ++bCurrent;
    }

    while ( aCurrent != aEnd )
    {
        if ( *aCurrent != ' ' && *aCurrent != '\t' )
            return true;
        ++aCurrent;
    }

    while ( bCurrent != bEnd )
    {
        if ( *bCurrent != ' ' && *bCurrent != '\t' )
            return true;
        ++bCurrent;
    }

    return false;
}

bool AStylePlugin::FormatEditor( cbEditor *ed )
{
    cbStyledTextCtrl* control = ed->GetControl();
    if (control->GetReadOnly())
    {
        cbMessageBox(_("The file is read-only!"), _("Error"), wxICON_ERROR);
        return false;
    }

    bool onlySelected = false;
    wxString edText;
    wxString selText;
    int leftBracesNumber = 0;
    const int pos = control->GetCurrentPos();
    int start = control->GetSelectionStart();
    int end = control->GetSelectionEnd();
    wxString fromWord;
    if (start != end)
    {
        onlySelected = true;
        start = control->GetLineIndentPosition(control->LineFromPosition(start));
        control->GotoPos(start);
        control->Home();
        start = control->GetCurrentPos();
        control->SetSelectionStart(start);
        end = control->GetLineEndPosition(control->LineFromPosition(end));
        control->SetSelectionEnd(end);
        selText = control->GetTextRange(start, end);

        wxChar ch;
        int findBracesPos = start;
        while (--findBracesPos > 0 )
        {
            ch = control->GetCharAt(findBracesPos);
            int style = control->GetStyleAt(findBracesPos);
            int lexer = control->GetLexer();
            if (lexer == wxSCI_LEX_CPP)
            {
                if (   style == wxSCI_C_COMMENT           || style == wxSCI_C_COMMENTDOC
                    || style == wxSCI_C_COMMENTDOCKEYWORD || style == wxSCI_C_COMMENTDOCKEYWORDERROR
                    || style == wxSCI_C_COMMENTLINE       || style == wxSCI_C_COMMENTLINEDOC
                    || style == wxSCI_C_STRING            || style == wxSCI_C_CHARACTER)
                    continue;
            }
            else if (lexer == wxSCI_LEX_D)
            {
                if (   style == wxSCI_D_COMMENT           || style == wxSCI_D_COMMENTDOC
                    || style == wxSCI_D_COMMENTDOCKEYWORD || style == wxSCI_D_COMMENTDOCKEYWORDERROR
                    || style == wxSCI_D_COMMENTLINE       || style == wxSCI_D_COMMENTLINEDOC
                    || style == wxSCI_D_STRING            || style == wxSCI_D_CHARACTER)
                    continue;
            }

            if      (ch == _T('}')) --leftBracesNumber;
            else if (ch == _T('{')) ++leftBracesNumber;
        }

        for (int i = leftBracesNumber; i > 0; --i)
            edText.Append(_T('{'));
        edText.Append(selText);
    }
    else
        edText = control->GetText();

    wxString formattedText;

    astyle::ASFormatter formatter;

    // load settings
    FormatterSettings settings;
    settings.ApplyTo(formatter);

    const wxString& eolChars = GetEOLStr(control->GetEOLMode());

    if (edText.size() && edText.Last() != _T('\r') && edText.Last() != _T('\n') && !onlySelected)
        edText += eolChars;

    ASStreamIterator *asi = new ASStreamIterator(ed, edText);

    formatter.init(asi);

    int lineCounter = 0;
    std::vector<int> new_bookmark;
    std::vector<int> ed_breakpoints;

    // hack: we need to evaluate the special case of having a bookmark in the first line here
    if (!onlySelected)
    {
        if (ed->HasBookmark(0))
            new_bookmark.push_back(0);
        if (ed->HasBreakpoint(0))
            ed_breakpoints.push_back(0);
    }

    wxSetCursor(*wxHOURGLASS_CURSOR);

    while (formatter.hasMoreLines())
    {
        formattedText << cbC2U(formatter.nextLine().c_str());

        if (formatter.hasMoreLines())
            formattedText << eolChars;

        ++lineCounter;

        if (asi->FoundBookmark())
        {
            new_bookmark.push_back(lineCounter);
            asi->ClearFoundBookmark();
        }
        if (asi->FoundBreakpoint())
        {
            ed_breakpoints.push_back(lineCounter);
            asi->ClearFoundBreakpoint();
        }
    }

    if (onlySelected && leftBracesNumber > 0)
    {
        while (leftBracesNumber > 0)
        {
            --leftBracesNumber;
            formattedText = formattedText.SubString(formattedText.Find(_T('{')) + 1, formattedText.Length());
        }
        formattedText = formattedText.SubString(formattedText.Find(eolChars) + eolChars.Length(), formattedText.Length());
    }

    bool changed = BuffersDiffer( formattedText, !onlySelected ? edText : selText);

    if ( changed )
    {
        control->BeginUndoAction();
        if (onlySelected)
            ApplyTextDelta(control, formattedText, control->GetSelectionStart(), control->GetSelectionEnd());
        else
            ApplyTextDelta(control, formattedText, 0, control->GetLength());

        for (std::vector<int>::const_iterator i = new_bookmark.begin(); i != new_bookmark.end(); ++i)
            ed->ToggleBookmark(*i);

        for (std::vector<int>::const_iterator i = ed_breakpoints.begin(); i != ed_breakpoints.end(); ++i)
            ed->ToggleBreakpoint(*i);

        control->EndUndoAction();
        control->GotoPos(pos);
        ed->SetModified(true);
    }

    wxSetCursor(wxNullCursor);

    return changed;
}

void AStylePlugin::ApplyTextDelta(cbStyledTextCtrl* stc, wxString text, int rangeStart, int rangeEnd)
{
    rangeStart = stc->LineFromPosition(rangeStart);
    rangeEnd = stc->LineFromPosition(rangeEnd) + 1;
    const wxString& eolChars = GetEOLStr(stc->GetEOLMode());
    std::vector<wxString> textLines;
    int seplen = eolChars.Length();
    for (;;)
    {
        int idx = text.Find(eolChars);
        if (idx == wxNOT_FOUND)
        {
            textLines.push_back(text);
            break;
        }
        wxString part = text.Left(idx);
        text.Remove(0, idx + seplen);
        textLines.push_back(part);
    }
    typedef std::vector<wxString>::const_iterator StrVecItr;
    StrVecItr lnItr = textLines.begin();
    while (rangeStart < rangeEnd && lnItr != textLines.end())
    {
        if (stc->GetLine(rangeStart).BeforeLast(eolChars[0]) == *lnItr)
        {
            ++rangeStart;
            ++lnItr;
        }
        else
        {
            int i = rangeStart + 1;
            for (; i < rangeEnd; ++i)
            {
                StrVecItr searchItr = std::find(lnItr + 1, (StrVecItr)textLines.end(), stc->GetLine(i).BeforeLast(eolChars[0]));
                if (searchItr != textLines.end())
                {
                    stc->SetTargetStart(stc->PositionFromLine(rangeStart));
                    stc->SetTargetEnd(stc->GetLineEndPosition(i - 1));
                    wxString buffer = *lnItr;
                    for (StrVecItr itr = lnItr + 1; itr != searchItr; ++itr)
                        buffer += eolChars + *itr;
                    stc->ReplaceTarget(buffer);
                    const int offset = (searchItr - lnItr) - (i - rangeStart);
                    rangeEnd += offset;
                    rangeStart = i + offset;
                    lnItr = searchItr;
                    break;
                }
            }
            if (i == rangeEnd)
                break;
        }
    }
    wxString buffer;
    for (; lnItr != textLines.end(); ++lnItr)
        buffer += eolChars + *lnItr;
    if (rangeStart == rangeEnd)
        stc->SetTargetStart(stc->GetLineEndPosition(rangeEnd - 1));
    else
    {
        stc->SetTargetStart(stc->PositionFromLine(rangeStart));
        if (!buffer.IsEmpty())
            buffer = buffer.Mid(eolChars.Length());
    }
    stc->SetTargetEnd(stc->GetLineEndPosition(rangeEnd - 1));
    stc->ReplaceTarget(buffer);
}
