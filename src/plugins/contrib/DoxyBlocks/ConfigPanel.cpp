/**************************************************************************//**
 * \file      ConfigPanel.cpp
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
#include "ConfigPanel.h"
#include <sdk.h> // Code::Blocks SDK

//(*InternalHeaders(ConfigPanel)
#include <wx/string.h>
#include <wx/intl.h>
//*)
#include <wx/filedlg.h>
#include <wx/fontutil.h>

#include <configmanager.h>
#include <editorcolourset.h>
#include <editormanager.h>
#include <tinyxml.h>

#include "version.h"


//(*IdInit(ConfigPanel)
const long ConfigPanel::ID_RADIOBOX_BLOCKCOMMENTS = wxNewId();
const long ConfigPanel::ID_TEXTCTRL_BLOCKCOMMENT = wxNewId();
const long ConfigPanel::ID_RADIOBOX_LINECOMMENTS = wxNewId();
const long ConfigPanel::ID_TEXTCTRL_LINECOMMENT = wxNewId();
const long ConfigPanel::ID_PANEL2 = wxNewId();
const long ConfigPanel::ID_STATICTEXT1 = wxNewId();
const long ConfigPanel::ID_TEXTCTRL_PROJECTNUMBER = wxNewId();
const long ConfigPanel::ID_CHECKBOX_USEAUTOVERSION = wxNewId();
const long ConfigPanel::ID_STATICTEXT8 = wxNewId();
const long ConfigPanel::ID_TEXTCTRLOUTPUT_DIRECTORY = wxNewId();
const long ConfigPanel::ID_STATICTEXT5 = wxNewId();
const long ConfigPanel::ID_CHOICE_OUTPUT_LANGUAGE = wxNewId();
const long ConfigPanel::ID_CHECKBOX_EXTRACT_ALL = wxNewId();
const long ConfigPanel::ID_CHECKBOX_EXTRACTPRIVATE = wxNewId();
const long ConfigPanel::ID_CHECKBOX_EXTRACTSTATIC = wxNewId();
const long ConfigPanel::ID_CHECKBOX_WARNINGS = wxNewId();
const long ConfigPanel::ID_CHECKBOX_WARN_IF_DOC_ERROR = wxNewId();
const long ConfigPanel::ID_CHECKBOX_WARN_IF_UNDOCUMENTED = wxNewId();
const long ConfigPanel::ID_CHECKBOX_WARN_NO_PARAMDOC = wxNewId();
const long ConfigPanel::ID_CHECKBOX_ALPHABETICAL_INDEX = wxNewId();
const long ConfigPanel::ID_PANEL3 = wxNewId();
const long ConfigPanel::ID_CHECKBOX_GENERATE_HTML = wxNewId();
const long ConfigPanel::ID_CHECKBOX_GENERATE_HTMLHELP = wxNewId();
const long ConfigPanel::ID_CHECKBOX_GENERATE_CHI = wxNewId();
const long ConfigPanel::ID_CHECKBOX_BINARY_TOC = wxNewId();
const long ConfigPanel::ID_CHECKBOX_GENERATE_LATEX = wxNewId();
const long ConfigPanel::ID_CHECKBOX_GENERATE_RTF = wxNewId();
const long ConfigPanel::ID_CHECKBOX_GENERATE_MAN = wxNewId();
const long ConfigPanel::ID_CHECKBOX_GENERATE_XML = wxNewId();
const long ConfigPanel::ID_CHECKBOX_GENERATE_AUTOGEN_DEF = wxNewId();
const long ConfigPanel::ID_CHECKBOX_GENERATE_PERLMOD = wxNewId();
const long ConfigPanel::ID_CHECKBOX_ENABLE_PREPROCESSING = wxNewId();
const long ConfigPanel::ID_CHECKBOX_CLASS_DIAGRAMS = wxNewId();
const long ConfigPanel::ID_CHECKBOX_HAVE_DOT = wxNewId();
const long ConfigPanel::ID_PANEL4 = wxNewId();
const long ConfigPanel::ID_STATICTEXT2 = wxNewId();
const long ConfigPanel::ID_TEXTCTRL_PATHDOXYGEN = wxNewId();
const long ConfigPanel::ID_BUTTON_BROWSEDOXYGEN = wxNewId();
const long ConfigPanel::ID_STATICTEXT4 = wxNewId();
const long ConfigPanel::ID_TEXTCTRL_PATHDOXYWIZARD = wxNewId();
const long ConfigPanel::ID_BUTTON_BROWSEDOXYWIZARD = wxNewId();
const long ConfigPanel::ID_STATICTEXT3 = wxNewId();
const long ConfigPanel::ID_TEXTCTRL_PATHHHC = wxNewId();
const long ConfigPanel::ID_BUTTON_BROWSEHHC = wxNewId();
const long ConfigPanel::ID_STATICTEXT6 = wxNewId();
const long ConfigPanel::ID_TEXTCTRL_PATHDOT = wxNewId();
const long ConfigPanel::ID_BUTTON_BROWSEDOT = wxNewId();
const long ConfigPanel::ID_STATICTEXT7 = wxNewId();
const long ConfigPanel::ID_TEXTCTRL_PATHCHMVIEWER = wxNewId();
const long ConfigPanel::ID_BUTTON_BROWSECHMVIEWER = wxNewId();
const long ConfigPanel::ID_CHECKBOX_OVERWRITEDOXYFILE = wxNewId();
const long ConfigPanel::ID_CHECKBOX_PROMPTB4OVERWRITING = wxNewId();
const long ConfigPanel::ID_CHECKBOX_USEATINTAGS = wxNewId();
const long ConfigPanel::ID_CHECKBOX_LOADTEMPLATE = wxNewId();
const long ConfigPanel::ID_CHECKBOX_USEINTERNALVIEWER = wxNewId();
const long ConfigPanel::ID_CHECKBOX_RUNHTML = wxNewId();
const long ConfigPanel::ID_CHECKBOX_RUNCHM = wxNewId();
const long ConfigPanel::ID_PANEL1 = wxNewId();
const long ConfigPanel::ID_NOTEBOOK_PREFS = wxNewId();
//*)

BEGIN_EVENT_TABLE(ConfigPanel,wxPanel)
    //(*EventTable(ConfigPanel)
    //*)
END_EVENT_TABLE()

/*! \brief Constructor.
 */
