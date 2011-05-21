/**************************************************************************//**
 * \file      Config.h
 * \author    Gary Harris
 * \date      7/3/10
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
#ifndef CONFIG_H
#define CONFIG_H

#include <wx/string.h>

/*! \brief DoxyBlocks' configuration class.
 */
class DoxyBlocksConfig
{
    public:
        /** Default constructor */
        DoxyBlocksConfig();

        // Setters and getters.
        // Comments.
        /** Access m_iBlockComment
         * \return The current value of m_iBlockComment
         */
        wxInt8 GetBlockComment() const { return m_iBlockComment; }

        /** Set m_iBlockComment
         * \param val New value to set
         */
        void SetBlockComment(wxInt8 val) { m_iBlockComment = val; }

        /** Access m_iLineComment
         * \return The current value of m_iLineComment
         */
        wxInt8 GetLineComment() const { return m_iLineComment; }

        /** Set m_iLineComment
         * \param val New value to set
         */
        void SetLineComment(wxInt8 val) { m_iLineComment = val; }

        // == Doxyfile defaults. ==
        // Project.
        /** Access m_sProjectNumber
         * \return The current value of m_sProjectNumber
         */
        wxString GetProjectNumber() const { return m_sProjectNumber; }

        /** Set m_sProjectNumber
         * \param val New value to set
         */
        void SetProjectNumber(const wxString& val) { m_sProjectNumber = val; }

        /** Access m_sOutputDirectory
         * \return The current value of m_sOutputDirectory
         */
        wxString GetOutputDirectory() const { return m_sOutputDirectory; }

        /** Set m_sOutputDirectory
         * \param val New value to set
         */
        void SetOutputDirectory(const wxString& val) { m_sOutputDirectory = val; }

        /** Access m_sOutputLanguage
         * \return The current value of m_sOutputLanguage
         */
        wxString GetOutputLanguage() const { return m_sOutputLanguage; }

        /** Set m_sOutputLanguage
         * \param val New value to set
         */
        void SetOutputLanguage(const wxString& val) { m_sOutputLanguage = val; }

        /** Access m_bUseAutoVersion
         * \return The current value of m_bUseAutoVersion
         */
        bool GetUseAutoVersion() const { return m_bUseAutoVersion; }

        /** Set m_bUseAutoVersion
         * \param val New value to set
         */
        void SetUseAutoVersion(bool val) { m_bUseAutoVersion = val; }

        // Build.
        /** Access m_bExtractAll
         * \return The current value of m_bExtractAll
         */
        bool GetExtractAll() const { return m_bExtractAll; }

        /** Set m_bExtractAll
         * \param val New value to set
         */
        void SetExtractAll(bool val) { m_bExtractAll = val; }

        /** Access m_bExtractPrivate
         * \return The current value of m_bExtractPrivate
         */
        bool GetExtractPrivate() const { return m_bExtractPrivate; }

        /** Set m_bExtractPrivate
         * \param val New value to set
         */
        void SetExtractPrivate(bool val) { m_bExtractPrivate = val; }

        /** Access m_bExtractStatic
         * \return The current value of m_bExtractStatic
         */
        bool GetExtractStatic() const { return m_bExtractStatic; }

        /** Set m_bExtractStatic
         * \param val New value to set
         */
        void SetExtractStatic(bool val) { m_bExtractStatic = val; }

        // Warnings.
        /** Access m_bWarnings
         * \return The current value of m_bWarnings
         */
        bool GetWarnings() const { return m_bWarnings; }

        /** Set m_bWarnings
         * \param val New value to set
         */
        void SetWarnings(bool val) { m_bWarnings = val; }

        /** Access m_bWarnIfDocError
         * \return The current value of m_bWarnIfDocError
         */
        bool GetWarnIfDocError() const { return m_bWarnIfDocError; }

        /** Set m_bWarnIfDocError
         * \param val New value to set
         */
        void SetWarnIfDocError(bool val) { m_bWarnIfDocError = val; }

        /** Access m_bWarnIfUndocumented
         * \return The current value of m_bWarnIfUndocumented
         */
        bool GetWarnIfUndocumented() const { return m_bWarnIfUndocumented; }

        /** Set m_bWarnIfUndocumented
         * \param val New value to set
         */
        void SetWarnIfUndocumented(bool val) { m_bWarnIfUndocumented = val; }

        /** Access m_bWarnNoParamdoc
         * \return The current value of m_bWarnNoParamdoc
         */
        bool GetWarnNoParamdoc() const { return m_bWarnNoParamdoc; }

