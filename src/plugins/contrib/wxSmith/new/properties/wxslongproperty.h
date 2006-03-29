#ifndef WXSLONGPROPERTY_H
#define WXSLONGPROPERTY_H

#include "wxsproperty.h"

/** \brief Base long integer property */
class wxsLongProperty: public wxsProperty
{
	public:

        /** \brief Ctor
         *  \param PGName   name of property in Property Grid
         *  \param DataName name of property in data stuctures
         *  \param Offset   offset of value (returned from wxsOFFSET macro)
         *  \param Default  default value applied on read errors
         */
		wxsLongProperty(const wxString& PGName,const wxString& DataName,long Offset,long Default=0);

		/** \brief Returning type name */
		virtual const wxString GetTypeName() { return _T("long"); }

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
        long Default;
};

/** \addtogroup properties_macros
 *  \{ */

/** \brief Macro automatically declaring long integer property
 *  \param ClassName name of class holding this property
 *  \param VarName name of variable inside class
 *  \param Flags flags of availability, see \link wxsPropertyContainer::Property
 *         wxsPropertyContainer::Property \endlink for details, use 0 to always
 *         use this property
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param Default value applied on read errors / validation failures
 */
#define WXS_LONG(ClassName,VarName,Flags,PGName,DataName,Default) \
    static wxsLongProperty PropertyLong##ClassName##VarName(PGName,DataName,wxsOFFSET(ClassName,VarName),Default); \
    Property(PropertyLong##ClassName##VarName,Flags);

/** \} */

#endif
