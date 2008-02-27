/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include "cbproject.h"
    #include "compilerfactory.h"
    #include <wx/xrc/xmlres.h>
    #include <wx/intl.h>
    #include <wx/choice.h>
    #include <wx/checkbox.h>
    #include <wx/textctrl.h>
    #include <wx/button.h>
    #include <wx/filename.h>
    #include <wx/file.h>
    #include <wx/checklst.h>
    #include <wx/stattext.h>
#endif

#include "projectfileoptionsdlg.h"
#include <wx/slider.h>
#include <wx/notebook.h>
#include <wx/textfile.h>

BEGIN_EVENT_TABLE(ProjectFileOptionsDlg, wxDialog)
    EVT_CHOICE(-1, ProjectFileOptionsDlg::OnCompilerCombo)
    EVT_UPDATE_UI(-1, ProjectFileOptionsDlg::OnUpdateUI)
END_EVENT_TABLE()

// some help functions and type (copied and adapted from the codestat plug-in)
struct SLanguageDef
{
    wxArrayString ext;
    wxString single_line_comment;
    wxString multiple_line_comment[2];
};

void AnalyseLine(const SLanguageDef &language, wxString line, bool &comment, bool &code, bool &multi_line_comment)
{
    int first_single_line_comment, first_multi_line_comment_begin, first_multi_line_comment_end;

    // Delete first and trailing spaces
    line = line.Trim(true);
    line = line.Trim(false);

    if (line.IsEmpty())
        return;

    // Searching for single and multi-lines comment signs
    if (language.single_line_comment.Length() > 0)
        first_single_line_comment = line.Find(language.single_line_comment);
    else first_single_line_comment = -1;
    if (language.multiple_line_comment[0].Length() > 0)
        first_multi_line_comment_begin = line.Find(language.multiple_line_comment[0]);
    else first_multi_line_comment_begin = -1;
    if (language.multiple_line_comment[1].Length() > 0)
        first_multi_line_comment_end = line.Find(language.multiple_line_comment[1]);
    else first_multi_line_comment_end = -1;

    // We are in a multiple line comment => finding the "end of multiple line comment" sign
    if (multi_line_comment)
    {
        comment = true;
        if (first_multi_line_comment_end > -1)
        {
            multi_line_comment = false;
            if (first_multi_line_comment_end+language.multiple_line_comment[1].Length() < line.Length())
                AnalyseLine(language, line.Mid(first_multi_line_comment_end+language.multiple_line_comment[1].Length()), comment, code, multi_line_comment);
        }
    }
    // We are not in a multiple line comment
    else if (!multi_line_comment)
    {
        // First comment sign found is a single line comment sign
        if ( (first_single_line_comment>-1)
        &&((first_multi_line_comment_begin==-1)||((first_multi_line_comment_begin>-1)&&(first_single_line_comment<first_multi_line_comment_begin))) )
        {
            comment = true;
            if (first_single_line_comment > 0)
                code = true;
        }
        // First comment sign found is a multi-line comment begin sign
        else if (first_multi_line_comment_begin>-1)
        {
            multi_line_comment = true;
            comment = true;
            if (first_multi_line_comment_begin > 0)
                code = true;
            if (first_multi_line_comment_begin+language.multiple_line_comment[0].Length() < line.Length())
                AnalyseLine(language, line.Mid(first_multi_line_comment_begin+language.multiple_line_comment[0].Length()), comment, code, multi_line_comment);
        }
        else
        {
            code = true;
        }
    }
}

