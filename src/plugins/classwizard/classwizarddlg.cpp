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
#include <wx/arrstr.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/event.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/intl.h>
#include <wx/textctrl.h>
#include <wx/xrc/xmlres.h>
#include "cbeditor.h"
#include "cbproject.h"
#include "configmanager.h"
#include "editormanager.h"
#include "globals.h"
#include "manager.h"
#include "macrosmanager.h"
#include "projectmanager.h"
#endif
#include "cbstyledtextctrl.h"

#include "classwizarddlg.h"
#include <wx/tokenzr.h>
#include <wx/dirdlg.h>

namespace {


void ForceDirectory(const wxFileName & filename)
{
    wxFileName parentname(filename);
    parentname.RemoveLastDir();

    if (filename != parentname)
        ForceDirectory(parentname);

    if (!wxDirExists(filename.GetPath()))
        wxMkdir(filename.GetPath());
}

}// namespace

BEGIN_EVENT_TABLE(ClassWizardDlg, wxDialog)
    EVT_UPDATE_UI(-1, ClassWizardDlg::OnUpdateUI)
    EVT_TEXT(XRCID("txtName"), ClassWizardDlg::OnNameChange)
    EVT_TEXT(XRCID("txtInheritance"), ClassWizardDlg::OnAncestorChange)
    EVT_BUTTON(XRCID("btnCommonDir"), ClassWizardDlg::OnCommonDirClick)
    EVT_CHECKBOX(XRCID("chkLowerCase"), ClassWizardDlg::OnLowerCaseClick)
    EVT_BUTTON(XRCID("btnIncludeDir"), ClassWizardDlg::OnIncludeDirClick)
    EVT_BUTTON(XRCID("btnImplDir"), ClassWizardDlg::OnImplDirClick)
    EVT_TEXT(XRCID("txtHeader"), ClassWizardDlg::OnHeaderChange)
    EVT_BUTTON(XRCID("wxID_OK"), ClassWizardDlg::OnOKClick)
    EVT_BUTTON(XRCID("wxID_CANCEL"), ClassWizardDlg::OnCancelClick)
END_EVENT_TABLE()

ClassWizardDlg::ClassWizardDlg(wxWindow* parent)
{
    wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgNewClass"));

    ProjectManager* prjMan = Manager::Get()->GetProjectManager();
    cbProject* prj = prjMan->GetActiveProject();
    if (prj)
    {
        XRCCTRL(*this, "txtIncludeDir", wxTextCtrl)->SetValue(prj->GetCommonTopLevelPath() + _T("include"));
        XRCCTRL(*this, "txtImplDir", wxTextCtrl)->SetValue(prj->GetCommonTopLevelPath() + _T("src"));
        XRCCTRL(*this, "txtCommonDir", wxTextCtrl)->SetValue(prj->GetCommonTopLevelPath());
    }
    else
    {
        XRCCTRL(*this, "txtIncludeDir", wxTextCtrl)->SetValue(::wxGetCwd());
        XRCCTRL(*this, "txtImplDir", wxTextCtrl)->SetValue(::wxGetCwd());
        XRCCTRL(*this, "txtCommonDir", wxTextCtrl)->SetValue(::wxGetCwd());
    }
    XRCCTRL(*this, "txtInheritanceFilename", wxTextCtrl)->SetValue(_T("<>"));
    XRCCTRL(*this, "cmbInheritanceScope", wxComboBox)->SetSelection(0);
    XRCCTRL(*this, "txtHeaderInclude", wxTextCtrl)->SetValue(_T("\"\""));

    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("classwizard"));
    if (cfg)
    {
        XRCCTRL(*this, "chkCommonDir", wxCheckBox)->SetValue(cfg->ReadBool(_T("common_dir")));
        XRCCTRL(*this, "chkLowerCase", wxCheckBox)->SetValue(cfg->ReadBool(_T("lower_case")));
    }
}

ClassWizardDlg::~ClassWizardDlg()
{
    // NOTE (Morten#3#): Not nice to have it here (should be in OnApply of the plugin)
    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("classwizard"));
    if (cfg)
    {
        cfg->Write(_T("common_dir"), (bool) XRCCTRL(*this, "chkCommonDir", wxCheckBox)->GetValue());
        cfg->Write(_T("lower_case"), (bool) XRCCTRL(*this, "chkLowerCase", wxCheckBox)->GetValue());
    }
}

