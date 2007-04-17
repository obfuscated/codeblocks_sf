#ifndef WXSDIMENSIONPROPERTY_H
#define WXSDIMENSIONPROPERTY_H

#include "../../properties/wxsproperties.h"
#include "../../wxscodinglang.h"

struct wxsDimensionData
{
    long Value;
    bool DialogUnits;

    wxsDimensionData(): Value(0), DialogUnits(false) {}

    /** \brief Function converting dimension to pixels
     *  \param Value value in dialog/pixel units
     *  \param DialogUnits true when Value is in dualog units, false if it's in pixels
     *  \param Parent parent window used as base for calculating size
     */
    inline long GetPixels(wxWindow* Parent)
    {
        return DialogUnits ?
            wxDLG_UNIT(Parent,wxSize(Value,0)).GetWidth() :
            Value;
    }

    /** \brief Function converting pixel units to dialog ones
     *  \param Value value in pixels
     *  \param DialogUnits true when should convert units to dialog, false if should be left in pixels
     *  \param Parent parent window used as base for recalculating size
     */
    inline long GetDialogUnits(wxWindow* Parent)
    {
        return !DialogUnits ?
            Parent->ConvertPixelsToDialog(wxSize(Value,0)).GetWidth() :
            Value;
    }

    /** \brief Function getting code returning valid value in pixels
     *
     * This function may be used when generating code. It will return
     * correct representation of value which may be directly passed
     * to created object.
     *
     *  \param Value value in dialog/pixel units
     *  \param DialogUnits true when Value is in dualog units, false if it's in pixels
     *  \param ParentName name of variable holding pointer to parent class
     *  \param Language language of generated code
     */
    wxString GetPixelsCode(wxString ParentName,wxsCodingLang Language);

};

/** \brief Dimension property (long integer value which may be in pixel or dialog units) */
class wxsDimensionProperty: public wxsProperty
{
	public:

        /** \brief Ctor
         *  \param PGName               name of property in Property Grid
         *  \param PGDUName             name of "use dialog units" property
         *  \param DataName             name of property in data stuctures
         *  \param Offset               offset of wxsDimensionData value (taken from wxsOFFSET macro)
         *  \param DefaultValue         default value applied on read errors
         *  \param DefaultDialogUnits   default value applied for pixel / dialog units switch on read errors
         *  \param Priority             priority of this property
         */
		wxsDimensionProperty(
            const wxString& PGName,
            const wxString& PGDUName,
            const wxString& DataName,
            long Offset,
            long DefaultValue=0,
            bool DefaultDialogUnits=false,
            int Priority=100);

		/** \brief Returning type name */
		virtual const wxString GetTypeName() { return _T("wxsDimension"); }

    protected:

        virtual void PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent);
        virtual bool PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid, wxPGId Id,long Index);
        virtual bool PGWrite(wxsPropertyContainer* Object,wxPropertyGridManager* Grid, wxPGId Id,long Index);
        virtual bool XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream);
        virtual bool PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream);

	private:
        long Offset;
        long DefaultValue;
        bool DefaultDialogUnits;

        wxString PGDUName;
};

/** \addtogroup ext_properties_macros
 *  \{ */

/** \brief Macro automatically declaring dimension property
 *  \param ClassName name of class holding this property
 *  \param VarName name of wxsDimensionData variable inside class
 *  \param PGName name used in property grid
 *  \param PGDUName name of "use dialog units" property in grid
 *  \param DataName name used in Xml / Data Streams
 *  \param Default default value for dimension (integer)
 *  \param DUDefault default value for dualog units (boolean)
 */
#define WXS_DIMENSION(ClassName,VarName,PGName,PGDUName,DataName,Default,DUDefault) \
    { static wxsDimensionProperty _Property(PGName,PGDUName,DataName,wxsOFFSET(ClassName,VarName),Default,DUDefault); \
      Property(_Property); }

/** \brief Macro automatically declaring dimension property with custom priority
 *  \param ClassName name of class holding this property
 *  \param VarName name of wxsDimensionData variable inside class
 *  \param PGName name used in property grid
 *  \param PGDUName name of "use dialog units" property in grid
 *  \param DataName name used in Xml / Data Streams
 *  \param Default default value for dimension (integer)
 *  \param DUDefault default value for dualog units (boolean)
 *  \param Priority priority of this property
 */
#define WXS_DIMENSION_P(ClassName,VarName,PGName,PGDUName,DataName,Default,DUDefault,Priority) \
    { static wxsDimensionProperty _Property(PGName,PGDUName,DataName,wxsOFFSET(ClassName,VarName),Default,DUDefault,Priority); \
      Property(_Property); }

/** \} */

#endif
