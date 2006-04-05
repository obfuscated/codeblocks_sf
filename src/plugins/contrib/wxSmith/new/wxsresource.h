#ifndef WXSRESOURCE_H
#define WXSRESOURCE_H

#include <wx/treectrl.h>
#include <tinyxml/tinyxml.h>
#include "wxscodinglang.h"

// Forward declarations
class wxSmith;
class wxsProject;
class wxsEditor;

/** \brief Class representing one resource
 *
 * This class provides generic interface ans some basic managment for
 * resources. All resource classes must be derived from this one.
 *
 * This class should not hold data always. Data should be read on EditOpen
 * and freed on EditClose funcitons. When there's no editor, this class just
 * informs that given resource does exist somewhere (or more precise: should
 * exist if it's not broken).
 */
class wxsResource: public wxObject
{
	public:

        /** \brief Ctor */
		wxsResource(wxsProject* Project);

		/** \brief Dctor */
		virtual ~wxsResource();

		/** \brief Function returing resource type */
		virtual wxString GetType() = 0;

        /** \brief Getting resource name */
        virtual const wxString& GetResourceName() = 0;

		/** \brief Getting current project */
		inline wxsProject* GetProject() { return Project; }

        /** \brief Function opening this resource in eeditor window,
         *         if editor window already exists, it must be activated
         */
        void EditOpen();

        /** \brief Function closing editor window, if window is closed, no action
         *         should be performed
         */
        void EditClose();

        /** \brief Function returning currently openeditor, NULL if there's no one */
        inline wxsEditor* GetEditor() { return Editor; }

        /** \brief Function building entry in resource browser
         *
         * This function should build one entry in resource browser.
         * Note that it should not create child nodes now, but where children
         * are loaded into memory.
         */
        virtual void BuildTree(wxTreeCtrl* Tree,wxTreeItemId ParentId) = 0;

        /** \brief Getting current tree item in resource browser */
        inline wxTreeItemId GetTreeItemId() { return ItemId; }

        /** \brief Function loading configuration of this resource from
         *         given xml element
         */
        virtual bool LoadConfiguration(TiXmlElement* Element) = 0;

        /** \brief Function storing configuration of this resource to given
         *         xml element
         */
        virtual bool SaveConfiguration(TiXmlElement* Element) = 0;

        /** \brief Function checking ifthis resource can be main resource of application */
        virtual bool CanBeMain() { return false; }

        /** \brief Function checking if this resource uses xrc files */
        virtual bool UsingXRC() { return false; }

        /** \brief Function generating source code initializing and displaying
         *         resource in application class
         *
         * \param Code string where new code should be appended. Each line of
         *        added code should start with _T("\t")
         * \param Language language of generated code
         */
        virtual void BuildShowingCode(wxString& Code,wxsCodingLang Language) { }

        /** \brief Function returning name of file with declaration for this resource.
         *
         * This function should return declarating file specific for used coding
         * language. For example, c++ should return *.h file with declaration
         * of resource class.
         * This header file should be added into list of includes to allow
         * using resource.
         */
        virtual wxString GetDeclarationFile() { return wxEmptyString; }

        /** \brief Function checking if given file is used in this resource
         *  \param Name name of file given as absolute path
         */
        virtual bool UsingFile(const wxString& Name) { return false; }

        /** \brief Function getting current coding language */
        inline wxsCodingLang GetLanguage() { return Language; }

    protected:

        /** \brief Function which should create editor window.
         *
         * This function is called when user called EditOpen and there's no
         * editor yet.
         */
        virtual wxsEditor* CreateEditor() = 0;

        /** \brief Function notifying that editor has just been closed */
        virtual void EditorClosed() {}

        /** \brief Function setting id of item in tree resource browser
         *
         * This functino must be called only from BuildTree() function when
         * new item id is taken
         */
        inline void SetTreeItemId(wxTreeItemId NewId) { ItemId = NewId; }

        /** \brief Function setting coding language for given one */
        inline void SetLanguage(wxsCodingLang NewLanguage) { Language=NewLanguage; }

    private:

        /** \brief Function called from wxsEditor class when it's closing */
        void EditorSaysHeIsClosing();

        wxsEditor* Editor;      ///< \brief Current editor or NULL if there's no opened yet
        wxsProject* Project;    ///< \brief Resource project
        wxTreeItemId ItemId;    ///< \brief Id of tree item in resource browser
        wxsCodingLang Language; ///< \brief Coding language used in this resource

        friend class wxsEditor;

};

#endif
