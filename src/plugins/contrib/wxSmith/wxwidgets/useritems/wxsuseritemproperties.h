#ifndef WXSUSERITEMPROPERTIES_H
#define WXSUSERITEMPROPERTIES_H

#include "wxsgenericpropertyvaluelist.h"

/** \brief Structure dectipting properties of user-defined items
 *
 * \note This class is different from wxsGenericProperyValueList,
 *       it does not handle values for properties but only descriptions of them
 */
class wxsUserItemProperties
{
    public:

        /** \brief Fetching PropertyType from wxsGenericPropertyValue into this class */
        typedef wxsGenericPropertyValue::PropertyType PropertyType;

        /** \brief Structure describing property */
        struct PropertyDescription
        {
            PropertyType  m_Type;           ///< \brief Type of property
            wxString      m_Name;           ///< \brief Name of data
            wxString      m_PropGridName;   ///< \brief Root name used in property grid
            wxString      m_PropGridName2;  ///< \brief Helper 2nd name used in property grid
            wxString      m_PropGridName3;  ///< \brief Helper 3rd name used in property grid
            wxString      m_PropGridName4;  ///< \brief Helper 4th name used in property grid
            wxString      m_DefaultValue;   ///< \brief Default value given as string
        };

        /** \brief Ctor */
        wxsUserItemProperties();

        /** \brief Dctor */
        virtual ~wxsUserItemProperties();

        /** \brief Getting number of descriptions */
        inline int GetDescriptionsCount() { return (int)m_Descriptions.Count(); }

        /** \brief Fetching description */
        inline const PropertyDescription *GetDescription(int Number) { return ((Number>=0)&&(Number<GetDescriptionsCount())) ? m_Descriptions[Number] : NULL; }

        /** \brief Setting description */
        bool SetDescription(int Number,const PropertyDescription* NewDescription);

        /** \brief Inserting new description */
        bool InsertDescription(int Position,const PropertyDescription* NewDescription);

        /** \brief Appending new description to the end of list */
        inline bool InsertDescription(const PropertyDescription* NewDescription) { return InsertDescription(-1,NewDescription); }

        /** \brief Deleting description */
        bool DeleteDescription(int Number);

        /** \brief Building GenericPropertyValueList from set of descriptions */
        wxsGenericPropertyValueList* BuildList();

        /** \brief Reading data from config manager */
        void ReadFromConfig(ConfigManager* Manager,const wxString& BasePath);

        /** \brief Writing data to config manager */
        void WriteToConfig(ConfigManager* Manager,const wxString& BasePath);

    private:

        WX_DEFINE_ARRAY(PropertyDescription*,Descriptions);

        /** \brief Dynamic binding of properti descriptions */
        Descriptions m_Descriptions;
};

#endif
