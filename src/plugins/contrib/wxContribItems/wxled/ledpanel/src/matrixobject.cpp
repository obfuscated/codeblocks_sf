/***************************************************************
 * Name:      matrixobejct.cpp
 * Purpose:   Code for Class MatrixObject
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

#include "wx/matrixobject.h"

MatrixObject::MatrixObject() :
	m_data(0), m_width(0), m_height(0), m_length(0)
{
}

MatrixObject::MatrixObject(const char* data, int width, int height) :
	m_data(0)
{
	Init(data,width,height);
}

MatrixObject::MatrixObject(const MatrixObject& mo) :
	m_data(0)
{
	Init(mo);
}

MatrixObject::~MatrixObject()
{
    wxDELETE(m_data);
}

MatrixObject& MatrixObject::operator=(const MatrixObject& mo)
{
	this->Init(mo);

	return *this;
}

void MatrixObject::Init(const char* data, int width, int height)
{
	// auf gleichheit testen
	if(m_data == data && data!=0)
	{
		wxLogMessage(wxT("Error. You cant init the Object with itself!"));
		return;
	}

	// wenn schon initialiesiert (wird erst alles freigegeben)
	this->Destroy();

	// Quadratisch wenn nichts angegeben
	if(height==0) height=width;

	// Größe Speichern
    m_width=width;
    m_height=height;
    m_length=width*height;

    // space for data?
    if(m_length==0) return; // nothing to do

	// Array zum internen Speichern erzeugen
    m_data=new char[m_length];

	// Daten aus dem vorgegeben Array übernehmen oder mit 0 Füllen
	if(data)
		memcpy(m_data,data,m_length*sizeof(char));
	else
		memset(m_data,0,m_length*sizeof(char));
}

void MatrixObject::Init(const wxImage img)
{
    if(!img.IsOk()) return;

    // wenn schon initialiesiert (wird erst alles freigegeben)
    this->Destroy();

    // Größe Speichern
    m_width = img.GetWidth();
    m_height = img.GetHeight();
    m_length=m_width*m_height;

    // space for data?
    if(m_length==0) return; // nothing to do

	// Array zum internen Speichern erzeugen
    m_data=new char[m_length];
    memset(m_data,0,m_length*sizeof(char)); // fill with nulls

    // jeden Pixel durchgehen und setzten wenn ungleich schwarz
    const unsigned char* idat = img.GetData();
    for(int i=0;i<m_length;i++)
    {
        if(idat[i*3] || idat[i*3+1] || idat[i*3+2])
            m_data[i] = 1;
    }
}

char MatrixObject::GetDataFrom(int x, int y) const
{
	if(x<0 || x>=m_width) return -1;
	if(y<0 || y>=m_height) return -1;
	return m_data[x+y*m_width];
}

char MatrixObject::GetDataFrom(int p) const
{
	if(p>=(m_length) || p<0) return -1;
	return m_data[p];
}

void MatrixObject::Clear()
{
	memset(m_data,0,m_length*sizeof(char));
}

void MatrixObject::Destroy()
{
	// Wenn das Objekt noch gar nicht Initialisiert wurde
	if(!m_data) return;

	delete m_data;
	m_data=0;
	m_length=m_width=m_height=0;
}

bool MatrixObject::SetDataAt(int x, int y, char data)
{
    // Prüfen ob der Punkt gültig
    if(x<0) return false;
    if(y<0) return false;
    if(x>=m_width) return false;
    if(y>=m_height) return false;

    // Data setzen
    m_data[x+y*m_width]=data;

    return true;
}

bool MatrixObject::SetDataAt(int p, char data)
{
    // Prüfen ob der Punkt gültig
    if(p<0) return false;
    if(p>=m_length) return false;

    // Date setzen
    m_data[p]=data;

    return true;
}

bool MatrixObject::SetDatesAt(int x, int y, const MatrixObject &mo)
{
	// are we ready?
	if(!m_data) return false;

	// has mo data?
	if(mo.IsEmpty()) return false ;

    int dx=0,dy=0;
    char data;

    int l=mo.GetLength();
    int w=mo.GetWidth();
    const char* mod = mo.GetData();

	// TODO optimze with memcpy, copy lines
    for(int i=0;i<l;++i)
    {
    	// test the left dst bound
    	if(x+dx<0)
    	{
    		++dx;
    		continue;
    	}

    	// test the right and top dst bound
    	if(x+dx>=m_width || y+dy<0)
    	{
    		++dy;
    		dx=0;
    		i=dy*w-1;
    		continue;
    	}

    	// test the top dst bound
    	if(y+dy>=m_height) break;	// becaus the rest is also out of bound

		// if we are inside the bound, get the data
		data=mod[i];

        // 0 -> do nothing
        if(data>0)
            m_data[(x+dx)+(y+dy)*m_width]=data;
            //SetDataAt(x+dx,y+dy,data); // copy data
        else if(data<0)
            m_data[(x+dx)+(y+dy)*m_width]=0;
        	//SetDataAt(x+dx,y+dy,0); // set 0

		// count up
		++dx;
		if(dx==w) {++dy; dx=0;}
    }

    return true;
}

bool MatrixObject::IsEmpty() const
{
	for(int i=0;i<m_length;++i)
		if(m_data[i]!=0) return false;

	return true;
}

wxImage MatrixObject::GetAsImage() const
{
    wxImage img;

    if(!m_data) return img;

    img.Create(m_width,m_height);
    unsigned char* d = img.GetData();

    for(int i=0;i<m_length;i++)
    {
        if(m_data[i])
            memset(&d[i*3],-1,3);
    }

    return img;
}
