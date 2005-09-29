#ifndef WXSWINDOWRESDATAOBJECT_H
#define WXSWINDOWRESDATAOBJECT_H

#include <wx/dataobj.h>

class wxsWidget;
class wxsWindowRes;

/** Class representing wxsWidget and all it's children as wxDataObject */
class wxsWindowResDataObject : public wxDataObject
{
	public:
        /** Ctor */
		wxsWindowResDataObject();
		
		/** Dctor */
		virtual ~wxsWindowResDataObject();
		
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
		
		/** Generating this data object from wxsWidget class */
		void MakeFromWidget(wxsWidget* Widget);
		
		/** Building wxsWidget class from this data object
		 * \param Resource - resource owning widget
		 */
		wxsWidget* BuildWidget(wxsWindowRes* Resource) const;
		
		/** Setting Xml string describing widget */
		inline void SetXmlData(const wxString& Data) { XmlData = Data; }
		
		/** Getting Xml strting desecribing widget */
		inline const wxString& GetXmlData() { return XmlData; }
		
    private:
        wxString XmlData;
};

#endif // WXSWINDOWRESDATAOBJECT_H
