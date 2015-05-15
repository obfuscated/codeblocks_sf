/**************************************************************************//**
 * \file      ConfigPanel.h
 * \author    Gary Harris
 * \date      01-02-2010
 *
 * DoxyBlocks - doxygen integration for Code::Blocks. \n
 * Copyright (C) 2010 Gary Harris.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/
#ifndef CONFIGPANEL_H
#define CONFIGPANEL_H

//(*Headers(ConfigPanel)
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/notebook.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/radiobox.h>
//*)
#include <cbstyledtextctrl.h>
#include <configurationpanel.h>

#include "DoxyBlocks.h"

/*! \brief The configuration panel class.
*/
class ConfigPanel: public cbConfigurationPanel
{
    public:

        ConfigPanel(wxWindow* parent, DoxyBlocks *pOwner, wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
        virtual ~ConfigPanel();
        void Init();

        // Setters and getters.
        // Comments.
        /** Access RadioBoxBlockComments
         * \return The current value of RadioBoxBlockComments
         */
        wxInt8 GetBlockComment() { return RadioBoxBlockComments->GetSelection(); }
        /** Set RadioBoxBlockComments
         * \param val New value to set
         */
        void SetBlockComment(wxInt8 val) { RadioBoxBlockComments->SetSelection(val); }
        /** Access RadioBoxLineComments
         * \return The current value of RadioBoxLineComments
         */
        wxInt8 GetLineComment() { return RadioBoxLineComments->GetSelection(); }
        /** Set RadioBoxLineComments
         * \param val New value to set
         */
        void SetLineComment(wxInt8 val) { RadioBoxLineComments->SetSelection(val); }
        // == Doxyfile defaults. ==
        // Project.
        /** Access TextCtrlProjectNumber
         * \return The current value of TextCtrlProjectNumber
         */
        wxString GetProjectNumber() { return TextCtrlProjectNumber->GetValue(); }
        /** Set TextCtrlProjectNumber
         * \param val New value to set
         */
        void SetProjectNumber(wxString val) { TextCtrlProjectNumber->SetValue(val); }
        /** Access TextCtrlOutputDirectory
         * \return The current value of TextCtrlOutputDirectory
         */
        wxString GetOutputDirectory() { return TextCtrlOutputDirectory->GetValue(); }
        /** Set TextCtrlOutputDirectory
         * \param val New value to set
         */
        void SetOutputDirectory(wxString val) { TextCtrlOutputDirectory->SetValue(val); }
        /** Access ChoiceOutputLanguage
         * \return The current value of ChoiceOutputLanguage
         */
        wxString GetOutputLanguage() { return ChoiceOutputLanguage->GetStringSelection(); }
        /** Set ChoiceOutputLanguage
         * \param val New value to set
         */
        void SetOutputLanguage(wxString val) { ChoiceOutputLanguage->SetStringSelection(val); }
        /** Access CheckBoxUseAutoVersion
         * \return The current value of CheckBoxUseAutoVersion
         */
        bool GetUseAutoVersion() { return CheckBoxUseAutoVersion->GetValue(); }
        /** Set CheckBoxUseAutoVersion
         * \param val New value to set
         */
        void SetUseAutoVersion(bool val) { m_bUseAutoVersion = val; }
        // Build.
        /** Access CheckBoxExtractAll
         * \return The current value of CheckBoxExtractAll
         */
        bool GetExtractAll() { return CheckBoxExtractAll->GetValue(); }
        /** Set CheckBoxExtractAll
         * \param val New value to set
         */
        void SetExtractAll(bool val) { CheckBoxExtractAll->SetValue(val); }
        /** Access CheckBoxExtractPrivate
         * \return The current value of CheckBoxExtractPrivate
         */
        bool GetExtractPrivate() { return CheckBoxExtractPrivate->GetValue(); }
        /** Set CheckBoxExtractPrivate
         * \param val New value to set
         */
        void SetExtractPrivate(bool val) { CheckBoxExtractPrivate->SetValue(val); }
        /** Access CheckBoxExtractStatic
         * \return The current value of CheckBoxExtractStatic
         */
        bool GetExtractStatic() { return CheckBoxExtractStatic->GetValue(); }
        /** Set CheckBoxExtractStatic
         * \param val New value to set
         */
        void SetExtractStatic(bool val) { CheckBoxExtractStatic->SetValue(val); }
        // Warnings.
        /** Access CheckBoxWarnings
         * \return The current value of CheckBoxWarnings
         */
        bool GetWarnings() { return CheckBoxWarnings->GetValue(); }
        /** Set CheckBoxWarnings
         * \param val New value to set
         */
        void SetWarnings(bool val) { CheckBoxWarnings->SetValue(val); }
        /** Access CheckBoxWarnIfDocError
         * \return The current value of CheckBoxWarnIfDocError
         */
        bool GetWarnIfDocError() { return CheckBoxWarnIfDocError->GetValue(); }
        /** Set CheckBoxWarnIfDocError
         * \param val New value to set
         */
        void SetWarnIfDocError(bool val) { CheckBoxWarnIfDocError->SetValue(val); }
        /** Access CheckBoxWarnIfUndocumented
         * \return The current value of CheckBoxWarnIfUndocumented
         */
        bool GetWarnIfUndocumented() { return CheckBoxWarnIfUndocumented->GetValue(); }
        /** Set CheckBoxWarnIfUndocumented
         * \param val New value to set
         */
        void SetWarnIfUndocumented(bool val) { CheckBoxWarnIfUndocumented->SetValue(val); }
        /** Access CheckBoxWarnNoParamdoc
         * \return The current value of CheckBoxWarnNoParamdoc
         */
        bool GetWarnNoParamdoc() { return CheckBoxWarnNoParamdoc->GetValue(); }
        /** Set CheckBoxWarnNoParamdoc
         * \param val New value to set
         */
        void SetWarnNoParamdoc(bool val) { CheckBoxWarnNoParamdoc->SetValue(val); }
        //  Alphabetical Class Index.
        /** Access CheckBoxAlphabeticalIndex
         * \return The current value of CheckBoxAlphabeticalIndex
         */
        bool GetAlphabeticalIndex() { return CheckBoxAlphabeticalIndex->GetValue(); }
        /** Set CheckBoxAlphabeticalIndex
         * \param val New value to set
         */
        void SetAlphabeticalIndex(bool val) { CheckBoxAlphabeticalIndex->SetValue(val); }
        // Output.
        /** Access CheckBoxGenerateHTML
         * \return The current value of CheckBoxGenerateHTML
         */
        bool GetGenerateHTML() { return CheckBoxGenerateHTML->GetValue(); }
        /** Set CheckBoxGenerateHTML
         * \param val New value to set
         */
        void SetGenerateHTML(bool val) { CheckBoxGenerateHTML->SetValue(val); }
        /** Access CheckBoxGenerateHTMLHelp
         * \return The current value of CheckBoxGenerateHTMLHelp
         */
        bool GetGenerateHTMLHelp() { return CheckBoxGenerateHTMLHelp->GetValue(); }
        /** Set CheckBoxGenerateHTMLHelp
         * \param val New value to set
         */
        void SetGenerateHTMLHelp(bool val) { CheckBoxGenerateHTMLHelp->SetValue(val); }
        /** Access CheckBoxGenerateCHI
         * \return The current value of CheckBoxGenerateCHI
         */
        bool GetGenerateCHI() { return CheckBoxGenerateCHI->GetValue(); }
        /** Set CheckBoxGenerateCHI
         * \param val New value to set
         */
        void SetGenerateCHI(bool val) { CheckBoxGenerateCHI->SetValue(val); }
        /** Access CheckBoxBinaryTOC
         * \return The current value of CheckBoxBinaryTOC
         */
        bool GetBinaryTOC() { return CheckBoxBinaryTOC->GetValue(); }
        /** Set CheckBoxBinaryTOC
         * \param val New value to set
         */
        void SetBinaryTOC(bool val) { CheckBoxBinaryTOC->SetValue(val); }
        /** Access CheckBoxGenerateLatex;
         * \return The current value of CheckBoxGenerateLatex;
         */
        bool GetGenerateLatex() { return CheckBoxGenerateLatex->GetValue(); }
        /** Set CheckBoxGenerateLatex;
         * \param val New value to set
         */
        void SetGenerateLatex(bool val) { CheckBoxGenerateLatex->SetValue(val); }
        /** Access CheckBoxGenerateRTF
         * \return The current value of CheckBoxGenerateRTF
         */
        bool GetGenerateRTF() { return CheckBoxGenerateRTF->GetValue(); }
        /** Set CheckBoxGenerateRTF
         * \param val New value to set
         */
        void SetGenerateRTF(bool val) { CheckBoxGenerateRTF->SetValue(val); }
        /** Access CheckBoxGenerateMan
         * \return The current value of CheckBoxGenerateMan
         */
        bool GetGenerateMan() { return CheckBoxGenerateMan->GetValue(); }
        /** Set CheckBoxGenerateMan
         * \param val New value to set
         */
        void SetGenerateMan(bool val) { CheckBoxGenerateMan->SetValue(val); }
        /** Access CheckBoxGenerateXML
         * \return The current value of CheckBoxGenerateXML
         */
        bool GetGenerateXML() { return CheckBoxGenerateXML->GetValue(); }
        /** Set CheckBoxGenerateXML
         * \param val New value to set
         */
        void SetGenerateXML(bool val) { CheckBoxGenerateXML->SetValue(val); }
        /** Access CheckBoxGenerateAutogenDef
         * \return The current value of CheckBoxGenerateAutogenDef
         */
        bool GetGenerateAutogenDef() { return CheckBoxGenerateAutogenDef->GetValue(); }
        /** Set CheckBoxGenerateAutogenDef
         * \param val New value to set
         */
        void SetGenerateAutogenDef(bool val) { CheckBoxGenerateAutogenDef->SetValue(val); }
        /** Access CheckBoxGeneratePerlMod
         * \return The current value of CheckBoxGeneratePerlMod
         */
        bool GetGeneratePerlMod() { return CheckBoxGeneratePerlMod->GetValue(); }
        /** Set CheckBoxGeneratePerlMod
         * \param val New value to set
         */
        void SetGeneratePerlMod(bool val) { CheckBoxGeneratePerlMod->SetValue(val); }
        // Pre-processor.
        /** Access CheckBoxEnablePreprocessing
         * \return The current value of CheckBoxEnablePreprocessing
         */
        bool GetEnablePreprocessing() { return CheckBoxEnablePreprocessing->GetValue(); }
        /** Set CheckBoxEnablePreprocessing
         * \param val New value to set
         */
        void SetEnablePreprocessing(bool val) { CheckBoxEnablePreprocessing->SetValue(val); }
        // Dot.
        /** Access CheckBoxHaveDot
         * \return The current value of CheckBoxHaveDot
         */
        bool GetHaveDot() { return CheckBoxHaveDot->GetValue(); }
        /** Set CheckBoxHaveDot
         * \param val New value to set
         */
        void SetHaveDot(bool val) { CheckBoxHaveDot->SetValue(val); }
        /** Access CheckBoxClassDiagrams
         * \return The current value of CheckBoxClassDiagrams
         */
        bool GetClassDiagrams() { return CheckBoxClassDiagrams->GetValue(); }
        /** Set CheckBoxClassDiagrams
         * \param val New value to set
         */
        void SetClassDiagrams(bool val) { CheckBoxClassDiagrams->SetValue(val); }
        // Paths.
        /** Access TextCtrlPathDoxygen
         * \return The current value of TextCtrlPathDoxygen
         */
        wxString GetPathDoxygen() { return TextCtrlPathDoxygen->GetValue(); }
        /** Set TextCtrlPathDoxygen
         * \param val New value to set
         */
        void SetPathDoxygen(wxString val) { TextCtrlPathDoxygen->SetValue(val); }
        /** Access TextCtrlPathDoxywizard
         * \return The current value of TextCtrlPathDoxywizard
         */
        wxString GetPathDoxywizard() { return TextCtrlPathDoxywizard->GetValue(); }
        /** Set TextCtrlPathDoxywizard
         * \param val New value to set
         */
        void SetPathDoxywizard(wxString val) { TextCtrlPathDoxywizard->SetValue(val); }
        /** Access TextCtrlPathHHC
         * \return The current value of TextCtrlPathHHC
         */
        wxString GetPathHHC() { return TextCtrlPathHHC->GetValue(); }
        /** Set TextCtrlPathHHC
         * \param val New value to set
         */
        void SetPathHHC(wxString val) { TextCtrlPathHHC->SetValue(val); }
        /** Access TextCtrlPathDot
         * \return The current value of TextCtrlPathDot
         */
        wxString GetPathDot() { return TextCtrlPathDot->GetValue(); }
        /** Set TextCtrlPathDot
         * \param val New value to set
         */
        void SetPathDot(wxString val) { TextCtrlPathDot->SetValue(val); }
        /** Access TextCtrlPathCHMViewer
         * \return The current value of TextCtrlPathCHMViewer
         */
        wxString GetPathCHMViewer() { return TextCtrlPathCHMViewer->GetValue(); }
        /** Set TextCtrlPathCHMViewer
         * \param val New value to set
         */
        void SetPathCHMViewer(wxString val) { TextCtrlPathCHMViewer->SetValue(val); }
        // General Options
        /** Access CheckBoxOverwriteDoxyfile
         * \return The current value of CheckBoxOverwriteDoxyfile
         */
        bool GetOverwriteDoxyfile() { return CheckBoxOverwriteDoxyfile->GetValue(); }
        /** Set CheckBoxOverwriteDoxyfile
         * \param val New value to set
         */
        void SetOverwriteDoxyfile(bool val) { CheckBoxOverwriteDoxyfile->SetValue(val); }
        /** Access CheckBoxPromptBeforeOverwriting
         * \return The current value of CheckBoxPromptBeforeOverwriting
         */
        bool GetPromptBeforeOverwriting() { return CheckBoxPromptBeforeOverwriting->GetValue(); }
        /** Set CheckBoxPromptBeforeOverwriting
         * \param val New value to set
         */
        void SetPromptBeforeOverwriting(bool val) { CheckBoxPromptBeforeOverwriting->SetValue(val); }
        /** Access CheckBoxUseAtInTags
         * \return The current value of CheckBoxUseAtInTags
         */
        bool GetUseAtInTags() { return CheckBoxUseAtInTags->GetValue(); }
        /** Set CheckBoxUseAtInTags
         * \param val New value to set
         */
        void SetUseAtInTags(bool val) { CheckBoxUseAtInTags->SetValue(val); }
        /** Access CheckBoxLoadTemplate
         * \return The current value of CheckBoxLoadTemplate
         */
        bool GetLoadTemplate() { return CheckBoxLoadTemplate->GetValue(); }
        /** Set CheckBoxLoadTemplate
         * \param val New value to set
         */
        void SetLoadTemplate(bool val) { CheckBoxLoadTemplate->SetValue(val); }
        /** Access CheckBoxUseInternalViewer
         * \return The current value of CheckBoxUseInternalViewer
         */
        bool GetUseInternalViewer() { return CheckBoxUseInternalViewer->GetValue(); }
        /** Set CheckBoxUseInternalViewer
         * \param val New value to set
         */
        void SetUseInternalViewer(bool val) { CheckBoxUseInternalViewer->SetValue(val); }
        /** Access CheckBoxRunHTML
         * \return The current value of CheckBoxRunHTML
         */
        bool GetRunHTML() { return CheckBoxRunHTML->GetValue(); }
        /** Set CheckBoxRunHTML
         * \param val New value to set
         */
        void SetRunHTML(bool val) { CheckBoxRunHTML->SetValue(val); }
        /** Access CheckBoxRunCHM
         * \return The current value of CheckBoxRunCHM
         */
        bool GetRunCHM() { return CheckBoxRunCHM->GetValue(); }
        /** Set CheckBoxRunCHM
         * \param val New value to set
         */
        void SetRunCHM(bool val) { CheckBoxRunCHM->SetValue(val); }

        /** Set m_bAutoVersioning
         * \param val New value to set
         */
        void SetAutoVersioning(bool val) { m_bAutoVersioning = val; }

        //(*Declarations(ConfigPanel)
        wxCheckBox* CheckBoxRunCHM;
        wxButton* ButtonBrowseDoxygen;
        wxCheckBox* CheckBoxExtractStatic;
        cbStyledTextCtrl* TextCtrlBlockComment;
        wxCheckBox* CheckBoxWarnings;
        wxRadioBox* RadioBoxLineComments;
        wxTextCtrl* TextCtrlPathDoxygen;
        cbStyledTextCtrl* TextCtrlLineComment;
        wxCheckBox* CheckBoxGeneratePerlMod;
        wxCheckBox* CheckBoxEnablePreprocessing;
        wxCheckBox* CheckBoxUseInternalViewer;
        wxCheckBox* CheckBoxWarnIfUndocumented;
        wxRadioBox* RadioBoxBlockComments;
        wxCheckBox* CheckBoxHaveDot;
        wxCheckBox* CheckBoxOverwriteDoxyfile;
        wxCheckBox* CheckBoxWarnIfDocError;
        wxCheckBox* CheckBoxUseAtInTags;
        wxCheckBox* CheckBoxBinaryTOC;
        wxCheckBox* CheckBoxGenerateHTMLHelp;
        wxCheckBox* CheckBoxExtractAll;
        wxCheckBox* CheckBoxRunHTML;
        wxCheckBox* CheckBoxWarnNoParamdoc;
        wxTextCtrl* TextCtrlOutputDirectory;
        wxCheckBox* CheckBoxLoadTemplate;
        wxChoice* ChoiceOutputLanguage;
        wxCheckBox* CheckBoxClassDiagrams;
        wxCheckBox* CheckBoxGenerateMan;
        wxCheckBox* CheckBoxGenerateXML;
        wxCheckBox* CheckBoxPromptBeforeOverwriting;
        wxCheckBox* CheckBoxAlphabeticalIndex;
        wxCheckBox* CheckBoxGenerateLatex;
        wxCheckBox* CheckBoxExtractPrivate;
        wxTextCtrl* TextCtrlPathHHC;
        wxButton* ButtonBrowseDoxywizard;
        wxTextCtrl* TextCtrlProjectNumber;
        wxCheckBox* CheckBoxGenerateCHI;
        wxCheckBox* CheckBoxGenerateAutogenDef;
        wxButton* ButtonBrowseDot;
        wxTextCtrl* TextCtrlPathDot;
        wxButton* ButtonBrowseHHC;
        wxTextCtrl* TextCtrlPathDoxywizard;
        wxTextCtrl* TextCtrlPathCHMViewer;
        wxCheckBox* CheckBoxGenerateRTF;
        wxButton* ButtonBrowseCHMViewer;
        wxCheckBox* CheckBoxUseAutoVersion;
        wxCheckBox* CheckBoxGenerateHTML;
        //*)

    protected:

        //(*Identifiers(ConfigPanel)
        static const long ID_RADIOBOX_BLOCKCOMMENTS;
        static const long ID_TEXTCTRL_BLOCKCOMMENT;
        static const long ID_RADIOBOX_LINECOMMENTS;
        static const long ID_TEXTCTRL_LINECOMMENT;
        static const long ID_PANEL2;
        static const long ID_STATICTEXT1;
        static const long ID_TEXTCTRL_PROJECTNUMBER;
        static const long ID_CHECKBOX_USEAUTOVERSION;
        static const long ID_STATICTEXT8;
        static const long ID_TEXTCTRLOUTPUT_DIRECTORY;
        static const long ID_STATICTEXT5;
        static const long ID_CHOICE_OUTPUT_LANGUAGE;
        static const long ID_CHECKBOX_EXTRACT_ALL;
        static const long ID_CHECKBOX_EXTRACTPRIVATE;
        static const long ID_CHECKBOX_EXTRACTSTATIC;
        static const long ID_CHECKBOX_WARNINGS;
        static const long ID_CHECKBOX_WARN_IF_DOC_ERROR;
        static const long ID_CHECKBOX_WARN_IF_UNDOCUMENTED;
        static const long ID_CHECKBOX_WARN_NO_PARAMDOC;
        static const long ID_CHECKBOX_ALPHABETICAL_INDEX;
        static const long ID_PANEL3;
        static const long ID_CHECKBOX_GENERATE_HTML;
        static const long ID_CHECKBOX_GENERATE_HTMLHELP;
        static const long ID_CHECKBOX_GENERATE_CHI;
        static const long ID_CHECKBOX_BINARY_TOC;
        static const long ID_CHECKBOX_GENERATE_LATEX;
        static const long ID_CHECKBOX_GENERATE_RTF;
        static const long ID_CHECKBOX_GENERATE_MAN;
        static const long ID_CHECKBOX_GENERATE_XML;
        static const long ID_CHECKBOX_GENERATE_AUTOGEN_DEF;
        static const long ID_CHECKBOX_GENERATE_PERLMOD;
        static const long ID_CHECKBOX_ENABLE_PREPROCESSING;
        static const long ID_CHECKBOX_CLASS_DIAGRAMS;
        static const long ID_CHECKBOX_HAVE_DOT;
        static const long ID_PANEL4;
        static const long ID_STATICTEXT2;
        static const long ID_TEXTCTRL_PATHDOXYGEN;
        static const long ID_BUTTON_BROWSEDOXYGEN;
        static const long ID_STATICTEXT4;
        static const long ID_TEXTCTRL_PATHDOXYWIZARD;
        static const long ID_BUTTON_BROWSEDOXYWIZARD;
        static const long ID_STATICTEXT3;
        static const long ID_TEXTCTRL_PATHHHC;
        static const long ID_BUTTON_BROWSEHHC;
        static const long ID_STATICTEXT6;
        static const long ID_TEXTCTRL_PATHDOT;
        static const long ID_BUTTON_BROWSEDOT;
        static const long ID_STATICTEXT7;
        static const long ID_TEXTCTRL_PATHCHMVIEWER;
        static const long ID_BUTTON_BROWSECHMVIEWER;
        static const long ID_CHECKBOX_OVERWRITEDOXYFILE;
        static const long ID_CHECKBOX_PROMPTB4OVERWRITING;
        static const long ID_CHECKBOX_USEATINTAGS;
        static const long ID_CHECKBOX_LOADTEMPLATE;
        static const long ID_CHECKBOX_USEINTERNALVIEWER;
        static const long ID_CHECKBOX_RUNHTML;
        static const long ID_CHECKBOX_RUNCHM;
        static const long ID_PANEL1;
        static const long ID_NOTEBOOK_PREFS;
        //*)

    private:

        //(*Handlers(ConfigPanel)
        void OnRadioBoxBlockCommentsSelect(wxCommandEvent& event);
        void OnRadioBoxLineCommentsSelect(wxCommandEvent& event);
        void OnButtonBrowseDoxygenClick(wxCommandEvent& event);
        void OnButtonBrowseDoxywizardClick(wxCommandEvent& event);
        void OnButtonBrowseHHCClick(wxCommandEvent& event);
        void OnButtonBrowseDotClick(wxCommandEvent& event);
        void OnCheckBoxGenerateHTMLClick(wxCommandEvent& event);
        void OnCheckBoxOverwriteDoxyfileClick(wxCommandEvent& event);
        void OnCheckBoxWarningsClick(wxCommandEvent& event);
        void OnCheckBoxUseAutoversionClick(wxCommandEvent& event);
        void OnCheckBoxUseAtInTagsClick(wxCommandEvent& event);
        void OnButtonBrowseCHMViewerClick(wxCommandEvent& event);
        //*)

        // virtual routines required by cbConfigurationPanel
        /*! \brief Get the title to show in the settings image list and in the panel header.
         *
         * \return    wxString
         *
         * Virtual routine required by cbConfigurationPanel
         */
        wxString GetTitle() const { return _("DoxyBlocks"); }
        wxString GetBitmapBaseName() const;
        void OnApply();
        /*! \brief Cancel configuration changes.
         *
         * Virtual routine required by cbConfigurationPanel
         */
        void OnCancel(){}

        wxString GetApplicationPath();
        void InitSTC(cbStyledTextCtrl * stc);
        void WriteBlockComment(cbStyledTextCtrl *stc, int iBlockComment, bool bUseAtInTags);
        void WriteLineComment(cbStyledTextCtrl *stc, int iLineComment);

        // Pointer to owner of the configuration dialog needed to complete the OnApply/OnCancel EndModal() logic
        DoxyBlocks* m_pOwnerClass;    //!< The class that owns this dialogue. Used for calling OnDialogueDone().

        bool m_bAutoVersioning;       //!< Whether AutoVersioning is active for the current project.
        bool m_bUseAutoVersion;       //!< Whether to to use the AutoVersion-generated version string.


        DECLARE_EVENT_TABLE()

        // These comments show how to document the variables declared by wxSmith. They can't be documented in place because the will
        // be overwritten by the next change, so I use remote documentation. While these comments don't contribute a great deal they do prevent
        // the variables from remaining undocumented, show how to get documentation into the extracted docs and allow you to run doxygen with
        // all warnings active without receiving a warning for each one.
        /*! \var ButtonBrowseCHMViewer
         *    \brief A pointer to the CHM viewer Browse wxButton.
         */
        /*! \var ButtonBrowseDoxywizard
         *    \brief A pointer to the Doxywizard Browse wxButton.
         */
        /*! \var ButtonBrowseDot
         *    \brief A pointer to the Dot Browse wxButton.
         */
        /*! \var ButtonBrowseDoxygen
         *    \brief A pointer to the Doxygen Browse wxButton.
         */
        /*! \var ButtonBrowseHHC
         *    \brief A pointer to the HHC Browse wxButton.
         */
        /*! \var CheckBoxAlphabeticalIndex
         *    \brief A pointer to the Alphabetical Index wxCheckBox.
         */
        /*! \var CheckBoxBinaryTOC
         *    \brief A pointer to the Binary TOC wxCheckBox.
         */
        /*! \var CheckBoxClassDiagrams
         *    \brief A pointer to the Class Diagrams wxCheckBox.
         */
        /*! \var CheckBoxEnablePreprocessing
         *    \brief A pointer to the Enable Preprocessing wxCheckBox.
         */
        /*! \var CheckBoxExtractAll
         *    \brief A pointer to Extract All wxCheckBox.
         */
        /*! \var CheckBoxExtractPrivate
         *    \brief A pointer to the Extract Private wxCheckBox.
         */
        /*! \var CheckBoxExtractStatic
         *    \brief A pointer to the Extract Static wxCheckBox.
         */
        /*! \var CheckBoxGenerateAutogenDef
         *    \brief A pointer to the Generate Autogen Def wxCheckBox.
         */
        /*! \var CheckBoxGenerateCHI
         *    \brief A pointer to the Generate CHI wxCheckBox.
         */
        /*! \var CheckBoxGenerateHTML
         *    \brief A pointer to the Generate HTML wxCheckBox.
         */
        /*! \var CheckBoxGenerateHTMLHelp
         *    \brief A pointer to the Generate HTML Help wxCheckBox.
         */
        /*! \var CheckBoxGenerateLatex
         *    \brief A pointer to the Generate Latex wxCheckBox.
         */
        /*! \var CheckBoxGenerateMan
         *    \brief A pointer to the Generate Man wxCheckBox.
         */
        /*! \var CheckBoxGeneratePerlMod
         *    \brief A pointer to the Generate PerlMod wxCheckBox.
         */
        /*! \var CheckBoxGenerateRTF
         *    \brief A pointer to the Generate RTF wxCheckBox.
         */
        /*! \var CheckBoxGenerateXML
         *    \brief A pointer to the Generate XML wxCheckBox.
         */
        /*! \var CheckBoxHaveDot
         *    \brief A pointer to the Have Dot wxCheckBox.
         */
        /*! \var CheckBoxLoadTemplate
         *    \brief A pointer to the Load Template wxCheckBox.
         */
        /*! \var CheckBoxOverwriteDoxyfile
         *    \brief A pointer to the Overwrite Doxyfile wxCheckBox.
         */
        /*! \var CheckBoxPromptBeforeOverwriting
         *    \brief A pointer to the Prompt Before Overwriting wxCheckBox.
         */
        /*! \var CheckBoxRunCHM
         *    \brief A pointer to the Run CHM wxCheckBox.
         */
        /*! \var CheckBoxRunHTML
         *    \brief A pointer to the Run HTML wxCheckBox.
         */
        /*! \var CheckBoxUseAtInTags
         *    \brief A pointer to the Use At In Tags wxCheckBox.
         */
        /*! \var CheckBoxUseAutoVersion
         *    \brief A pointer to the Use Auto Version wxCheckBox.
         */
        /*! \var CheckBoxUseInternalViewer
         *    \brief A pointer to the Use Internal Viewer wxCheckBox.
         */
        /*! \var CheckBoxWarnIfDocError
         *    \brief A pointer to the Warn If Doc Error wxCheckBox.
         */
        /*! \var CheckBoxWarnIfUndocumented
         *    \brief A pointer to the Warn If Undocumented wxCheckBox.
         */
        /*! \var CheckBoxWarnings
         *    \brief A pointer to the Warnings wxCheckBox.
         */
        /*! \var CheckBoxWarnNoParamdoc
         *    \brief A pointer to the Warn No Paramdoc wxCheckBox.
         */
        /*! \var ChoiceOutputLanguage
         *    \brief A pointer to a wxChoice control listing the language options.
         */
        /*! \var RadioBoxBlockComments
         *    \brief A pointer to the Block Comment types wxRadioBox.
         */
        /*! \var RadioBoxLineComments
         *    \brief A pointer to the Line Comment types wxRadioBox.
         */
        /*! \var CheckBoxWarnNoParamdoc
         *    \brief A pointer to the Warn No Paramdoc wxCheckBox.
         */
        /*! \var TextCtrlBlockComment
         *    \brief A pointer to a cbStyledTextCtrl containing the sample block comment.
        */
        /*! \var TextCtrlLineComment
         *    \brief A pointer to a cbStyledTextCtrl containing the sample line comment.
        */
        /*! \var TextCtrlOutputDirectory
         *    \brief A pointer to the Output Directory wxTextCtrl.
         */
        /*! \var TextCtrlPathCHMViewer
         *    \brief A pointer to the CHM Viewer Path wxTextCtrl.
         */
        /*! \var TextCtrlPathDot
         *    \brief A pointer to the Dot Path wxTextCtrl.
         */
        /*! \var TextCtrlPathDoxygen
         *    \brief A pointer to the Doxygen Path wxTextCtrl.
         */
        /*! \var TextCtrlPathDoxywizard
         *    \brief A pointer to the Doxywizard Path wxTextCtrl.
         */
        /*! \var TextCtrlPathHHC
         *    \brief A pointer to the HHC Path wxTextCtrl.
         */
        /*! \var TextCtrlProjectNumber
         *    \brief A pointer to the Project Number wxTextCtrl.
         */
         // IDs
        /*! \var ID_BUTTON_BROWSECHMVIEWER
         *    \brief Control ID.
         */
        /*! \var ID_BUTTON_BROWSEDOT
         *    \brief Control ID.
         */
        /*! \var ID_BUTTON_BROWSEDOXYGEN
         *    \brief Control ID.
         */
        /*! \var ID_BUTTON_BROWSEDOXYWIZARD
         *    \brief Control ID.
         */
        /*! \var ID_BUTTON_BROWSEHHC
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_ALPHABETICAL_INDEX
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_BINARY_TOC
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_CLASS_DIAGRAMS
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_ENABLE_PREPROCESSING
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_EXTRACT_ALL
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_EXTRACTPRIVATE
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_EXTRACTSTATIC
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_GENERATE_AUTOGEN_DEF
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_GENERATE_CHI
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_GENERATE_HTML
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_GENERATE_HTMLHELP
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_GENERATE_LATEX
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_GENERATE_MAN
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_GENERATE_PERLMOD
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_GENERATE_RTF
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_GENERATE_XML
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_HAVE_DOT
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_LOADTEMPLATE
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_OVERWRITEDOXYFILE
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_PROMPTB4OVERWRITING
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_RUNCHM
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_RUNHTML
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_USEATINTAGS
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_USEAUTOVERSION
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_USEINTERNALVIEWER
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_WARN_IF_DOC_ERROR
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_WARN_IF_UNDOCUMENTED
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_WARN_NO_PARAMDOC
         *    \brief Control ID.
         */
        /*! \var ID_CHECKBOX_WARNINGS
         *    \brief Control ID.
         */
        /*! \var ID_CHOICE_OUTPUT_LANGUAGE
         *    \brief Control ID.
         */
        /*! \var ID_NOTEBOOK_PREFS
         *    \brief Control ID.
         */
        /*! \var ID_PANEL1
         *    \brief Control ID.
         */
        /*! \var ID_PANEL2
         *    \brief Control ID.
         */
        /*! \var ID_PANEL3
         *    \brief Control ID.
         */
        /*! \var ID_PANEL4
         *    \brief Control ID.
         */
        /*! \var ID_RADIOBOX_BLOCKCOMMENTS
         *    \brief Control ID.
         */
        /*! \var ID_RADIOBOX_LINECOMMENTS
         *    \brief Control ID.
         */
        /*! \var ID_STATICTEXT1
         *    \brief Control ID.
         */
        /*! \var ID_STATICTEXT2
         *    \brief Control ID.
         */
        /*! \var ID_STATICTEXT3
         *    \brief Control ID.
         */
        /*! \var ID_STATICTEXT4
         *    \brief Control ID.
         */
        /*! \var ID_STATICTEXT5
         *    \brief Control ID.
         */
        /*! \var ID_STATICTEXT6
         *    \brief Control ID.
         */
        /*! \var ID_STATICTEXT7
         *    \brief Control ID.
         */
        /*! \var ID_STATICTEXT8
         *    \brief Control ID.
         */
        /*! \var ID_TEXTCTRL_BLOCKCOMMENT
         *    \brief Control ID.
         */
        /*! \var ID_TEXTCTRL_LINECOMMENT
         *    \brief Control ID.
         */
        /*! \var ID_TEXTCTRL_PATHCHMVIEWER
         *    \brief Control ID.
         */
        /*! \var ID_TEXTCTRL_PATHDOT
         *    \brief Control ID.
         */
        /*! \var ID_TEXTCTRL_PATHDOXYGEN
         *    \brief Control ID.
         */
        /*! \var ID_TEXTCTRL_PATHDOXYWIZARD
         *    \brief Control ID.
         */
        /*! \var ID_TEXTCTRL_PATHHHC
         *    \brief Control ID.
         */
        /*! \var ID_TEXTCTRL_PROJECTNUMBER
         *    \brief Control ID.
         */
        /*! \var ID_TEXTCTRLOUTPUT_DIRECTORY
         *    \brief Control ID.
         */
};

#endif
