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
    #include <wx/button.h>
    #include <wx/checkbox.h>
    #include <wx/colordlg.h>
    #include <wx/combobox.h>
    #include <wx/intl.h>
    #include <wx/listbox.h>
    #include <wx/radiobut.h>
    #include <wx/regex.h>
    #include <wx/slider.h>
    #include <wx/spinctrl.h>
    #include <wx/stattext.h>
    #include <wx/treectrl.h>
    #include <wx/xrc/xmlres.h>

    #include <cbstyledtextctrl.h>
    #include <configmanager.h>
    #include <globals.h>
    #include <logmanager.h>
    #include <manager.h>
#endif

#include <editpairdlg.h>

#include "ccoptionsdlg.h"
#include "codecompletion.h"

static const wxString g_SampleClasses =
    _T("class A_class"
    "{"
    "    public:"
    "        int someInt_A;"
    "    protected:"
    "        bool mSomeVar_A;"
    "    private:"
    "        char* mData_A;"
    "};"
    "class B_class"
    "{"
    "    public:"
    "        int someInt_B;"
    "    protected:"
    "        bool mSomeVar_B;"
    "    private:"
    "        char* mData_B;"
    "};"
    "class C_class : public A_class"
    "{"
    "    public:"
    "        int someInt_C;"
    "    protected:"
    "        bool mSomeVar_C;"
    "    private:"
    "        char* mData_C;"
    "};"
    "enum SomeEnum"
    "{"
    "    optOne,"
    "    optTwo,"
    "    optThree"
    "};"
    "int x;"
    "int y;"
    "#define SOME_DEFINITION\n"
    "#define SOME_DEFINITION_2\n\n");

BEGIN_EVENT_TABLE(CCOptionsDlg, wxPanel)
    EVT_UPDATE_UI(-1,                       CCOptionsDlg::OnUpdateUI)
    EVT_BUTTON(XRCID("btnAddRepl"),         CCOptionsDlg::OnAddRepl)
    EVT_BUTTON(XRCID("btnEditRepl"),        CCOptionsDlg::OnEditRepl)
    EVT_BUTTON(XRCID("btnDelRepl"),         CCOptionsDlg::OnDelRepl)
    EVT_BUTTON(XRCID("btnColour"),          CCOptionsDlg::OnChooseColour)
    EVT_COMMAND_SCROLL(XRCID("sldCCDelay"), CCOptionsDlg::OnCCDelayScroll)
END_EVENT_TABLE()

