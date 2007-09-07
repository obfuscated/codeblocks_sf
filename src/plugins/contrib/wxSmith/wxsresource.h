#ifndef WXSRESOURCE_H
#define WXSRESOURCE_H

#include "wxseditor.h"
#include "wxscodinglang.h"
#include "wxsresourcetree.h"
#include "wxsproject.h"

/** \brief Class used to manage basic resource information
 *
 * This class represents resource inside project (or external resource).
 * It should keep only basic resource information but not resource data.
 * Data is stored inside editor's structures.
 *
 * Main purpose of this class is to hold information that given resource
 * exists inside project, create editor for it and create source code allowing
 * to use resource as main application's resource.
 */
class wxsResource: public wxObject
{
    DECLARE_CLASS(wxsResource)
    public:

        /** \brief Ctor
         *  \param Owher project owning resource
         *  \param ResourceName name of resource (f.ex. class name)
         *  \param ResourceType name of resource type (f.ex. wxDialog)
         *  \param GUI name of gui using this resource, put empty string if this is universal resource like bitmap file
         *  \param Language coding language used for this resource
         */
        wxsResource(wxsProject* Owner,const wxString& ResourceType,const wxString& GUI);

        /** \brief dctor */
        virtual ~wxsResource();

        /** \brief Getting resource type */
        inline const wxString& GetResourceType() { return m_ResourceType; }

        /** \brief Getting GUI */
        inline const wxString& GetGUI() { return m_GUI; }

        /** \brief Getting coding language used inside this resource */
        inline wxsCodingLang GetLanguage() { return m_Language; }

        /** \brief Setting new language */
        inline void SetLanguage(wxsCodingLang Lang) { m_Language = Lang; }

        /** \brief Getting resource name */
        inline const wxString& GetResourceName() { return m_ResourceName; }

        /** \brief Changing resource's name */
        inline void SetResourceName(const wxString& NewName) { m_ResourceName = NewName; }

        /** \brief Opening editor, if one exists, it will be activated in set of all C::B editors */
        void EditOpen();

        /** \brief Closing editor if exist */
        void EditClose();

        /** \brief Checking if editor for this resource is opened */
        inline bool IsEditorOpened() { return m_Editor!=0; }

        /** \brief Getting pointer to editor or 0 if there's none */
        inline wxsEditor* GetEditor() { return m_Editor; }

        /** \brief Getting tree item id in resource browser */
        inline const wxsResourceItemId& GetTreeItemId() { return m_TreeItemId; }

        /** \brief Creating entry in resoruce browser */
        void BuildTreeEntry(const wxsResourceItemId& Parent);

        /** \brief Getting name of declaration file
          * \note this function is only a wrapper to OnGetDeclarationFile (to hold ocnsistency of functions to override)
          */
        inline wxString GetDeclarationFile() { return OnGetDeclarationFile(); }

        /** \brief Getting code creating this resource
         *  \note this function is only a wrapper to OnGetAppBuildingCode (to hold ocnsistency of functions to override)
         */
        inline wxString GetAppBuildingCode() { return OnGetAppBuildingCode(); }

        /** \brief Reading configuration of resource from Xml node */
        bool ReadConfig(const TiXmlElement* Node);

        /** \brief Writing configuration of resource to Xml node */
        bool WriteConfig(TiXmlElement* Node);

        /** \brief Helper function for fetching project path */
        inline wxString GetProjectPath() { return m_Owner ? m_Owner->GetProjectPath() : _T(""); }

        /** \brief Getting project owning this resource */
        inline wxsProject* GetProject() { return m_Owner; }

        /** \brief Cleaning up before deleting this resource from project */
        inline bool DeleteCleanup(bool ShowDialog=true) { return OnDeleteCleanup(ShowDialog); }

    protected:

        /** \brief Function called when there's need to create new editor
         *
         * This function is called when there's need to open editor and when
         * editor has not been created yet (or has been closed), so there's no
         * need to check whether editor is opened or not.
         * \param Parent pointer to parent window (notebook with editors)
         * \return Pointer to wxsEditor class
         */
        virtual wxsEditor* OnCreateEditor(wxWindow* Parent) = 0;

        /** \brief Function called when reading resource configuration from .cbp file
         *
         * \param Node - Xml node in cbp file defined for this resource only.
         * It's in form:
         *  \code <{Resource_Type} name={Resource_Name} language={Resource_Language}... /> \endcode
         * where {Resource_Type} is type of resource returned from GetResourceType() function
         * and {Resource_Name} is name returned from GetResourceName(). Name attribute is always
         * present.
         */
        virtual bool OnReadConfig(const TiXmlElement* Node) = 0;

        /** \brief Function called when writing resource configuration to .cbp file
         *
         * \param Node - Xml node where all data should be written to. "name" attribute,
         *        "language" attribute and Element's value (node name) should not be overwritten.
         */
        virtual bool OnWriteConfig(TiXmlElement* Node) = 0;

        /** \brief function called to check if this resource handles given file
         *
         * This function is called from mime plugin to check if opening some
         * file should open editor of this resource. If true, EditOpen is called
         * automatically.
         *
         * \return true when opening FileName should result in opening this resource's
         *         editor.
         */
        virtual bool OnCanHandleFile(const wxString& FileName) = 0;

        /** \brief Function which should return name of file
         *         with resource declaration (f.ex. in c++ it should be .h file)
         * \return File name relative to project's CBP file path
         */
        virtual wxString OnGetDeclarationFile() { return wxEmptyString; }

        /** \brief Function which should build this resource in application initializing code */
        virtual wxString OnGetAppBuildingCode() { return wxEmptyString; }

        /** \brief Index of icon in resource browser */
        virtual int OnGetTreeIcon() { return -1; }

        /** \brief Filling extra entries in popup menu invoked from resource browser */
        virtual void OnFillPopupMenu(wxMenu* Menu) {}

        /** \brief Reacting on popup menu event */
        virtual bool OnPopupMenu(long Id) { return false; }

        /** \brief Cleaning up before deleting this resource from project */
        virtual bool OnDeleteCleanup(bool ShowDialog) { return true; }

    private:

        /** \brief Function called from wxsEditor just before it's deletion */
        void EditorClosed();

        wxString m_ResourceType;
        wxString m_ResourceName;
        wxString m_GUI;
        wxsProject* m_Owner;
        wxsEditor* m_Editor;
        wxsResourceItemId m_TreeItemId;
        wxsCodingLang m_Language;

        class wxsResourceRootTreeItemData;
        friend wxsEditor::~wxsEditor();
        friend class wxsProject;
        friend class wxsResourceRootTreeItemData;
};

#endif
