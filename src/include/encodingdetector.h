/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef ENCODINGDETECTOR_H
#define ENCODINGDETECTOR_H

#include "settings.h"
#include "filemanager.h"
#include <wx/fontmap.h>

class wxString;

/** Try to detect the encoding of a file on disk. */
class DLLIMPORT EncodingDetector
{
	public:
		EncodingDetector(const wxString& filename);
		EncodingDetector(LoaderBase* fileLdr);
		EncodingDetector(const wxByte* buffer, size_t size);
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
		wxString GetWxStr() const;
	protected:
        /** @return True if succeeded, false if not (e.g. file didn't exist). */
		bool DetectEncoding(const wxString& filename, bool ConvertToWxString = true);
		bool DetectEncoding(const wxByte* buffer, size_t size, bool ConvertToWxString = true);

        bool m_IsOK;
		bool m_UseBOM;
		int m_BOMSizeInBytes;
		wxFontEncoding m_Encoding;
	private:
        wxString m_ConvStr;
        bool ConvertToWxStr(const wxByte* buffer, size_t size);
        inline bool IsUTF8Tail(wxByte b) { return ((b & 0xC0) == 0x80); };
        bool DetectUTF8(wxByte* byt, size_t size);
        bool IsTextUTF16BE(wxByte *text, size_t size);
        bool IsTextUTF16LE(wxByte *text, size_t size);
        bool DetectUTF16(wxByte* byt, size_t size);
        bool IsTextUTF32BE(wxByte *text, size_t size);
        bool IsTextUTF32LE(wxByte *text, size_t size);
        bool DetectUTF32(wxByte* byt, size_t size);
};

#endif // ENCODINGDETECTOR_H
