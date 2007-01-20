#ifndef WXSBITMAPICONPROPERTY_H
#define WXSBITMAPICONPROPERTY_H

#include "../../properties/wxsproperties.h"
#include "../../wxscodinglang.h"

#include <wx/artprov.h>

/** \brief Structure holding bitmap configuration
 *
 * Id determines whether we use art provider or not,
 * if both Id and FileName are empty, no bitmap is given
 */

struct wxsBitmapIconData
{
    wxString Id;
    wxString Client;
    wxString FileName;

    /** \brief Generating preview for this bitmap / icon */
    wxBitmap GetPreview(const wxSize& Size,const wxString& DefaultClient = wxART_OTHER);

    /** \brief Generating code building this bitmap / icon.
     *
     * Empty code is returned when there's no valid bitmap.
     */
    wxString BuildCode(bool NoResize,const wxString& SizeCode,wxsCodingLang Language,const wxString& DefaultClient = wxART_OTHER);

    /** \brief Checking if this is empty bitmap/icon */
    bool IsEmpty();
};

typedef wxsBitmapIconData wxsBitmapData;
typedef wxsBitmapIconData wxsIconData;

/** \brief Property for editing arrays of strings with checked option
 *
 * \note This property doesn't take any default value.
 *       By default array is cleared.
 */
class wxsBitmapIconProperty: public wxsCustomEditorProperty
{
	public:

        /** \brief Ctor
         *  \param PGName           name of property in Property Grid
         *  \param DataName         name of property in data stuctures
         *  \param Offset           offset of wxsBitmapIconData structure (returned from wxsOFFSET macro)
         *  \param DefaultClient    name of default art provider client
         */
		wxsBitmapIconProperty(
            const wxString& PGName,
            const wxString& DataName,
            long Offset,
            const wxString& DefaultClient = _T("wxART_OTHER"));

		/** \brief Returning type name */
		virtual const wxString GetTypeName() { return _T("wxBitmapIcon"); }

        /** \brief Showing editor for this property */
        virtual bool ShowEditor(wxsPropertyContainer* Object);

    protected:

        virtual bool XmlRead(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool XmlWrite(wxsPropertyContainer* Object,TiXmlElement* Element);
        virtual bool PropStreamRead(wxsPropertyContainer* Object,wxsPropertyStream* Stream);
        virtual bool PropStreamWrite(wxsPropertyContainer* Object,wxsPropertyStream* Stream);

	private:

        long Offset;
        wxString DefaultClient;
};

typedef wxsBitmapIconProperty wxsBitmapProperty;
typedef wxsBitmapIconProperty wxsIconProperty;


/** \addtogroup ext_properties_macros
 *  \{ */

/** \brief Macro automatically declaring bitmap property
 *  \param ClassName name of class holding this property
 *  \param VarName name of wxBitmapData inside class
 *  \param Flags flags of availability, see \link wxsPropertyContainer::Property
           wxsPropertyContainer::Property \endlink for details, use 0 to always
           use this property
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param DefaultArtClient default art provider client (use _T("wxART_OTHER") if not sure)
 */
#define WXS_BITMAP(ClassName,VarName,Flags,PGName,DataName,DefaultArtClient) \
    static wxsBitmapProperty PropertyBitmap##ClassName##VarName(PGName,DataName,wxsOFFSET(ClassName,VarName),DefaultArtClient); \
    Property(PropertyBitmap##ClassName##VarName,Flags);

/** \brief Macro automatically declaring icon property
 *  \param ClassName name of class holding this property
 *  \param VarName name of wxIconData inside class
 *  \param Flags flags of availability, see \link wxsPropertyContainer::Property
           wxsPropertyContainer::Property \endlink for details, use 0 to always
           use this property
 *  \param PGName name used in property grid
 *  \param DataName name used in Xml / Data Streams
 *  \param DefaultArtClient default art provider client (use _T("wxART_OTHER") if not sure)
 */
#define WXS_ICON(ClassName,VarName,Flags,PGName,DataName,DefaultArtClient) \
    static wxsIconProperty PropertyIcon##ClassName##VarName(PGName,DataName,wxsOFFSET(ClassName,VarName),DefaultArtClient); \
    Property(PropertyIcon##ClassName##VarName,Flags);

/** \} */


#endif