void CountLines(wxFileName filename, const SLanguageDef &language,
                long int &code_lines, long int &codecomments_lines,
                long int &comment_lines, long int &empty_lines, long int &total_lines)
{
    wxTextFile file;
    if (file.Open(filename.GetFullPath(), wxConvFile))
    {
        bool multi_line_comment = false;
        total_lines += file.GetLineCount();
        for (unsigned int i = 0; i < file.GetLineCount(); ++i)
        {
            wxString line = file[i];
            line = line.Trim(true);
            line = line.Trim(false);
            bool comment = false;
            bool code = false;
            if (line.IsEmpty())
            {
                ++empty_lines;
            }
            else
            {
                AnalyseLine(language, line, comment, code, multi_line_comment);
                if (comment&&code) ++codecomments_lines;
                else if (comment) ++comment_lines;
                else if (code) ++code_lines;
            }
        } // end for : idx : i
    }
}


ProjectFileOptionsDlg::ProjectFileOptionsDlg(wxWindow* parent, ProjectFile* pf)
    : m_ProjectFile(pf),
    m_LastBuildStageCompilerSel(-1)
{
    wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgProjectFileOptions"));

    if (pf)
    {
        cbProject* prj = pf->GetParentProject();
        wxCheckListBox *list = XRCCTRL(*this, "lstTargets", wxCheckListBox);
        for (int i = 0; i < prj->GetBuildTargetsCount(); ++i)
        {
            wxString targetName = prj->GetBuildTarget(i)->GetTitle();
            list->Append(targetName);
            if (pf->buildTargets.Index(targetName) != -1)
                list->Check(i, true);
        }

        FillGeneralProperties(pf->file.GetFullPath());

        XRCCTRL(*this, "txtCompiler", wxTextCtrl)->SetValue(pf->compilerVar);
        XRCCTRL(*this, "chkCompile", wxCheckBox)->SetValue(pf->compile);
        XRCCTRL(*this, "chkLink", wxCheckBox)->SetValue(pf->link);
        XRCCTRL(*this, "sliderWeight", wxSlider)->SetValue(pf->weight);
        XRCCTRL(*this, "txtObjName", wxTextCtrl)->SetValue(pf->GetObjName());
        FillCompilers();
        UpdateBuildCommand();

        XRCCTRL(*this, "txtProject", wxTextCtrl)->SetValue(prj?(prj->GetTitle() + _T("\n") + prj->GetFilename()):_T("-"));
        XRCCTRL(*this, "txtAbsName", wxTextCtrl)->SetValue(pf->file.GetFullPath());
        XRCCTRL(*this, "txtRelName", wxTextCtrl)->SetValue(pf->relativeFilename);

        SetTitle(_("Properties of ") + wxString(_("\"")) + pf->relativeFilename + wxString(_("\"")));
    }
    XRCCTRL(*this, "txtObjName", wxTextCtrl)->Enable(false);
    // included files not implemented yet -> hide it
    XRCCTRL(*this, "staticIncludedFilesLabel", wxTextCtrl)->Hide();
    XRCCTRL(*this, "staticIncludedFiles", wxTextCtrl)->Hide();
    
    if (pf->autoGeneratedBy)
    {
    	XRCCTRL(*this, "tabBuild", wxPanel)->Enable(false);
    	XRCCTRL(*this, "tabAdvanced", wxPanel)->Enable(false);
    }
    XRCCTRL(*this, "lblAutoGen", wxStaticText)->Show(pf->autoGeneratedBy);
} // end of constructor

ProjectFileOptionsDlg::ProjectFileOptionsDlg(wxWindow* parent, const wxString& fileName)
    : m_ProjectFile(0),
    m_LastBuildStageCompilerSel(-1)
{
    wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgProjectFileOptions"));

    FillGeneralProperties(fileName);

    XRCCTRL(*this, "txtAbsName", wxTextCtrl)->SetValue(fileName);

    SetTitle(_("Properties of ") + wxString(_("\"")) + fileName + wxString(_("\"")));
//
//  // hide un-needed pages (in reverse order!)
//  wxNotebook* nb = XRCCTRL(*this, "nbMain", wxNotebook);
//  nb->DeletePage(2);
//  nb->DeletePage(1);
}

ProjectFileOptionsDlg::~ProjectFileOptionsDlg()
{
}

