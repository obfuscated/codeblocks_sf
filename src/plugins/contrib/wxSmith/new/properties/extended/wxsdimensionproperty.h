#ifndef WXSDIMENSIONPROPERTY_H
#define WXSDIMENSIONPROPERTY_H

#include "../wxsproperty.h"
#include "../../wxscodinglang.h"

// TODO: Create one class containing dimension data and managing it

/** \brief Dimension property (long integer value which may be in pixel or dialog units) */
class wxsDimensionProperty: public wxsProperty
{
	public:

        /** \brief Ctor
         *  \param PGName               name of property in Property Grid
         *  \param PGDUName             name of "use dialog units" property
         *  \param DataName             name of property in data stuctures
         *  \param ValueOffset          offset of long value (taken from wxsOFFSET macro)
         *  \param DialogUnitsOffset    offset of bool value which switches between pixel and dialog units (offset taken from wxsOFFSET macro)
         *  \param Default              default value applied on read errors
         *  \param DefaultDialogUnits   default value applied for pixel / dialog units switch on read errors
         */
		wxsDimensionProperty(
            const wxString& PGName,
            const wxString& PGDUName,
            const wxString& DataName,
            long ValueOffset,
            long DialogUnitsOffset,
            long Default=0,
            bool DefaultDialogUnits=false);

		/** \brief Returning type name */
		virtual const wxString GetTypeName() { return _T("dimension"); }

		/** \brief Function converting dimension to pixels
		 *  \param Value value in dialog/pixel units
		 *  \param DialogUnits true when Value is in dualog units, false if it's in pixels
		 *  \param Parent parent window used as base for calculating size
		 */
		static inline long GetPixels(long Value,bool DialogUnits,wxWindow* Parent)
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
		static inline long GetDimension(long Value,bool DialogUnits,wxWindow* Parent)
		{
		    return DialogUnits ?
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
        static wxString GetPixelsCode(long Value,bool DialogUnits,wxString ParentName,wxsCodingLang Language);

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
        long Default;
        long DialogUnitsOffset;
        bool DefaultDialogUnits;

        wxString PGDUName;
};

/** \addtogroup ext_properties_macros
 *  \{ */

/** \brief Macro automatically declaring dimension property
 *  \param ClassName name of class holding this property
 *  \param VarName name of integer variable inside class holding value
 *  \param DUVarName name of boolean variable inside class holding flag switching between pixel and dialog units
 *  \param Flags flags of availability, see \link wxsPropertyContainer::Property
           wxsPropertyContainer::Property \endlink for details, use 0 to always
           use this property
 *  \param PGName name used in property grid
 *  \param PGDUName name of "use dialog units" property in grid
 *  \param DataName name used in Xml / Data Streams
 *  \param Default default value for dimension (integer)
 *  \param DUDefault default value for dualog units (boolean)
 */
#define WXS_DIMENSION(ClassName,VarName,DUVarName,Flags,PGName,PGDUName,DataName,Default,DUDefault) \
    static wxsDimensionProperty PropertyDimension##ClassName##VarName(PGName,PGDUName,DataName,wxsOFFSET(ClassName,VarName),wxsOFFSET(ClassName,DUVarName),Default,DUDefault); \
    Property(PropertyDimension##ClassName##VarName,Flags);

/** \} */

#endif