        /** Set m_bWarnNoParamdoc
         * \param val New value to set
         */
        void SetWarnNoParamdoc(bool val) { m_bWarnNoParamdoc = val; }

        //  Alphabetical Class Index.
        /** Access m_bAlphabeticalIndex
         * \return The current value of m_bAlphabeticalIndex
         */
        bool GetAlphabeticalIndex() const { return m_bAlphabeticalIndex; }

        /** Set m_bAlphabeticalIndex
         * \param val New value to set
         */
        void SetAlphabeticalIndex(bool val) { m_bAlphabeticalIndex = val; }

        // Output.
        /** Access m_bGenerateHTML
         * \return The current value of m_bGenerateHTML
         */
        bool GetGenerateHTML() const { return m_bGenerateHTML; }

        /** Set m_bGenerateHTML
         * \param val New value to set
         */
        void SetGenerateHTML(bool val) { m_bGenerateHTML = val; }

        /** Access m_bGenerateHTMLHelp
         * \return The current value of m_bGenerateHTMLHelp
         */
        bool GetGenerateHTMLHelp() const { return m_bGenerateHTMLHelp; }

        /** Set m_bGenerateHTMLHelp
         * \param val New value to set
         */
        void SetGenerateHTMLHelp(bool val) { m_bGenerateHTMLHelp = val; }

        /** Access m_bGenerateCHI
         * \return The current value of m_bGenerateCHI
         */
        bool GetGenerateCHI() const { return m_bGenerateCHI; }

        /** Set m_bGenerateCHI
         * \param val New value to set
         */
        void SetGenerateCHI(bool val) { m_bGenerateCHI = val; }

        /** Access m_bBinaryTOC
         * \return The current value of m_bBinaryTOC
         */
        bool GetBinaryTOC() const { return m_bBinaryTOC; }

        /** Set m_bBinaryTOC
         * \param val New value to set
         */
        void SetBinaryTOC(bool val) { m_bBinaryTOC = val; }

        /** Access m_bGenerateLatex;
         * \return The current value of m_bGenerateLatex;
         */
        bool GetGenerateLatex() const { return m_bGenerateLatex; }

        /** Set m_bGenerateLatex;
         * \param val New value to set
         */
        void SetGenerateLatex(bool val) { m_bGenerateLatex = val; }

        /** Access m_bGenerateRTF
         * \return The current value of m_bGenerateRTF
         */
        bool GetGenerateRTF() const { return m_bGenerateRTF; }

        /** Set m_bGenerateRTF
         * \param val New value to set
         */
        void SetGenerateRTF(bool val) { m_bGenerateRTF = val; }

        /** Access m_bGenerateMan
         * \return The current value of m_bGenerateMan
         */
        bool GetGenerateMan() const { return m_bGenerateMan; }

        /** Set m_bGenerateMan
         * \param val New value to set
         */
        void SetGenerateMan(bool val) { m_bGenerateMan = val; }

        /** Access m_bGenerateXML
         * \return The current value of m_bGenerateXML
         */
        bool GetGenerateXML() const { return m_bGenerateXML; }

        /** Set m_bGenerateXML
         * \param val New value to set
         */
        void SetGenerateXML(bool val) { m_bGenerateXML = val; }

        /** Access m_bGenerateAutogenDef
         * \return The current value of m_bGenerateAutogenDef
         */
        bool GetGenerateAutogenDef() const { return m_bGenerateAutogenDef; }

        /** Set m_bGenerateAutogenDef
         * \param val New value to set
         */
        void SetGenerateAutogenDef(bool val) { m_bGenerateAutogenDef = val; }

        /** Access m_bGeneratePerlMod
         * \return The current value of m_bGeneratePerlMod
         */
        bool GetGeneratePerlMod() const { return m_bGeneratePerlMod; }

        /** Set m_bGeneratePerlMod
         * \param val New value to set
         */
        void SetGeneratePerlMod(bool val) { m_bGeneratePerlMod = val; }

        // Pre-processor.
        /** Access m_bEnablePreprocessing
         * \return The current value of m_bEnablePreprocessing
         */
        bool GetEnablePreprocessing() const { return m_bEnablePreprocessing; }

        /** Set m_bEnablePreprocessing
         * \param val New value to set
         */
        void SetEnablePreprocessing(bool val) { m_bEnablePreprocessing = val; }

        // Dot.
        /** Access m_bClassDiagrams
         * \return The current value of m_bClassDiagrams
         */
        bool GetClassDiagrams() const { return m_bClassDiagrams; }

