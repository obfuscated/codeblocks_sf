#ifndef WXSRESOURCETREE_H
#define WXSRESOURCETREE_H

#include <wx/treectrl.h>

class wxsResource;
class wxsProject;

/** \brief Definition of resource tree identifier. Separate name of type could be usefull in future */
typedef wxTreeItemId wxsResourceItemId;

// TODO: Rewrite this class with custom widget

/** \brief Resource tree class */
class wxsResourceTree: public wxTreeCtrl
{
    public:

        /** \brief Ctor */
        wxsResourceTree(wxWindow* Parent);

        /** \brief Dctor */
        virtual ~wxsResourceTree();

        /** \brief Function building new tree item for project
         *
         * This item takes name of associated cbProject and initializes
         * item data to point to project.
         */
        wxsResourceItemId NewProjectItem(const wxString& ProjectTitle,wxsProject* Project);

        /** \brief Function returning item id for external resources */
        wxsResourceItemId ExternalResourcesId();

        /** \brief Function deleting identifier for external resources */
        void DeleteExternalResourcesId();

        /** \brief Getting singleton instance */
        static inline wxsResourceTree* Get() { return m_Singleton; }

    private:

        wxsResourceItemId m_ExtId;              ///< \brief id of tree item containing external resources
        bool m_IsExt;                           ///< \brief True if there is item for external resources
        static wxsResourceTree* m_Singleton;    ///< \brief singleton object

        void OnSelect(wxTreeEvent& event);
        void OnRightClick(wxTreeEvent& event);

        DECLARE_EVENT_TABLE()
};

/** \brief Helper function for getting resource tree singleton object */
inline wxsResourceTree* wxsTree() { return wxsResourceTree::Get(); }

#endif
