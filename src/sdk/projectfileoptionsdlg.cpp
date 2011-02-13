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
    #include "logmanager.h"
    #include "projectmanager.h"
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
    #include <wx/sizer.h>
#endif

#if (__WXMSW__)
// TODO: equivalent??? -> #include <errno.h>
#else
#include <errno.h>
#endif

#include "projectfileoptionsdlg.h"
#include <wx/slider.h>
#include <wx/notebook.h>
#include <wx/textfile.h>

BEGIN_EVENT_TABLE(ProjectFileOptionsDlg, wxScrollingDialog)
    EVT_CHECKBOX (-1, ProjectFileOptionsDlg::OnReadOnlyCheck)
    EVT_CHOICE   (-1, ProjectFileOptionsDlg::OnCompilerCombo)
    EVT_UPDATE_UI(-1, ProjectFileOptionsDlg::OnUpdateUI)
END_EVENT_TABLE()

// some help functions and type (copied and adapted from the codestat plug-in)
struct SLanguageDef
{
    wxArrayString ext;
    wxString      single_line_comment;
    wxString      multiple_line_comment[2];
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

ProjectFileOptionsDlg::ProjectFileOptionsDlg(wxWindow* parent, ProjectFile* pf) :
    m_ProjectFile(pf),
    m_FileNameStr(wxEmptyString),
    m_FileName(),
    m_LastBuildStageCompilerSel(-1)
{
    wxXmlResource::Get()->LoadObject(this, parent, _T("dlgProjectFileOptions"),_T("wxScrollingDialog"));

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

        m_FileNameStr = pf->file.GetFullPath();
        FillGeneralProperties();

        XRCCTRL(*this, "txtCompiler",  wxTextCtrl)->SetValue(pf->compilerVar);
        XRCCTRL(*this, "chkCompile",   wxCheckBox)->SetValue(pf->compile);
        XRCCTRL(*this, "chkLink",      wxCheckBox)->SetValue(pf->link);
        XRCCTRL(*this, "sliderWeight", wxSlider)->SetValue(pf->weight);
        XRCCTRL(*this, "txtObjName",   wxTextCtrl)->SetValue(pf->GetObjName());

        FillCompilers();
        UpdateBuildCommand();

        XRCCTRL(*this, "txtProject", wxTextCtrl)->SetValue(prj?(prj->GetTitle() + _T("\n") + prj->GetFilename()):_T("-"));
        XRCCTRL(*this, "txtAbsName", wxTextCtrl)->SetValue(m_FileNameStr);
        XRCCTRL(*this, "txtRelName", wxTextCtrl)->SetValue(pf->relativeFilename);

        SetTitle(_("Properties of ") + wxString(_("\"")) + pf->relativeFilename + wxString(_("\"")));
    }
    XRCCTRL(*this, "txtObjName",               wxTextCtrl)->Enable(false);
    // included files not implemented yet -> hide it
    XRCCTRL(*this, "staticIncludedFilesLabel", wxStaticText)->Hide();
    XRCCTRL(*this, "staticIncludedFiles",      wxStaticText)->Hide();

    if (pf->AutoGeneratedBy())
    {
        XRCCTRL(*this, "tabBuild",    wxPanel)->Enable(false);
        XRCCTRL(*this, "tabAdvanced", wxPanel)->Enable(false);
    }
    XRCCTRL(*this, "lblAutoGen", wxStaticText)->Show(pf->AutoGeneratedBy());
}

ProjectFileOptionsDlg::ProjectFileOptionsDlg(wxWindow* parent, const wxString& fileName) :
    m_ProjectFile(0),
    m_FileNameStr(fileName),
    m_FileName(),
    m_LastBuildStageCompilerSel(-1)
{
    wxXmlResource::Get()->LoadObject(this, parent, _T("dlgProjectFileOptions"),_T("wxScrollingDialog"));

    FillGeneralProperties();

    XRCCTRL(*this, "txtAbsName", wxTextCtrl)->SetValue(m_FileNameStr);

    SetTitle(_("Properties of ") + wxString(_("\"")) + m_FileNameStr + wxString(_("\"")));
}

ProjectFileOptionsDlg::~ProjectFileOptionsDlg()
{
}

