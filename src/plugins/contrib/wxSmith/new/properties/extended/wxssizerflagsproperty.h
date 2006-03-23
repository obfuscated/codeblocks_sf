#ifndef WXSSIZERFLAGSPROPERTY_H
#define WXSSIZERFLAGSPROPERTY_H

#include "../wxsproperty.h"

/** \brief Property responsible for editing sizer flags;
 *
 * Sizer flags are: border flags, alignment and other flags: expand, shaped,
 * fixed min size.
 *
 * \note This property uses standard names for property grid / data.
 */
class wxsSizerFlagsProperty: public wxsProperty
{
    public:
    
        static const long BorderTop             = 0x0001;
        static const long BorderBottom          = 0x0002;
        static const long BorderLeft            = 0x0004;
        static const long BorderRight           = 0x0008;
        static const long Expand                = 0x0010;
        static const long Shaped                = 0x0020;
        static const long FixedMinSize          = 0x0040;
        static const long AlignLeft             = 0x0080;
        static const long AlignRight            = 0x0100;
        static const long AlignTop              = 0x0200;
        static const long AlignBottom           = 0x0400;
        static const long AlignCenterVertical   = 0x0800;
        static const long AlignCenterHorizontal = 0x1000;
    
        /** \brief Ctor
         *  \param Offset   offset to long handling border flags
         */
		wxsSizerFlagsProperty(long Offset);
            
        /** \brief Returning type of this property */
        virtual const wxString GetTypeName() { return _T("SizerFlags"); }
        
        /** \brief Getting string representation of flags */
        static wxString GetString(long Flags);
        
        /** \brief Getting wxWidgets-ready flags */
        static long GetWxFlags(long Flags);
        
    protected:

        virtual void PGCreate(wxsPropertyContainer* Object,wxPropertyGridManager* Grid,wxPGId Parent);
        virtual bool PGRead(wxsPropertyContainer* Object,wxPropertyGridManager* Grid, wxPGId Id,long Index);
        virtual bool PGWrite(wxsPropertyContainer* Object,wxPropertyGridManager* Grid, wxPGId Id,long Index);
        virtual bool XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream);
        virtual bool PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream);

    private:
    
        static long ParseString(const wxString& String);
        static void FixFlags(long& Flags);
    
        long Offset;

        static const long BorderAll = 0x10000;
        static const long BorderPrevAll = 0x20000;
        
        static const long BorderMask = BorderTop|BorderBottom|BorderLeft|BorderRight;
        static const long AlignHMask = AlignLeft|AlignRight|AlignCenterHorizontal;
        static const long AlignVMask = AlignTop|AlignBottom|AlignCenterVertical;
};

/** \addtogroup ext_properties_macros
 *  \{ */
 
/** \brief Macro automatically declaring sizer flags property
 *  \param ClassName name of class holding this property
 *  \param VarName name of long integer variable inside class used to keep flags
 *  \param Flags flags of availability, see \link wxsPropertyContainer::Property 
 *         wxsPropertyContainer::Property \endlink for details, use 0 to always
 *         use this property
 */
#define WXS_SIZERFLAGS(ClassName,VarName,Flags) \
    static wxsSizerFlagsProperty PropertySizerFlags##ClassName##VarName(wxsOFFSET(ClassName,VarName)); \
    Property(PropertySizerFlags##ClassName##VarName,Flags);

/** \} */

#endif
