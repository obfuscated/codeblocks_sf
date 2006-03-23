#ifndef WXSSTRINGPROPERTY_H
#define WXSSTRINGPROPERTY_H

#include "wxsproperty.h"

/** \brief Base string property (working on wxString class) */
class wxsStringProperty: public wxsProperty
{
	public:

        /** \brief Ctor 
         *  \param PGName           name of property in Property Grid
         *  \param DataName         name of property in data stuctures
         *  \param Offset           offset of value (returned from wxsOFFSET macro)
         *  \param IsLongString     if true, long string editor will be used (used for multiline strings)
         *  \param XmlStoreEmpty    if true, string will be also stored when it's value is equal to default one
         *  \param Default          default value applied on read errors
         */
		wxsStringProperty(const wxString& PGName,const wxString& DataName,long Offset,bool IsLongString=true,bool XmlStoreEmpty=false,const wxString& Default=wxEmptyString);
		
		/** \brief Returning type name */
		virtual const wxString GetTypeName() { return _T("wxString"); }

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
        bool IsLongString;
        bool XmlStoreEmpty;
        wxString Default;
};

/** \addtogroup properties_macros
 *  \{ */
 
/** \brief Macro automatically declaring wxString property 
 *  \param ClassName name of class holding this property
 *  \param VarName name of variable inside class
 *  \param Flags flags of availability, see \link wxsPropertyContainer::Property 
           wxsPropertyContainer::Property \endlink for details, use 0 to always
           use this property
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param Default value applied on read errors / validation failures
 *  \param IsLong true when Long string editor should be used (multiline edit), false otherwise (one line string)
 *  \param XmlStoreEmpty true if strings equal to default value should be written to xml structure, false otherwise
 */
#define WXS_STRING(ClassName,VarName,Flags,PGName,DataName,Default,IsLong,XmlStoreEmpty) \
    static wxsStringProperty PropertyString##ClassName##VarName(PGName,DataName,wxsOFFSET(ClassName,VarName),IsLong,XmlStoreEmpty,Default); \
    Property(PropertyString##ClassName##VarName,Flags);
    
/** \} */


#endif
