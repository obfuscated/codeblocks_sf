/**************************************************************************//**
 * \file      Config.cpp
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

#include "Config.h"

#include "sdk.h"
#ifndef CB_PRECOMP
    #include <wx/filename.h>

    #include <configmanager.h>
    #include <manager.h>
    #include <personalitymanager.h>
#endif //CB_PRECOMP
#include <wx/fileconf.h>

/*! \brief Constructor.
 */
DoxyBlocksConfig::DoxyBlocksConfig()
{
    //Set default values.
    // Comment style.
    m_iBlockComment = GetDefaultBlockComment();
    m_iLineComment = GetDefaultLineComment();
    // Doxyfile defaults.
     m_sProjectNumber = GetDefaultProjectNumber();
    m_bUseAutoVersion = GetDefaultUseAutoVersion();
     m_sOutputDirectory = GetDefaultOutputDirectory();
     m_sOutputLanguage = GetDefaultOutputLanguage();
    m_bExtractAll = GetDefaultExtractAll();
    m_bExtractPrivate = GetDefaultExtractPrivate();
    m_bExtractStatic = GetDefaultExtractStatic();
    m_bWarnings = GetDefaultWarnings();
    m_bWarnIfDocError = GetDefaultWarnIfDocError();
    m_bWarnIfUndocumented = GetDefaultWarnIfUndocumented();
    m_bWarnNoParamdoc = GetDefaultWarnNoParamdoc();
    m_bAlphabeticalIndex = GetDefaultAlphabeticalIndex();
    m_bGenerateHTML = GetDefaultGenerateHTML();
    m_bGenerateHTMLHelp = GetDefaultGenerateHTMLHelp();
    m_bGenerateCHI = GetDefaultGenerateCHI();
    m_bBinaryTOC = GetDefaultBinaryTOC();
    m_bGenerateLatex = GetDefaultGenerateLatex();
    m_bGenerateRTF = GetDefaultGenerateRTF();
    m_bGenerateMan = GetDefaultGenerateMan();
    m_bGenerateXML = GetDefaultGenerateXML();
    m_bGenerateAutogenDef = GetDefaultGenerateAutogenDef();
    m_bGeneratePerlMod = GetDefaultGeneratePerlMod();
    m_bEnablePreprocessing = GetDefaultEnablePreprocessing();
    m_bClassDiagrams = GetDefaultClassDiagrams();
    m_bHaveDot = GetDefaultHaveDot();
    // General.
     m_sPathDoxygen = GetDefaultPathDoxygen();
     m_sPathDoxywizard = GetDefaultPathDoxywizard();
     m_sPathHHC = GetDefaultPathHHC();
     m_sPathDot = GetDefaultPathDot();
     m_sPathCHMViewer = GetDefaultPathCHMViewer();
    m_bOverwriteDoxyfile = GetDefaultOverwriteDoxyfile();
    m_bPromptBeforeOverwriting = GetDefaultPromptBeforeOverwriting();
    m_bUseAtInTags = GetDefaultUseAtInTags();
    m_bLoadTemplate = GetDefaultLoadTemplate();
    m_bUseInternalViewer = GetDefaultUseInternalViewer();
    m_bRunHTML = GetDefaultRunHTML();
    m_bRunCHM = GetDefaultRunCHM();
}

/*! \brief Write the settings template.
 *
 * \return bool    True if the data is flushed to the file successfully, false otherwise.
 *
 * This function looks for a settings template file in the global configuration directory for Code::Blocks.
 * If not found, it creates a new file. It then writes the current values of the data member variables to the file.
 * Since default values are supplied when this class is instantiated, the data members should always
 * contain valid values.
 */
