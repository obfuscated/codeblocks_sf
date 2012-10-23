/**************************************************************************//**
 * \file        DoxyBlocks.h
 * \author    Gary Harris
 * \date        01-02-2010
 *
 * DoxyBlocks - doxygen integration for Code::Blocks.                    \n
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
#ifndef DOXYBLOCKS_H_INCLUDED
#define DOXYBLOCKS_H_INCLUDED

#include <cbplugin.h>
#include <cbstyledtextctrl.h>
#include <tinyxml/tinyxml.h>

#include "Config.h"

/*! \brief Toolbar control IDs
 */
const long ID_TB_WIZARD           = wxNewId();
const long ID_TB_EXTRACTPROJECT   = wxNewId();
const long ID_TB_BLOCKCOMMENT     = wxNewId();
const long ID_TB_LINECOMMENT      = wxNewId();
const long ID_TB_RUNHTML          = wxNewId();
const long ID_TB_RUNCHM           = wxNewId();
const long ID_TB_CONFIG           = wxNewId();

const long ID_MENU_DOXYBLOCKS     = wxNewId();
const long ID_MENU_DOXYWIZARD     = wxNewId();
const long ID_MENU_EXTRACTPROJECT = wxNewId();
const long ID_MENU_BLOCKCOMMENT   = wxNewId();
const long ID_MENU_LINECOMMENT    = wxNewId();
const long ID_MENU_RUNHTML        = wxNewId();
const long ID_MENU_RUNCHM         = wxNewId();
const long ID_MENU_CONFIG         = wxNewId();
const long ID_MENU_SAVE_TEMPLATE  = wxNewId();
const long ID_MENU_LOAD_TEMPLATE  = wxNewId();

// Forward declarations.
class ConfigPanel;
class DoxyBlocksLogger;

/*! \brief The main plug-in application class.
 */
class DoxyBlocks : public cbPlugin
{
    public:
        /*! \brief Log entry style IDs
         */
        enum eLogLevel
        {
            LOG_NORMAL,  //!< A normal message.
            LOG_WARNING, //!< A warning message.
            LOG_ERROR    //!< An error message.
        };

        /** Constructor. */
        DoxyBlocks();

        /** Destructor. */
        virtual ~DoxyBlocks();

        /** Invoke configuration dialog.
         * @return The configuration dialogue's return value.
         */
        virtual int Configure();

        /** Return the plugin's configuration priority.
          * This is a number (default is 50) that is used to sort plugins
          * in configuration dialogs. Lower numbers mean the plugin's
          * configuration is put higher in the list.
          * @return The plugin's configuration priority.
          */
        virtual int GetConfigurationPriority() const { return 50; }

        /** Return the configuration group for this plugin. Default is cgUnknown.
          * Notice that you can logically OR more than one configuration groups,
          * so you could set it, for example, as "cgCompiler | cgContribPlugin".
          * @return The configuration group.
          */
        virtual int GetConfigurationGroup() const { return cgEditor; }