void ProjectFileOptionsDlg::FillGeneralProperties(const wxString& fileName)
{
    // count some statistics of the file (only c/c++ files for the moment)
    SLanguageDef langCPP;
    langCPP.ext.Add(_T("c"));
    langCPP.ext.Add(_T("cc"));
    langCPP.ext.Add(_T("cxx"));
    langCPP.ext.Add(_T("cpp"));
    langCPP.ext.Add(_T("h"));
    langCPP.ext.Add(_T("hpp"));
    langCPP.single_line_comment = _T("//");
    langCPP.multiple_line_comment[0] = _T("/*");
    langCPP.multiple_line_comment[1] = _T("*/");
    wxFileName filename = fileName;
    if (filename.FileExists())
    {
        bool bExtOk = false;
        for (int j = 0; j < (int) langCPP.ext.Count(); ++j)
        {
            if (filename.GetExt() == langCPP.ext[j])
            {
                bExtOk = true;
                break;
            }
        }
        if(bExtOk)
        {
            long int total_lines = 0;
            long int code_lines = 0;
            long int empty_lines = 0;
            long int comment_lines = 0;
            long int codecomments_lines = 0;
            CountLines(filename, langCPP, code_lines, codecomments_lines, comment_lines, empty_lines, total_lines);
            XRCCTRL(*this, "staticTotalLines", wxStaticText)->SetLabel(wxString::Format(_("%ld"), total_lines));
            XRCCTRL(*this, "staticEmptyLines", wxStaticText)->SetLabel(wxString::Format(_("%ld"), empty_lines));
            XRCCTRL(*this, "staticActualLines", wxStaticText)->SetLabel(wxString::Format(_("%ld"), code_lines + codecomments_lines));
            XRCCTRL(*this, "staticCommentLines", wxStaticText)->SetLabel(wxString::Format(_("%ld"), comment_lines));
        }
        wxFile file(filename.GetFullPath());
        if(file.IsOpened())
        {
            long Length = static_cast<long>(file.Length());
            XRCCTRL(*this, "staticFileSize", wxStaticText)->SetLabel(wxString::Format(_("%ld"), Length));
            file.Close();
        }
        wxDateTime ModTime = filename.GetModificationTime();
        XRCCTRL(*this, "staticDateTimeStamp", wxStaticText)->SetLabel(
            wxString::Format(_("%ld/%ld/%ld %ld:%ld:%ld"), ModTime.GetDay(),
            ModTime.GetMonth() + 1, ModTime.GetYear(), ModTime.GetHour(), // seems I have to add 1 for the month ?
            ModTime.GetMinute(), ModTime.GetSecond()));
    }
}

void ProjectFileOptionsDlg::FillCompilers()
{
    // fill compilers combo
    wxChoice* cmb = XRCCTRL(*this, "cmbBuildStageCompiler", wxChoice);
    cmb->Clear();
    for (unsigned int i = 0; i < CompilerFactory::GetCompilersCount(); ++i)
    {
        cmb->Append(CompilerFactory::GetCompiler(i)->GetName());
    }
    // select project default compiler
    m_LastBuildStageCompilerSel = CompilerFactory::GetCompilerIndex(m_ProjectFile->GetParentProject()->GetCompilerID());
    cmb->SetSelection(m_LastBuildStageCompilerSel);
}

void ProjectFileOptionsDlg::UpdateBuildCommand()
{
    wxChoice* cmb = XRCCTRL(*this, "cmbBuildStageCompiler", wxChoice);
    int idx = cmb->GetSelection();
    Compiler* compiler = CompilerFactory::GetCompiler(idx);
    if (!compiler)
      return;

    FileType ft = FileTypeOf(m_ProjectFile->relativeFilename);
    wxString cmd;
    if (ft == ftResource)
        cmd = compiler->GetCommand(ctCompileResourceCmd);
    else if (ft == ftSource || ft == ftHeader)
        cmd = compiler->GetCommand(ctCompileObjectCmd);
    XRCCTRL(*this, "lblBuildCommand", wxStaticText)->SetLabel(_("Default: ") + cmd);
    Layout();

    XRCCTRL(*this, "chkBuildStage", wxCheckBox)->SetValue(m_ProjectFile->customBuild[compiler->GetID()].useCustomBuildCommand);
    XRCCTRL(*this, "txtBuildStage", wxTextCtrl)->SetValue(m_ProjectFile->customBuild[compiler->GetID()].buildCommand);
}