bool DoxyBlocksConfig::WritePrefsTemplate()
{
    // Sections.
    wxString sSectionCommentStyle(wxT("/CommentStyle/"));
    wxString sSectionProject(wxT("/Project/"));
    wxString sSectionBuild(wxT("/Build/"));
    wxString sSectionWarnings(wxT("/Warnings/"));
    wxString sSectionAlphabeticalClassIndex(wxT("/AlphabeticalClassIndex/"));
    wxString sSectionOutput(wxT("/Output/"));
    wxString sSectionPreProcessor(wxT("/PreProcessor/"));
    wxString sSectionDot(wxT("/Dot/"));
    wxString sSectionGeneral(wxT("/General/"));
    // Prefs.
    wxString sCommentBlock(wxT("Block"));
    wxString sCommentLine(wxT("Line"));
    wxString sUseAutoVersion(wxT("UseAutoVersion"));
    wxString sOutputLanguage(wxT("OutputLanguage"));
    wxString sExtractAll(wxT("ExtractAll"));
    wxString sExtractPrivate(wxT("ExtractPrivate"));
    wxString sExtractStatic(wxT("ExtractStatic"));
    wxString sWarnings(wxT("Warnings"));
    wxString sWarnIfDocError(wxT("WarnIfDocError"));
    wxString sWarnIfUndocumented(wxT("WarnIfUndocumented"));
    wxString sWarnNoParamDoc(wxT("WarnNoParamDoc"));
    wxString sAlphabeticalIndex(wxT("AlphabeticalIndex"));
    wxString sGenerateHTML(wxT("GenerateHTML"));
    wxString sGenerateHTMLHelp(wxT("GenerateHTMLHelp"));
    wxString sGenerateCHI(wxT("GenerateCHI"));
    wxString sBinaryTOC(wxT("BinaryTOC"));
    wxString sGenerateLatex(wxT("GenerateLatex"));
    wxString sGenerateRTF(wxT("GenerateRTF"));
    wxString sGenerateMan(wxT("GenerateMan"));
    wxString sGenerateXML(wxT("GenerateXML"));
    wxString sGenerateAutogenDef(wxT("GenerateAutogenDef"));
    wxString sGeneratePerlMod(wxT("GeneratePerlMod"));
    wxString sEnablePreProcessing(wxT("EnablePreProcessing"));
    wxString sClassDiagrams(wxT("ClassDiagrams"));
    wxString sHaveDot(wxT("HaveDot"));
    wxString sUseAtInTags(wxT("UseAtInTags"));

    wxString sCfgIni(wxT("DoxyBlocks.ini"));
    wxString sCfgPath(Manager::Get()->GetConfigManager(_T("app"))->GetConfigFolder() + wxFILE_SEP_PATH + sCfgIni);

    wxFileConfig *cfgFile = new wxFileConfig(wxEmptyString, wxEmptyString, sCfgPath, wxEmptyString, wxCONFIG_USE_GLOBAL_FILE);

    // Comment style.
    cfgFile->Write(sSectionCommentStyle + sCommentBlock, static_cast<int>(m_iBlockComment));
    cfgFile->Write(sSectionCommentStyle + sCommentLine, static_cast<int>(m_iLineComment));
    // Project.
    cfgFile->Write(sSectionProject + sUseAutoVersion, static_cast<int>(m_bUseAutoVersion));
    cfgFile->Write(sSectionProject + sOutputLanguage, m_sOutputLanguage);
    // Build.
    cfgFile->Write(sSectionBuild + sExtractAll, static_cast<int>(m_bExtractAll));
    cfgFile->Write(sSectionBuild + sExtractPrivate, static_cast<int>(m_bExtractPrivate));
    cfgFile->Write(sSectionBuild + sExtractStatic, static_cast<int>(m_bExtractStatic));
    // Warnings.
    cfgFile->Write(sSectionWarnings + sWarnings, static_cast<int>(m_bWarnings));
    cfgFile->Write(sSectionWarnings + sWarnIfDocError, static_cast<int>(m_bWarnIfDocError));
    cfgFile->Write(sSectionWarnings + sWarnIfUndocumented, static_cast<int>(m_bWarnIfUndocumented));
    cfgFile->Write(sSectionWarnings + sWarnNoParamDoc, static_cast<int>(m_bWarnNoParamdoc));
    // Alphabetical class index.
    cfgFile->Write(sSectionAlphabeticalClassIndex + sAlphabeticalIndex, static_cast<int>(m_bAlphabeticalIndex));
    // Output.
    cfgFile->Write(sSectionOutput + sGenerateHTML, static_cast<int>(m_bGenerateHTML));
    cfgFile->Write(sSectionOutput + sGenerateHTMLHelp, static_cast<int>(m_bGenerateHTMLHelp));
    cfgFile->Write(sSectionOutput + sGenerateCHI, static_cast<int>(m_bGenerateCHI));
    cfgFile->Write(sSectionOutput + sBinaryTOC, static_cast<int>(m_bBinaryTOC));
    cfgFile->Write(sSectionOutput + sGenerateLatex, static_cast<int>(m_bGenerateLatex));
    cfgFile->Write(sSectionOutput + sGenerateRTF, static_cast<int>(m_bGenerateRTF));
    cfgFile->Write(sSectionOutput + sGenerateMan, static_cast<int>(m_bGenerateMan));
    cfgFile->Write(sSectionOutput + sGenerateXML, static_cast<int>(m_bGenerateXML));
    cfgFile->Write(sSectionOutput + sGenerateAutogenDef, static_cast<int>(m_bGenerateAutogenDef));
    cfgFile->Write(sSectionOutput + sGeneratePerlMod, static_cast<int>(m_bGeneratePerlMod));
    // Pre-processor.
    cfgFile->Write(sSectionPreProcessor + sEnablePreProcessing, static_cast<int>(m_bEnablePreprocessing));
    // Dot.
    cfgFile->Write(sSectionDot + sClassDiagrams, static_cast<int>(m_bClassDiagrams));
    cfgFile->Write(sSectionDot + sHaveDot, static_cast<int>(m_bHaveDot));
    // General.
    cfgFile->Write(sSectionGeneral + sUseAtInTags, static_cast<int>(m_bUseAtInTags));

    bool bOK = cfgFile->Flush();

    wxDELETE(cfgFile);
    wxASSERT(!cfgFile);
    return bOK;
}

