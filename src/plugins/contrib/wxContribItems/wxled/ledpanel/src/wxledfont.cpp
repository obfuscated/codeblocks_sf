/***************************************************************
 * Name:      wxledfont.cpp
 * Purpose:   Code for Class wxLEDFont
 * Author:    Christian Gräfe (info@mcs-soft.de)
 * Created:   2007-03-02
 * Copyright: Christian Gräfe (www.mcs-soft.de)
 * License:	  wxWindows licence
 **************************************************************/
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "wx/wxledfont.h"

#include "font75.cpp"
#include "font77.cpp"

wxLEDFont::wxLEDFont(wxLEDFontType t) :
	m_letterspace(1),
	m_type((t==wxLEDFont7x5)?(wxLEDFont7x7):(wxLEDFont7x5))	// it's a littl trick because SetFontType checks the corrent font
{
	SetFontType(t);
}

wxLEDFont::~wxLEDFont()
{
    Destroy();
}

void wxLEDFont::SetFontType(wxLEDFontType t)
{
	if(t==m_type) return;

	// Destroy the old Font
	Destroy();

	// save the Font-parameters
	m_type=t;
	m_LetterWidth=((t==wxLEDFont7x5)?(font75_letterWidth):(font77_letterWidth));
	m_LetterHeight=((t==wxLEDFont7x5)?(font75_letterHeight):(font77_letterHeight));

	// set parameters for loading
	const char* lettersData=((t==wxLEDFont7x5)?(font75_LettersData):(font77_LettersData));

	// set standartletters
	// SPACE without fit
	m_letters[ms_LettersChar[0]]= new MatrixObject(lettersData,m_LetterWidth,m_LetterHeight);
	// all the other letters
	AdvancedMatrixObject* tmp=0;
	for(unsigned int i=1;i<ms_LettersChar.Len();++i)
	{
		tmp=new AdvancedMatrixObject(lettersData+i*m_LetterWidth*m_LetterHeight*sizeof(char),m_LetterWidth,m_LetterHeight);
		tmp->FitLeft();
		tmp->FitRight();
		m_letters[ms_LettersChar[i]]= new MatrixObject(*tmp);
		wxDELETE(tmp);
	}
}

const MatrixObject* wxLEDFont::GetLetter(wxChar l)
{
	// suchen
	wxLEDFontHashMap::iterator it=m_letters.find(l);
	if(it==m_letters.end()) return NULL;
	else return it->second;
}

AdvancedMatrixObject* wxLEDFont::GetMOForText(const wxString& text, wxAlignment a)
{
	// string empty?
	if(text.IsEmpty()) return NULL;

	// ausmase des matrix_object suchen
	wxString s_tmp=text;
	int w=0,h=0,npos=s_tmp.Find('\n'),i=0;
	while(npos!=wxNOT_FOUND)
	{
		h++;
		if(w<npos) w=npos;
		s_tmp=s_tmp.AfterFirst('\n');
		npos=s_tmp.Find('\n');
	}
	h++;
	if(w<s_tmp.Len()) w=s_tmp.Len();

	// create matrixobject for the text
	AdvancedMatrixObject* mo_forText=new AdvancedMatrixObject(
					0,w*(m_LetterWidth+m_letterspace),
					h*(m_LetterHeight+m_letterspace)-m_letterspace);

	// create array for the textlines
	AdvancedMatrixObject** mo_lines=new AdvancedMatrixObject*[h+1];

	// create the AMOs fo the textlines
	for(i=0;i<=h;++i)
		mo_lines[i]=new AdvancedMatrixObject(
							0,w*(m_LetterWidth+m_letterspace),
							m_LetterHeight);

	// Get the Letters and save in the MO of the line
	int x=0,line=0;
	const MatrixObject* mo_tmp=0;
	for(i=0; i<text.Len(); ++i)
	{
		wxChar c=text.GetChar(i);
		if(c=='\n')
		{
			++line;
			x=0;
		}
		else
		{
			mo_tmp=this->GetLetter(c);
			if(mo_tmp!=NULL)
			{
				// Save Letter in the line for the text
				mo_lines[line]->SetDatesAt(x,0,*mo_tmp);

				// x-Pos of the next Letter
				x+=mo_tmp->GetWidth()+m_letterspace;
			}
		}
	}

	// shrink the mo of the lines and save in one MO
	int y=0;
	for(i=0;i<=h;++i)
	{
		if(mo_lines[i]->IsEmpty()==false)
		{
			mo_lines[i]->FitRight();

			// find x pos
			if(a==wxALIGN_RIGHT)
				x=mo_forText->GetWidth()-mo_lines[i]->GetWidth();
			else if(a==wxALIGN_CENTER_HORIZONTAL)
				x=(mo_forText->GetWidth()-mo_lines[i]->GetWidth())/2;
			else  // wxALIGN_LEFT
				x=0;

			// set the line
			mo_forText->SetDatesAt(x,y,*mo_lines[i]);
		}

		// find y pos for next line
		y+=m_LetterHeight+m_letterspace;

		// free the line
		wxDELETE(mo_lines[i]);
	}

	// Fit the right and left border
	mo_forText->FitLeft();
	mo_forText->FitRight();

	// free the array for the lines
	delete[] mo_lines;

	// return the mo
	return mo_forText;
}

void wxLEDFont::Destroy()
{
	// has the Hashmap elements
	if(m_letters.empty()) return;

	// free the Letter objects
	wxLEDFontHashMap::iterator it;
    for( it = m_letters.begin(); it != m_letters.end(); ++it )
    {
        MatrixObject* value = it->second;
        delete value;
    }

    // clear the Hashmap
    m_letters.clear();
}

const wxString wxLEDFont::ms_LettersChar = wxT(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");

