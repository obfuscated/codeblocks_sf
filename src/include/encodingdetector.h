#ifndef ENCODINGDETECTOR_H
#define ENCODINGDETECTOR_H

#include "settings.h"
#include <wx/fontmap.h>

class wxString;

/** Try to detect the encoding of a file on disk. */
class DLLIMPORT EncodingDetector
{
	public:
		EncodingDetector(const wxString& filename);
		EncodingDetector(const EncodingDetector& rhs);
		~EncodingDetector();

        /** @return True if file was read, false if not. */
        bool IsOK() const;
        /** @return True if the file contains a BOM (Byte Order Mark), false if not. */
		bool UsesBOM() const;
		/** @return The BOM size in bytes. Returns zero if no BOM. */
		int GetBOMSizeInBytes() const;
		/** @return The detected encoding. Currently ISO8859-1 is returned if no BOM is present. */
		wxFontEncoding GetFontEncoding() const;
	protected:
        /** @return True if succeeded, false if not (e.g. file didn't exist). */
		bool DetectEncoding(const wxString& filename);

        bool m_IsOK;
		bool m_UseBOM;
		int m_BOMSizeInBytes;
		wxFontEncoding m_Encoding;
	private:

};

#endif // ENCODINGDETECTOR_H
