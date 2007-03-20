#ifndef WXSGENERICPROPERTYVALUELIST_H
#define WXSGENERICPROPERTYVALUELIST_H

#include "wxsgenericpropertyvalue.h"

/** \brief Property container with dynamic list of generic properties */
class wxsGenericPropertyValueList: public wxsPropertyContainer
{
    public:

        /** \brief Ctor */
        wxsGenericPropertyValueList();

        /** \brief Dctor */
        virtual ~wxsGenericPropertyValueList();

        /** \brief Adding new property */
        void AddProperty(
            wxsGenericPropertyValue::PropertyType Type,
            const wxString& Name,
            const wxString& PropertyName1,
            const wxString& PropertyName2,
            const wxString& PropertyName3,
            const wxString& PropertyName4,
            const wxString& InitialValue);

        /** \brief Getting value of property by name */
        bool GetPropertyValue(const wxString& Name,wxString& Value,wxsCodingLang Language,const wxString& WindowParent);

    private:

        virtual void OnEnumProperties(long Flags);

        WX_DEFINE_ARRAY(wxsGenericPropertyValue*,PropertiesT);

        /** \brief List of properties */
        PropertiesT m_Properties;
};



#endif
