#ifndef WXSCOLOURPROPERTY_H
#define WXSCOLOURPROPERTY_H

#include "../../properties/wxsproperties.h"
#include "../../wxscodinglang.h"

#include <wx/dialog.h>
#include <wx/propgrid/propdev.h>
#include <wx/propgrid/advprops.h>

#define wxsCOLOUR_DEFAULT   (wxPG_COLOUR_CUSTOM - 1)

/** \brief Class handling colour data for wxSmith */
class wxsColourData: public wxColourPropertyValue
{
    public:

        wxsColourData(wxUint32 type, const wxColour &colour): wxColourPropertyValue(type,colour) {}
        wxsColourData(wxUint32 type = wxsCOLOUR_DEFAULT): wxColourPropertyValue(type) {}
        wxsColourData(const wxColour &colour): wxColourPropertyValue(colour) {}
        wxsColourData(const wxColourPropertyValue& cp): wxColourPropertyValue(cp) {}

		/** \brief Getting wxColour object from wxColourPropertyValue
		 *  \return wxColour class, if wxColour.Ok() will return false, default colour was used
		 */
        wxColour GetColour();

        /** \brief Getting code building colour
         *  \return code with colour or empty string if there's default colour
         */
        wxString BuildCode(wxsCodingLang Language);
};

/** \brief Colour property - property used for handling wxColour property
 *
 *  This property uses wxColourPropertyValue to keep data
 *
 */
class wxsColourProperty: public wxsProperty
{
	public:

        /** \brief Ctor
         *  \param PGName               name of property in Property Grid
         *  \param DataName             name of property in data stuctures
         *  \param ValueOffset          offset of wxColourPropertyValue member (taken from wxsOFFSET macro)
         */
		wxsColourProperty(
            const wxString& PGName,
            const wxString& DataName,
            long ValueOffset);

		/** \brief Returning type name */
		virtual const wxString GetTypeName() { return _T("wxsColour"); }

		/** \brief Getting wxColour object from wxColourPropertyValue
		 *  \return wxColour class, if wxColour.Ok() will return false, default colour was used
		 */
        static wxColour GetColour(const wxColourPropertyValue& value);

        /** \brief Getting code building colour
         *  \return code with colour or empty string if there's default colour
         */
        static wxString GetColourCode(const wxColourPropertyValue& value,wxsCodingLang Language);

    protected:

        virtual void PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent);
        virtual bool PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid, wxPGId Id,long Index);
        virtual bool PGWrite(wxsPropertyContainer* Object,wxPropertyGridManager* Grid, wxPGId Id,long Index);
        virtual bool XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream);
        virtual bool PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream);

	private:
        long ValueOffset;
};

/** \addtogroup ext_properties_macros
 *  \{ */

/** \brief Macro automatically declaring colour property
 *  \param ClassName name of class holding this property
 *  \param VarName name of wxsColourData variable inside class
 *  \param Flags flags of availability, see \link wxsPropertyContainer::Property
           wxsPropertyContainer::Property \endlink for details, use 0 to always
           use this property
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 */
#define WXS_COLOUR(ClassName,VarName,Flags,PGName,DataName) \
    static wxsColourProperty PropertyColour##ClassName##VarName(PGName,DataName,wxsOFFSET(ClassName,VarName)); \
    Property(PropertyColour##ClassName##VarName,Flags);

/** \} */

#endif
