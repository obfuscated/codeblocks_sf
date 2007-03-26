#ifndef WXSUSERITEMDESCRIPTION_H
#define WXSUSERITEMDESCRIPTION_H

#include "wxsuseritem.h"
#include "wxsuseritemproperties.h"
#include "../wxsiteminfo.h"

/** \brief description of one user item */
class wxsUserItemDescription
{
    public:

        /** \brief Ctor */
        wxsUserItemDescription(const wxString& Name);

        /** \brief Dctor */
        ~wxsUserItemDescription();

        /** \brief Getting name of user item */
        inline const wxString& GetName() { return m_Name; }

        /** \brief Setting new name for user item
         *  \note Use Reregister to register item with different name in factory
         */
        void SetName(const wxString& NewName);

        /** \brief Getting item's info */
        inline wxsItemInfo* GetInfo() { return &m_Info; }

        /** \brief Getting bitmap used for preview */
        wxBitmap& GetPreviewBitmap() { return m_PreviewBitmap; }

        /** \brief Building user item class which is deacribed by this class */
        wxsUserItem* BuildUserItem(wxsItemResData* Data);

        /** \brief Loading data from ConfigManager */
        void ReadFromConfig(ConfigManager* Manager,const wxString& BasePath);

        /** \brief Writing data to ConfigManager */
        void WriteToConfig(ConfigManager* Manager,const wxString& BasePath);

        /** \brief Increasing reference count for this description
         *
         * This is needed since there can be some items using description which
         * has been removed from manager
         */
        void AddReference();

        /** \brief Decreasing reference count for this description
         *
         * When DeleteFactory is set to true, this mean that actory will be
         * deleted in this function disabling possibility to create more items
         * of this type
         */
        void DecReference(bool DeleteFactory=false);

        /** \brief Unregistering item from factory */
        void Unregister();

        /** \brief Reregistering item inside facory */
        void Reregister();

        /** \brief Updating tree icon */
        void UpdateTreeIcon();

    private:

        wxString                  m_Name;                   ///< \brief Name of represented class
        wxsItemInfo               m_Info;                   ///< \brief Info of this item
        wxBitmap                  m_PreviewBitmap;          ///< \brief Preview bitmap
        wxsUserItemProperties     m_Properties;             ///< \brief Description of properties
        wxSize                    m_DefaultSize;            ///< \brief Default size of preview when there's no size given

        wxString                  m_CppCodeTemplate;        ///< \brief Code template for C++
        wxArrayString             m_CppDeclarationHeaders;  ///< \brief Declaration header files for C++
        wxArrayString             m_CppDefinitionHeaders;   ///< \brief Definition header files for C++

        wxsItemFactory*           m_Factory;                ///< \brief Factory for item described by this class
        wxsAutoResourceTreeImage* m_TreeImage;              ///< \brief Need this object here to dynamically create and delete resource tree images

        int                       m_RefCount;               ///< \brief Reference count for this item
};

#endif