        /** Set m_bClassDiagrams
         * \param val New value to set
         */
        void SetClassDiagrams(bool val) { m_bClassDiagrams = val; }

        /** Access m_bHaveDot
         * \return The current value of m_bHaveDot
         */
        bool GetHaveDot() const { return m_bHaveDot; }

        /** Set m_bHaveDot
         * \param val New value to set
         */
        void SetHaveDot(bool val) { m_bHaveDot = val; }

        // Paths.
        /** Access m_sPathDoxygen
         * \return The current value of m_sPathDoxygen
         */
        wxString GetPathDoxygen() const { return m_sPathDoxygen; }

        /** Set m_sPathDoxygen
         * \param val New value to set
         */
        void SetPathDoxygen(const wxString& val) { m_sPathDoxygen = val; }

        /** Access m_sPathDoxywizard
         * \return The current value of m_sPathDoxywizard
         */
        wxString GetPathDoxywizard() const { return m_sPathDoxywizard; }

        /** Set m_sPathDoxywizard
         * \param val New value to set
         */
        void SetPathDoxywizard(const wxString& val) { m_sPathDoxywizard = val; }

        /** Access m_sPathHHC
         * \return The current value of m_sPathHHC
         */
        wxString GetPathHHC() const { return m_sPathHHC; }

        /** Set m_sPathHHC
         * \param val New value to set
         */
        void SetPathHHC(const wxString& val) { m_sPathHHC = val; }

        /** Access m_sPathDot
         * \return The current value of m_sPathDot
         */
        wxString GetPathDot() const { return m_sPathDot; }

        /** Set m_sPathDot
         * \param val New value to set
         */
        void SetPathDot(const wxString& val) { m_sPathDot = val; }

        /** Access m_sPathCHMViewer
         * \return The current value of m_sPathCHMViewer
         */
        wxString GetPathCHMViewer() const { return m_sPathCHMViewer; }

        /** Set m_sPathCHMViewer
         * \param val New value to set
         */
        void SetPathCHMViewer(const wxString& val) { m_sPathCHMViewer = val; }

        // General Options
        /** Access m_bOverwriteDoxyfile
         * \return The current value of m_bOverwriteDoxyfile
         */
        bool GetOverwriteDoxyfile() const { return m_bOverwriteDoxyfile; }

        /** Set m_bOverwriteDoxyfile
         * \param val New value to set
         */
        void SetOverwriteDoxyfile(bool val) { m_bOverwriteDoxyfile = val; }

        /** Access m_bPromptBeforeOverwriting
         * \return The current value of m_bPromptBeforeOverwriting
         */
        bool GetPromptBeforeOverwriting() const { return m_bPromptBeforeOverwriting; }

        /** Set m_bPromptBeforeOverwriting
         * \param val New value to set
         */
        void SetPromptBeforeOverwriting(bool val) { m_bPromptBeforeOverwriting = val; }

        /** Access m_bUseAtInTags
         * \return The current value of m_bUseAtInTags
         */
        bool GetUseAtInTags() const { return m_bUseAtInTags; }

        /** Set m_bUseAtInTags
         * \param val New value to set
         */
        void SetUseAtInTags(bool val) { m_bUseAtInTags = val; }

        /** Access m_bUseInternalViewer
         * \return The current value of m_bUseInternalViewer
         */
        bool GetUseInternalViewer() const { return m_bUseInternalViewer; }

        /** Set m_bLoadTemplate
         * \param val New value to set
         */
        void SetLoadTemplate(bool val) { m_bLoadTemplate = val; }

        /** Access m_bLoadTemplate
         * \return The current value of m_bLoadTemplate
         */
        bool GetLoadTemplate() const { return m_bLoadTemplate; }

        /** Set m_bUseInternalViewer
         * \param val New value to set
         */
        void SetUseInternalViewer(bool val) { m_bUseInternalViewer = val; }

        /** Access m_bRunHTML
         * \return The current value of m_bRunHTML
         */
        bool GetRunHTML() const { return m_bRunHTML; }

        /** Set m_bRunHTML
         * \param val New value to set
         */
        void SetRunHTML(bool val) { m_bRunHTML = val; }

        /** Access m_bRunCHM
         * \return The current value of m_bRunCHM
         */
        bool GetRunCHM() const { return m_bRunCHM; }

        /** Set m_bRunCHM
         * \param val New value to set
         */
        void SetRunCHM(bool val) { m_bRunCHM = val; }