        /** Return plugin's configuration panel.
          * @param parent The parent window.
          * @return A pointer to the plugin's cbConfigurationPanel. It is deleted by the caller.
          */
        virtual cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);

        /** Return plugin's configuration panel for projects.
          * The panel returned from this function will be added in the project's
          * configuration dialog.
          * @param parent The parent window.
          * @param project The project that is being edited.
          * @return A pointer to the plugin's cbConfigurationPanel. It is deleted by the caller.
          *
          * \note This function's body was moved to Doxygen.cpp to prevent the disabled variables causing warnings in doxygen.
          *             Warnings will occur if there is a single declaration/implementation and variables are disabled, as doxygen can't find them.
          */
        virtual cbConfigurationPanel* GetProjectConfigurationPanel(wxWindow* parent, cbProject* project);

        /** This method is called by Code::Blocks and is used by the plugin
          * to add any menu items it needs on Code::Blocks's menu bar.\n
          * It is a pure virtual method that needs to be implemented by all
          * plugins. If the plugin does not need to add items on the menu,
          * just do nothing ;)
          * @param menuBar the wxMenuBar to create items in
          */
        virtual void BuildMenu(wxMenuBar* menuBar);

        /** This method is called by Code::Blocks core modules (EditorManager,
          * ProjectManager etc) and is used by the plugin to add any menu
          * items it needs in the module's popup menu. For example, when
          * the user right-clicks on a project file in the project tree,
          * ProjectManager prepares a popup menu to display with context
          * sensitive options for that file. Before it displays this popup
          * menu, it asks all attached plugins (by asking PluginManager to call
          * this method), if they need to add any entries
          * in that menu. This method is called.\n
          * If the plugin does not need to add items in the menu,
          * just do nothing ;)
          * @param type the module that's preparing a popup menu
          * @param menu pointer to the popup menu
          * @param data pointer to FileTreeData object (to access/modify the file tree)
          */
        virtual void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0);

        /** This method is called by Code::Blocks and is used by the plugin
          * to add any toolbar items it needs on Code::Blocks's toolbar.\n
          * It is a pure virtual method that needs to be implemented by all
          * plugins. If the plugin does not need to add items on the toolbar,
          * just do nothing ;)
          * @param toolBar the wxToolBar to create items on
          * @return The plugin should return true if it needed the toolbar, false if not
          */
        virtual bool BuildToolBar(wxToolBar* toolBar);
        void OnDialogueDone(ConfigPanel* pDlg);

    protected:
        /** Any descendent plugin should override this virtual method and
          * perform any necessary initialization. This method is called by
          * Code::Blocks (PluginManager actually) when the plugin has been
          * loaded and should attach in Code::Blocks. When Code::Blocks
          * starts up, it finds and <em>loads</em> all plugins but <em>does
          * not</em> activate (attaches) them. It then activates all plugins
          * that the user has selected to be activated on start-up.\n
          * This means that a plugin might be loaded but <b>not</b> activated...\n
          * Think of this method as the actual constructor...
          */
        virtual void OnAttach();

        /** Any descendent plugin should override this virtual method and
          * perform any necessary de-initialization. This method is called by
          * Code::Blocks (PluginManager actually) when the plugin has been
          * loaded, attached and should de-attach from Code::Blocks.\n
          * Think of this method as the actual destructor...
          * @param appShutDown If true, the application is shutting down. In this
          *         case *don't* use Manager::Get()->Get...() functions or the
          *         behaviour is undefined...
          */
        virtual void OnRelease(bool appShutDown);

        void OnRunDoxywizard(wxCommandEvent & event);
        void OnBlockComment(wxCommandEvent & event);
        void OnLineComment(wxCommandEvent & event);
        void OnExtractProject(wxCommandEvent & event);
        void OnRunHTML(wxCommandEvent & event);
        void OnRunCHM(wxCommandEvent & event);

        void OnConfigure(wxCommandEvent & event);
        void OnReadPrefsTemplate(wxCommandEvent & event);
        void OnWritePrefsTemplate(wxCommandEvent & event);

    private:
        bool DoRunDoxywizard();
        void DoBlockComment();
        void DoLineComment();
        // Extract docs.
        void DoExtractProject();

        void WriteConfigFiles(cbProject *prj, wxString sPrjName, wxString sPrjPath, wxString sDoxygenDir, wxFileName fnDoxyfile, wxFileName fnDoxygenLog);
        int GenerateDocuments(cbProject *prj);
        void AppendToLog(const wxString& sText, eLogLevel flag = LOG_NORMAL, bool bReturnFocus = true) const;
        bool IsProjectOpen() const;
        wxString GetInputList(cbProject *prj, wxFileName fnDoxyfile);
        void DoRunHTML();
        void DoRunCHM();
        void RunCompiledHelp(wxString sDocPath, wxString sPrjName);
        wxString GetDocPath() const;
        wxString GetProjectName();
        // Autodoc.
        void GetBlockCommentStrings(int iBlockComment, wxString &sStartComment, wxString &sMidComment, wxString &sEndComment);
        void StartComment(cbStyledTextCtrl *control, int &iPos, int iBlockComment, wxString sStartComment, wxString sMidComment, wxString sTagBrief, wxString sIndent);
        void CommentFunction(cbStyledTextCtrl *control, int &iPos, wxString sMidComment, wxString sTagParam, wxString sTagReturn, wxString sIndent, wxString sParams, wxString sReturn,  wxString sFunction);
        void AddCommentLine(cbStyledTextCtrl *control, int &iPos, wxString sText);
        wxString ProcessReturnString(wxString sReturn, wxString sFunction);
        // Autoversioning.
        void CheckForAutoVersioning();
        wxString GetAutoVersion();
        // System events.
        void OnProjectActivate(CodeBlocksEvent& event);
        void OnEditorOpen(CodeBlocksEvent& event);
        void OnEditorClose(CodeBlocksEvent& event);
        void OnUpdateUI(wxUpdateUIEvent& event);
        void OnTextURL(wxTextUrlEvent& event);
        // Configuration.
        void LoadSettings();
        void SaveSettings();
        wxString ValidateRelativePath(wxString path);
        void DoReadPrefsTemplate();
        void DoWritePrefsTemplate();

        wxToolBar               *m_pToolbar;       //!< The plug-in toolbar.
        DoxyBlocksLogger        *m_DoxyBlocksLog;  //!< The log tab in the message pane.
        int                     m_LogPageIndex;    //!< The index of the log tab.
        bool                    m_bAutoVersioning; //!< Whether Autoversioning is active for the current project.
        wxString                m_sAutoVersion;    //!< The autoversion string - MAJOR.MINOR.BUILD.
        wxString                m_sVersionHeader;  //!< The Autoversion header path for the current project.
        DoxyBlocksConfig        *m_pConfig;        //!< Configuration object.

        /*! \brief  Declare the event table. */
        DECLARE_EVENT_TABLE();
};


