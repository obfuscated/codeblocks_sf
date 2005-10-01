#ifndef WXSWINDOWRESDATAOBJECT_H
#define WXSWINDOWRESDATAOBJECT_H

#include <wx/dataobj.h>
#include <tinyxml/tinyxml.h>

#define wxsDF_WIDGET   _T("wxSmith XML")

class wxsWidget;
class wxsWindowRes;

/** Class representing wxsWidget and all it's children as wxDataObject
 *
 * This data object can also handle set of widgets stored continously
 */
class wxsWindowResDataObject : public wxDataObject
{
	public:
        /** Ctor */
		wxsWindowResDataObject();

		/** Dctor */
		virtual ~wxsWindowResDataObject();

    //=====================================
    // Opertating on data
    //=====================================

        /** Clering all data */
        void Clear();

		/** Adding widget into this data object */
		bool AddWidget(wxsWidget* Widget);

		/** Getting number of handled widgets inside this object */
		int GetWidgetCount() const;

		/** Building wxsWidget class from this data object
		 * \param Resource - resource owning widget
		 * \param Index - id of widget (in range 0..GetWidgetCount()-1)
		 * \return created widget or NULL on error
		 */
		wxsWidget* BuildWidget(wxsWindowRes* Resource,int Index = 0) const;

		/** Setting Xml string describing widget */
		bool SetXmlData(const wxString& Data);

		/** Getting Xml strting desecribing widget */
		wxString GetXmlData() const;

    //=====================================
    // Members of wxDataObject class
    //=====================================

		/** Enumerating all data formats.
		 *
		 * Formats available for reading and writing:
         * - wxDF_TEXT
         * - internal type ("wxSmith XML")
         */
		virtual void GetAllFormats( wxDataFormat *formats, Direction dir = Get) const;

		/** Copying data to raw buffer */
		virtual bool GetDataHere(const wxDataFormat& format, void *buf ) const;

		/** Returns number of data bytes */
		virtual size_t GetDataSize(const wxDataFormat& format ) const;

		/** Returns number of suported formats (in both cases - 2) */
		virtual size_t GetFormatCount(Direction dir = Get) const;

		/** Returning best format - "wxSmith XML" */
		virtual wxDataFormat GetPreferredFormat(Direction dir = Get) const;

		/** Setting data - will load Xml data */
		virtual bool SetData( const wxDataFormat& format, size_t len, const void *buf );

    private:

        TiXmlDocument XmlDoc;
        TiXmlElement* XmlElem;
        int WidgetsCount;
};

#endif // WXSWINDOWRESDATAOBJECT_H
