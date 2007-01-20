#ifndef WXSARRAYSTRINGPROPERTY_H
#define WXSARRAYSTRINGPROPERTY_H

#include "wxscustomeditorproperty.h"

/** \brief Property for wxArrayString class
 *
 * \note This property doesn't take any default value.
 *       By default array is cleared.
 */
class wxsArrayStringProperty: public wxsCustomEditorProperty
{
	public:

        /** \brief Ctor 
         *  \param PGName       name of property in Property Grid
         *  \param DataName     name of property in data stuctures
         *  \param DataSubName  name of name applied for each array element
         *  \param Offset       offset of value (returned from wxsOFFSET macro)
         */
		wxsArrayStringProperty(const wxString& PGName,const wxString& DataName,const wxString& DataSubName,long Offset);
		
		/** \brief Returning type name */
		virtual const wxString GetTypeName() { return _T("wxArrayString"); }

        /** \brief Showing editor for this property */
        virtual bool ShowEditor(wxsPropertyContainer* Object);
        
    protected:
    
        virtual bool XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream);
        virtual bool PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream);
        virtual wxString GetStr(wxsPropertyContainer* Object);

	private:
        long Offset;
        wxString DataSubName;
};

/** \addtogroup properties_macros
 *  \{
 *  \brief Macro automatically declaring wxArrayString property 
 *  \param ClassName name of class holding this property
 *  \param VarName name of variable inside class
 *  \param Flags flags of availability, see \link wxsPropertyContainer::Property 
           wxsPropertyContainer::Property \endlink for details, use 0 to always
           use this property
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param DataSubName name for subelement used in Xml / Data Streams
 */
#define WXS_ARRAYSTRING(ClassName,VarName,Flags,PGName,DataName,DataSubName) \
    static wxsArrayStringProperty PropertyArrayString##ClassName##VarName(PGName,DataName,DataSubName,wxsOFFSET(ClassName,VarName)); \
    Property(PropertyArrayString##ClassName##VarName,Flags);
    
/** \} */

#endif
