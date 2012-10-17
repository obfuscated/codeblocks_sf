/**************************************************************************//**
 * \file        ExtractDocs.cpp
 * \author    Gary Harris
 * \date        01-02-2010
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
 *----------------------------------------------------------------------------------------------- \n
 * The initial inspiration and framework for DoxyBlocks were derived
 * from:
 *
 * doxygen_docs.script by jomeggs.             \n
 * Script for automated doxygen documentation. \n
 * Released under GPL3.                        \n
 * (appears to be version 04)
 *
 * and this code, which is where DoxyBlocks started, owes it's origins
 * to that script and jomeggs' lead-up work.
 *
 * Many thanks.
 *****************************************************************************/
#include "sdk.h"
#ifndef CB_PRECOMP
    #include <cbproject.h>
    #include <macrosmanager.h>
    #include <projectfile.h>
    #include <projectmanager.h>
#endif
#include <wx/busyinfo.h>
#include <wx/ffile.h>
#include <wx/utils.h>

#include "DoxyBlocks.h"


void DoxyBlocks::ExtractProject(wxCommandEvent & WXUNUSED(event))
{
    DoExtractProject();
}

/*! \brief Extract and compile documentation for the currently active project.
 */
void DoxyBlocks::DoExtractProject()
{
    if(!IsProjectOpen()){
        return;
    }

    cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    if(!prj){
        wxString sMsg = _("Failed to get the active project!");
        AppendToLog(sMsg, LOG_ERROR);
        return;
    }

    // Check whether AutoVersioning is active for this project.
    if(m_bAutoVersioning){
        // If we're using autoversion for docs, get the value.
        if(m_pConfig->GetUseAutoVersion()){
            m_sAutoVersion = GetAutoVersion();
            m_pConfig->SetProjectNumber(m_sAutoVersion);
            // Update the config object and mark the project as modified so the new version gets saved on exit.
            SaveSettings();
            prj->SetModified();
        }
    }

    AppendToLog(wxT("----------------------------------------------------------------------------------------------------"));
    AppendToLog(_("Extracting documentation for ") + prj->GetTitle() + wxT("."));
    AppendToLog(_("DoxyBlocks is working, please wait a few moments..."));

    {
        wxBusyInfo running(_("Running doxygen. Please wait..."), Manager::Get()->GetAppWindow());
        GenerateDocuments(prj);
    } // end lifetime of wxBusyInfo.

    AppendToLog(_("\nDone.\n"));
}

namespace
{
/*! \brief Converts a boolean value to a string.
 *
 * \param val             The boolean value to convert.
 * \return wxString    "YES" or "NO".
 *
 */
wxString BoolToString(bool val)
{
    return (val ? wxT("YES") : wxT("NO"));
}
}


/*! \brief Write the doxygen configuration and log files.
 *
 * \param    prj             cbProject*        The project.
 * \param    sPrjName        wxString          The project's name.
 * \param    sPrjPath        wxString          The path to the project.
 * \param    sDoxygenDir     wxString          The relative path to the doxygen files.
 * \param    fnDoxyfile      wxFileName        Doxyfile filename object.
 * \param    fnDoxygenLog    wxFileName        Doxygen log filename object.
 *
 */
