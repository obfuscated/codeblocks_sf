#ifndef WXSGENERICPROPERTYVALUE_H
#define WXSGENERICPROPERTYVALUE_H

#include "../../properties/wxsproperties.h"
#include "../../wxscodinglang.h"
#include "../properties/wxsbitmapiconproperty.h"
#include "../properties/wxscolourproperty.h"
#include "../properties/wxsdimensionproperty.h"
#include "../properties/wxspositionsizeproperty.h"

/** \brief This class represents generic property container for only one property */
class wxsGenericPropertyValue: public wxsPropertyContainer
{
    public:

        /** \brief List of available property types */
        enum PropertyType
        {
            ptString,
            ptLongString,
            ptLong,
            ptBool,
            ptBitmap,
            ptColour,
            ptDimension,
            ptPosition,
            ptSize
        };

        /** \brief Ctor */
        wxsGenericPropertyValue(
            PropertyType Type,
            const wxString& Name,
            const wxString& PropertyName,
            const wxString& PropertyName2 = wxEmptyString,
            const wxString& PropertyName3 = wxEmptyString,
            const wxString& PropertyName4 = wxEmptyString
            );

        /** \brief Getting property's name */
        inline const wxString& GetName() { return m_Name; }

        /** \brief Getting value of property as string */
        wxString GetValueAsString(wxsCodingLang Language,const wxString& WindowParent);

        /** \brief Setting value from string */
        void SetValueFromString(const wxString& String);

    private:

        /** \brief Enumerating only one property */
        virtual void OnEnumProperties(long Flags);

        PropertyType        m_Type;
        wxString            m_Name;
        wxString            m_PropertyName;
        wxString            m_PropertyName2;
        wxString            m_PropertyName3;
        wxString            m_PropertyName4;

        wxString            m_String;
        long                m_Long;
        bool                m_Bool;
        wxsBitmapIconData   m_Bitmap;
        wxsColourData       m_Colour;
        wxsDimensionData    m_Dimension;
        wxsPositionSizeData m_PositionSize;
};



#endif