        // Config. template.
        bool ReadPrefsTemplate();
        bool WritePrefsTemplate();

    private:
        // Comment style.
        wxInt8       m_iBlockComment;               //!< Block comment type.
        wxInt8       m_iLineComment;                //!< Line comment type.
        // Doxyfile defaults.
        wxString     m_sProjectNumber;              //!< Project Number.
        wxString     m_sOutputDirectory;            //!< Output Directory.
        wxString     m_sOutputLanguage;             //!< Output Language.
        bool         m_bUseAutoVersion;             //!< Use AutoVersion version number.
        bool         m_bExtractAll;                 //!< Extract All.
        bool         m_bExtractPrivate;             //!< Extract Private.
        bool         m_bExtractStatic;              //!< Extract Static.
        bool         m_bWarnings;                   //!< Warnings.
        bool         m_bWarnIfDocError;             //!< Warn If Doc Error.
        bool         m_bWarnIfUndocumented;         //!< Warn If Undocumented.
        bool         m_bWarnNoParamdoc;             //!< Warn No Paramdoc.
        bool         m_bAlphabeticalIndex;          //!< Alphabetical Index.
        bool         m_bGenerateHTML;               //!< Generate HTML.
        bool         m_bGenerateHTMLHelp;           //!< Generate HTML Help.
        bool         m_bGenerateCHI;                //!< Generate CHI.
        bool         m_bBinaryTOC;                  //!< Generate Binary TOC.
        bool         m_bGenerateLatex;              //!< Generate Latex.
        bool         m_bGenerateRTF;                //!< Generate RTF.
        bool         m_bGenerateMan;                //!< Generate Man.
        bool         m_bGenerateXML;                //!< Generate XML.
        bool         m_bGenerateAutogenDef;         //!< Generate Autogen Def.
        bool         m_bGeneratePerlMod;            //!< Generate Perl Mod.
        bool         m_bEnablePreprocessing;        //!< Enable Preprocessing.
        bool         m_bClassDiagrams;              //!< Class Diagrams.
        bool         m_bHaveDot;                    //!< Have Dot.
        // General.
        wxString     m_sPathDoxygen;                //!< Path to Doxygen.
        wxString     m_sPathDoxywizard;             //!< Path to Doxywizard.
        wxString     m_sPathHHC;                    //!< Path to HHC.
        wxString     m_sPathDot;                    //!< Path to Dot.
        wxString     m_sPathCHMViewer;              //!< Path to CHM viewer.
        bool         m_bOverwriteDoxyfile;          //!< Overwrite the doxyfile.
        bool         m_bPromptBeforeOverwriting;    //!< Prompt before overwriting the doxyfile.
        bool         m_bUseAtInTags;                //!< Use @ in tags.
        bool         m_bLoadTemplate;               //!< Load settings template instead of using default settings.
        bool         m_bUseInternalViewer;          //!< Use internal viewer to display HTML.
        bool         m_bRunHTML;                    //!< Run HTML after compilation.
        bool         m_bRunCHM;                     //!< Run CHM after compilation.

        // Config. defaults.
        //-------------------------
        // Comment style.
        /** Get the default value for m_iBlockComment.
         * \return The default value.
         */
        int GetDefaultBlockComment() const { return 0; }

        /** Get the default value for m_iLineComment.
         * \return The default value.
         */
        int GetDefaultLineComment() const { return 0; }

        // Doxyfile defaults.
        /** Get the default value for m_sProjectNumber.
         * \return The default value.
         */
        wxString GetDefaultProjectNumber() const { return wxEmptyString; }

        /** Get the default value for m_bUseAutoVersion.
         * \return The default value.
         */
        bool GetDefaultUseAutoVersion() const { return false; }

        /** Get the default value for m_sOutputDirectory.
         * \return The default value.
         */
        wxString GetDefaultOutputDirectory() const { return wxEmptyString; }

        /** Get the default value for m_sOutputLanguage.
         * \return The default value.
         */
        wxString GetDefaultOutputLanguage() const { return wxT("English"); }

        /** Get the default value for m_bExtractAll.
         * \return The default value.
         */
        bool GetDefaultExtractAll() const { return false; }

        /** Get the default value for m_bExtractPrivate.
         * \return The default value.
         */
        bool GetDefaultExtractPrivate() const { return false; }

        /** Get the default value for m_bExtractStatic.
         * \return The default value.
         */
        bool GetDefaultExtractStatic() const { return false; }

        /** Get the default value for m_bWarnings.
         * \return The default value.
         */
        bool GetDefaultWarnings() const { return true; }