void DoxyBlocks::WriteConfigFiles(cbProject *prj, wxString sPrjName, wxString /*sPrjPath*/, wxString /*sDoxygenDir*/, wxFileName fnDoxyfile, wxFileName fnDoxygenLog)
{
    wxArrayString sOutput;
    wxArrayString sErrors;
    MacrosManager    *pMacMngr = Manager::Get()->GetMacrosManager();

     // If there is no config file, create one. If it exists, check prefs.
    bool bWrite = true;
    if(wxFile::Exists(fnDoxyfile.GetFullPath())){
        bWrite = false;
        AppendToLog(_("Found existing doxyfile..."));
        bool bOverwriteDoxyfile = m_pConfig->GetOverwriteDoxyfile();
        if(bOverwriteDoxyfile){
            bool bPromptB4Overwriting = m_pConfig->GetPromptBeforeOverwriting();
            if(bPromptB4Overwriting){
                if(wxMessageBox(_("Overwrite existing doxyfile?"), wxT("DoxyBlocks"), wxYES_NO|wxCENTRE) == wxYES){
                    bWrite = true;
                }
            }
            else{
                bWrite = true;
            }
        }
    }

    if(bWrite){
        AppendToLog(_("Writing doxyfile..."));
        // Keep the CHM separate for easy access.
        wxString sChmFile = wxT("../") + sPrjName + wxT(".chm");
        // Get the list of files from which to extract documentation.
        wxString sInputList = GetInputList(prj, fnDoxyfile);

        // Project.
        wxString sPrjNum = m_pConfig->GetProjectNumber();
        wxString sLanguage = m_pConfig->GetOutputLanguage();
        // Build.
        wxString sExtractAll = BoolToString(m_pConfig->GetExtractAll());
        wxString sExtractPrivate = BoolToString(m_pConfig->GetExtractPrivate());
        wxString sExtractStatic = BoolToString(m_pConfig->GetExtractStatic());
        // Warnings.
        wxString sWarnings = BoolToString(m_pConfig->GetWarnings());
        wxString sWarnIfUndoc = BoolToString(m_pConfig->GetWarnIfUndocumented());
        wxString sWarnIfDocError = BoolToString(m_pConfig->GetWarnIfDocError());
        wxString sWarnNoParamDoc = BoolToString(m_pConfig->GetWarnNoParamdoc());
        //  Alphabetical Class Index.
        wxString sAlphaIndex = BoolToString(m_pConfig->GetAlphabeticalIndex());
        // Output.
        wxString sGenerateHTML = BoolToString(m_pConfig->GetGenerateHTML());
        wxString sGenerateHTMLHelp = BoolToString(m_pConfig->GetGenerateHTMLHelp());
        wxString sPathHHC = pMacMngr->ReplaceMacros(m_pConfig->GetPathHHC());
        wxString sGenerateCHI = BoolToString(m_pConfig->GetGenerateCHI());
        wxString sBinaryTOC = BoolToString(m_pConfig->GetBinaryTOC());
        wxString sGenerateLatex = BoolToString(m_pConfig->GetGenerateLatex());
        wxString sGenerateRTF = BoolToString(m_pConfig->GetGenerateRTF());
        wxString sGenerateMan = BoolToString(m_pConfig->GetGenerateMan());
        wxString sGenerateXML = BoolToString(m_pConfig->GetGenerateXML());
        wxString sGenerateAutogenDef = BoolToString(m_pConfig->GetGenerateAutogenDef());
        wxString sGeneratePerlMod = BoolToString(m_pConfig->GetGeneratePerlMod());
        // Pre-processor.
        wxString sEnablePreproc = BoolToString(m_pConfig->GetEnablePreprocessing());
        // Dot.
        wxString sClassDiag = BoolToString(m_pConfig->GetClassDiagrams());
        wxString sHaveDot = BoolToString(m_pConfig->GetHaveDot());
        wxString sPathDot = pMacMngr->ReplaceMacros(m_pConfig->GetPathDot());

        // Create a full doxygen 1.7.3 config file without comments.
        wxString sText;
        wxString nl = wxT("\n");
        wxString qnl = wxT("\"\n");
        sText   =  wxT("#******************************************************************************\n");
        sText +=  wxString(wxT("# ")) + _("Base configuration for doxygen, generated by DoxyBlocks") + wxT(".\n");
        sText +=  wxString(wxT("# ")) + _("You may change these defaults to suit your purposes") + wxT(".\n");
        sText +=  wxT("#******************************************************************************\n\n");

        sText +=  wxT("# Doxyfile 1.7.3\n");
        sText +=  wxT("\n");
        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("# Project related configuration options\n");
        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("DOXYFILE_ENCODING      = UTF-8\n");
        sText +=  wxT("PROJECT_NAME           = ") + sPrjName + nl;
        sText +=  wxT("PROJECT_NUMBER         = ") + sPrjNum + nl;
        sText +=  wxT("PROJECT_BRIEF          =\n");
        sText +=  wxT("PROJECT_LOGO           =\n");
        sText +=  wxT("OUTPUT_DIRECTORY       =\n");
        sText +=  wxT("CREATE_SUBDIRS         = NO\n");
        sText +=  wxT("OUTPUT_LANGUAGE        = ") + sLanguage + nl;
        sText +=  wxT("BRIEF_MEMBER_DESC      = YES\n");
        sText +=  wxT("REPEAT_BRIEF           = YES\n");
        sText +=  wxT("ABBREVIATE_BRIEF       =\n");
        sText +=  wxT("ALWAYS_DETAILED_SEC    = NO\n");
        sText +=  wxT("INLINE_INHERITED_MEMB  = NO\n");
        sText +=  wxT("FULL_PATH_NAMES        = NO\n");
        sText +=  wxT("STRIP_FROM_PATH        =\n");
        sText +=  wxT("STRIP_FROM_INC_PATH    =\n");
        sText +=  wxT("SHORT_NAMES            = NO\n");
        sText +=  wxT("JAVADOC_AUTOBRIEF      = NO\n");
        sText +=  wxT("QT_AUTOBRIEF           = NO\n");
        sText +=  wxT("MULTILINE_CPP_IS_BRIEF = NO\n");
        sText +=  wxT("INHERIT_DOCS           = YES\n");
        sText +=  wxT("SEPARATE_MEMBER_PAGES  = NO\n");
        sText +=  wxT("TAB_SIZE               = 8\n");
        sText +=  wxT("ALIASES                =\n");
        sText +=  wxT("OPTIMIZE_OUTPUT_FOR_C  = NO\n");
        sText +=  wxT("OPTIMIZE_OUTPUT_JAVA   = NO\n");
        sText +=  wxT("OPTIMIZE_FOR_FORTRAN   = NO\n");
        sText +=  wxT("OPTIMIZE_OUTPUT_VHDL   = NO\n");
        sText +=  wxT("EXTENSION_MAPPING      =\n");
        sText +=  wxT("BUILTIN_STL_SUPPORT    = NO\n");
        sText +=  wxT("CPP_CLI_SUPPORT        = NO\n");
        sText +=  wxT("SIP_SUPPORT            = NO\n");
        sText +=  wxT("IDL_PROPERTY_SUPPORT   = YES\n");
        sText +=  wxT("DISTRIBUTE_GROUP_DOC   = NO\n");
        sText +=  wxT("SUBGROUPING            = YES\n");
        sText +=  wxT("TYPEDEF_HIDES_STRUCT   = NO\n");
        sText +=  wxT("SYMBOL_CACHE_SIZE      = 0\n");

        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("# Build related configuration options\n");
        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("EXTRACT_ALL            = ") + sExtractAll + nl;
        sText +=  wxT("EXTRACT_PRIVATE        = ") + sExtractPrivate + nl;
        sText +=  wxT("EXTRACT_STATIC         = ") + sExtractStatic + nl;
        sText +=  wxT("EXTRACT_LOCAL_CLASSES  = YES\n");
        sText +=  wxT("EXTRACT_LOCAL_METHODS  = NO\n");
        sText +=  wxT("EXTRACT_ANON_NSPACES   = NO\n");
        sText +=  wxT("HIDE_UNDOC_MEMBERS     = NO\n");
        sText +=  wxT("HIDE_UNDOC_CLASSES     = NO\n");
        sText +=  wxT("HIDE_FRIEND_COMPOUNDS  = NO\n");
        sText +=  wxT("HIDE_IN_BODY_DOCS      = NO\n");
        sText +=  wxT("INTERNAL_DOCS          = NO\n");
        sText +=  wxT("CASE_SENSE_NAMES       = NO\n");
        sText +=  wxT("HIDE_SCOPE_NAMES       = NO\n");
        sText +=  wxT("SHOW_INCLUDE_FILES     = YES\n");
        sText +=  wxT("FORCE_LOCAL_INCLUDES   = NO\n");
        sText +=  wxT("INLINE_INFO            = YES\n");
        sText +=  wxT("SORT_MEMBER_DOCS       = YES\n");
        sText +=  wxT("SORT_BRIEF_DOCS        = NO\n");
        sText +=  wxT("SORT_MEMBERS_CTORS_1ST = NO\n");
        sText +=  wxT("SORT_GROUP_NAMES       = NO\n");
        sText +=  wxT("SORT_BY_SCOPE_NAME     = NO\n");
        sText +=  wxT("STRICT_PROTO_MATCHING  = NO\n");
        sText +=  wxT("GENERATE_TODOLIST      = YES\n");
        sText +=  wxT("GENERATE_TESTLIST      = YES\n");
        sText +=  wxT("GENERATE_BUGLIST       = YES\n");
        sText +=  wxT("GENERATE_DEPRECATEDLIST= YES\n");
        sText +=  wxT("ENABLED_SECTIONS       =\n");
        sText +=  wxT("MAX_INITIALIZER_LINES  = 30\n");
        sText +=  wxT("SHOW_USED_FILES        = YES\n");
        sText +=  wxT("SHOW_DIRECTORIES       = NO\n");
        sText +=  wxT("SHOW_FILES             = YES\n");
        sText +=  wxT("SHOW_NAMESPACES        = YES\n");
        sText +=  wxT("FILE_VERSION_FILTER    =\n");
        sText +=  wxT("LAYOUT_FILE            =\n");

        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("# configuration options related to warning and progress messages\n");
        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("QUIET                  = NO\n");
        sText +=  wxT("WARNINGS               = ") + sWarnings + nl;
        sText +=  wxT("WARN_IF_UNDOCUMENTED   = ") + sWarnIfUndoc + nl;
        sText +=  wxT("WARN_IF_DOC_ERROR      = ") + sWarnIfDocError + nl;
        sText +=  wxT("WARN_NO_PARAMDOC       = ") + sWarnNoParamDoc + nl;
        sText +=  wxT("WARN_FORMAT            = \"$file:$line: $text\"\n");
        sText +=  wxT("WARN_LOGFILE            = \"") + fnDoxygenLog.GetFullPath() + qnl;

        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("# configuration options related to the input files\n");
        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText += sInputList;
        sText +=  wxT("INPUT_ENCODING         = UTF-8\n");
        sText +=  wxT("FILE_PATTERNS          =\n");
        sText +=  wxT("RECURSIVE              = NO\n");
        sText +=  wxT("EXCLUDE                =\n");
        sText +=  wxT("EXCLUDE_SYMLINKS       = NO\n");
        sText +=  wxT("EXCLUDE_PATTERNS       =\n");
        sText +=  wxT("EXCLUDE_SYMBOLS        =\n");
        sText +=  wxT("EXAMPLE_PATH           =\n");
        sText +=  wxT("EXAMPLE_PATTERNS       =\n");
        sText +=  wxT("EXAMPLE_RECURSIVE      = NO\n");
        sText +=  wxT("IMAGE_PATH             =\n");
        sText +=  wxT("INPUT_FILTER           =\n");
        sText +=  wxT("FILTER_PATTERNS        =\n");
        sText +=  wxT("FILTER_SOURCE_FILES    = NO\n");
        sText +=  wxT("FILTER_SOURCE_PATTERNS =\n");

        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("# configuration options related to source browsing\n");
        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("SOURCE_BROWSER         = NO\n");
        sText +=  wxT("INLINE_SOURCES         = NO\n");
        sText +=  wxT("STRIP_CODE_COMMENTS    = YES\n");
        sText +=  wxT("REFERENCED_BY_RELATION = NO\n");
        sText +=  wxT("REFERENCES_RELATION    = NO\n");
        sText +=  wxT("REFERENCES_LINK_SOURCE = YES\n");
        sText +=  wxT("USE_HTAGS              = NO\n");
        sText +=  wxT("VERBATIM_HEADERS       = YES\n");

        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("# configuration options related to the alphabetical class index\n");
        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("ALPHABETICAL_INDEX     = ") + sAlphaIndex + nl;
        sText +=  wxT("COLS_IN_ALPHA_INDEX    = 5\n");
        sText +=  wxT("IGNORE_PREFIX          =\n");

        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("# configuration options related to the HTML output\n");
        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("GENERATE_HTML          = ") + sGenerateHTML + nl;
        sText +=  wxT("HTML_OUTPUT            = html\n");
        sText +=  wxT("HTML_FILE_EXTENSION    = .html\n");
        sText +=  wxT("HTML_HEADER            =\n");
        sText +=  wxT("HTML_FOOTER            =\n");
        sText +=  wxT("HTML_STYLESHEET        =\n");
        sText +=  wxT("HTML_COLORSTYLE_HUE    = 220\n");
        sText +=  wxT("HTML_COLORSTYLE_SAT    = 100\n");
        sText +=  wxT("HTML_COLORSTYLE_GAMMA  = 80\n");
        sText +=  wxT("HTML_TIMESTAMP         = YES\n");
        sText +=  wxT("HTML_ALIGN_MEMBERS     = YES\n");
        sText +=  wxT("HTML_DYNAMIC_SECTIONS  = NO\n");
        sText +=  wxT("GENERATE_DOCSET        = NO\n");
        sText +=  wxT("DOCSET_FEEDNAME        = \"Doxygen generated docs\"\n");
        sText +=  wxT("DOCSET_BUNDLE_ID       = org.doxygen.Project\n");
        sText +=  wxT("DOCSET_PUBLISHER_ID    = org.doxygen.Publisher\n");
        sText +=  wxT("DOCSET_PUBLISHER_NAME  = Publisher\n");
        sText +=  wxT("GENERATE_HTMLHELP      = ") + sGenerateHTMLHelp + nl;
        sText +=  wxT("CHM_FILE               = \"") + sChmFile    + qnl;
        if(!sPathHHC.IsEmpty()){
            sText +=  wxT("HHC_LOCATION           = \"") + sPathHHC + qnl;
        }
        else{
            sText +=  wxT("HHC_LOCATION           =\n");
        }
        sText +=  wxT("GENERATE_CHI           = ") + sGenerateCHI + nl;
        sText +=  wxT("CHM_INDEX_ENCODING     =\n");
        sText +=  wxT("BINARY_TOC             = ") + sBinaryTOC + nl;
        sText +=  wxT("TOC_EXPAND             = NO\n");
        sText +=  wxT("GENERATE_QHP           = NO\n");
        sText +=  wxT("QCH_FILE               =\n");
        sText +=  wxT("QHP_NAMESPACE          = org.doxygen.Project\n");
        sText +=  wxT("QHP_VIRTUAL_FOLDER     = doc\n");
        sText +=  wxT("QHP_CUST_FILTER_NAME   =\n");
        sText +=  wxT("QHP_CUST_FILTER_ATTRS  =\n");
        sText +=  wxT("QHP_SECT_FILTER_ATTRS  =\n");
        sText +=  wxT("QHG_LOCATION           =\n");
        sText +=  wxT("GENERATE_ECLIPSEHELP   = NO\n");
        sText +=  wxT("ECLIPSE_DOC_ID         = org.doxygen.Project\n");
        sText +=  wxT("DISABLE_INDEX          = NO\n");
        sText +=  wxT("ENUM_VALUES_PER_LINE   = 4\n");
        sText +=  wxT("GENERATE_TREEVIEW      = YES\n");
        sText +=  wxT("USE_INLINE_TREES       = NO\n");
        sText +=  wxT("TREEVIEW_WIDTH         = 250\n");
        sText +=  wxT("EXT_LINKS_IN_WINDOW    = NO\n");
        sText +=  wxT("FORMULA_FONTSIZE       = 10\n");
        sText +=  wxT("FORMULA_TRANSPARENT    = YES\n");
        sText +=  wxT("USE_MATHJAX            = NO\n");
        sText +=  wxT("MATHJAX_RELPATH        = http://www.mathjax.org/mathjax\n");
        sText +=  wxT("SEARCHENGINE           = YES\n");
        sText +=  wxT("SERVER_BASED_SEARCH    = NO\n");

        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("# configuration options related to the LaTeX output\n");
        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("GENERATE_LATEX         = ") + sGenerateLatex + nl;
        sText +=  wxT("LATEX_OUTPUT           = latex\n");
        sText +=  wxT("LATEX_CMD_NAME         = latex\n");
        sText +=  wxT("MAKEINDEX_CMD_NAME     = makeindex\n");
        sText +=  wxT("COMPACT_LATEX          = NO\n");
        sText +=  wxT("PAPER_TYPE             = a4\n");
        sText +=  wxT("EXTRA_PACKAGES         =\n");
        sText +=  wxT("LATEX_HEADER           =\n");
        sText +=  wxT("PDF_HYPERLINKS         = YES\n");
        sText +=  wxT("USE_PDFLATEX           = YES\n");
        sText +=  wxT("LATEX_BATCHMODE        = NO\n");
        sText +=  wxT("LATEX_HIDE_INDICES     = NO\n");
        sText +=  wxT("LATEX_SOURCE_CODE      = NO\n");

        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("# configuration options related to the RTF output\n");
        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("GENERATE_RTF           = ") + sGenerateRTF + nl;
        sText +=  wxT("RTF_OUTPUT             = rtf\n");
        sText +=  wxT("COMPACT_RTF            = NO\n");
        sText +=  wxT("RTF_HYPERLINKS         = NO\n");
        sText +=  wxT("RTF_STYLESHEET_FILE    =\n");
        sText +=  wxT("RTF_EXTENSIONS_FILE    =\n");

        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("# configuration options related to the man page output\n");
        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("GENERATE_MAN           = ") + sGenerateMan + nl;
        sText +=  wxT("MAN_OUTPUT             = man\n");
        sText +=  wxT("MAN_EXTENSION          = .3\n");
        sText +=  wxT("MAN_LINKS              = NO\n");

        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("# configuration options related to the XML output\n");
        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("GENERATE_XML           = ") + sGenerateXML + nl;
        sText +=  wxT("XML_OUTPUT             = xml\n");
        sText +=  wxT("XML_SCHEMA             =\n");
        sText +=  wxT("XML_DTD                =\n");
        sText +=  wxT("XML_PROGRAMLISTING     = YES\n");

        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("# configuration options for the AutoGen Definitions output\n");
        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("GENERATE_AUTOGEN_DEF   = ") + sGenerateAutogenDef + nl;
        sText +=  wxT("#---------------------------------------------------------------------------\n");

        sText +=  wxT("# configuration options related to the Perl module output\n");
        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("GENERATE_PERLMOD       = ") + sGeneratePerlMod + nl;
        sText +=  wxT("PERLMOD_LATEX          = NO\n");
        sText +=  wxT("PERLMOD_PRETTY         = YES\n");
        sText +=  wxT("PERLMOD_MAKEVAR_PREFIX =\n");
        sText +=  wxT("#---------------------------------------------------------------------------\n");

        sText +=  wxT("# Configuration options related to the preprocessor\n");
        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("ENABLE_PREPROCESSING   = ") + sEnablePreproc + nl;
        sText +=  wxT("MACRO_EXPANSION        = YES\n");
        sText +=  wxT("EXPAND_ONLY_PREDEF     = YES\n");
        sText +=  wxT("SEARCH_INCLUDES        = YES\n");
        sText +=  wxT("INCLUDE_PATH           =\n");
        sText +=  wxT("INCLUDE_FILE_PATTERNS  =\n");
        sText +=  wxT("PREDEFINED             = WXUNUSED()=\n");        /// TODO (Gary#1#): I should look at other macros for inclusion here.
        sText +=  wxT("EXPAND_AS_DEFINED      =\n");
        sText +=  wxT("SKIP_FUNCTION_MACROS   = YES\n");

        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("# Configuration::additions related to external references\n");
        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("TAGFILES               =\n");
        sText +=  wxT("GENERATE_TAGFILE       =\n");
        sText +=  wxT("ALLEXTERNALS           = NO\n");
        sText +=  wxT("EXTERNAL_GROUPS        = YES\n");
        sText +=  wxT("PERL_PATH              = /usr/bin/perl\n");

        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("# Configuration options related to the dot tool\n");
        sText +=  wxT("#---------------------------------------------------------------------------\n");
        sText +=  wxT("CLASS_DIAGRAMS         = ") + sClassDiag + nl;
        sText +=  wxT("MSCGEN_PATH            =\n");
        sText +=  wxT("HIDE_UNDOC_RELATIONS   = YES\n");
        sText +=  wxT("HAVE_DOT               = ") + sHaveDot + nl;
        sText +=  wxT("DOT_NUM_THREADS        = 0\n");
        sText +=  wxT("DOT_FONTNAME           = Helvetica\n");
        sText +=  wxT("DOT_FONTSIZE           = 10\n");
        sText +=  wxT("DOT_FONTPATH           =\n");
        sText +=  wxT("CLASS_GRAPH            = YES\n");
        sText +=  wxT("COLLABORATION_GRAPH    = YES\n");
        sText +=  wxT("GROUP_GRAPHS           = YES\n");
        sText +=  wxT("UML_LOOK               = NO\n");
        sText +=  wxT("TEMPLATE_RELATIONS     = NO\n");
        sText +=  wxT("INCLUDE_GRAPH          = YES\n");
        sText +=  wxT("INCLUDED_BY_GRAPH      = YES\n");
        sText +=  wxT("CALL_GRAPH             = YES\n");
        sText +=  wxT("CALLER_GRAPH           = NO\n");
        sText +=  wxT("GRAPHICAL_HIERARCHY    = YES\n");
        sText +=  wxT("DIRECTORY_GRAPH        = YES\n");
        sText +=  wxT("DOT_IMAGE_FORMAT       = png\n");
        if(!sPathDot.IsEmpty()){
            sText +=  wxT("DOT_PATH           = \"") + sPathDot + qnl;
        }
        else{
            sText +=  wxT("DOT_PATH               =\n");
        }
        sText +=  wxT("DOTFILE_DIRS           =\n");
        sText +=  wxT("MSCFILE_DIRS           =\n");
        sText +=  wxT("DOT_GRAPH_MAX_NODES    = 50\n");
        sText +=  wxT("MAX_DOT_GRAPH_DEPTH    = 0\n");
        sText +=  wxT("DOT_TRANSPARENT        = NO\n");
        sText +=  wxT("DOT_MULTI_TARGETS      = NO\n");
        sText +=  wxT("GENERATE_LEGEND        = YES\n");
        sText +=  wxT("DOT_CLEANUP            = YES\n");
        sText +=  wxT("\n");

        wxFFile fileDoxyfile(fnDoxyfile.GetFullPath(), wxT("w"));
        fileDoxyfile.Write(sText);
    }
}

   /**************************************************************************
     this is where our work is performed :)  (text stolen from Yiannis)
   **************************************************************************/

