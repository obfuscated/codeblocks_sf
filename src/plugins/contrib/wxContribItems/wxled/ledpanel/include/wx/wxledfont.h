/***************************************************************
 * Name:      wxledfont.h
 * Purpose:   Defines Class wxLEDFont
 * Author:    Christian Gräfe (info@mcs-soft.de)
 * Created:   2007-03-02
 * Copyright: Christian Gräfe (www.mcs-soft.de)
 * License:	  wxWindows licence
 **************************************************************/

#ifndef WXLEDFONT_H
#define WXLEDFONT_H

#include "advancedmatrixobject.h"
#include <wx/wx.h>

enum wxLEDFontType
{
	wxLEDFont7x5=0,	// default
	wxLEDFont7x7=1
};

WX_DECLARE_HASH_MAP( wxChar , MatrixObject*, wxIntegerHash, wxIntegerEqual, wxLEDFontHashMap );

class wxLEDFont
{
	public:
		wxLEDFont(wxLEDFontType t=wxLEDFont7x5);
		virtual ~wxLEDFont();

		// Get the MatrixObject from a letter of the Font
		// the MatrixObject is deletet, when the font is destroyed
		const MatrixObject* GetLetter(wxChar l);

		// Get the MatrixObject for an Text
		// please delete the MO which you get, if you don't need it anymore
		AdvancedMatrixObject* GetMOForText(const wxString& text, wxAlignment a=wxALIGN_LEFT);

		// Set the Fonttype 7x5 (default) or 7x7
		void SetFontType(wxLEDFontType t);
		wxLEDFontType GetFontType() const {return m_type;}

		void SetLetterSpace(int letterspace) {m_letterspace=letterspace;}
		int GetLetterSpace() const {return m_letterspace;}

		int GetLetterWidth() const {return m_LetterWidth;}
		int GetLetterHeight() const {return m_LetterHeight;}

	private:
		void Destroy();
		wxLEDFontHashMap m_letters;
		int m_letterspace;
		int m_LetterWidth;
		int m_LetterHeight;
		static const wxString ms_LettersChar;
		wxLEDFontType m_type;
};

#endif // WXLEDFONT_H