        /** Get the default value for m_bWarnIfDocError.
         * \return The default value.
         */
        bool GetDefaultWarnIfDocError() const { return true; }

        /** Get the default value for m_bWarnIfUndocumented.
         * \return The default value.
         */
        bool GetDefaultWarnIfUndocumented() const { return false; }

        /** Get the default value for m_bWarnNoParamdoc.
         * \return The default value.
         */
        bool GetDefaultWarnNoParamdoc() const { return true; }

        /** Get the default value for m_bAlphabeticalIndex.
         * \return The default value.
         */
        bool GetDefaultAlphabeticalIndex() const { return true; }

        /** Get the default value for m_bGenerateHTML.
         * \return The default value.
         */
        bool GetDefaultGenerateHTML() const { return true; }

        /** Get the default value for m_bGenerateHTMLHelp.
         * \return The default value.
         */
        bool GetDefaultGenerateHTMLHelp() const { return false; }

        /** Get the default value for m_bGenerateCHI.
         * \return The default value.
         */
        bool GetDefaultGenerateCHI() const { return false; }

        /** Get the default value for m_bBinaryTOC.
         * \return The default value.
         */
        bool GetDefaultBinaryTOC() const { return false; }

        /** Get the default value for m_bGenerateLatex.
         * \return The default value.
         */
        bool GetDefaultGenerateLatex() const { return false; }

        /** Get the default value for m_bGenerateRTF.
         * \return The default value.
         */
        bool GetDefaultGenerateRTF() const { return false; }

        /** Get the default value for m_bGenerateMan.
         * \return The default value.
         */
        bool GetDefaultGenerateMan() const { return false; }

        /** Get the default value for m_bGenerateXML.
         * \return The default value.
         */
        bool GetDefaultGenerateXML() const { return false; }

        /** Get the default value for m_bGenerateAutogenDef.
         * \return The default value.
         */
        bool GetDefaultGenerateAutogenDef() const { return false; }

        /** Get the default value for m_bGeneratePerlMod.
         * \return The default value.
         */
        bool GetDefaultGeneratePerlMod() const { return false; }

        /** Get the default value for m_bEnablePreprocessing.
         * \return The default value.
         */
        bool GetDefaultEnablePreprocessing() const { return true; }

        /** Get the default value for m_bClassDiagrams.
         * \return The default value.
         */
        bool GetDefaultClassDiagrams() const { return false; }

        /** Get the default value for m_bHaveDot.
         * \return The default value.
         */
        bool GetDefaultHaveDot() const { return false; }

        // General.
        /** Get the default value for m_sPathDoxygen.
         * \return The default value.
         */
        wxString GetDefaultPathDoxygen() const { return wxEmptyString; }

        /** Get the default value for m_sPathDoxywizard.
         * \return The default value.
         */
        wxString GetDefaultPathDoxywizard() const { return wxEmptyString; }

        /** Get the default value for m_sPathHHC.
         * \return The default value.
         */
        wxString GetDefaultPathHHC() const { return wxEmptyString; }

        /** Get the default value for m_sPathDot.
         * \return The default value.
         */
        wxString GetDefaultPathDot() const { return wxEmptyString; }

        /** Get the default value for m_sPathCHMViewer.
         * \return The default value.
         */
        wxString GetDefaultPathCHMViewer() const { return wxEmptyString; }

        /** Get the default value for m_bOverwriteDoxyfile.
         * \return The default value.
         */
        bool GetDefaultOverwriteDoxyfile() const { return false; }

        /** Get the default value for m_bPromptBeforeOverwriting.
         * \return The default value.
         */
        bool GetDefaultPromptBeforeOverwriting() const { return false; }

        /** Get the default value for m_bUseAtInTags.
         * \return The default value.
         */
        bool GetDefaultUseAtInTags() const { return false; }

        /** Get the default value for m_bLoadTemplate.
         * \return The default value.
         */
        bool GetDefaultLoadTemplate() const { return false; }

        /** Get the default value for m_bUseInternalViewer.
         * \return The default value.
         */
        bool GetDefaultUseInternalViewer() const { return false; }

        /** Get the default value for m_bRunHTML.
         * \return The default value.
         */
        bool GetDefaultRunHTML() const { return false; }

        /** Get the default value for m_bRunCHM.
         * \return The default value.
         */
        bool GetDefaultRunCHM() const { return false; }
    public:
        // Config. template.
        wxString GetCBConfigDir();
};

#endif // CONFIG_H