void ProjectFileOptionsDlg::SaveBuildCommandSelection()
{
    Compiler* compiler = CompilerFactory::GetCompiler(m_LastBuildStageCompilerSel);
    m_ProjectFile->customBuild[compiler->GetID()].useCustomBuildCommand = XRCCTRL(*this, "chkBuildStage", wxCheckBox)->GetValue();
    m_ProjectFile->customBuild[compiler->GetID()].buildCommand = XRCCTRL(*this, "txtBuildStage", wxTextCtrl)->GetValue();
}

void ProjectFileOptionsDlg::OnCompilerCombo(wxCommandEvent& event)
{
    if (m_LastBuildStageCompilerSel != event.GetSelection())
    {
        // first save old selection
        SaveBuildCommandSelection();
        m_LastBuildStageCompilerSel = event.GetSelection();

        // then load new selection
        UpdateBuildCommand();
    }
}

void ProjectFileOptionsDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
    if (m_ProjectFile)
    {
        bool en = XRCCTRL(*this, "chkBuildStage", wxCheckBox)->GetValue();
        XRCCTRL(*this, "txtBuildStage", wxTextCtrl)->Enable(en);
    }
    else
    {
        XRCCTRL(*this, "txtCompiler", wxTextCtrl)->Enable(false);
        XRCCTRL(*this, "lstTargets", wxCheckListBox)->Enable(false);
        XRCCTRL(*this, "chkCompile", wxCheckBox)->Enable(false);
        XRCCTRL(*this, "chkLink", wxCheckBox)->Enable(false);
        XRCCTRL(*this, "txtObjName", wxTextCtrl)->Enable(false);;
        XRCCTRL(*this, "chkBuildStage", wxCheckBox)->Enable(false);
        XRCCTRL(*this, "txtBuildStage", wxTextCtrl)->Enable(false);
        XRCCTRL(*this, "sliderWeight", wxSlider)->Enable(false);
    }
}

void ProjectFileOptionsDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK && m_ProjectFile)
    {
        m_ProjectFile->buildTargets.Clear();
        wxCheckListBox *list = XRCCTRL(*this, "lstTargets", wxCheckListBox);
        for (int i = 0; i < (int)list->GetCount(); i++)
        {
            if (list->IsChecked(i))
                m_ProjectFile->AddBuildTarget(list->GetString(i));
        }

        m_ProjectFile->compile = XRCCTRL(*this, "chkCompile", wxCheckBox)->GetValue();
        m_ProjectFile->link = XRCCTRL(*this, "chkLink", wxCheckBox)->GetValue();
        m_ProjectFile->weight = XRCCTRL(*this, "sliderWeight", wxSlider)->GetValue();
    //  m_ProjectFile->SetObjName(XRCCTRL(*this, "txtObjName", wxTextCtrl)->GetValue());
        SaveBuildCommandSelection();
        m_ProjectFile->compilerVar = XRCCTRL(*this, "txtCompiler", wxTextCtrl)->GetValue();

        // make sure we have a compiler var, if the file is to be compiled
        if (m_ProjectFile->compile && m_ProjectFile->compilerVar.IsEmpty())
            m_ProjectFile->compilerVar = _T("CPP");

        cbProject* prj = m_ProjectFile->GetParentProject();
        prj->SetModified(true);
    }

    wxDialog::EndModal(retCode);
}