CCOptionsDlg::CCOptionsDlg(wxWindow* parent, NativeParser* np, CodeCompletion* cc) :
    m_NativeParser(np),
    m_CodeCompletion(cc),
    m_Parser(np->GetParser())
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("code_completion"));

    wxXmlResource::Get()->LoadPanel(this, parent, _T("dlgCCSettings"));

    // -----------------------------------------------------------------------
    // Handle all options that are being directly applied from config
    // -----------------------------------------------------------------------

    // Page "Code Completion"
    XRCCTRL(*this, "chkNoCC",               wxCheckBox)->SetValue(!cfg->ReadBool(_T("/use_code_completion"), true));
    XRCCTRL(*this, "chkEvalTooltip",        wxCheckBox)->SetValue(cfg->ReadBool(_T("/eval_tooltip"),         true));
    XRCCTRL(*this, "chkAutoSelectOne",      wxCheckBox)->SetValue(cfg->ReadBool(_T("/auto_select_one"),      false));
    XRCCTRL(*this, "chkAutoAddParentheses", wxCheckBox)->SetValue(cfg->ReadBool(_T("/auto_add_parentheses"), true));
    XRCCTRL(*this, "chkAddDoxgenComment",   wxCheckBox)->SetValue(cfg->ReadBool(_T("/add_doxgen_comment"),   false));
    XRCCTRL(*this, "chkEnableHeaders",      wxCheckBox)->SetValue(cfg->ReadBool(_T("/enable_headers"),       true));
    XRCCTRL(*this, "chkAutoLaunch",         wxCheckBox)->SetValue(cfg->ReadBool(_T("/auto_launch"),          true));
    XRCCTRL(*this, "spnAutoLaunchChars",    wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/auto_launch_chars"),     3));
    XRCCTRL(*this, "spnMaxMatches",         wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/max_matches"),           16384));
    XRCCTRL(*this, "txtFillupChars",        wxTextCtrl)->SetValue(cfg->Read(_T("/fillup_chars"),             wxEmptyString));
    XRCCTRL(*this, "sldCCDelay",            wxSlider)->SetValue(cfg->ReadInt(_T("/cc_delay"),                300) / 100);
    UpdateCCDelayLabel();
    XRCCTRL(*this, "chkKL_1",               wxCheckBox)->SetValue(cfg->ReadBool(_T("/lexer_keywords_set1"),  true));
    XRCCTRL(*this, "chkKL_2",               wxCheckBox)->SetValue(cfg->ReadBool(_T("/lexer_keywords_set2"),  true));
    XRCCTRL(*this, "chkKL_3",               wxCheckBox)->SetValue(cfg->ReadBool(_T("/lexer_keywords_set3"),  false));
    XRCCTRL(*this, "chkKL_4",               wxCheckBox)->SetValue(cfg->ReadBool(_T("/lexer_keywords_set4"),  false));
    XRCCTRL(*this, "chkKL_5",               wxCheckBox)->SetValue(cfg->ReadBool(_T("/lexer_keywords_set5"),  false));
    XRCCTRL(*this, "chkKL_6",               wxCheckBox)->SetValue(cfg->ReadBool(_T("/lexer_keywords_set6"),  false));
    XRCCTRL(*this, "chkKL_7",               wxCheckBox)->SetValue(cfg->ReadBool(_T("/lexer_keywords_set7"),  false));
    XRCCTRL(*this, "chkKL_8",               wxCheckBox)->SetValue(cfg->ReadBool(_T("/lexer_keywords_set8"),  false));
    XRCCTRL(*this, "chkKL_9",               wxCheckBox)->SetValue(cfg->ReadBool(_T("/lexer_keywords_set9"),  false));

    // Page "C / C++ parser"
    // NOTE (Morten#1#): Keep this in sync with files in the XRC file (settings.xrc) and nativeparser.cpp
    XRCCTRL(*this, "txtPriorityHeaders",       wxTextCtrl)->SetValue(cfg->Read(_T("/priority_headers"),
        _T("<cstddef>, <w32api.h>, ")
        _T("<wx/defs.h>, <wx/dlimpexp.h>, <wx/toplevel.h>, ")
        _T("<boost/config.hpp>, <boost/filesystem/config.hpp>, ")
        _T("\"pch.h\", \"sdk.h\", \"stdafx.h\"")));
    XRCCTRL(*this, "spnThreadsNum",            wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/max_threads"), 1));
    XRCCTRL(*this, "spnThreadsNum",            wxSpinCtrl)->Enable(false);
    XRCCTRL(*this, "spnParsersNum",            wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/max_parsers"), 5));

    // Page "C / C++ parser (adv.)"
    // NOTE (Morten#1#): Keep this in sync with files in the XRC file (settings.xrc) and parser.cpp
    XRCCTRL(*this, "txtCCFileExtHeader",       wxTextCtrl)->SetValue(cfg->Read(_T("/header_ext"),    _T("h,hpp,tcc,xpm")));
    XRCCTRL(*this, "chkCCFileExtEmpty",        wxCheckBox)->SetValue(cfg->ReadBool(_T("/empty_ext"), true));
    XRCCTRL(*this, "txtCCFileExtSource",       wxTextCtrl)->SetValue(cfg->Read(_T("/source_ext"),    _T("c,cpp,cxx,cc,c++")));

    const wxStringHashMap& repl = Tokenizer::GetTokenReplacementsMap();
    wxStringHashMap::const_iterator it = repl.begin();
    while (it != repl.end())
    {
        XRCCTRL(*this, "lstRepl", wxListBox)->Append(it->first + _T(" -> ") + it->second);
        ++it;
    }

    // Page "Symbol browser"
    XRCCTRL(*this, "chkNoSB",        wxCheckBox)->SetValue(!cfg->ReadBool(_T("/use_symbols_browser"), true));
    XRCCTRL(*this, "chkFloatCB",     wxCheckBox)->SetValue(cfg->ReadBool(_T("/as_floating_window"),   false));
    XRCCTRL(*this, "chkScopeFilter", wxCheckBox)->SetValue(cfg->ReadBool(_T("/scope_filter"),         true));

    // -----------------------------------------------------------------------
    // Handle all options that are being handled by m_Parser
    // -----------------------------------------------------------------------

    // Page "Code Completion"
    XRCCTRL(*this, "chkUseSmartSense",      wxCheckBox)->SetValue(!m_Parser.Options().useSmartSense);
    XRCCTRL(*this, "chkWhileTyping",        wxCheckBox)->SetValue(m_Parser.Options().whileTyping);
    XRCCTRL(*this, "chkCaseSensitive",      wxCheckBox)->SetValue(m_Parser.Options().caseSensitive);

    // Page "C / C++ parser"
    XRCCTRL(*this, "chkLocals",             wxCheckBox)->SetValue(m_Parser.Options().followLocalIncludes);
    XRCCTRL(*this, "chkGlobals",            wxCheckBox)->SetValue(m_Parser.Options().followGlobalIncludes);
    XRCCTRL(*this, "chkPreprocessor",       wxCheckBox)->SetValue(m_Parser.Options().wantPreprocessor);
    XRCCTRL(*this, "chkComplexMacros",      wxCheckBox)->SetValue(m_Parser.Options().parseComplexMacros);

    XRCCTRL(*this, "rdoOneParserPerWorkspace", wxRadioButton)->SetValue( m_NativeParser->IsParserPerWorkspace());
    XRCCTRL(*this, "rdoOneParserPerProject",   wxRadioButton)->SetValue(!m_NativeParser->IsParserPerWorkspace());

    // Page "Symbol browser"
    XRCCTRL(*this, "chkInheritance",        wxCheckBox)->SetValue(m_Parser.ClassBrowserOptions().showInheritance);
    XRCCTRL(*this, "chkExpandNS",           wxCheckBox)->SetValue(m_Parser.ClassBrowserOptions().expandNS);
    XRCCTRL(*this, "chkTreeMembers",        wxCheckBox)->SetValue(m_Parser.ClassBrowserOptions().treeMembers);

//    m_Parser.ParseBuffer(g_SampleClasses, true);
//    m_Parser.BuildTree(*XRCCTRL(*this, "treeClasses", wxTreeCtrl));
}

CCOptionsDlg::~CCOptionsDlg()
{
}

void CCOptionsDlg::OnApply()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("code_completion"));

    // -----------------------------------------------------------------------
    // Handle all options that are being directly applied / written from UI:
    // -----------------------------------------------------------------------

    // Page "Code Completion"
    cfg->Write(_T("/use_code_completion"),  (bool)!XRCCTRL(*this, "chkNoCC",               wxCheckBox)->GetValue());
    cfg->Write(_T("/use_SmartSense"),       (bool) XRCCTRL(*this, "chkUseSmartSense",      wxCheckBox)->GetValue());
    cfg->Write(_T("/while_typing"),         (bool) XRCCTRL(*this, "chkWhileTyping",        wxCheckBox)->GetValue());
    cfg->Write(_T("/case_sensitive"),       (bool) XRCCTRL(*this, "chkCaseSensitive",      wxCheckBox)->GetValue());
    cfg->Write(_T("/eval_tooltip"),         (bool) XRCCTRL(*this, "chkEvalTooltip",        wxCheckBox)->GetValue());
    cfg->Write(_T("/auto_select_one"),      (bool) XRCCTRL(*this, "chkAutoSelectOne",      wxCheckBox)->GetValue());
    cfg->Write(_T("/auto_add_parentheses"), (bool) XRCCTRL(*this, "chkAutoAddParentheses", wxCheckBox)->GetValue());
    cfg->Write(_T("/add_doxgen_comment"),   (bool) XRCCTRL(*this, "chkAddDoxgenComment",   wxCheckBox)->GetValue());
    cfg->Write(_T("/enable_headers"),       (bool) XRCCTRL(*this, "chkEnableHeaders",      wxCheckBox)->GetValue());
    cfg->Write(_T("/auto_launch"),          (bool) XRCCTRL(*this, "chkAutoLaunch",         wxCheckBox)->GetValue());
    cfg->Write(_T("/auto_launch_chars"),    (int)  XRCCTRL(*this, "spnAutoLaunchChars",    wxSpinCtrl)->GetValue());
    cfg->Write(_T("/max_matches"),          (int)  XRCCTRL(*this, "spnMaxMatches",         wxSpinCtrl)->GetValue());
    cfg->Write(_T("/fillup_chars"),                XRCCTRL(*this, "txtFillupChars",        wxTextCtrl)->GetValue());
    cfg->Write(_T("/cc_delay"),             (int)  XRCCTRL(*this, "sldCCDelay",            wxSlider)->GetValue() * 100);
    cfg->Write(_T("/lexer_keywords_set1"),  (bool) XRCCTRL(*this, "chkKL_1",               wxCheckBox)->GetValue());
    cfg->Write(_T("/lexer_keywords_set2"),  (bool) XRCCTRL(*this, "chkKL_2",               wxCheckBox)->GetValue());
    cfg->Write(_T("/lexer_keywords_set3"),  (bool) XRCCTRL(*this, "chkKL_3",               wxCheckBox)->GetValue());
    cfg->Write(_T("/lexer_keywords_set4"),  (bool) XRCCTRL(*this, "chkKL_4",               wxCheckBox)->GetValue());
    cfg->Write(_T("/lexer_keywords_set5"),  (bool) XRCCTRL(*this, "chkKL_5",               wxCheckBox)->GetValue());
    cfg->Write(_T("/lexer_keywords_set6"),  (bool) XRCCTRL(*this, "chkKL_6",               wxCheckBox)->GetValue());
    cfg->Write(_T("/lexer_keywords_set7"),  (bool) XRCCTRL(*this, "chkKL_7",               wxCheckBox)->GetValue());
    cfg->Write(_T("/lexer_keywords_set8"),  (bool) XRCCTRL(*this, "chkKL_8",               wxCheckBox)->GetValue());
    cfg->Write(_T("/lexer_keywords_set9"),  (bool) XRCCTRL(*this, "chkKL_9",               wxCheckBox)->GetValue());

    // Page "C / C++ parser"
    cfg->Write(_T("/parser_follow_local_includes"),  (bool) XRCCTRL(*this, "chkLocals",                wxCheckBox)->GetValue());
    cfg->Write(_T("/parser_follow_global_includes"), (bool) XRCCTRL(*this, "chkGlobals",               wxCheckBox)->GetValue());
    cfg->Write(_T("/want_preprocessor"),             (bool) XRCCTRL(*this, "chkPreprocessor",          wxCheckBox)->GetValue());
    cfg->Write(_T("/parse_complex_macros"),          (bool) XRCCTRL(*this, "chkComplexMacros",         wxCheckBox)->GetValue());
    cfg->Write(_T("/priority_headers"),                     XRCCTRL(*this, "txtPriorityHeaders",       wxTextCtrl)->GetValue());
    cfg->Write(_T("/max_threads"),                   (int)  XRCCTRL(*this, "spnThreadsNum",            wxSpinCtrl)->GetValue());
    cfg->Write(_T("/parser_per_workspace"),          (bool) XRCCTRL(*this, "rdoOneParserPerWorkspace", wxRadioButton)->GetValue());
    cfg->Write(_T("/max_parsers"),                   (int)  XRCCTRL(*this, "spnParsersNum",            wxSpinCtrl)->GetValue());

    // Page "C / C++ parser (adv.)"
    cfg->Write(_T("/header_ext"),        XRCCTRL(*this, "txtCCFileExtHeader", wxTextCtrl)->GetValue());
    cfg->Write(_T("/empty_ext"),  (bool) XRCCTRL(*this, "chkCCFileExtEmpty",  wxCheckBox)->GetValue());
    cfg->Write(_T("/source_ext"),        XRCCTRL(*this, "txtCCFileExtSource", wxTextCtrl)->GetValue());

    // Page "Symbol browser"
    cfg->Write(_T("/use_symbols_browser"),      (bool)!XRCCTRL(*this, "chkNoSB",        wxCheckBox)->GetValue());
    cfg->Write(_T("/browser_show_inheritance"), (bool) XRCCTRL(*this, "chkInheritance", wxCheckBox)->GetValue());
    cfg->Write(_T("/browser_expand_ns"),        (bool) XRCCTRL(*this, "chkExpandNS",    wxCheckBox)->GetValue());
    cfg->Write(_T("/as_floating_window"),       (bool) XRCCTRL(*this, "chkFloatCB",     wxCheckBox)->GetValue());
    cfg->Write(_T("/browser_tree_members"),     (bool) XRCCTRL(*this, "chkTreeMembers", wxCheckBox)->GetValue());
    cfg->Write(_T("/scope_filter"),             (bool) XRCCTRL(*this, "chkScopeFilter", wxCheckBox)->GetValue());

    // -----------------------------------------------------------------------
    // Handle all options that are being be read by m_Parser.ReadOptions():
    // -----------------------------------------------------------------------

    // Force parser to read its options that we write in the config
    // Also don't forget to update the Parser option according UI!
    m_Parser.ReadOptions();

    // Page "Code Completion"
    m_Parser.Options().useSmartSense = !XRCCTRL(*this, "chkUseSmartSense",    wxCheckBox)->GetValue();
    m_Parser.Options().whileTyping   =  XRCCTRL(*this, "chkWhileTyping",      wxCheckBox)->GetValue();
    m_Parser.Options().caseSensitive =  XRCCTRL(*this, "chkCaseSensitive",    wxCheckBox)->GetValue();

    // Page "C / C++ parser"
    m_Parser.Options().followLocalIncludes  = XRCCTRL(*this, "chkLocals",             wxCheckBox)->GetValue();
    m_Parser.Options().followGlobalIncludes = XRCCTRL(*this, "chkGlobals",            wxCheckBox)->GetValue();
    m_Parser.Options().wantPreprocessor     = XRCCTRL(*this, "chkPreprocessor",       wxCheckBox)->GetValue();
    m_Parser.Options().parseComplexMacros   = XRCCTRL(*this, "chkComplexMacros",      wxCheckBox)->GetValue();

    // Page "Symbol browser"
    m_Parser.ClassBrowserOptions().showInheritance = XRCCTRL(*this, "chkInheritance", wxCheckBox)->GetValue();
    m_Parser.ClassBrowserOptions().expandNS        = XRCCTRL(*this, "chkExpandNS",    wxCheckBox)->GetValue();
    m_Parser.ClassBrowserOptions().treeMembers     = XRCCTRL(*this, "chkTreeMembers", wxCheckBox)->GetValue();

    // Now write the parser options and re-read them again to make sure they are up-to-date
    m_Parser.WriteOptions();
    m_NativeParser->RereadParserOptions();
    m_CodeCompletion->RereadOptions();
}

void CCOptionsDlg::OnAddRepl(cb_unused wxCommandEvent& event)
{
    wxString key;
    wxString value;
    EditPairDlg dlg(this, key, value, _("Add new replacement token"), EditPairDlg::bmDisable);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        if ( ValidateReplacementToken(key, value) )
        {
            Tokenizer::SetReplacementString(key, value);
            XRCCTRL(*this, "lstRepl", wxListBox)->Append(key + _T(" -> ") + value);
        }
    }
}