void ProjectFileOptionsDlg::OnReadOnlyCheck(wxCommandEvent& event)
{
    if (m_FileNameStr.IsEmpty() || !m_FileName.FileExists())
        return;

    if (event.IsChecked())
    {
        // make read-only
        if (!ToggleFileReadOnly(true))
            #if wxCHECK_VERSION(2, 9, 0)
            Manager::Get()->GetLogManager()->DebugLog(F(_T("Unable to set file '%s' read-only (probably missing access rights)."), m_FileNameStr.wx_str()));
            #else
            Manager::Get()->GetLogManager()->DebugLog(F(_T("Unable to set file '%s' read-only (probably missing access rights)."), m_FileNameStr.c_str()));
            #endif
    }
    else
    {
        // make writeable
        if (!ToggleFileReadOnly(false))
            #if wxCHECK_VERSION(2, 9, 0)
            Manager::Get()->GetLogManager()->DebugLog(F(_T("Unable to set file '%s' writeable (probably missing access rights)."), m_FileNameStr.wx_str()));
            #else
            Manager::Get()->GetLogManager()->DebugLog(F(_T("Unable to set file '%s' writeable (probably missing access rights)."), m_FileNameStr.c_str()));
            #endif
    }

    // Update UI
    XRCCTRL(*this, "chkReadOnly", wxCheckBox)->SetValue(!m_FileName.IsFileWritable());

    Manager::Get()->GetEditorManager()->CheckForExternallyModifiedFiles();
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

void ProjectFileOptionsDlg::OnUpdateUI(wxUpdateUIEvent& /*event*/)
{
    if (m_ProjectFile)
    {
        bool en = XRCCTRL(*this, "chkBuildStage", wxCheckBox)->GetValue();
        XRCCTRL(*this, "txtBuildStage", wxTextCtrl)->Enable(en);
    }
    else
    {
        XRCCTRL(*this, "txtCompiler",   wxTextCtrl)->Enable(false);
        XRCCTRL(*this, "lstTargets",    wxCheckListBox)->Enable(false);
        XRCCTRL(*this, "chkCompile",    wxCheckBox)->Enable(false);
        XRCCTRL(*this, "chkLink",       wxCheckBox)->Enable(false);
        XRCCTRL(*this, "txtObjName",    wxTextCtrl)->Enable(false);
        XRCCTRL(*this, "chkBuildStage", wxCheckBox)->Enable(false);
        XRCCTRL(*this, "txtBuildStage", wxTextCtrl)->Enable(false);
        XRCCTRL(*this, "sliderWeight",  wxSlider)->Enable(false);
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

        m_ProjectFile->compile = XRCCTRL(*this, "chkCompile",   wxCheckBox)->GetValue();
        m_ProjectFile->link    = XRCCTRL(*this, "chkLink",      wxCheckBox)->GetValue();
        m_ProjectFile->weight  = XRCCTRL(*this, "sliderWeight", wxSlider)->GetValue();
//      m_ProjectFile->SetObjName(XRCCTRL(*this, "txtObjName",  wxTextCtrl)->GetValue());
        SaveBuildCommandSelection();
        m_ProjectFile->compilerVar = XRCCTRL(*this, "txtCompiler", wxTextCtrl)->GetValue();

        // make sure we have a compiler var, if the file is to be compiled
        if (m_ProjectFile->compile && m_ProjectFile->compilerVar.IsEmpty())
            m_ProjectFile->compilerVar = _T("CPP");

        cbProject* prj = m_ProjectFile->GetParentProject();
        prj->SetModified(true);
        Manager::Get()->GetProjectManager()->RebuildTree();
    }

    wxScrollingDialog::EndModal(retCode);
}

void ProjectFileOptionsDlg::FillGeneralProperties()
{
    // count some statistics of the file (only c/c++ files for the moment)
    SLanguageDef langCPP;
    langCPP.ext.Add(_T("c"));
    langCPP.ext.Add(_T("cc"));
    langCPP.ext.Add(_T("cxx"));
    langCPP.ext.Add(_T("cpp"));
    langCPP.ext.Add(_T("c++"));
    langCPP.ext.Add(_T("h"));
    langCPP.ext.Add(_T("hh"));
    langCPP.ext.Add(_T("hxx"));
    langCPP.ext.Add(_T("hpp"));
    langCPP.ext.Add(_T("h++"));
    langCPP.single_line_comment = _T("//");
    langCPP.multiple_line_comment[0] = _T("/*");
    langCPP.multiple_line_comment[1] = _T("*/");

    m_FileName.Assign(m_FileNameStr);
    if (m_FileName.FileExists())
    {
        bool bExtOk = false;
        for (int j = 0; j < (int) langCPP.ext.Count(); ++j)
        {
            if (m_FileName.GetExt() == langCPP.ext[j])
            {
                bExtOk = true;
                break;
            }
        }
        if (bExtOk)
        {
            long int total_lines = 0;
            long int code_lines = 0;
            long int empty_lines = 0;
            long int comment_lines = 0;
            long int codecomments_lines = 0;
            CountLines(m_FileName, langCPP, code_lines, codecomments_lines, comment_lines, empty_lines, total_lines);
            XRCCTRL(*this, "staticTotalLines",   wxStaticText)->SetLabel(wxString::Format(_T("%ld"), total_lines));
            XRCCTRL(*this, "staticEmptyLines",   wxStaticText)->SetLabel(wxString::Format(_T("%ld"), empty_lines));
            XRCCTRL(*this, "staticActualLines",  wxStaticText)->SetLabel(wxString::Format(_T("%ld"), code_lines + codecomments_lines));
            XRCCTRL(*this, "staticCommentLines", wxStaticText)->SetLabel(wxString::Format(_T("%ld"), comment_lines));
            XRCCTRL(*this, "staticEmptyLines", wxStaticText)->GetContainingSizer()->Layout();
        }
        wxFile file(m_FileName.GetFullPath());
        if (file.IsOpened())
        {
            long length = static_cast<long>(file.Length());
            XRCCTRL(*this, "staticFileSize", wxStaticText)->SetLabel(wxString::Format(_("%ld Bytes"), length));
            XRCCTRL(*this, "staticFileSize", wxStaticText)->GetContainingSizer()->Layout();
            file.Close();
        }
        XRCCTRL(*this, "chkReadOnly", wxCheckBox)->SetValue(!m_FileName.IsFileWritable());
        wxDateTime modTime = m_FileName.GetModificationTime();
        XRCCTRL(*this, "staticDateTimeStamp", wxStaticText)->SetLabel(
            wxString::Format(_("%02hd/%02hd/%d %02hd:%02hd:%02hd"), modTime.GetDay(),
            modTime.GetMonth() + 1, modTime.GetYear(), modTime.GetHour(), // seems I have to add 1 for the month ?
            modTime.GetMinute(), modTime.GetSecond()));                   // because the return value of GetMonth() is an enum
    }
}

void ProjectFileOptionsDlg::FillCompilers()
{
    // fill compilers combo
    wxChoice* cmb = XRCCTRL(*this, "cmbBuildStageCompiler", wxChoice);
    cmb->Clear();
    for (unsigned int i = 0; i < CompilerFactory::GetCompilersCount(); ++i)
    {
		Compiler* compiler = CompilerFactory::GetCompiler(i);
		if (compiler)
			cmb->Append(compiler->GetName());
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
	if (compiler)
	{
		m_ProjectFile->customBuild[compiler->GetID()].useCustomBuildCommand = XRCCTRL(*this, "chkBuildStage", wxCheckBox)->GetValue();
		m_ProjectFile->customBuild[compiler->GetID()].buildCommand          = XRCCTRL(*this, "txtBuildStage", wxTextCtrl)->GetValue();
	}
}

bool ProjectFileOptionsDlg::ToggleFileReadOnly(bool setReadOnly)
{
#if (__WXMSW__)
    // Check for failure
    const int MS_MODE_MASK = 0x0000ffff; // low word
    int mask = setReadOnly ? _S_IREAD : ( _S_IREAD | _S_IWRITE );
    int res  = _chmod(m_FileNameStr.mb_str(wxConvUTF8), mask & MS_MODE_MASK);
    if (res != 0)
    {
        if      (errno == ENOENT)
            Manager::Get()->GetLogManager()->DebugLog(_T("Error calling chmod (ENOENT)."));
        else if (errno == EINVAL)
            Manager::Get()->GetLogManager()->DebugLog(_T("Error calling chmod (EINVAL)."));
        else if (errno == EBADF)
            Manager::Get()->GetLogManager()->DebugLog(_T("Error calling chmod (EBADF)."));
        else
            Manager::Get()->GetLogManager()->DebugLog(_T("Error calling chmod."));
        return false; // chmod error
    }
#else
    const int X_MODE_MASK = S_IRGRP | S_IROTH; // should be always the case
    int mask = setReadOnly ? S_IRUSR : ( S_IRUSR | S_IWUSR );
    int res  = chmod(m_FileNameStr.mb_str(wxConvUTF8), mask | X_MODE_MASK);
    if (res != 0)
    {
        Manager::Get()->GetLogManager()->DebugLog(F(_T("Error calling chmod : errno = %d."), errno));
        return false; // chmod error
    }

#endif

    return (   ( setReadOnly && !m_FileName.IsFileWritable())
            || (!setReadOnly &&  m_FileName.IsFileWritable()) );
}