void ClassWizardDlg::DoGuardBlock()
{
    m_Header = XRCCTRL(*this, "txtHeader", wxTextCtrl)->GetValue();
    wxString GuardWord = m_Header;
    GuardWord.MakeUpper();
    while (GuardWord.Replace(_T("."), _T("_")))
        ;
    while (GuardWord.Replace(_T("/"), _T("_")))
        ;
    while (GuardWord.Replace(_T("\\"), _T("_")))
        ;
    XRCCTRL(*this, "txtGuardBlock", wxTextCtrl)->SetValue(GuardWord);
}

void ClassWizardDlg::DoFileNames()
{
    wxString name = XRCCTRL(*this, "txtName", wxTextCtrl)->GetValue();
    if (XRCCTRL(*this, "chkLowerCase", wxCheckBox)->GetValue())
        name.MakeLower();

    while (name.Replace(_T("::"), _T("/")))
        ;

    XRCCTRL(*this, "txtHeader", wxTextCtrl)->SetValue(name + _T(".h"));
    XRCCTRL(*this, "txtImplementation", wxTextCtrl)->SetValue(name + _T(".cpp"));
    XRCCTRL(*this, "txtHeaderInclude", wxTextCtrl)->SetValue(_T("\"") + name + _T(".h\""));
}

// events

void ClassWizardDlg::OnUpdateUI(wxUpdateUIEvent& WXUNUSED(event))
{
    bool inherits = XRCCTRL(*this, "chkInherits", wxCheckBox)->GetValue();
    XRCCTRL(*this, "txtInheritance", wxTextCtrl)->Enable(inherits);
    XRCCTRL(*this, "txtInheritanceFilename", wxTextCtrl)->Enable(inherits);
    XRCCTRL(*this, "cmbInheritanceScope", wxComboBox)->Enable(inherits);

    bool hasdestructor = XRCCTRL(*this, "chkHasDestructor", wxCheckBox)->GetValue();
    XRCCTRL(*this, "chkVirtualDestructor", wxCheckBox)->Enable(hasdestructor);

    bool genimpl = XRCCTRL(*this, "chkImplementation", wxCheckBox)->GetValue();
    XRCCTRL(*this, "txtImplementation", wxTextCtrl)->Enable(genimpl);
    XRCCTRL(*this, "txtHeaderInclude", wxTextCtrl)->Enable(genimpl);

    bool commonDir = XRCCTRL(*this, "chkCommonDir", wxCheckBox)->GetValue();
    XRCCTRL(*this, "txtImplDir", wxTextCtrl)->Enable(genimpl && !commonDir);
    XRCCTRL(*this, "btnImplDir", wxButton)->Enable(genimpl && !commonDir);
    XRCCTRL(*this, "txtIncludeDir", wxTextCtrl)->Enable(!commonDir);
    XRCCTRL(*this, "btnIncludeDir", wxButton)->Enable(!commonDir);
    XRCCTRL(*this, "txtCommonDir", wxTextCtrl)->Enable(commonDir);
    XRCCTRL(*this, "btnCommonDir", wxButton)->Enable(commonDir);

    bool genguard = XRCCTRL(*this, "chkGuardBlock", wxCheckBox)->GetValue();
    XRCCTRL(*this, "txtGuardBlock", wxTextCtrl)->Enable(genguard);
}

void ClassWizardDlg::OnNameChange(wxCommandEvent& WXUNUSED(event))
{
    DoFileNames();
    DoGuardBlock();
}

void ClassWizardDlg::OnAncestorChange(wxCommandEvent& WXUNUSED(event))
{
    wxString name = XRCCTRL(*this, "txtInheritance", wxTextCtrl)->GetValue();

    while (name.Replace(_T("::"), _T("/")))
        ;

    wxString old = XRCCTRL(*this, "txtInheritanceFilename", wxTextCtrl)->GetValue();
    wxChar first = old.GetChar(0);
    wxChar last = old.Last();
    XRCCTRL(*this, "txtInheritanceFilename", wxTextCtrl)->SetValue(first + name + _T(".h") + last);
    DoGuardBlock();
}

