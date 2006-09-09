#ifndef WXSARRAYSTRINGCHECKPROPERTY_H
#define WXSARRAYSTRINGCHECKPROPERTY_H

#include "../wxscustomeditorproperty.h"

WX_DEFINE_ARRAY(bool,wxArrayBool);

/** \brief Property for editing arrays of strings with checked option
 *
 * \note This property doesn't take any default value.
 *       By default array is cleared.
 */
class wxsArrayStringCheckProperty: public wxsCustomEditorProperty
{
	public:
	
        /** \brief Ctor 
         *  \param PGName       name of property in Property Grid
         *  \param DataName     name of property in data stuctures
         *  \param DataSubName  name of name applied for each array element
         *  \param Offset       offset of wxArrayString (returned from wxsOFFSET macro)
         *  \param CheckOffset  offset of wxArrayBool (returned from wxsOFFSET macro)
         */
		wxsArrayStringCheckProperty(const wxString& PGName,const wxString& DataName,const wxString& DataSubName,long Offset,long CheckOffset);
		
		/** \brief Returning type name */
		virtual const wxString GetTypeName() { return _T("wxArrayStringCheck"); }

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
        long CheckOffset;
        wxString DataSubName;
};

/** \addtogroup ext_properties_macros  Macros automatically dedfining extended properties
 *  \{ */
 
/** \brief Macro automatically declaring wxArrayStrting combined with wxArrayBool properties
 *  \param ClassName name of class holding this property
 *  \param StringVarName name of wxArrayString variable inside class
 *  \param BoolVarName name of wxArrayBool variable inside class
 *  \param Flags flags of availability, see \link wxsPropertyContainer::Property 
           wxsPropertyContainer::Property \endlink for details, use 0 to always
           use this property
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param DataSubName name of subnode used in Xml / Data Streams
 */
#define WXS_ARRAYSTRINGCHECK(ClassName,StringVarName,BoolVarName,Flags,PGName,DataName,DataSubName) \
    static wxsArrayStringCheckProperty PropertyArrayStringCheck##ClassName##VarName(PGName,DataName,DataSubName,wxsOFFSET(ClassName,StringVarName),wxsOFFSET(ClassName,BoolVarName)); \
    Property(PropertyArrayStringCheck##ClassName##VarName,Flags);

/** \} */


#endif