ConfigPanel::ConfigPanel(wxWindow* parent, DoxyBlocks *pOwner, wxWindowID /*id*/,const wxPoint& /*pos*/,const wxSize& /*size*/) :
    m_pOwnerClass(pOwner)
{
    // ********************************************************************************************
    // I don't use XRC for this plug-in because cbStyledTextCtrl isn't handled properly.
    // This method still requires some adjustments. See below.
    // ********************************************************************************************
    //(*Initialize(ConfigPanel)
    wxBoxSizer* BoxSizer15;
    wxStaticBoxSizer* StaticBoxSizer2;
    wxStaticBoxSizer* StaticBoxSizer10;
    wxPanel* Panel1;
    wxBoxSizer* BoxSizer3;
    wxStaticBoxSizer* StaticBoxSizer7;
    wxBoxSizer* BoxSizer16;
    wxStaticBoxSizer* StaticBoxSizer5;
    wxBoxSizer* BoxSizer10;
    wxStaticBoxSizer* StaticBoxSizer12;
    wxBoxSizer* BoxSizer7;
    wxBoxSizer* BoxSizer11;
    wxBoxSizer* BoxSizer13;
    wxBoxSizer* BoxSizer22;
    wxBoxSizer* BoxSizer2;
    wxStaticText* StaticText1;
    wxStaticBoxSizer* StaticBoxSizer3;
    wxBoxSizer* BoxSizer17;
    wxPanel* Panel2;
    wxBoxSizer* BoxSizer9;
    wxBoxSizer* BoxSizer18;
    wxStaticText* StaticText3;
    wxPanel* Panel4;
    wxBoxSizer* BoxSizer21;
    wxStaticBoxSizer* StaticBoxSizer8;
    wxBoxSizer* BoxSizer19;
    wxStaticText* StaticText8;
    wxBoxSizer* BoxSizer4;
    wxStaticBoxSizer* StaticBoxSizer4;
    wxStaticBoxSizer* StaticBoxSizer9;
    wxPanel* Panel3;
    wxStaticText* StaticText7;
    wxBoxSizer* BoxSizer8;
    wxStaticBoxSizer* StaticBoxSizer6;
    wxBoxSizer* BoxSizer20;
    wxStaticText* StaticText4;
    wxBoxSizer* BoxSizer1;
    wxStaticBoxSizer* StaticBoxSizer11;
    wxStaticText* StaticText5;
    wxStaticText* StaticText2;
    wxStaticText* StaticText6;
    wxBoxSizer* BoxSizer12;
    wxBoxSizer* BoxSizer14;
    wxBoxSizer* BoxSizer6;
    wxBoxSizer* BoxSizer5;
    wxNotebook* NotebookPrefs;
    wxStaticBoxSizer* StaticBoxSizer1;

    Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    NotebookPrefs = new wxNotebook(this, ID_NOTEBOOK_PREFS, wxDefaultPosition, wxDefaultSize, 0, _T("ID_NOTEBOOK_PREFS"));
    Panel2 = new wxPanel(NotebookPrefs, ID_PANEL2, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL2"));
    BoxSizer6 = new wxBoxSizer(wxVERTICAL);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    wxString __wxRadioBoxChoices_1[6] =
    {
    	_("C/JavaDoc"),
    	_("C++ Exclamation"),
    	_("C++ Slash"),
    	_("Qt"),
    	_("Visible C Style"),
    	_("Visible C++ Style")
    };
    RadioBoxBlockComments = new wxRadioBox(Panel2, ID_RADIOBOX_BLOCKCOMMENTS, _("Block Comment Style"), wxDefaultPosition, wxDefaultSize, 6, __wxRadioBoxChoices_1, 1, wxRA_SPECIFY_COLS, wxDefaultValidator, _T("ID_RADIOBOX_BLOCKCOMMENTS"));
    RadioBoxBlockComments->SetToolTip(_("Select your preferred block comment style."));
    BoxSizer2->Add(RadioBoxBlockComments, 1, wxALL|wxEXPAND, 5);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, Panel2, _("Block Comment Sample"));
    TextCtrlBlockComment = new cbStyledTextCtrl(Panel2, ID_TEXTCTRL_BLOCKCOMMENT, wxDefaultPosition, wxSize(150,150));
    TextCtrlBlockComment->SetToolTip(_("Sample code using the selected block comment style."));
    // wxSmith doesn't handle cbStyledTextCtrl initialisation properly. Replace the line above with the commented one below.
    // You will need to do this every time wxSmith rewrites this entry.
    // TextCtrlBlockComment = new cbStyledTextCtrl(Panel2, ID_TEXTCTRL_BLOCKCOMMENT, wxDefaultPosition, wxSize(150,150));
    StaticBoxSizer1->Add(TextCtrlBlockComment, 1, wxALL|wxEXPAND, 5);
    BoxSizer2->Add(StaticBoxSizer1, 3, wxALL|wxEXPAND, 5);
    BoxSizer6->Add(BoxSizer2, 1, wxALL|wxEXPAND, 5);
    BoxSizer10 = new wxBoxSizer(wxHORIZONTAL);
    wxString __wxRadioBoxChoices_2[4] =
    {
    	_("C/JavaDoc"),
    	_("C++ Exclamation"),
    	_("C++ Slash"),
    	_("Qt")
    };
    RadioBoxLineComments = new wxRadioBox(Panel2, ID_RADIOBOX_LINECOMMENTS, _("Line Comment Style"), wxDefaultPosition, wxDefaultSize, 4, __wxRadioBoxChoices_2, 1, wxRA_SPECIFY_COLS, wxDefaultValidator, _T("ID_RADIOBOX_LINECOMMENTS"));
    RadioBoxLineComments->SetToolTip(_("Select your preferred line comment style."));
    BoxSizer10->Add(RadioBoxLineComments, 1, wxALL|wxEXPAND, 5);
    StaticBoxSizer3 = new wxStaticBoxSizer(wxVERTICAL, Panel2, _("Line Comment Sample"));
    TextCtrlLineComment = new cbStyledTextCtrl(Panel2, ID_TEXTCTRL_LINECOMMENT, wxDefaultPosition, wxSize(150,150));
    TextCtrlLineComment->SetToolTip(_("Sample code using the selected line comment style."));
    // wxSmith doesn't handle cbStyledTextCtrl initialisation properly. Replace the line above with the commented one below.
    // You will need to do this every time wxSmith rewrites this entry.
    // TextCtrlLineComment = new cbStyledTextCtrl(Panel2, ID_TEXTCTRL_LINECOMMENT, wxDefaultPosition, wxSize(150,150));
    StaticBoxSizer3->Add(TextCtrlLineComment, 1, wxALL|wxEXPAND, 5);
    BoxSizer10->Add(StaticBoxSizer3, 3, wxALL|wxEXPAND, 5);
    BoxSizer6->Add(BoxSizer10, 1, wxALL|wxEXPAND, 5);
    Panel2->SetSizer(BoxSizer6);
    BoxSizer6->Fit(Panel2);
    BoxSizer6->SetSizeHints(Panel2);
    Panel3 = new wxPanel(NotebookPrefs, ID_PANEL3, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL3"));
    BoxSizer4 = new wxBoxSizer(wxVERTICAL);
    StaticBoxSizer2 = new wxStaticBoxSizer(wxVERTICAL, Panel3, _("Project"));
    BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    StaticText1 = new wxStaticText(Panel3, ID_STATICTEXT1, _("PROJECT_NUMBER"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    BoxSizer5->Add(StaticText1, 1, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlProjectNumber = new wxTextCtrl(Panel3, ID_TEXTCTRL_PROJECTNUMBER, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL_PROJECTNUMBER"));
    TextCtrlProjectNumber->SetToolTip(_("The PROJECT_NUMBER tag can be used to enter a project or revision number.\nThis could be handy for archiving the generated documentation or\nif some version control system is used."));
    BoxSizer5->Add(TextCtrlProjectNumber, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    CheckBoxUseAutoVersion = new wxCheckBox(Panel3, ID_CHECKBOX_USEAUTOVERSION, _("Use AutoVersion"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_USEAUTOVERSION"));
    CheckBoxUseAutoVersion->SetValue(false);
    CheckBoxUseAutoVersion->Disable();
    CheckBoxUseAutoVersion->SetToolTip(_("Use the version number provided by the Autoversion plug-in.\nIf this option is selected, DoxyBlocks will extract the version number\nfrom the Autoversion header file.\nThe format used is MAJOR.MINOR.BUILD.\nThis option is disabled if Autoversioning is not enabled for the project."));
    BoxSizer5->Add(CheckBoxUseAutoVersion, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer2->Add(BoxSizer5, 1, wxALL|wxALIGN_LEFT, 5);
    BoxSizer18 = new wxBoxSizer(wxHORIZONTAL);
    StaticText8 = new wxStaticText(Panel3, ID_STATICTEXT8, _("OUTPUT_DIRECTORY"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT8"));
    BoxSizer18->Add(StaticText8, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlOutputDirectory = new wxTextCtrl(Panel3, ID_TEXTCTRLOUTPUT_DIRECTORY, wxEmptyString, wxDefaultPosition, wxSize(252,21), 0, wxDefaultValidator, _T("ID_TEXTCTRLOUTPUT_DIRECTORY"));
    TextCtrlOutputDirectory->SetToolTip(_("The OUTPUT_DIRECTORY tag is used to specify the (relative or\nabsolute) base path where the generated documentation will be put.\nIf a relative path is entered, it will be relative to the location\nwhere doxygen was started. If left blank the current directory will be used.\n\nDoxyBlocks will use the path name entered here to create a directory\nrelative to <project dir>. This allows you to create separate doxygen\ndirectories for projects that reside in the same directory, or just use a\ndifferent directory name. If this field is blank, documents will be created\nin <project dir>/doxygen. Enter directory names without dots, \nleading separators, volume names, etc. DoxyBlocks does validation on\nthe path name and will strip extraneous characters.\n\nExamples:\n[blank]                     -> <project dir>/doxygen.\n\"docs\"                      -> <project dir>/docs.\n\"docs/sub1/sub2\"  -> <project dir>/docs/sub1/sub2.\n\"doxygen/docs\"     -> <project dir>/doxygen/docs.\n"));
    BoxSizer18->Add(TextCtrlOutputDirectory, 2, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer2->Add(BoxSizer18, 1, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_LEFT, 5);
    BoxSizer20 = new wxBoxSizer(wxHORIZONTAL);
    StaticText5 = new wxStaticText(Panel3, ID_STATICTEXT5, _("OUTPUT_LANGUAGE"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
    BoxSizer20->Add(StaticText5, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    ChoiceOutputLanguage = new wxChoice(Panel3, ID_CHOICE_OUTPUT_LANGUAGE, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE_OUTPUT_LANGUAGE"));
    ChoiceOutputLanguage->Append(_("Afrikaans"));
    ChoiceOutputLanguage->Append(_("Arabic"));
    ChoiceOutputLanguage->Append(_("Brazilian"));
    ChoiceOutputLanguage->Append(_("Catalan"));
    ChoiceOutputLanguage->Append(_("Chinese"));
    ChoiceOutputLanguage->Append(_("Chinese-Traditional"));
    ChoiceOutputLanguage->Append(_("Croatian"));
    ChoiceOutputLanguage->Append(_("Czech"));
    ChoiceOutputLanguage->Append(_("Danish"));
    ChoiceOutputLanguage->Append(_("Dutch"));
    ChoiceOutputLanguage->Append(_("English"));
    ChoiceOutputLanguage->Append(_("Esperanto"));
    ChoiceOutputLanguage->Append(_("Farsi"));
    ChoiceOutputLanguage->Append(_("Finnish"));
    ChoiceOutputLanguage->Append(_("French"));
    ChoiceOutputLanguage->Append(_("German"));
    ChoiceOutputLanguage->Append(_("Greek"));
    ChoiceOutputLanguage->Append(_("Hungarian"));
    ChoiceOutputLanguage->Append(_("Italian"));
    ChoiceOutputLanguage->Append(_("Japanese"));
    ChoiceOutputLanguage->Append(_("Japanese-en"));
    ChoiceOutputLanguage->Append(_("Korean"));
    ChoiceOutputLanguage->Append(_("Korean-en"));
    ChoiceOutputLanguage->Append(_("Lithuanian"));
    ChoiceOutputLanguage->Append(_("Norwegian"));
    ChoiceOutputLanguage->Append(_("Macedonian"));
    ChoiceOutputLanguage->Append(_("Persian"));
    ChoiceOutputLanguage->Append(_("Polish"));
    ChoiceOutputLanguage->Append(_("Portuguese"));
    ChoiceOutputLanguage->Append(_("Romanian"));
    ChoiceOutputLanguage->Append(_("Russian"));
    ChoiceOutputLanguage->Append(_("Serbian"));
    ChoiceOutputLanguage->Append(_("Serbian-Cyrilic"));
    ChoiceOutputLanguage->Append(_("Slovak,"));
    ChoiceOutputLanguage->Append(_("Slovene"));
    ChoiceOutputLanguage->Append(_("Spanish"));
    ChoiceOutputLanguage->Append(_("Swedish"));
    ChoiceOutputLanguage->Append(_("Ukrainian"));
    ChoiceOutputLanguage->Append(_("Vietnamese"));
    ChoiceOutputLanguage->SetToolTip(_("The OUTPUT_LANGUAGE tag is used to specify the language in which all\ndocumentation generated by doxygen is written. Doxygen will use this\ninformation to generate all constant output in the proper language.\nThe default language is English, other supported languages are:\nAfrikaans, Arabic, Brazilian, Catalan, Chinese, Chinese-Traditional,\nCroatian, Czech, Danish, Dutch, Esperanto, Farsi, Finnish, French, German,\nGreek, Hungarian, Italian, Japanese, Japanese-en (Japanese with English\nmessages), Korean, Korean-en, Lithuanian, Norwegian, Macedonian, Persian,\nPolish, Portuguese, Romanian, Russian, Serbian, Serbian-Cyrilic, Slovak,\nSlovene, Spanish, Swedish, Ukrainian, and Vietnamese."));
    BoxSizer20->Add(ChoiceOutputLanguage, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer2->Add(BoxSizer20, 1, wxALL|wxALIGN_LEFT, 5);
    BoxSizer4->Add(StaticBoxSizer2, 2, wxALL|wxEXPAND, 5);
    StaticBoxSizer5 = new wxStaticBoxSizer(wxHORIZONTAL, Panel3, _("Build"));
    CheckBoxExtractAll = new wxCheckBox(Panel3, ID_CHECKBOX_EXTRACT_ALL, _("EXTRACT_ALL"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_EXTRACT_ALL"));
    CheckBoxExtractAll->SetValue(false);
    CheckBoxExtractAll->SetToolTip(_("If the EXTRACT_ALL tag is set to YES doxygen will assume all entities in\ndocumentation are documented, even if no documentation was available.\nPrivate class members and static file members will be hidden unless\nthe EXTRACT_PRIVATE and EXTRACT_STATIC tags are set to YES"));
    StaticBoxSizer5->Add(CheckBoxExtractAll, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    CheckBoxExtractPrivate = new wxCheckBox(Panel3, ID_CHECKBOX_EXTRACTPRIVATE, _("EXTRACT_PRIVATE"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_EXTRACTPRIVATE"));
    CheckBoxExtractPrivate->SetValue(false);
    CheckBoxExtractPrivate->SetToolTip(_("If the EXTRACT_PRIVATE tag is set to YES all private members of a\nclass will be included in the documentation."));
    StaticBoxSizer5->Add(CheckBoxExtractPrivate, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    CheckBoxExtractStatic = new wxCheckBox(Panel3, ID_CHECKBOX_EXTRACTSTATIC, _("EXTRACT_STATIC"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_EXTRACTSTATIC"));
    CheckBoxExtractStatic->SetValue(false);
    CheckBoxExtractStatic->SetToolTip(_("If the EXTRACT_STATIC tag is set to YES all static members of a file\nwill be included in the documentation.\n"));
    StaticBoxSizer5->Add(CheckBoxExtractStatic, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer4->Add(StaticBoxSizer5, 1, wxALL|wxEXPAND, 5);
    StaticBoxSizer10 = new wxStaticBoxSizer(wxVERTICAL, Panel3, _("Warnings"));
    CheckBoxWarnings = new wxCheckBox(Panel3, ID_CHECKBOX_WARNINGS, _("WARNINGS"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_WARNINGS"));
    CheckBoxWarnings->SetValue(false);
    CheckBoxWarnings->SetToolTip(_("The WARNINGS tag can be used to turn on/off the warning messages that are\ngenerated by doxygen. "));
    StaticBoxSizer10->Add(CheckBoxWarnings, 1, wxALL|wxALIGN_LEFT, 5);
    BoxSizer11 = new wxBoxSizer(wxHORIZONTAL);
    CheckBoxWarnIfDocError = new wxCheckBox(Panel3, ID_CHECKBOX_WARN_IF_DOC_ERROR, _("WARN_IF_DOC_ERROR"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_WARN_IF_DOC_ERROR"));
    CheckBoxWarnIfDocError->SetValue(false);
    CheckBoxWarnIfDocError->SetToolTip(_("If WARN_IF_DOC_ERROR is set to YES, doxygen will generate warnings for\npotential errors in the documentation, such as not documenting some\nparameters in a documented function, or documenting parameters that\ndon\'t exist or using markup commands wrongly."));
    BoxSizer11->Add(CheckBoxWarnIfDocError, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    CheckBoxWarnIfUndocumented = new wxCheckBox(Panel3, ID_CHECKBOX_WARN_IF_UNDOCUMENTED, _("WARN_IF_UNDOCUMENTED"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_WARN_IF_UNDOCUMENTED"));
    CheckBoxWarnIfUndocumented->SetValue(false);
    CheckBoxWarnIfUndocumented->SetToolTip(_("If WARN_IF_UNDOCUMENTED is set to YES, then doxygen will generate warnings\nfor undocumented members. If EXTRACT_ALL is set to YES then this flag will\nautomatically be disabled."));
    BoxSizer11->Add(CheckBoxWarnIfUndocumented, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    CheckBoxWarnNoParamdoc = new wxCheckBox(Panel3, ID_CHECKBOX_WARN_NO_PARAMDOC, _("WARN_NO_PARAMDOC"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_WARN_NO_PARAMDOC"));
    CheckBoxWarnNoParamdoc->SetValue(false);
    CheckBoxWarnNoParamdoc->SetToolTip(_("This WARN_NO_PARAMDOC option can be abled to get warnings for\nfunctions that are documented, but have no documentation for their parameters\nor return value. If set to NO (the default) doxygen will only warn about\nwrong or incomplete parameter documentation, but not about the absence of\ndocumentation."));
    BoxSizer11->Add(CheckBoxWarnNoParamdoc, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer10->Add(BoxSizer11, 1, wxALL|wxEXPAND, 5);
    BoxSizer4->Add(StaticBoxSizer10, 1, wxALL|wxEXPAND, 5);
    StaticBoxSizer6 = new wxStaticBoxSizer(wxVERTICAL, Panel3, _(" Alphabetical Class Index"));
    CheckBoxAlphabeticalIndex = new wxCheckBox(Panel3, ID_CHECKBOX_ALPHABETICAL_INDEX, _("ALPHABETICAL_INDEX"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_ALPHABETICAL_INDEX"));
    CheckBoxAlphabeticalIndex->SetValue(false);
    CheckBoxAlphabeticalIndex->SetToolTip(_("If the ALPHABETICAL_INDEX tag is set to YES, an alphabetical index\nof all compounds will be generated. Enable this if the project\ncontains a lot of classes, structs, unions or interfaces.\n"));
    StaticBoxSizer6->Add(CheckBoxAlphabeticalIndex, 1, wxALL|wxALIGN_LEFT, 5);
    BoxSizer4->Add(StaticBoxSizer6, 1, wxALL|wxEXPAND, 5);
    Panel3->SetSizer(BoxSizer4);
    BoxSizer4->Fit(Panel3);
    BoxSizer4->SetSizeHints(Panel3);
    Panel4 = new wxPanel(NotebookPrefs, ID_PANEL4, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL4"));
    BoxSizer16 = new wxBoxSizer(wxVERTICAL);
    StaticBoxSizer7 = new wxStaticBoxSizer(wxVERTICAL, Panel4, _("Output"));
    BoxSizer12 = new wxBoxSizer(wxVERTICAL);
    CheckBoxGenerateHTML = new wxCheckBox(Panel4, ID_CHECKBOX_GENERATE_HTML, _("GENERATE_HTML"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_GENERATE_HTML"));
    CheckBoxGenerateHTML->SetValue(true);
    CheckBoxGenerateHTML->SetToolTip(_("If the GENERATE_HTML tag is set to YES (the default) Doxygen will\ngenerate HTML output.\n"));
    BoxSizer12->Add(CheckBoxGenerateHTML, 1, wxALL|wxALIGN_LEFT, 5);
    BoxSizer17 = new wxBoxSizer(wxHORIZONTAL);
    CheckBoxGenerateHTMLHelp = new wxCheckBox(Panel4, ID_CHECKBOX_GENERATE_HTMLHELP, _("GENERATE_HTMLHELP"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_GENERATE_HTMLHELP"));
    CheckBoxGenerateHTMLHelp->SetValue(false);
    CheckBoxGenerateHTMLHelp->SetToolTip(_("If the GENERATE_HTMLHELP tag is set to YES, additional index files\nwill be generated that can be used as input for tools like the\nMicrosoft HTML help workshop to generate a compiled HTML help file (.chm)\nof the generated HTML documentation."));
    BoxSizer17->Add(CheckBoxGenerateHTMLHelp, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    CheckBoxGenerateCHI = new wxCheckBox(Panel4, ID_CHECKBOX_GENERATE_CHI, _("GENERATE_CHI"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_GENERATE_CHI"));
    CheckBoxGenerateCHI->SetValue(false);
    CheckBoxGenerateCHI->SetToolTip(_("If the GENERATE_HTMLHELP tag is set to YES, the GENERATE_CHI flag\ncontrols if a separate .chi index file is generated (YES) or that\nit should be included in the master .chm file (NO)."));
    BoxSizer17->Add(CheckBoxGenerateCHI, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    CheckBoxBinaryTOC = new wxCheckBox(Panel4, ID_CHECKBOX_BINARY_TOC, _("BINARY_TOC"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_BINARY_TOC"));
    CheckBoxBinaryTOC->SetValue(false);
    CheckBoxBinaryTOC->SetToolTip(_("If the GENERATE_HTMLHELP tag is set to YES, the BINARY_TOC flag\ncontrols whether a binary table of contents is generated (YES) or a\nnormal table of contents (NO) in the .chm file."));
    BoxSizer17->Add(CheckBoxBinaryTOC, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer12->Add(BoxSizer17, 1, wxALL|wxEXPAND, 5);
    StaticBoxSizer7->Add(BoxSizer12, 1, wxEXPAND, 5);
    BoxSizer14 = new wxBoxSizer(wxHORIZONTAL);
    CheckBoxGenerateLatex = new wxCheckBox(Panel4, ID_CHECKBOX_GENERATE_LATEX, _("GENERATE_LATEX"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_GENERATE_LATEX"));
    CheckBoxGenerateLatex->SetValue(false);
    CheckBoxGenerateLatex->SetToolTip(_("If the GENERATE_LATEX tag is set to YES (the default) Doxygen will\ngenerate Latex output.\n"));
    BoxSizer14->Add(CheckBoxGenerateLatex, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    CheckBoxGenerateRTF = new wxCheckBox(Panel4, ID_CHECKBOX_GENERATE_RTF, _("GENERATE_RTF"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_GENERATE_RTF"));
    CheckBoxGenerateRTF->SetValue(false);
    CheckBoxGenerateRTF->SetToolTip(_("If the GENERATE_RTF tag is set to YES Doxygen will generate RTF output\nThe RTF output is optimized for Word 97 and may not look very pretty with\nother RTF readers or editors."));
    BoxSizer14->Add(CheckBoxGenerateRTF, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    CheckBoxGenerateMan = new wxCheckBox(Panel4, ID_CHECKBOX_GENERATE_MAN, _("GENERATE_MAN"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_GENERATE_MAN"));
    CheckBoxGenerateMan->SetValue(true);
    CheckBoxGenerateMan->SetToolTip(_("If the GENERATE_MAN tag is set to YES (the default) Doxygen will\ngenerate man pages"));
    BoxSizer14->Add(CheckBoxGenerateMan, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer7->Add(BoxSizer14, 1, wxEXPAND, 5);
    BoxSizer15 = new wxBoxSizer(wxHORIZONTAL);
    CheckBoxGenerateXML = new wxCheckBox(Panel4, ID_CHECKBOX_GENERATE_XML, _("GENERATE_XML"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_GENERATE_XML"));
    CheckBoxGenerateXML->SetValue(false);
    CheckBoxGenerateXML->SetToolTip(_("If the GENERATE_XML tag is set to YES Doxygen will\ngenerate an XML file that captures the structure of\nthe code including all documentation."));
    BoxSizer15->Add(CheckBoxGenerateXML, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    CheckBoxGenerateAutogenDef = new wxCheckBox(Panel4, ID_CHECKBOX_GENERATE_AUTOGEN_DEF, _("GENERATE_AUTOGEN_DEF"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_GENERATE_AUTOGEN_DEF"));
    CheckBoxGenerateAutogenDef->SetValue(false);
    CheckBoxGenerateAutogenDef->SetToolTip(_("If the GENERATE_AUTOGEN_DEF tag is set to YES Doxygen will\ngenerate an AutoGen Definitions (see autogen.sf.net) file\nthat captures the structure of the code including all\ndocumentation. Note that this feature is still experimental\nand incomplete at the moment.\n"));
    BoxSizer15->Add(CheckBoxGenerateAutogenDef, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    CheckBoxGeneratePerlMod = new wxCheckBox(Panel4, ID_CHECKBOX_GENERATE_PERLMOD, _("GENERATE_PERLMOD"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_GENERATE_PERLMOD"));
    CheckBoxGeneratePerlMod->SetValue(false);
    CheckBoxGeneratePerlMod->SetToolTip(_("If the GENERATE_PERLMOD tag is set to YES Doxygen will\ngenerate a Perl module file that captures the structure of\nthe code including all documentation. Note that this\nfeature is still experimental and incomplete at the\nmoment."));
    BoxSizer15->Add(CheckBoxGeneratePerlMod, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer7->Add(BoxSizer15, 1, wxEXPAND, 5);
    BoxSizer16->Add(StaticBoxSizer7, 3, wxALL|wxEXPAND, 5);
    StaticBoxSizer8 = new wxStaticBoxSizer(wxHORIZONTAL, Panel4, _("Pre-processor"));
    CheckBoxEnablePreprocessing = new wxCheckBox(Panel4, ID_CHECKBOX_ENABLE_PREPROCESSING, _("ENABLE_PREPROCESSING"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_ENABLE_PREPROCESSING"));
    CheckBoxEnablePreprocessing->SetValue(false);
    CheckBoxEnablePreprocessing->SetToolTip(_("If the ENABLE_PREPROCESSING tag is set to YES (the default) Doxygen will\nevaluate all C-preprocessor directives found in the sources and include\nfiles."));
    StaticBoxSizer8->Add(CheckBoxEnablePreprocessing, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer16->Add(StaticBoxSizer8, 1, wxALL|wxEXPAND, 5);
    StaticBoxSizer9 = new wxStaticBoxSizer(wxHORIZONTAL, Panel4, _("Dot"));
    CheckBoxClassDiagrams = new wxCheckBox(Panel4, ID_CHECKBOX_CLASS_DIAGRAMS, _("CLASS_DIAGRAMS"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_CLASS_DIAGRAMS"));
    CheckBoxClassDiagrams->SetValue(false);
    CheckBoxClassDiagrams->SetToolTip(_("If the CLASS_DIAGRAMS tag is set to YES (the default) Doxygen will\ngenerate a inheritance diagram (in HTML, RTF and LaTeX) for classes with base\nor super classes. Setting the tag to NO turns the diagrams off. Note that\nthis option is superseded by the HAVE_DOT option below. This is only a\nfallback. It is recommended to install and use dot, since it yields more\npowerful graphs."));
    StaticBoxSizer9->Add(CheckBoxClassDiagrams, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    CheckBoxHaveDot = new wxCheckBox(Panel4, ID_CHECKBOX_HAVE_DOT, _("HAVE_DOT"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_HAVE_DOT"));
    CheckBoxHaveDot->SetValue(false);
    CheckBoxHaveDot->SetToolTip(_("If you set the HAVE_DOT tag to YES then doxygen will assume the dot tool is\navailable from the path. This tool is part of Graphviz, a graph visualization\ntoolkit from AT&T and Lucent Bell Labs. The other options in this section\nhave no effect if this option is set to NO (the default)"));
    StaticBoxSizer9->Add(CheckBoxHaveDot, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer16->Add(StaticBoxSizer9, 1, wxALL|wxEXPAND, 5);
    Panel4->SetSizer(BoxSizer16);
    BoxSizer16->Fit(Panel4);
    BoxSizer16->SetSizeHints(Panel4);
    Panel1 = new wxPanel(NotebookPrefs, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    BoxSizer7 = new wxBoxSizer(wxVERTICAL);
    StaticBoxSizer4 = new wxStaticBoxSizer(wxVERTICAL, Panel1, _("Paths"));
    BoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
    StaticText2 = new wxStaticText(Panel1, ID_STATICTEXT2, _("Path To doxygen"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    BoxSizer8->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlPathDoxygen = new wxTextCtrl(Panel1, ID_TEXTCTRL_PATHDOXYGEN, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL_PATHDOXYGEN"));
    TextCtrlPathDoxygen->SetToolTip(_("The path to the doxygen executable file.\nIf this field is blank, DoxyBlocks will assume that doxygen\nis available somewhere in your environment\'s path.\nYou can use macros such as $(CODEBLOCKS) in your path\nand they will be expanded automatically."));
    BoxSizer8->Add(TextCtrlPathDoxygen, 3, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    ButtonBrowseDoxygen = new wxButton(Panel1, ID_BUTTON_BROWSEDOXYGEN, _("Browse"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_BROWSEDOXYGEN"));
    ButtonBrowseDoxygen->SetToolTip(_("Browse for doxygen."));
    BoxSizer8->Add(ButtonBrowseDoxygen, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer4->Add(BoxSizer8, 1, wxALL|wxEXPAND, 5);
    BoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    StaticText4 = new wxStaticText(Panel1, ID_STATICTEXT4, _("Path To doxywizard"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
    BoxSizer9->Add(StaticText4, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlPathDoxywizard = new wxTextCtrl(Panel1, ID_TEXTCTRL_PATHDOXYWIZARD, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL_PATHDOXYWIZARD"));
    TextCtrlPathDoxywizard->SetToolTip(_("The path to the doxywizard executable file.\nIf this field is blank, DoxyBlocks will assume that doxywizard\nis available somewhere in your environment\'s path.\nYou can use macros such as $(CODEBLOCKS) in your path\nand they will be expanded automatically."));
    BoxSizer9->Add(TextCtrlPathDoxywizard, 3, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    ButtonBrowseDoxywizard = new wxButton(Panel1, ID_BUTTON_BROWSEDOXYWIZARD, _("Browse"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_BROWSEDOXYWIZARD"));
    ButtonBrowseDoxywizard->SetToolTip(_("Browse for doxywizard."));
    BoxSizer9->Add(ButtonBrowseDoxywizard, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer4->Add(BoxSizer9, 1, wxALL|wxEXPAND, 5);
    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    StaticText3 = new wxStaticText(Panel1, ID_STATICTEXT3, _("Path To HHC"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    BoxSizer3->Add(StaticText3, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlPathHHC = new wxTextCtrl(Panel1, ID_TEXTCTRL_PATHHHC, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL_PATHHHC"));
    TextCtrlPathHHC->SetToolTip(_("The path to the HTML Help Compiler executable file.\nIf this field is blank, DoxyBlocks will assume that HHC\nis available somewhere in your environment\'s path.\nYou can use macros such as $(CODEBLOCKS) in your path\nand they will be expanded automatically."));
    BoxSizer3->Add(TextCtrlPathHHC, 3, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    ButtonBrowseHHC = new wxButton(Panel1, ID_BUTTON_BROWSEHHC, _("Browse"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_BROWSEHHC"));
    ButtonBrowseHHC->SetToolTip(_("Browse for HHC."));
    BoxSizer3->Add(ButtonBrowseHHC, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer4->Add(BoxSizer3, 1, wxALL|wxEXPAND, 5);
    BoxSizer13 = new wxBoxSizer(wxHORIZONTAL);
    StaticText6 = new wxStaticText(Panel1, ID_STATICTEXT6, _("Path To Dot"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
    BoxSizer13->Add(StaticText6, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlPathDot = new wxTextCtrl(Panel1, ID_TEXTCTRL_PATHDOT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL_PATHDOT"));
    TextCtrlPathDot->SetToolTip(_("The path to the dot executable file.\nIf this field is blank, DoxyBlocks will assume that dot\nis available somewhere in your environment\'s path.\nYou can use macros such as $(CODEBLOCKS) in your path\nand they will be expanded automatically."));
    BoxSizer13->Add(TextCtrlPathDot, 3, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    ButtonBrowseDot = new wxButton(Panel1, ID_BUTTON_BROWSEDOT, _("Browse"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_BROWSEDOT"));
    ButtonBrowseDot->SetToolTip(_("Browse for Dot."));
    BoxSizer13->Add(ButtonBrowseDot, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer4->Add(BoxSizer13, 1, wxALL|wxEXPAND, 5);
    BoxSizer19 = new wxBoxSizer(wxHORIZONTAL);
    StaticText7 = new wxStaticText(Panel1, ID_STATICTEXT7, _("Path To CHM Viewer"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
    BoxSizer19->Add(StaticText7, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlPathCHMViewer = new wxTextCtrl(Panel1, ID_TEXTCTRL_PATHCHMVIEWER, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL_PATHCHMVIEWER"));
    TextCtrlPathCHMViewer->SetToolTip(_("The path to the CHM viewer executable file.\nOn Windows, if this field is blank, DoxyBlocks will use\nthe operating system\'s standard viewer.\nOn other operating systems, if this field is blank,\nDoxyBlocks will assume that a viewer is available\nsomewhere in your environment\'s path.\nYou can use macros such as $(CODEBLOCKS) in your path\nand they will be expanded automatically."));
    BoxSizer19->Add(TextCtrlPathCHMViewer, 3, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    ButtonBrowseCHMViewer = new wxButton(Panel1, ID_BUTTON_BROWSECHMVIEWER, _("Browse"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_BROWSECHMVIEWER"));
    ButtonBrowseCHMViewer->SetToolTip(_("Browse for a CHM viewer."));
    BoxSizer19->Add(ButtonBrowseCHMViewer, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer4->Add(BoxSizer19, 1, wxALL|wxEXPAND, 5);
    BoxSizer7->Add(StaticBoxSizer4, 3, wxALL|wxEXPAND, 5);
    StaticBoxSizer11 = new wxStaticBoxSizer(wxVERTICAL, Panel1, _("General Options"));
    BoxSizer21 = new wxBoxSizer(wxHORIZONTAL);
    CheckBoxOverwriteDoxyfile = new wxCheckBox(Panel1, ID_CHECKBOX_OVERWRITEDOXYFILE, _("Overwrite Doxyfile"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_OVERWRITEDOXYFILE"));
    CheckBoxOverwriteDoxyfile->SetValue(false);
    CheckBoxOverwriteDoxyfile->SetToolTip(_("Allow DoxyBlocks to overwrite the doxyfile.\nBy default, if a doxyfile already exists it is not overwritten to protect\nany changes that have been made outside DoxyBlocks however this\nbehaviour prevents changes made within DoxyBlocks being written to\nan existing doxyfile."));
    BoxSizer21->Add(CheckBoxOverwriteDoxyfile, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    CheckBoxPromptBeforeOverwriting = new wxCheckBox(Panel1, ID_CHECKBOX_PROMPTB4OVERWRITING, _("Prompt Before Overwriting"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_PROMPTB4OVERWRITING"));
    CheckBoxPromptBeforeOverwriting->SetValue(false);
    CheckBoxPromptBeforeOverwriting->SetToolTip(_("Prompt before overwriting an existing doxyfile.\nWhen used in conjunction with \"Overwrite doxyfile\" this setting forces\nDoxyBlocks to prompt you before overwriting an existing doxyfile."));
    BoxSizer21->Add(CheckBoxPromptBeforeOverwriting, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    CheckBoxUseAtInTags = new wxCheckBox(Panel1, ID_CHECKBOX_USEATINTAGS, _("Use \"@\" In Tags"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_USEATINTAGS"));
    CheckBoxUseAtInTags->SetValue(false);
    CheckBoxUseAtInTags->SetToolTip(_("Use \"@\" in document tags instead of \"\\\".\nThis option causes DoxyBlocks to create document tags like\n\"@brief\" and \"@param\" rather than \"\\brief\" and \"\\param\"."));
    BoxSizer21->Add(CheckBoxUseAtInTags, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer11->Add(BoxSizer21, 1, wxEXPAND, 5);
    BoxSizer22 = new wxBoxSizer(wxHORIZONTAL);
    CheckBoxLoadTemplate = new wxCheckBox(Panel1, ID_CHECKBOX_LOADTEMPLATE, _("Load Settings Template If No Saved Settings Exist"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_LOADTEMPLATE"));
    CheckBoxLoadTemplate->SetValue(false);
    CheckBoxLoadTemplate->SetToolTip(_("This option allows you to use your preferred settings as the default settings by loading a previously saved settings template, if one exists, rather than using the system default settings when loading a project that doesn\'t contain saved DoxyBlocks settings. DoxyBlocks falls back to the default settings if a template is not found."));
    BoxSizer22->Add(CheckBoxLoadTemplate, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer11->Add(BoxSizer22, 1, wxEXPAND, 5);
    BoxSizer7->Add(StaticBoxSizer11, 1, wxALL|wxEXPAND, 5);
    StaticBoxSizer12 = new wxStaticBoxSizer(wxHORIZONTAL, Panel1, _("HTML"));
    CheckBoxUseInternalViewer = new wxCheckBox(Panel1, ID_CHECKBOX_USEINTERNALVIEWER, _("Use Internal Viewer"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_USEINTERNALVIEWER"));
    CheckBoxUseInternalViewer->SetValue(false);
    CheckBoxUseInternalViewer->SetToolTip(_("Use the internal viewer to view HTML docs.\nIf this option is not set DoxyBlocks will use the system\'s default\nbrowser.\nThis setting has no effect if GENERATE_HTML is not set."));
    StaticBoxSizer12->Add(CheckBoxUseInternalViewer, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    CheckBoxRunHTML = new wxCheckBox(Panel1, ID_CHECKBOX_RUNHTML, _("Run HTML After Compilation"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_RUNHTML"));
    CheckBoxRunHTML->SetValue(false);
    CheckBoxRunHTML->SetToolTip(_("Run the HTML docs after compilation.\nIf \"Use Internal Viewer\" is set DoxyBlocks will use Code::Blocks\' internal\nviewer. If not, it will use the system\'s default browser.\nThis setting has no effect if GENERATE_HTML is not set."));
    StaticBoxSizer12->Add(CheckBoxRunHTML, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    CheckBoxRunCHM = new wxCheckBox(Panel1, ID_CHECKBOX_RUNCHM, _("Run CHM After Compilation"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX_RUNCHM"));
    CheckBoxRunCHM->SetValue(false);
    CheckBoxRunCHM->SetToolTip(_("Run the compiled help after it\'s compiled.\nThis setting currently only has effect on Windows.\nIf no CHM file exists this setting is ignored.\nThis setting has no effect if GENERATE_HTML is not set."));
    StaticBoxSizer12->Add(CheckBoxRunCHM, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer7->Add(StaticBoxSizer12, 1, wxALL|wxEXPAND, 5);
    Panel1->SetSizer(BoxSizer7);
    BoxSizer7->Fit(Panel1);
    BoxSizer7->SetSizeHints(Panel1);
    NotebookPrefs->AddPage(Panel2, _("Comment Style"), false);
    NotebookPrefs->AddPage(Panel3, _("Doxyfile Defaults"), false);
    NotebookPrefs->AddPage(Panel4, _("Doxyfile Defaults 2"), false);
    NotebookPrefs->AddPage(Panel1, _("General"), false);
    BoxSizer1->Add(NotebookPrefs, 1, wxALL|wxEXPAND, 5);
    SetSizer(BoxSizer1);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);

    Connect(ID_RADIOBOX_BLOCKCOMMENTS,wxEVT_COMMAND_RADIOBOX_SELECTED,(wxObjectEventFunction)&ConfigPanel::OnRadioBoxBlockCommentsSelect);
    Connect(ID_RADIOBOX_LINECOMMENTS,wxEVT_COMMAND_RADIOBOX_SELECTED,(wxObjectEventFunction)&ConfigPanel::OnRadioBoxLineCommentsSelect);
    Connect(ID_CHECKBOX_USEAUTOVERSION,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&ConfigPanel::OnCheckBoxUseAutoversionClick);
    Connect(ID_CHECKBOX_WARNINGS,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&ConfigPanel::OnCheckBoxWarningsClick);
    Connect(ID_CHECKBOX_GENERATE_HTML,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&ConfigPanel::OnCheckBoxGenerateHTMLClick);
    Connect(ID_BUTTON_BROWSEDOXYGEN,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ConfigPanel::OnButtonBrowseDoxygenClick);
    Connect(ID_BUTTON_BROWSEDOXYWIZARD,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ConfigPanel::OnButtonBrowseDoxywizardClick);
    Connect(ID_BUTTON_BROWSEHHC,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ConfigPanel::OnButtonBrowseHHCClick);
    Connect(ID_BUTTON_BROWSEDOT,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ConfigPanel::OnButtonBrowseDotClick);
    Connect(ID_BUTTON_BROWSECHMVIEWER,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ConfigPanel::OnButtonBrowseCHMViewerClick);
    Connect(ID_CHECKBOX_OVERWRITEDOXYFILE,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&ConfigPanel::OnCheckBoxOverwriteDoxyfileClick);
    Connect(ID_CHECKBOX_USEATINTAGS,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&ConfigPanel::OnCheckBoxUseAtInTagsClick);
    //*)

}

/*! \brief Destructor
 */
ConfigPanel::~ConfigPanel()
{
    //(*Destroy(ConfigPanel)
    //*)
}

/*! \brief Initialise dependent control states.
 *
 * \return void
 *
 * This function must be called after the setters have been called so that initial
 * states are set first.
 */
void ConfigPanel::Init()
{
    //    If the project uses autoversioning, enable the "Use AutoVersioning" checkbox.
    if(m_bAutoVersioning){
        CheckBoxUseAutoVersion->Enable(true);
    }
    if(!CheckBoxOverwriteDoxyfile->IsChecked()){
        CheckBoxPromptBeforeOverwriting->Enable(false);
    }
    WriteBlockComment(TextCtrlBlockComment, RadioBoxBlockComments->GetSelection(), CheckBoxUseAtInTags->GetValue());
    WriteLineComment(TextCtrlLineComment, RadioBoxLineComments->GetSelection());
    InitSTC(TextCtrlBlockComment);
    InitSTC(TextCtrlLineComment);
    if(m_bAutoVersioning){
        CheckBoxUseAutoVersion->SetValue(m_bUseAutoVersion);
        // If "Use AutoVersion" is enabled, disable the version text box so that manual entry is
        // disallowed but the detected version is still visible.
        if(m_bUseAutoVersion){
            TextCtrlProjectNumber->Enable(false);
        }
    }

    if(CheckBoxGenerateHTML->IsChecked()){
        CheckBoxGenerateHTMLHelp->Enable(true);
        CheckBoxGenerateCHI->Enable(true);
        CheckBoxBinaryTOC->Enable(true);
    }
    else{
        CheckBoxGenerateHTMLHelp->Enable(false);
        CheckBoxGenerateCHI->Enable(false);
        CheckBoxBinaryTOC->Enable(false);
    }

    if(CheckBoxWarnings->IsChecked()){
        CheckBoxWarnIfDocError->Enable(true);
        CheckBoxWarnIfUndocumented->Enable(true);
        CheckBoxWarnNoParamdoc->Enable(true);
    }
    else{
        CheckBoxWarnIfDocError->Enable(false);
        CheckBoxWarnIfUndocumented->Enable(false);
        CheckBoxWarnNoParamdoc->Enable(false);
    }
}

/*! \brief Initialise a wxStyledTextControl.
 *
 * \param    stc    cbStyledTextCtrl*    The text editor control.
 *
 * This function initialises the styled text controls used for the comment samples.
 * It sets the lexer, margin, font and other parameters and colourises the text.
 */
void ConfigPanel::InitSTC(cbStyledTextCtrl *stc)
{
    stc->SetLexer(wxSCI_LEX_CPP);
    stc->SetMarginType(0, wxSCI_MARGIN_NUMBER);
    stc->SetMarginWidth(0, 32);
    stc->SetTabWidth(4);
    // Try to stop flickering.
    stc->SetBufferedDraw(true);
    stc->SetReadOnly(true);
    stc->SetUseHorizontalScrollBar(false);

    ConfigManager* cfg = Manager::Get()->GetConfigManager(wxT("editor"));
    wxString sFont = cfg->Read(wxT("/font"), wxEmptyString);
    wxFont fnt(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    if (!sFont.IsEmpty())
    {
        wxNativeFontInfo nfi;
        nfi.FromString(sFont);
        fnt.SetNativeFontInfo(nfi);
    }

    if (stc)
    {
        // Colourise.
        EditorColourSet* colour_set = Manager::Get()->GetEditorManager()->GetColourSet();
        if (!colour_set)
            return;
        stc->StyleSetFont(wxSCI_STYLE_DEFAULT, fnt);
        colour_set->Apply(colour_set->GetHighlightLanguage(wxT("C/C++")), stc);
    }
}

/*! \brief Write the block comment sample, based on the radio box selection.
 *
 * \param    stc                        cbStyledTextCtrl*    The text editor control.
 * \param    iBlockComment     int                        The comment style.
 * \param     bUseAtInTags         bool                        Whether or not to use "@" in tags rather than "\".
 *
 */
void ConfigPanel::WriteBlockComment(cbStyledTextCtrl *stc, int iBlockComment, bool bUseAtInTags)
{
    wxString sTagChar = bUseAtInTags ? wxT("@") : wxT("\\");
    wxString sSpace(wxT(" "));
    // Comment strings.
    wxString sBrief(sSpace + _("This is a brief description."));
    wxString sParam1(wxString(wxT("\tp1 ")) + _("The first parameter."));
    wxString sParam2(wxString(wxT("\tp2 "))  + _("The second parameter."));
    wxString sReturn(wxString(wxT("\t")) + _("Return value."));
    wxString sDetailed(sSpace + _("This is a detailed description."));
    wxString sFunction(wxT("wxString Function(wxInt32 p1, wxString p2)"));
    wxString sBrace(wxT("{"));
    wxString sTagBrief(sSpace + sTagChar + wxT("brief"));
    wxString sTagParam(sSpace + sTagChar + wxT("param"));
    wxString sTagReturn(sSpace + sTagChar + wxT("return"));
    wxString sStartComment;
    wxString sMidComment;
    wxString sEndComment;

    // Allow for the greater space used by C++ slashes. I like things neat. :-)
    if(iBlockComment == 1 || iBlockComment == 2 || iBlockComment == 5){
        sReturn = wxString(wxT("\t\t")) + _("Return value.");
    }
    switch(iBlockComment){
        case 0:            //C/Javadoc.
            sStartComment = wxT("/**");
            sMidComment = wxT(" *");
            sEndComment = wxT(" */");
            break;
        case 1:            // C++ exclamation.
            sStartComment = wxT("//!");
            sMidComment = wxT("//!");
            sEndComment = wxT("//!");
            break;
        case 2:            // C++ slash.
            sStartComment = wxT("///");
            sMidComment = wxT("///");
            sEndComment = wxT("///");
            break;
        case 3:            // Qt.
            sStartComment = wxT("/*!");
            sMidComment = wxT(" *");
            sEndComment = wxT(" */");
            break;
        case 4:            // Visible C.
            sStartComment = wxT("/********************************************//**");
            sMidComment = wxT(" *");
            sEndComment = wxT(" ***********************************************/");
            break;
        case 5:            // Visible C++.
            sStartComment = wxT("/////////////////////////////////////////////////");
            sMidComment = wxT("///");
            sEndComment = wxT("/////////////////////////////////////////////////");
            break;
        default:
            break;
    }
    if(iBlockComment == 4 || iBlockComment == 5){
        stc->AddText(sStartComment);
        stc->NewLine();
        stc->AddText(sMidComment + sTagBrief + sBrief);
    }
    else{
        stc->AddText(sStartComment + sTagBrief + sBrief);
    }
    // Use NewLine() to get the correct line ending chars.
    stc->NewLine();
    stc->AddText(sMidComment);
    stc->NewLine();
    stc->AddText(sMidComment +sTagParam + sParam1);
    stc->NewLine();
    stc->AddText(sMidComment + sTagParam + sParam2);
    stc->NewLine();
    stc->AddText(sMidComment + sTagReturn + sReturn);
    stc->NewLine();
    stc->AddText(sMidComment);
    stc->NewLine();
    stc->AddText(sMidComment + sDetailed);
    stc->NewLine();
    stc->AddText(sEndComment);
    stc->NewLine();
    stc->AddText(sFunction);
    stc->NewLine();
    stc->AddText(sBrace);

    stc->ScrollToLine(0);
}

/*! \brief Write the line comment sample, based on the radio box selection.
 *
 * \param    stc                        cbStyledTextCtrl*    The text editor control.
 * \param    iLineComment         int                        The comment style.
 *
 */
void ConfigPanel::WriteLineComment(cbStyledTextCtrl *stc, int iLineComment)
{
    // Comment strings.
    wxString sGlobal(wxT("wxInt32\tiGlob;\t"));
    wxString sComment(_("This is an inline comment."));
    wxString sEnum(wxT("enum Enum{"));
    wxString sVal1(wxT("\tval1\t\t\t\t\t"));
    wxString sVal2(wxT("\tval2\t\t\t\t\t"));
    wxString sFunction(wxT("wxString Function(wxInt32 p1, wxString p2)"));
    wxString sFuncVal(wxT("\twxInt32\tiVal;\t"));
    wxString sOpenBrace(wxT("{"));
    wxString sCloseBrace(wxT("}"));
    wxString sStartComment;
    wxString sEndComment;

    switch(iLineComment){
        case 0:            //C/Javadoc.
            sStartComment = wxT("/**< ");
            sEndComment = wxT(" */");
            break;
        case 1:            // C++ exclamation.
            sStartComment = wxT("//!< ");
            break;
        case 2:            // C++ slash.
            sStartComment = wxT("///< ");
            break;
        case 3:            // Qt.
            sStartComment = wxT("/*!< ");
            sEndComment = wxT(" */");
            break;
        default:
            break;
    }
    stc->AddText(sGlobal + sStartComment + sComment + sEndComment);
    // Use NewLine() to get the correct line ending chars.
    stc->NewLine();
    stc->NewLine();
    stc->AddText(sEnum);
    stc->NewLine();
    stc->AddText(sVal1 + sStartComment + sComment + sEndComment);
    stc->NewLine();
    stc->AddText(sVal2 + sStartComment + sComment + sEndComment);
    stc->NewLine();
    stc->AddText(sCloseBrace);
    stc->NewLine();
    stc->NewLine();
    stc->AddText(sFunction);
    stc->NewLine();
    stc->AddText(sOpenBrace);
    stc->NewLine();
    stc->AddText(sFuncVal + sStartComment + sComment + sEndComment);
}

/*! \brief Apply configuration changes.
 *
 * Virtual routine required by cbConfigurationPanel
 */
void ConfigPanel::OnApply()
{
    m_pOwnerClass->OnDialogueDone(this);
 }

/*! \brief Get the base name for the images to be displayed in the settings image list.
 *
 * \return wxString    The bitmap base name.
 *
 * Virtual routine required by cbConfigurationPanel
 */
wxString ConfigPanel::GetBitmapBaseName() const
{
    wxString pngName = wxT("generic-plugin");
    #ifdef __WXGTK__
     if ( ::wxFileExists(ConfigManager::GetDataFolder() + wxT("/images/settings/DoxyBlocks.png")) )
    #else
     if ( ::wxFileExists(ConfigManager::GetDataFolder() + wxT("\\images\\settings\\DoxyBlocks.png")) )
    #endif
        pngName = wxT("DoxyBlocks");
    return pngName;
}

/*! \brief The doxygen path browse button was clicked.
 *
 * \param     event    A wxCommandEvent reference.
 *
 */
void ConfigPanel::OnButtonBrowseDoxygenClick(wxCommandEvent& WXUNUSED(event))
{
    wxString sPathDoxygen = GetApplicationPath();
    if(!sPathDoxygen.empty()){
        TextCtrlPathDoxygen->SetValue(sPathDoxygen);
    }
}

/*! \brief The doxywizard path browse button was clicked.
 *
 * \param     event    wxCommandEvent&    A wxCommandEvent object.
 *
 */
void ConfigPanel::OnButtonBrowseDoxywizardClick(wxCommandEvent& WXUNUSED(event))
{
    wxString sPathDoxywizard = GetApplicationPath();
    if(!sPathDoxywizard.empty()){
        TextCtrlPathDoxywizard->SetValue(sPathDoxywizard);
    }
}

/*! \brief The HHC path browse button was clicked.
 *
 * \param     event    wxCommandEvent&    A wxCommandEvent object.
 *
 */
void ConfigPanel::OnButtonBrowseHHCClick(wxCommandEvent& WXUNUSED(event))
{
    wxString sPathHHC = GetApplicationPath();
    if(!sPathHHC.empty()){
        TextCtrlPathHHC->SetValue(sPathHHC);
    }
}

/*! \brief The Dot path browse button was clicked.
 *
 * \param     event    wxCommandEvent&    A wxCommandEvent object.
 *
 */
void ConfigPanel::OnButtonBrowseDotClick(wxCommandEvent& WXUNUSED(event))
{
    wxString sPathDot = GetApplicationPath();
    if(!sPathDot.empty()){
        TextCtrlPathDot->SetValue(sPathDot);
    }
}

/*! \brief The CHM Viewer path browse button was clicked.
 *
 * \param     event    wxCommandEvent&    A wxCommandEvent object.
 *
 */
void ConfigPanel::OnButtonBrowseCHMViewerClick(wxCommandEvent& WXUNUSED(event))
{
    wxString sPathCHMViewer = GetApplicationPath();
    if(!sPathCHMViewer.empty()){
        TextCtrlPathCHMViewer->SetValue(sPathCHMViewer);
    }
}

/*! \brief Display a file dialogue and return the selected path.
 *
 * \return     wxString    The application path.
 *
 */
wxString ConfigPanel::GetApplicationPath()
{
#ifdef __WXMSW__
    // Windows executables.
    wxString sAppFiles(_("Executable Files (*.exe)|*.exe"));
#else
    // Everyone else.
    wxString sAppFiles(_("All Files (*.*)|*.*"));
#endif
    return wxFileSelector(_("Path to application file"), wxEmptyString, wxEmptyString, wxEmptyString, sAppFiles, wxFD_OPEN|wxFD_FILE_MUST_EXIST, this);
}

/*! \brief A block comment radio button was selected.
 *
 * \param     event    wxCommandEvent&    A wxCommandEvent object.
 *
 */
void ConfigPanel::OnRadioBoxBlockCommentsSelect(wxCommandEvent& event)
{
    // We can't clear or write while readonly is set. :-(
    TextCtrlBlockComment->SetReadOnly(false);
    TextCtrlBlockComment->ClearAll();
    bool bUseAtInTags = CheckBoxUseAtInTags->IsChecked();
    WriteBlockComment(TextCtrlBlockComment, event.GetSelection(), bUseAtInTags);
    TextCtrlBlockComment->SetReadOnly(true);
}

/*! \brief A line comment radio button was selected.
 *
 * \param     event    wxCommandEvent&    A wxCommandEvent object.
 *
 */
void ConfigPanel::OnRadioBoxLineCommentsSelect(wxCommandEvent& event)
{
    // We can't clear or write while readonly is set. :-(
    TextCtrlLineComment->SetReadOnly(false);
    TextCtrlLineComment->ClearAll();
    WriteLineComment(TextCtrlLineComment, event.GetSelection());
    TextCtrlLineComment->SetReadOnly(true);
}

/*! \brief The Generate HTML check box was clicked. Enable or disable dependent checkboxes.
 *
 * \param     event    wxCommandEvent&    A wxCommandEvent object.
 *
 */
void ConfigPanel::OnCheckBoxGenerateHTMLClick(wxCommandEvent& event)
{
    if(event.IsChecked()){
        CheckBoxGenerateHTMLHelp->Enable(true);
        CheckBoxGenerateCHI->Enable(true);
        CheckBoxBinaryTOC->Enable(true);
    }
    else{
        CheckBoxGenerateHTMLHelp->Enable(false);
        CheckBoxGenerateCHI->Enable(false);
        CheckBoxBinaryTOC->Enable(false);
    }
}

/*! \brief The Overwrite Doxyfile check box was clicked. Enable or disable dependent checkboxes.
 *
 * \param     event    wxCommandEvent&    A wxCommandEvent object.
 *
 */
void ConfigPanel::OnCheckBoxOverwriteDoxyfileClick(wxCommandEvent& event)
{
    if(event.IsChecked()){
        CheckBoxPromptBeforeOverwriting->Enable(true);
    }
    else{
        CheckBoxPromptBeforeOverwriting->Enable(false);
    }
}

/*! \brief The Warnings check box was clicked. Enable or disable dependent checkboxes.
 *
 * \param     event    wxCommandEvent&    A wxCommandEvent object.
 *
 */
void ConfigPanel::OnCheckBoxWarningsClick(wxCommandEvent& event)
{
    if(event.IsChecked()){
        CheckBoxWarnIfDocError->Enable(true);
        CheckBoxWarnIfUndocumented->Enable(true);
        CheckBoxWarnNoParamdoc->Enable(true);
    }
    else{
        CheckBoxWarnIfDocError->Enable(false);
        CheckBoxWarnIfUndocumented->Enable(false);
        CheckBoxWarnNoParamdoc->Enable(false);
    }
}

/*! \brief The "Use AutoVersion" checkbox was clicked. Update the "Project Number" text control.
 *
 * \param     event    wxCommandEvent&    A wxCommandEvent object.
 * \return     void
 *
 * This function disables the "Project Number" text control if "Use AutoVersion" is selected.
 * This prevents manual entry but allows the detected version number to still be visible.
 */
void ConfigPanel::OnCheckBoxUseAutoversionClick(wxCommandEvent& event)
{
    if(event.IsChecked()){
        TextCtrlProjectNumber->Enable(false);
    }
    else{
        TextCtrlProjectNumber->Enable(true);
    }
}

/*! \brief The "Use @ In Tags" checkbox was clicked. Refresh the block comment sample with the new tag.
 *
 * \param     event    wxCommandEvent&    A wxCommandEvent object.
 * \return     void
 *
 */
void ConfigPanel::OnCheckBoxUseAtInTagsClick(wxCommandEvent& WXUNUSED(event))
{
    bool bUseAltInTags = CheckBoxUseAtInTags->IsChecked();
    int iBlockComment = RadioBoxBlockComments->GetSelection();
    // We can't clear or write while readonly is set. :-(
    TextCtrlBlockComment->SetReadOnly(false);
    TextCtrlBlockComment->ClearAll();
    WriteBlockComment(TextCtrlBlockComment, iBlockComment, bUseAltInTags);
    TextCtrlBlockComment->SetReadOnly(true);
}