void ClassWizardDlg::OnCommonDirClick(wxCommandEvent& WXUNUSED(event))
{
    wxString path = XRCCTRL(*this, "txtCommonDir", wxTextCtrl)->GetValue();
    wxDirDialog dlg (this, _T("Choose a directory"), path);
    if (dlg.ShowModal()==wxID_OK)
    {
        path = dlg.GetPath();
        XRCCTRL(*this, "txtCommonDir", wxTextCtrl)->SetValue(path);
    }
}

void ClassWizardDlg::OnLowerCaseClick(wxCommandEvent& WXUNUSED(event))
{
    DoFileNames();
}

void ClassWizardDlg::OnIncludeDirClick(wxCommandEvent& WXUNUSED(event))
{
    wxString path = XRCCTRL(*this, "txtIncludeDir", wxTextCtrl)->GetValue();
    wxDirDialog dlg (this, _T("Choose a directory"), path);
    if (dlg.ShowModal()==wxID_OK)
    {
        path = dlg.GetPath();
        XRCCTRL(*this, "txtIncludeDir", wxTextCtrl)->SetValue(path);
    }
}

void ClassWizardDlg::OnImplDirClick(wxCommandEvent& WXUNUSED(event))
{
    wxString path = XRCCTRL(*this, "txtImplDir", wxTextCtrl)->GetValue();
    wxDirDialog dlg (this, _T("Choose a directory"), path);
    if (dlg.ShowModal()==wxID_OK)
    {
        path = dlg.GetPath();
        XRCCTRL(*this, "txtImplDir", wxTextCtrl)->SetValue(path);
    }
}

void ClassWizardDlg::OnHeaderChange(wxCommandEvent& WXUNUSED(event))
{
    wxString name = XRCCTRL(*this, "txtHeader", wxTextCtrl)->GetValue();
    XRCCTRL(*this, "txtHeaderInclude", wxTextCtrl)->SetValue(_T("\"") + name + _T("\""));
}

