#ifndef WXSSTYLEPROPERTY_H
#define WXSSTYLEPROPERTY_H

#include "../../properties/wxsproperties.h"
#include "../../wxscodinglang.h"
#include "../wxsstyle.h"

/** \brief Property responsible for editing style or extra style
 *
 * Internal representation doesn't store styles in values which may be passed
 * to wxWidgets directly. Styles are sotred using bitfields, one bit per one
 * style, no matter if one styles overlap, during design time,
 * these styles are completely separated and do not affect each other.
 * There are some additional functions provided to operate on bitfields, you
 * should operate on bitfields using them.
 *
 * If widget has additional extra style, separate wxsStyleProperty must be
 * created for it.
 *
 * \note This property uses wxsPropertyContainer's GetPropertiesFlags() to
 *       determine which properties should be used. Special bit in Flags inside
 *       wxsStyle class is used to determine which styles can be used in XRC
 *       edit mode (when editing XRC file externally or when generating class
 *       which loads data from xrc) - that was added because XRC doesn't support
 *       all available styles. Flag bits are defined in wxsglobals.h in root
 *       wxSmith's directory.
 *
 */
class wxsStyleProperty: public wxsProperty
{
    public:

        /** \brief Ctor
         *  \param StyleName    name of style property used in Property Grid
         *  \param DataName     name of style data
         *  \param StyleSet     pointer to array of wxsStyle classes defining available styles
         *  \param Offset       offset to bitfield for styles
         *  \param Offset       offset for pointer to styleset class describing all styles
         *  \param Default      default style set (as string - names of styles separated through |)
         *  \param IsExtra      if true, this is extra style, false otherwise
         */
		wxsStyleProperty(
            const wxString& StyleName,
            const wxString& DataName,
            long  Offset,
            long  StyleSetPtrOffset,
            bool  IsExtra = false);

        /** \brief Returning type of this property */
        virtual const wxString GetTypeName() { return _T("Style"); }

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
        long StyleSetPtrOffset;
        bool IsExtra;

};

/** \addtogroup ext_properties_macros
 *  \{ */

/** \brief Macro automatically declaring style property
 *  \param ClassName name of class holding this property
 *  \param VarName name of long integer variable inside class used to keep style bits
 *  \param Flags flags of availability, see \link wxsPropertyContainer::Property
 *         wxsPropertyContainer::Property \endlink for details, use 0 to always
 *         use this property
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param StyleSetPtr pointer to wxsStyleSet structure describing styles (pointer must be declared inside class)
 */
#define WXS_STYLE(ClassName,VarName,Flags,PGName,DataName,StyleSetPtr) \
    static wxsStyleProperty PropertyStyle##ClassName##VarName(PGName,DataName,wxsOFFSET(ClassName,VarName),wxsOFFSET(ClassName,StyleSetPtr),false); \
    Property(PropertyStyle##ClassName##VarName,Flags);

/** \brief Macro automatically declaring extra style property
 *  \param ClassName name of class holding this property
 *  \param VarName name of long integer variable inside class used to keep style bits
 *  \param Flags flags of availability, see \link wxsPropertyContainer::Property
 *         wxsPropertyContainer::Property \endlink for details, use 0 to always
 *         use this property
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param StyleSetPtr pointer to wxsStyleSet structure describing styles (pointer must be declared inside class)
 */
#define WXS_EXSTYLE(ClassName,VarName,Flags,PGName,DataName,StyleSetPtr) \
    static wxsStyleProperty ExPropertyStyle##ClassName##VarName(PGName,DataName,wxsOFFSET(ClassName,VarName),wxsOFFSET(ClassName,StyleSetPtr),true); \
    Property(ExPropertyStyle##ClassName##VarName,Flags);

/** \} */

#endif