//##########################################################################
//                                    Main page for doxygen docs.
//##########################################################################

/*! \mainpage DoxyBlocks
 *
 *
 * \author     Gary Harris.
 * \date        6 February, 2011
 *
 * \section intro Introduction
 * DoxyBlocks is a plugin for Code::Blocks that integrates doxygen into the IDE. It allows you to create documentation, insert comment blocks and run HTML or CHM documents.
 * It also provides configuration of some of the more commonly used settings and access to doxywizard for more detailed configuration.
 *
 * DoxyBlocks has been built with SVN version 6130 and later of <a href="http://www.codeblocks.org">Code::Blocks</a> and requires a recent version of wxSmith which includes the "extra code" feature.
 *
 * You can download DoxyBlocks from http://sourceforge.net/projects/doxyblocks/.
 *
 * \section changing Changing DoxyBlocks
 *    DoxyBlocks is intended to be an example of good practice using doxygen in Code::Blocks. If you make changes to DoxyBlocks, please respect the work that has gone into making it what it is
 * and do the following:
 * \li Please document your changes using doxygen!
 * \li Run doxygen over the code with all warnings enabled and make sure that you correct all errors and warnings that your documentation generates before submission.
 * \li Update the main page changes and history at the bottom of DoxyBlocks.h.
 * \li Document any warnings that can't be removed. See the three warnings generated by regular expressions in Expressions.h for an example of what this means.
 * \li Update the change log using the Autoversion plug-in.
 * \li Thanks for your contribution.
 *
 * \section comments Comments & Feedback
 * Your input is welcomed. Please direct all comments, suggestions and feedback to the Code::Blocks forum thread at: http://forums.codeblocks.org/index.php/topic,12052.
 *
 * \section changes Changes
 * \li Fixed:     Hard coded doxygen directory name.
 * \li Updated:   Directory creation code.
 *
 *
 * \section history History
 * Version 1.6.614 - 2 Feb 2011.
 * \li Updated:   doxygen support to v1.7.3.
 *
 * Version 1.6.606 - 12 July 2010.
 * \li Fixed:     Fixed some additional inclusions and other minor factors for contrib inclusion.
 *
 * Version 1.5.596 - 5 July 2010.
 * \li A range of changes were made by Killerbot to prepare DoxyBlocks for inclusion as a contrib plugin, including fixing bugs with path detection and running the HTML docs.
 * \li Fixed:     Bugs that caused DoxyBlocks to crash when importing a Visual Studio solution.
 * \li Added:     Feature that allows you to load a saved template instead of the default settings, if no saved settings exist when loading a project. Requested by Codeur.
 * \li Updated:   Doxyfile output to doxygen 1.7.1.
 * \li New:       Prevented doxygen parse errors when using WXUNUSED() by adding it to doxygen's pre-processor settings.
 * \li Updated:   The document directory functionality to allow the use of directories other than "doxygen" as the root folder.
 * \li Cleaned:   The documentation to parse as cleanly as possible with all warnings enabled.
 *
 * Version 1.5.511 - 19 May, 2010.
 * \li Fixed:     Unix build files.
 * \li Removed:   Debug statement that was inadvertently left behind.
 *
 * Version 1.5.508 - 17 May, 2010.
 * \li Fixed:     Fixed a crash that occurred when saving settings from the system configuration dialogue when no project was open. Reported by danselmi.
 * \li Fixed:     Template menu items not disabled with the others.
 *
 * Version 1.5.502 - 21 April, 2010.
 * \li Added:     Functionality to better process the function return string and the keywords "static" and "inline".
 * \li Added:     Functionality to process ** in parameters and return values.
 *
 * Version 1.5.454 - 2 April, 2010.
 * \li Updated:   Function args including const should now be parsed correctly.
 *
 * Version 1.4.440 - 1 April, 2010.
 * \li Added:     Virtual target "All", for consistency.
 * \li Updated:   Menu text so that only the first word is capitalised, as per the existing convention.
 * \li Added:     New functionality to save and load a configuration template. Requested by Codeur.
 *
 * Version 1.4.366 - 27 March, 2010.
 * \li Added:     Information about macro expansion to the config. panel path tooltips.
 * \li Cleaned:   Removed the .cbplugin creation command from both Windows and Unix projects.
 * \li Added:     You can now click the URL displayed in the log window to view the HTML docs.
 * \li Updated:   Changed the config Get functions to be constant.
 *
 * Version 1.3.310 - 27 March, 2010.
 * \li Fixed:     Bug that prevented the output directory being created if a sub-directory was used and didn't already exist.
 * \li Fixed:     Bug in prefs loading code that prevented global prefs from being loaded for a project which hadn't previously used DoxyBlocks.
 * \li Updated:   The generated doxyfile couldn't be run directly by doxygen due to the way that relative paths were created. Changed how DoxyBlocks creates and runs the doxyfile so that it can be run directly by other agents.
 *
 * Version 1.3.289 - 26 March, 2010.
 * \li Fixed:     I have changed my setup and the project to run with the paths expected by C::B. Hopefully this will solve the failures to run experienced by .cbplugin users and bring it into line
 *                with other contrib projects.
 *
 * Version 1.2.236 - 22 March, 2010.
 * \li Added:     Configuration of EXTRACT_PRIVATE and EXTRACT_STATIC. Requested by ptDev.
 * \li Updated:   Changed the generated doxyfile to doxygen 1.6.3.
 * \li Updated:   For consistency, changed HTML_TIMESTAMP default to YES.
 * \li Updated:   For consistency, changed EXTRACT_LOCAL_METHODS default to NO.
 * \li Added:     Macro expansion in path prefs so that you can use things like "$(CODEBLOCKS)" in paths. Requested by Codeur.
 *
 * Version 1.2.223 - 21 March, 2010.
 * \li Fixed:     Bug in the config. panel that caused the OUTPUT_DIRECTORY label to be obscured on Linux.
 *
 * Version 1.2.221 - 20 March, 2010.
 * \li Fixed:     Finally got it to run on Linux with Jens' help by integrating with contrib plug-ins and building from there.
 * \li Updated:   Changed my fix from 1.2.218 to Jens' version as it's more efficient.
 *
 * Version 1.2.218 - 20 March, 2010.
 * \li Fixed:     Nasty bug that caused C::B to crash if no editors were open when DoxyBlocks functions were used.
 *
 * Version 1.2.213 - 19 March, 2010.
 * \li Cleaned:   General cleanup, added to and corrected some documentation errors, added credits, removed junk, updated .po.
 *
 * Version 1.2.204 - 18 March, 2010.
 * \li Added:     The ability to create separate output directories for projects that live in the same directory.
 * \li Added:     Missing warning when trying to run HTML Help and it's not found.
 *
 * Version 1.1.138 - 18 March, 2010.
 * \li Fixed:     The menu activation code was still not right. I think I now have the right combination of events to make things happen correctly.
 *
 * Version 1.1.123 - 17 March, 2010.
 * \li Added:     I forgot to disable the menu if there are no projects open at startup. That's now done.
 *
 * Version 1.0.111 - 16 March, 2010.
 * \li Updated:   Made a matching HTML Help icon.
 * \li Added:     Block comments can now handle more than one return value keyword.
 * \li Added:     The toolbar is now disabled if there are no projects open on startup.
 * \li Updated:   Changed the log output when doxygen is running slightly.
 *
 * Version 1.0.79 - 16 March, 2010.
 * \li Added:     The toolbar and menu are now disabled when all projects are closed and re-enabled when a project is opened.
 * \li Fixed:     The wxSmith block was being lost from the .cbp when saving during project closure. Reworked how config is saved.
 *
 * Version 1.0.14 - 10 March, 2010.
 * \li Added:     Precompiled headers for faster compilation.
 * \li Fixed:     I somehow lost the wxSmith block from the project file.
 * \li Added:     The ability to run a CHM on any OS, given the appropriate viewer. Windows still uses the OS viewer by default.
 * \li Added:     Jens' automake files and added the missing files.
 * \li Updated:   Improved and added to the documentation.
 *
 * Version 1.0.1 - 9 March, 2010.
 * \li Fixed:     Bug that slipped through preventing loading of some prefs.
 *
 * Version 1.0.0 - 9 March, 2010.
 * \li Added:     Disable comment toolbar buttons and menu items when there are no editors open.
 * \li Added:     Changed the config system to use the project file and load and save at project open and close.
 * \li Fixed:     Bug in config. panel that allowed dependent warning checkboxes to be active when the master warnings one wasn't.
 *
 * Version 0.7.669 - 6 March, 2010.
 * \li Added:     The block comment sample now updates to reflect the selection when "Use @ In Tags" selection is changed.
 * \li Cleaned:   Improved some text generation to simplify localisation strings.
 * \li Added:     Some missing documentation.
 * \li Updated:   Improved return value checking when running doxygen. Thanks to private_joker.
 * \li Updated:   Quoted the doxyfile path when running doxywizard to avoid doxywizard crash if the path contain spaces. Thanks to private_joker.
 *
 * Version 0.7.636 - 5 March, 2010.
 * \li Added:     Added the ability to use the AutoVersion plug-in's version string as doxygen's project number value.
 * \li Fixed:     The function parser was creating comments with the parameter type and name in the wrong order.
 * \li Fixed:     Bug in regex pattern matching that allowed multiple matches.
 * \li Added:     Inserted comments blocks are now indented to match the code that they're commenting.
 *
 * Version 0.5.457 - 3 March, 2010.
 * \li Added:     BusyInfo when running doxygen.
 * \li Added:     Ability to run HTML after compilation.
 * \li Added:     Ability to run HTML in the internal viewer.
 * \li Updated:   Header blocks for all files.
 * \li Added:     Now parses the selected line when inserting block comments and pre-fills the \\param and \\return values.
 * \li Added:     Option to use "@" in comments rather than "\".
 *
 * Version 0.3.287 - 28 February, 2010.
 * \li Fixed:     DOT_PATH was written twice due to some extraneous code.
 * \li Added:     New menu which mirrors toolbar functionality.
 * \li Added:     Context-sensitive menu.
 * \li Added:     Translation catalogue.
 * \li Cleaned:   Some general code tidying.
 * \li Added:     Resource file and Windows version info.
 * \li Added      Linux project files.
 * \li Added      Linux .cbplugin file to the repository.
 *
 * Version 0.2.177 - 22 February, 2010.
 * \li Fixed:     Prefs dlg order and improved the layout slightly.
 * \li Added:     Warning checkbox toggles other warnings which I forgot to add before.
 * \li Updated:   The entire project to run from /src/plugins/contrib/. Hopefully that will solve some problems.
 *
 * Version 0.2.172 - 22 February, 2010.
 * \li Initial version.
 * \li Added:     The ability to run the HTML and CHM from the toolbar.
 * \li Fixed:     Bug that caused a crash when using the plugin without a project open.
 * \li Added:     Doxyfile default prefs configuration.
 * \li Added:     Access to configuration from toolbar.
 * \li Added:     Ability to overwrite doxyfile either silently or after prompt.
 * \li Added:     Option to control whether CHM is run after compilation.
 *
 * Version 0.1 - 19 February, 2010.
 * \li Initial release.
 *
 */


#endif // DOXYBLOCKS_H_INCLUDED