void ClassWizardDlg::OnOKClick(wxCommandEvent& WXUNUSED(event))
{
    // set some variable for easy reference
    wxString Name = XRCCTRL(*this, "txtName", wxTextCtrl)->GetValue();
    if (Name.IsEmpty())
    {
        cbMessageBox(_T("Please specify a class name to continue."),
                     _T("Error"), wxOK | wxICON_ERROR);
        return;
    }
    wxArrayString NameSpaces;
    wxStringTokenizer tkz(Name, _T("::"));
    Name = wxEmptyString;
    while ( tkz.HasMoreTokens() )
    {
        if (!Name.IsEmpty())
        {
            NameSpaces.Add(Name);
        }
        Name = tkz.GetNextToken();
    }

    wxString includeDir;
    wxString implDir;

    bool CommonDir = XRCCTRL(*this, "chkCommonDir", wxCheckBox)->GetValue();
    if (CommonDir)
    {
        includeDir = XRCCTRL(*this, "txtCommonDir", wxTextCtrl)->GetValue();
        implDir = XRCCTRL(*this, "txtCommonDir", wxTextCtrl)->GetValue();
    }
    else
    {
        includeDir = XRCCTRL(*this, "txtIncludeDir", wxTextCtrl)->GetValue();
        implDir = XRCCTRL(*this, "txtImplDir", wxTextCtrl)->GetValue();
    }
    //wxSetWorkingDirectory(baseDir);
    wxString Constructor = XRCCTRL(*this, "txtConstructor", wxTextCtrl)->GetValue();
    bool VirtualDestructor = XRCCTRL(*this, "chkVirtualDestructor", wxCheckBox)->GetValue();
    bool HasDestructor = XRCCTRL(*this, "chkHasDestructor", wxCheckBox)->GetValue();
    if (!HasDestructor)
    {
        VirtualDestructor = false;
    }
    bool HasCopyCtor = XRCCTRL(*this, "chkHasCopyCtor", wxCheckBox)->GetValue();
    bool HasAssignmentOp = XRCCTRL(*this, "chkHasAssignmentOp", wxCheckBox)->GetValue();

    wxString Ancestor = XRCCTRL(*this, "txtInheritance", wxTextCtrl)->GetValue();
    wxString AncestorFilename = XRCCTRL(*this, "txtInheritanceFilename", wxTextCtrl)->GetValue();
    wxString AncestorScope = XRCCTRL(*this, "cmbInheritanceScope", wxComboBox)->GetValue();
    bool Inherits = XRCCTRL(*this, "chkInherits", wxCheckBox)->GetValue() && !Ancestor.IsEmpty();

    m_Header = XRCCTRL(*this, "txtHeader", wxTextCtrl)->GetValue();
    m_Implementation = XRCCTRL(*this, "txtImplementation", wxTextCtrl)->GetValue();
    wxString headerInclude = XRCCTRL(*this, "txtHeaderInclude", wxTextCtrl)->GetValue();
    bool GenerateImplementation = XRCCTRL(*this, "chkImplementation", wxCheckBox)->GetValue();
    bool GuardBlock = XRCCTRL(*this, "chkGuardBlock", wxCheckBox)->GetValue();
    wxString GuardWord = XRCCTRL(*this, "txtGuardBlock", wxTextCtrl)->GetValue();
    if (GuardWord.IsEmpty())
    {
        DoGuardBlock();
    }
    wxFileName headerFname(UnixFilename(m_Header));
    wxFileName implementationFname(UnixFilename(m_Implementation));
    headerFname.MakeAbsolute(includeDir);
    implementationFname.MakeAbsolute(implDir);

    bool usestabs = Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/use_tab"), false);
    int tabsize = Manager::Get()->GetConfigManager(_T("editor"))->ReadInt(_T("/tab_size"), 4);
    int eolmode = Manager::Get()->GetConfigManager(_T("editor"))->ReadInt(_T("/eol/eolmode"), 0);

    ForceDirectory(headerFname);
    cbEditor* new_ed = Manager::Get()->GetEditorManager()->New(headerFname.GetFullPath());
    if (!new_ed)
    {
        cbMessageBox(_T("Class wizard can't continue. Possibly the Header file name is invalid.\n"
                        "Please check the entered file name."),
                     _T("Error"), wxICON_ERROR);
        return;
    }
    wxString buffer = new_ed->GetControl()->GetText();
    Manager::Get()->GetMacrosManager()->ReplaceMacros(buffer);

    wxString tabstr = usestabs ? wxString(_T("\t")) : wxString(_T(' '),tabsize);
    wxString eolstr;
    if (eolmode == 2)
    {
        eolstr = _T("\n");
    }
    else if (eolmode == 1)
    {
        eolstr = _T("\r");
    }
    else
    {
        eolstr = _T("\r\n");
    }

    // actual creation starts here
    // let's start with the header file
    if (GuardBlock)
    {
        buffer << _T("#ifndef ") << GuardWord << eolstr;
        buffer << _T("#define ") << GuardWord << eolstr;
        buffer << eolstr;
    }

    if (!AncestorFilename.IsEmpty() && !AncestorFilename.IsSameAs(_T("<>")))
    {
        buffer << _T("#include ") << AncestorFilename << eolstr;
        buffer << eolstr;
    }

    for (unsigned int i=0; i<NameSpaces.GetCount(); ++i)
    {
        buffer << _T("namespace ") << NameSpaces[i] << _T(" {") << eolstr;
    }
    buffer << eolstr;


    buffer << _T("class ") << Name;
    if (Inherits)
    {
        buffer << _T(" : ") << AncestorScope << _T(" ") << Ancestor;
    }
    buffer << eolstr;
    buffer << _T("{") << eolstr;
    buffer << tabstr << _T("public:") << eolstr;
    buffer << tabstr << tabstr << Name << _T("(") << Constructor << _T(")");
    buffer << (!GenerateImplementation ? _T(" {}") : _T(";")) << eolstr;

    if (HasDestructor)
    {
        buffer << tabstr << tabstr;
        if (VirtualDestructor)
        {
            buffer << _T("virtual ");
        }
        buffer << _T('~') << Name << _T("()");
        buffer << (!GenerateImplementation ? _T(" {}") : _T(";")) << eolstr;
    }

    if (HasCopyCtor)
    {
        buffer << tabstr << tabstr;
        buffer << Name << _T("(const ") << Name << _T("& other)");
        buffer << (!GenerateImplementation ? _T(" {}") : _T(";")) << eolstr;
    }

    if (HasAssignmentOp)
    {
        buffer << tabstr << tabstr;
        buffer << Name << _T("& ") << _T("operator=(const ") << Name << _T("& other)");
        buffer << (!GenerateImplementation ? _T(" { return *this; }") : _T(";")) << eolstr;
    }

    buffer << tabstr << _T("protected:") << eolstr;
    buffer << tabstr << _T("private:") << eolstr;
    buffer << _T("};") << eolstr;

    if (NameSpaces.GetCount())
    {
        buffer << eolstr;
        for (int i=NameSpaces.GetCount(); i>0; --i)
        {
            buffer << _T("} // namespace ") << NameSpaces[i-1] << eolstr;
        }
    }

    if (GuardBlock)
    {
        buffer << eolstr;
        buffer << _T("#endif // ") << GuardWord << eolstr;
    }

    new_ed->GetControl()->SetText(buffer);
    if (!new_ed->Save())
    {
        wxString msg;
        msg.Printf(_("Could not save header file %s.\nAborting..."), headerFname.GetFullPath().c_str());
        cbMessageBox(msg, _("Error"), wxICON_ERROR);
        return;
    }

    if (!GenerateImplementation)
    {
        m_Header = headerFname.GetFullPath();
        m_Implementation = wxEmptyString;
        EndModal(wxID_OK);
        return;
    }

    // now the implementation file
    ForceDirectory(implementationFname);
    new_ed = Manager::Get()->GetEditorManager()->New(implementationFname.GetFullPath());
    if (!new_ed)
    {
        cbMessageBox(_T("Class wizard can't continue. Possibly the Implementation file name is invalid.\n"
                        "Please check the entered file name."),
                     _T("Error"), wxICON_ERROR);
        return;
    }
    buffer = new_ed->GetControl()->GetText();
    Manager::Get()->GetMacrosManager()->ReplaceMacros(buffer);

    buffer << _T("#include ") << headerInclude << eolstr;

    if (NameSpaces.GetCount())
    {
        buffer << eolstr;
        for (unsigned int i=0; i<NameSpaces.GetCount(); ++i)
        {
            buffer << _T("namespace ") << NameSpaces[i] << _T(" {") << eolstr;
        }
    }

    buffer << eolstr;
    buffer << Name << _T("::") << Name << _T("(") << Constructor << _T(")") << eolstr;
    buffer << _T("{") << eolstr;
    buffer << tabstr << _T("//ctor") << eolstr;
    buffer << _T("}") << eolstr;

    if (HasDestructor)
    {
        buffer << eolstr;
        buffer << Name << _T("::~") << Name << _T("()") << eolstr;
        buffer << _T("{") << eolstr;
        buffer << tabstr << _T("//dtor") << eolstr;
        buffer << _T("}") << eolstr;
    }

    if (HasCopyCtor)
    {
        buffer << eolstr;
        buffer << Name << _T("::") << Name << _T("(const ") << Name << _T("& other)") << eolstr;;
        buffer << _T("{") << eolstr;
        buffer << tabstr << _T("//copy ctor") << eolstr;
        buffer << _T("}") << eolstr;
    }

    if (HasAssignmentOp)
    {
        buffer << eolstr;
        buffer << Name << _T("& ") << Name << _T("::operator=(const ") << Name << _T("& rhs)") << eolstr;;
        buffer << _T("{") << eolstr;
        buffer << tabstr << _T("if (this == &rhs) return *this; // handle self assignment") << eolstr;
        buffer << tabstr << _T("//assignment operator") << eolstr;
        buffer << tabstr << _T("return *this;") << eolstr;
        buffer << _T("}") << eolstr;
    }

    if (NameSpaces.GetCount())
    {
        buffer << eolstr;
        for (int i=NameSpaces.GetCount(); i>0; --i)
        {
            buffer << _T("} // namespace ") << NameSpaces[i-1] << eolstr;
        }
    }

    new_ed->GetControl()->SetText(buffer);
    if (!new_ed->Save())
    {
        wxString msg;
        msg.Printf(_("Could not save implementation file %s.\nAborting..."), implementationFname.GetFullPath().c_str());
        cbMessageBox(msg, _("Error"), wxICON_ERROR);
        return;
    }
    m_Header = headerFname.GetFullPath();
    m_Implementation = implementationFname.GetFullPath();

    EndModal(wxID_OK);
}

void ClassWizardDlg::OnCancelClick(wxCommandEvent& WXUNUSED(event))
{
    EndModal(wxID_CANCEL);
}