/*! \brief Manage the generation of the doxygen configuration and log files.
 *
 * \param    prj    cbProject*       The project.
 * \return   int    0 on success, -1 on failure.
 * \todo Revisit the path management code and add support for allowing the docs to be created in a different location
 * to the doxyfile via the OUTPUT_PATH setting e.g. using something like ../docs.
 */
int DoxyBlocks::GenerateDocuments(cbProject *prj)
{
    wxString sMsg;

    // First, I need to change into the project directory. All following actions
    // will work with relative pathes. This way, stored pathes in doxygen
    // configuration files won't cause problems after moving to other places.
    // The current path is to be restored after my actions...

    wxString sOldPath = wxGetCwd();
    wxFileName fnProject;
    fnProject.Assign(prj->GetFilename(), ::wxPATH_NATIVE);

    wxString sPrjPath = fnProject.GetPath(wxPATH_GET_VOLUME);
    wxSetWorkingDirectory(sPrjPath);

    // project name, name and path of base config file and logfile
    wxString sPrjName = fnProject.GetName();
    wxString sOutputDir = m_pConfig->GetOutputDirectory();
    wxString sDoxygenDir = wxT("doxygen");
    wxString sCfgBaseFile = wxT("doxyfile");
    wxString sLogFile     = wxT("doxygen.log");

    if(!sOutputDir.IsEmpty()){
        sDoxygenDir = sOutputDir;
    }

    wxFileName fnOutput(sDoxygenDir, wxT(""));
    wxFileName fnDoxyfile(sDoxygenDir + wxFileName::GetPathSeparator() + sCfgBaseFile);
    wxFileName fnDoxygenLog(sDoxygenDir + wxFileName::GetPathSeparator() + sLogFile);
    fnOutput.Normalize();
    fnDoxyfile.Normalize();
    fnDoxygenLog.Normalize();

    if (!fnOutput.Mkdir(0777, wxPATH_MKDIR_FULL)){
        wxString sMsg = _("Failed. ") + fnOutput.GetFullPath() + _(" was not created.");
        AppendToLog(sMsg, LOG_WARNING);
        wxSetWorkingDirectory(sOldPath);
        return -1;
    }

    // I'm in the project directory, now create the doxygen configuration files
    WriteConfigFiles(prj, sPrjName, sPrjPath, sDoxygenDir, fnDoxyfile, fnDoxygenLog);

    if(!wxFile::Exists(fnDoxyfile.GetFullPath())){
        wxString sMsg = _("Failed. ") + fnDoxyfile.GetFullPath() + _(" was not created.");
        AppendToLog(sMsg, LOG_WARNING);
        wxSetWorkingDirectory(sOldPath);
        return -1;
    }
     // Drop into the doxygen dir.
     wxSetWorkingDirectory(sPrjPath + wxFileName::GetPathSeparator() + sDoxygenDir);

    // now tango, launch doxygen...
    wxArrayString sOutput;
    wxArrayString sErrors;
    long ret;
    // Default command.
    wxString cmd = wxT("doxygen");
    // If a path is configured, use that instead.
    wxString sDoxygenPath = Manager::Get()->GetMacrosManager()->ReplaceMacros(m_pConfig->GetPathDoxygen());
    if(!sDoxygenPath.IsEmpty()){
        cmd = sDoxygenPath;
    }
    ret = wxExecute(cmd + wxT(" ") + fnDoxyfile.GetFullPath(), sOutput, sErrors);
    if(ret != -1){
        // Write doxygen logfile to the log or remove it if it's empty
        if(wxFile::Exists(fnDoxygenLog.GetFullPath())){
            wxString sText;
            wxFFile fileLog(fnDoxygenLog.GetFullPath());
            if(fileLog.IsOpened()){
                fileLog.ReadAll(&sText);
                fileLog.Close();
            }
           else{
                AppendToLog(_("Failed to open ") + sLogFile, LOG_WARNING);
           }
           if(!sText.IsEmpty()){
                AppendToLog(_("\nContents of doxygen's log file:"));
                AppendToLog(sText, LOG_WARNING);
            }
           else{
                wxRemoveFile(sLogFile);
           }
        }

        // Run docs if HTML was created.
        if(m_pConfig->GetGenerateHTML()){
            // Open the newly created HTML docs, if prefs allow.
            if(m_pConfig->GetRunHTML()){
                DoRunHTML();
            }
            if(m_pConfig->GetGenerateHTMLHelp()){
                // Open the newly created CHM if prefs allow.
                if(m_pConfig->GetRunCHM()){
                    RunCompiledHelp(fnDoxyfile.GetPathWithSep() , sPrjName);
                }
            }
        }

        // tell the user where to find the docs
        sMsg = wxT("Success.\nYour documents are in: ");
        AppendToLog(sMsg + fnDoxyfile.GetPathWithSep());
    }
    else{
        // please google, install doxygen, set your path and...
        AppendToLog(wxString::Format(_("Execution of '%s' failed."), cmd.c_str()), LOG_ERROR);
        AppendToLog(_("Please ensure that the doxygen 'bin' directory is in your path or provide the specific path in DoxyBlocks' preferences.\n"));
    }

    // restore to before saved path and bye...
    wxSetWorkingDirectory(sOldPath);
    return ret;
}