void CCOptionsDlg::OnEditRepl(cb_unused wxCommandEvent& event)
{
    wxString key;
    wxString value;

    int sel = XRCCTRL(*this, "lstRepl", wxListBox)->GetSelection();
    if (sel == -1)
        return;

    key = XRCCTRL(*this, "lstRepl", wxListBox)->GetStringSelection();
    value = key;

    key = key.BeforeFirst(_T(' '));
    value = value.AfterLast(_T(' '));

    EditPairDlg dlg(this, key, value, _("Edit replacement token"), EditPairDlg::bmDisable);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        if ( ValidateReplacementToken(key, value) )
        {
            Tokenizer::SetReplacementString(key, value);
            XRCCTRL(*this, "lstRepl", wxListBox)->SetString(sel, key + _T(" -> ") + value);
        }
    }
}

void CCOptionsDlg::OnDelRepl(cb_unused wxCommandEvent& event)
{
    int sel = XRCCTRL(*this, "lstRepl", wxListBox)->GetSelection();
    if (sel == -1)
        return;

    if (cbMessageBox(_("Are you sure you want to delete this replacement token?"),
                     _("Confirmation"), wxICON_QUESTION | wxYES_NO) == wxID_YES)
    {
        wxString key = XRCCTRL(*this, "lstRepl", wxListBox)->GetStringSelection();
        key = key.BeforeFirst(_T(' '));
        Tokenizer::RemoveReplacementString(key);
        XRCCTRL(*this, "lstRepl", wxListBox)->Delete(sel);
    }
}

