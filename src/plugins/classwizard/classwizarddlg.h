/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef CLASSWIZARDDLG_H
#define CLASSWIZARDDLG_H

#include <wx/arrstr.h>
#include "scrollingdialog.h"
#include <wx/string.h>

#include <vector>

class wxCommandEvent;
class wxUpdateUIEvent;

class ClassWizardDlg : public wxScrollingDialog
{
    public:
        // livecycle
         ClassWizardDlg(wxWindow* parent);
        ~ClassWizardDlg();

        // access
        const wxString& GetHeaderFilename()         const { return m_Header;         }
        bool  IsValidImplementationFilename()       const { return m_GenerateImplementation && m_Implementation != _T(""); }
        const wxString& GetImplementationFilename() const { return m_Implementation; }
        bool  AddPathToProject()                    const { return m_AddPathToProject; }
        wxString GetIncludeDir();

    private:
        struct MemberVar_impl { wxString Typ; wxString Var; wxString Get; wxString Set; };
        typedef struct MemberVar_impl MemberVar;
        typedef std::vector<MemberVar> MemberVarsArray;

        // events
        void OnUpdateUI(wxUpdateUIEvent& event);
        void OnNameChange(wxCommandEvent& event);
        void OnAncestorChange(wxCommandEvent& event);
        void OnAddMemberVar(wxCommandEvent& event);
        void OnRemoveMemberVar(wxCommandEvent& event);
        void OnCommonDirClick(wxCommandEvent& event);
        void OnLowerCaseClick(wxCommandEvent& event);
        void OnIncludeDirClick(wxCommandEvent& event);
        void OnImplDirClick(wxCommandEvent& event);
        void OnHeaderChange(wxCommandEvent& event);
        void OnOKClick(wxCommandEvent& event);
        void OnCancelClick(wxCommandEvent& event);
        void OnAddPathToProjectClick(wxCommandEvent& event);

        // methods
        bool DoHeader();
        bool DoImpl();

        void DoGuardBlock();
        void DoFileNames();
        void DoForceDirectory(const wxFileName & filename);
        wxString DoMemVarRepr(const wxString & typ, const wxString & var);

        // member variables
        wxString        m_Header;
        wxString        m_Implementation;

        wxString        m_Name;
        wxString        m_Arguments;
        wxArrayString   m_NameSpaces;

        bool            m_HasDestructor;
        bool            m_VirtualDestructor;
        bool            m_HasCopyCtor;
        bool            m_HasAssignmentOp;

        bool            m_Inherits;
        wxString        m_Ancestor;
        wxString        m_AncestorFilename;
        wxString        m_AncestorScope;

        MemberVarsArray m_MemberVars;

        bool            m_Documentation;

        bool            m_AddPathToProject;
        bool            m_UseRelativePath;

        bool            m_CommonDir;
        wxString        m_IncludeDir;
        wxString        m_ImplDir;

        bool            m_GuardBlock;
        wxString        m_GuardWord;

        bool            m_GenerateImplementation;
        wxString        m_HeaderInclude;

        wxString        m_TabStr;
        wxString        m_EolStr;

        DECLARE_EVENT_TABLE()
};

#endif // CLASSWIZARDDLG_H