/*! \brief Get the list of input files for the doxygen configuration file.
 *
 * \param    prj            cbProject*        The project.
 * \param     fnDoxyfile     wxFileName    Doxyfile filename object.
 * \return    wxString    The input file list.
 *
 * This function queries the cbProject object for a list of files actually attached to the project.
 * Other  files in the project directory are ignored.
 */
wxString DoxyBlocks::GetInputList(cbProject *prj, wxFileName fnDoxyfile)
{
    // only files with extensions named here will be included in the documentation
    wxArrayString asExtList;
    asExtList.Add(wxT("*.c"));
    asExtList.Add(wxT("*.cc"));
    asExtList.Add(wxT("*.cxx"));
    asExtList.Add(wxT("*.cpp"));
    asExtList.Add(wxT("*.c++"));
    asExtList.Add(wxT("*.java"));
    asExtList.Add(wxT("*.ii"));
    asExtList.Add(wxT("*.ixx"));
    asExtList.Add(wxT("*.ipp"));
    asExtList.Add(wxT("*.i++"));
    asExtList.Add(wxT("*.inl"));
    asExtList.Add(wxT("*.h"));
    asExtList.Add(wxT("*.hh"));
    asExtList.Add(wxT("*.hxx"));
    asExtList.Add(wxT("*.hpp"));
    asExtList.Add(wxT("*.h++"));
    asExtList.Add(wxT("*.idl"));
    asExtList.Add(wxT("*.odl"));
    asExtList.Add(wxT("*.cs"));
    asExtList.Add(wxT("*.php"));
    asExtList.Add(wxT("*.php3"));
    asExtList.Add(wxT("*.inc"));
    asExtList.Add(wxT("*.m"));
    asExtList.Add(wxT("*.mm"));
    asExtList.Add(wxT("*.py"));
    asExtList.Add(wxT("*.f90"));

     // now let's build a string containing all the project files. To be on the safe side,
     // we will quote them all so spaces and other special chars don't break the actual
     // command later...
     wxString sInputList;
     const int cntExtList = asExtList.GetCount();

     sInputList += wxT("INPUT                  = ");

    for (FilesList::iterator it = prj->GetFilesList().begin(); it != prj->GetFilesList().end(); ++it)
    {
        ProjectFile* prjFile = *it;
        if(prjFile)
        {
            const wxString sFileName = prjFile->relativeFilename;
            wxFileName fnFileName = prjFile->file;
            fnFileName.MakeRelativeTo(fnDoxyfile.GetPath());
            const wxString RelName = fnFileName.GetFullPath();

            /* if the file matches one of the abovementioned filters, add it */
            for(int n = 0; n < cntExtList; ++n)
            {
               if(sFileName.Matches(asExtList.Item(n)))
               {
                  sInputList += wxT("\\\n\t\"") + RelName + wxT("\" ");
                  break;
               }
            }
        }
    }
     sInputList += wxT("\n");
     return sInputList;
}