/*! \brief Read the settings template.
 *
 * \return bool    True if a template file was found, false otherwise.
 *
 * This function looks for a settings template file in the global configuration directory for Code::Blocks
 * and reads the data into the correct member variables. Default values are supplied for entries that
 * don't contain values, although this should never happen unless the file is manually edited.
 */
bool DoxyBlocksConfig::ReadPrefsTemplate()
{
    // Sections.
    wxString sSectionCommentStyle(wxT("/CommentStyle/"));
    wxString sSectionProject(wxT("/Project/"));
    wxString sSectionBuild(wxT("/Build/"));
    wxString sSectionWarnings(wxT("/Warnings/"));
    wxString sSectionAlphabeticalClassIndex(wxT("/AlphabeticalClassIndex/"));
    wxString sSectionOutput(wxT("/Output/"));
    wxString sSectionPreProcessor(wxT("/PreProcessor/"));
    wxString sSectionDot(wxT("/Dot/"));
    wxString sSectionGeneral(wxT("/General/"));
    // Prefs.
    wxString sCommentBlock(wxT("Block"));
    wxString sCommentLine(wxT("Line"));
    wxString sUseAutoVersion(wxT("UseAutoVersion"));
    wxString sOutputLanguage(wxT("OutputLanguage"));
    wxString sExtractAll(wxT("ExtractAll"));
    wxString sExtractPrivate(wxT("ExtractPrivate"));
    wxString sExtractStatic(wxT("ExtractStatic"));
    wxString sWarnings(wxT("Warnings"));
    wxString sWarnIfDocError(wxT("WarnIfDocError"));
    wxString sWarnIfUndocumented(wxT("WarnIfUndocumented"));
    wxString sWarnNoParamDoc(wxT("WarnNoParamDoc"));
    wxString sAlphabeticalIndex(wxT("AlphabeticalIndex"));
    wxString sGenerateHTML(wxT("GenerateHTML"));
    wxString sGenerateHTMLHelp(wxT("GenerateHTMLHelp"));
    wxString sGenerateCHI(wxT("GenerateCHI"));
    wxString sBinaryTOC(wxT("BinaryTOC"));
    wxString sGenerateLatex(wxT("GenerateLatex"));
    wxString sGenerateRTF(wxT("GenerateRTF"));
    wxString sGenerateMan(wxT("GenerateMan"));
    wxString sGenerateXML(wxT("GenerateXML"));
    wxString sGenerateAutogenDef(wxT("GenerateAutogenDef"));
    wxString sGeneratePerlMod(wxT("GeneratePerlMod"));
    wxString sEnablePreProcessing(wxT("EnablePreProcessing"));
    wxString sClassDiagrams(wxT("ClassDiagrams"));
    wxString sHaveDot(wxT("HaveDot"));
    wxString sUseAtInTags(wxT("UseAtInTags"));

    wxString sCfgIni(wxT("DoxyBlocks.ini"));
    wxString sCfgPath(Manager::Get()->GetConfigManager(_T("app"))->GetConfigFolder() + wxFILE_SEP_PATH + sCfgIni);
    if(!wxFile::Exists(sCfgPath)){
        return false;
    }

    wxFileConfig *cfgFile = new wxFileConfig(wxEmptyString, wxEmptyString, sCfgPath, wxEmptyString, wxCONFIG_USE_GLOBAL_FILE);

    // Comment style.
    m_iBlockComment = cfgFile->Read(sSectionCommentStyle + sCommentBlock, GetDefaultBlockComment());
    m_iLineComment = cfgFile->Read(sSectionCommentStyle + sCommentLine, GetDefaultLineComment());
    // Project.
    m_bUseAutoVersion = cfgFile->Read(sSectionProject + sUseAutoVersion, GetDefaultUseAutoVersion());
    m_sOutputLanguage = cfgFile->Read(sSectionProject + sOutputLanguage, GetDefaultOutputLanguage());
    // Build.
    m_bExtractAll = cfgFile->Read(sSectionBuild + sExtractAll, GetDefaultExtractAll());
    m_bExtractPrivate = cfgFile->Read(sSectionBuild + sExtractPrivate, GetDefaultExtractPrivate());
    m_bExtractStatic = cfgFile->Read(sSectionBuild + sExtractStatic, GetDefaultExtractStatic());
    // Warnings.
    m_bWarnings = cfgFile->Read(sSectionWarnings + sWarnings, GetDefaultWarnings());
    m_bWarnIfDocError = cfgFile->Read(sSectionWarnings + sWarnIfDocError, GetDefaultWarnIfDocError());
    m_bWarnIfUndocumented = cfgFile->Read(sSectionWarnings + sWarnIfUndocumented, GetDefaultWarnIfUndocumented());
    m_bWarnNoParamdoc = cfgFile->Read(sSectionWarnings + sWarnNoParamDoc, GetDefaultWarnNoParamdoc());
    // Alphabetical class index.
    m_bAlphabeticalIndex = cfgFile->Read(sSectionAlphabeticalClassIndex + sAlphabeticalIndex, GetDefaultAlphabeticalIndex());
    // Output.
    m_bGenerateHTML = cfgFile->Read(sSectionOutput + sGenerateHTML, GetDefaultGenerateHTML());
    m_bGenerateHTMLHelp = cfgFile->Read(sSectionOutput + sGenerateHTMLHelp, GetDefaultGenerateHTMLHelp());
    m_bGenerateCHI = cfgFile->Read(sSectionOutput + sGenerateCHI, GetDefaultGenerateCHI());
    m_bBinaryTOC = cfgFile->Read(sSectionOutput + sBinaryTOC, GetDefaultBinaryTOC());
    m_bGenerateLatex = cfgFile->Read(sSectionOutput + sGenerateLatex, GetDefaultGenerateLatex());
    m_bGenerateRTF = cfgFile->Read(sSectionOutput + sGenerateRTF, GetDefaultGenerateRTF());
    m_bGenerateMan = cfgFile->Read(sSectionOutput + sGenerateMan, GetDefaultGenerateMan());
    m_bGenerateXML = cfgFile->Read(sSectionOutput + sGenerateXML, GetDefaultGenerateXML());
    m_bGenerateAutogenDef = cfgFile->Read(sSectionOutput + sGenerateAutogenDef, GetDefaultGenerateAutogenDef());
    m_bGeneratePerlMod = cfgFile->Read(sSectionOutput + sGeneratePerlMod, GetDefaultGeneratePerlMod());
    // Pre-processor.
    m_bEnablePreprocessing = cfgFile->Read(sSectionPreProcessor + sEnablePreProcessing, GetDefaultEnablePreprocessing());
    // Dot.
    m_bClassDiagrams = cfgFile->Read(sSectionDot + sClassDiagrams, GetDefaultClassDiagrams());
    m_bHaveDot = cfgFile->Read(sSectionDot + sHaveDot, GetDefaultHaveDot());
    // General.
    m_bUseAtInTags = cfgFile->Read(sSectionGeneral + sUseAtInTags, GetDefaultUseAtInTags());

    wxDELETE(cfgFile);
    wxASSERT(!cfgFile);
    return true;
}
