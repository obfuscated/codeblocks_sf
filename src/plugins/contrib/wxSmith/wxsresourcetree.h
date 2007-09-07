#ifndef WXSRESOURCETREE_H
#define WXSRESOURCETREE_H

#ifndef WX_PRECOMP
#   ifdef __WXMSW__
#       include <wx/msw/wrapwin.h>  // Needed to prevent LoadImage define bug.
#   endif
#endif

#include <wx/treectrl.h>
#include "wxsresourcetreeitemdata.h"

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

        /** \brief Getting global image list
         *
         * This list is declared as static, so it can be used even
         * before wxsResourceTree is created
         */
        static wxImageList& GetGlobalImageList();

        /** \brief Loading image to list
         *  \param FileName name relative to codeblock's data path
         *  \return Index in list
         */
        static int LoadImage(const wxString& FileName);

        /** \brief Adding bitmap into global list
         *  \param Bitmap bitmap to be added
         *  \return Index in list
         */
        static int InsertImage(const wxBitmap& Bitmap);

        /** \brief Freeing image at given index
         *
         * This function marks image at given index as freed, so
         * it may be replaced by LoadImage in future without
         * unnecessarily increasing size of list
         */
        static void FreeImage(int Index);

        /** \brief Blocking processing select events
         * \note This function must be paired with UnblockSelect
         */
        void BlockSelect();

        /** \brief Unblocking select events
         * \note This function must be paired with BlockSelect
         */
        void UnblockSelect();

    private:

        wxsResourceItemId m_ExtId;              ///< \brief id of tree item containing external resources
        bool m_IsExt;                           ///< \brief True if there is item for external resources
        int m_BlockCount;                       ///< \brief if > 0 must block all select events
        wxsResourceTreeItemData* m_Data;        ///< \brief Pointer to item data object used in last action
        static wxsResourceTree* m_Singleton;    ///< \brief singleton object

        static int m_RootImageId;
        static int m_ProjectImageId;
        static int m_ExternalImageId;

        static wxArrayInt& GetFreedList();

        void PopupMenu(wxMenu* Menu,wxsResourceTreeItemData* ItemData);
        void InvalidateItemData(wxsResourceTreeItemData* ItemData);

        void OnSelect(wxTreeEvent& event);
        void OnRightClick(wxTreeEvent& event);
        void OnPopupMenu(wxCommandEvent& event);

        friend void wxsResourceTreeItemData::PopupMenu(wxMenu* Menu);
        friend wxsResourceTreeItemData::~wxsResourceTreeItemData();

        DECLARE_EVENT_TABLE()
};

/** \brief Helper function for getting resource tree singleton object */
inline wxsResourceTree* wxsTree() { return wxsResourceTree::Get(); }

#endif
