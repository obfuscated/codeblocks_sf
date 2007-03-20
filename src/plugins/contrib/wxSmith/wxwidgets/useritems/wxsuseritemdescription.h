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

        /** \brief Getting item's info */
        inline const wxsItemInfo* GetInfo() { return &m_Info; }

        /** \brief Building user item class which is deacribed by this class */
        wxsUserItem* BuildUserItem(wxsItemResData* Data);

        /** \brief Loading data from ConfigManager */
        void ReadFromConfig(ConfigManager* Manager,const wxString& BasePath);

        /** \brief Writing data to ConfigManager */
        void WriteToConfig(ConfigManager* Manager,const wxString& BasePath);

    private:

        wxString                  m_Name;                   ///< \brief Name of represented class
        wxsItemInfo               m_Info;                   ///< \brief Info of this item
        wxBitmap                  m_Bitmap;                 ///< \brief Preview bitmap
        wxsUserItemProperties     m_Properties;             ///< \brief Description of properties
        wxSize                    m_DefaultSize;            ///< \brief Default size of preview when there's no size given

        wxString                  m_CppCodeTemplate;        ///< \brief Code template for C++
        wxArrayString             m_CppDeclarationHeaders;  ///< \brief Declaration header files for C++
        wxArrayString             m_CppDefinitionHeaders;   ///< \brief Definition header files for C++

        wxsItemFactory*           m_Factory;                ///< \brief Factory for item described by this class
        wxsAutoResourceTreeImage* m_TreeImage;              ///< \brief Need this object here to dynamically create and delete resource tree images
};

#endif