void CCOptionsDlg::OnChooseColour(wxCommandEvent& event)
{
    wxColourData data;
    wxWindow* sender = FindWindowById(event.GetId());
    data.SetColour(sender->GetBackgroundColour());

    wxColourDialog dlg(this, &data);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxColour colour = dlg.GetColourData().GetColour();
        sender->SetBackgroundColour(colour);
    }
}

void CCOptionsDlg::OnCCDelayScroll(cb_unused wxScrollEvent& event)
{
    UpdateCCDelayLabel();
}

void CCOptionsDlg::OnUpdateUI(cb_unused wxUpdateUIEvent& event)
{
    bool en = !XRCCTRL(*this, "chkNoCC",            wxCheckBox)->GetValue();
    bool al =  XRCCTRL(*this, "chkAutoLaunch",      wxCheckBox)->GetValue();

    // Page "Code Completion"
    XRCCTRL(*this, "chkUseSmartSense",              wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkWhileTyping",                wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkCaseSensitive",              wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkEvalTooltip",                wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkAutoSelectOne",              wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkAutoAddParentheses",         wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkAddDoxgenComment",           wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkEnableHeaders",              wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkAutoLaunch",                 wxCheckBox)->Enable(en);
    XRCCTRL(*this, "spnAutoLaunchChars",            wxSpinCtrl)->Enable(en && al);
    XRCCTRL(*this, "lblMaxMatches",                 wxStaticText)->Enable(en);
    XRCCTRL(*this, "spnMaxMatches",                 wxSpinCtrl)->Enable(en);
    XRCCTRL(*this, "lblFillupChars",                wxStaticText)->Enable(en);
    XRCCTRL(*this, "txtFillupChars",                wxTextCtrl)->Enable(en);
    XRCCTRL(*this, "sldCCDelay",                    wxSlider)->Enable(en);

    // Page "C / C++ parser"
    XRCCTRL(*this, "chkLocals",                     wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkGlobals",                    wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkPreprocessor",               wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkComplexMacros",              wxCheckBox)->Enable(en);

    XRCCTRL(*this, "txtPriorityHeaders",            wxTextCtrl)->Enable(en);
    en = XRCCTRL(*this, "rdoOneParserPerWorkspace", wxRadioButton)->GetValue();
    XRCCTRL(*this, "lblParsersNum",                 wxStaticText)->Enable(!en);
    XRCCTRL(*this, "spnParsersNum",                 wxSpinCtrl)->Enable(!en);

    // Page "C / C++ parser (adv.)"
    en = !XRCCTRL(*this, "chkNoCC",           wxCheckBox)->GetValue();
    XRCCTRL(*this, "txtCCFileExtHeader",      wxTextCtrl)->Enable(en);
    XRCCTRL(*this, "chkCCFileExtEmpty",       wxCheckBox)->Enable(en);
    XRCCTRL(*this, "txtCCFileExtSource",      wxTextCtrl)->Enable(en);

    int sel = XRCCTRL(*this, "lstRepl",       wxListBox)->GetSelection();
    XRCCTRL(*this, "btnEditRepl",             wxButton)->Enable(sel != -1);
    XRCCTRL(*this, "btnDelRepl",              wxButton)->Enable(sel != -1);

    XRCCTRL(*this, "chkKL_1",                 wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkKL_2",                 wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkKL_3",                 wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkKL_4",                 wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkKL_5",                 wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkKL_6",                 wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkKL_7",                 wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkKL_8",                 wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkKL_9",                 wxCheckBox)->Enable(en);

    // Page "Symbol browser"
    en = !XRCCTRL(*this, "chkNoSB",           wxCheckBox)->GetValue();
    XRCCTRL(*this, "chkInheritance",          wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkExpandNS",             wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkFloatCB",              wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkTreeMembers",          wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkScopeFilter",          wxCheckBox)->Enable(en);
}

void CCOptionsDlg::UpdateCCDelayLabel()
{
    int position = XRCCTRL(*this, "sldCCDelay", wxSlider)->GetValue();
    wxString lbl;
    if (position >= 10)
        lbl.Printf(_("%d.%d sec"), position / 10, position % 10);
    else
        lbl.Printf(_("%d ms"), position * 100);
    XRCCTRL(*this, "lblDelay", wxStaticText)->SetLabel(lbl);
}

bool CCOptionsDlg::ValidateReplacementToken(wxString& from, wxString& to)
{
    // cut off any leading / trailing spaces
    from.Trim(true).Trim(false);
    to.Trim(true).Trim(false);

    if (to.IsEmpty())
    {
        // Allow removing a token, but ask the user if that's OK.
        if (cbMessageBox( _("This setup will replace the token with an empty string.\n"
                            "This will *remove* the token and probably break CC for some cases.\n"
                            "Do you really want to *remove* that token?"),
                          _("Confirmation"),
                          wxICON_QUESTION | wxYES_NO ) == wxID_YES)
        {
            return true;
        }
    }
    else if (to.Contains(from))
    {
        cbMessageBox(_("Replacement token cannot contain search token.\n"
                       "This would cause an infinite loop otherwise."),
                     _("Error"), wxICON_ERROR);
        return false;
    }

    wxRegEx re(_T("[A-Za-z_]+[0-9]*[A-Za-z_]*"));
    if (!re.Matches(from))
    {
        cbMessageBox(_("Search token can only contain alphanumeric characters and underscores."),
                     _("Error"), wxICON_ERROR);
        return false;
    }
    if (!re.Matches(to))
    {
        // Allow replacing with special characters only if the user says it's ok.
        if (cbMessageBox( _("You are replacing a token with a string that contains\n"
                            "characters other than alphanumeric and underscores.\n"
                            "This could make parsing the file impossible.\n"
                            "Are you sure?"),
                          _("Confirmation"),
                          wxICON_QUESTION | wxYES_NO ) != wxID_YES)
        {
            return false;
        }
    }

    return true;
}
