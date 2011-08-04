/////////////////////////////////////////////////////////////////////////////
// Name:        zipstream.h
// Purpose:     Memory stream classes
// Author:      Ryan Norton
// Modified by:
// Created:     09/05/03
// RCS-ID:      $Id: bzipstream.h,v 1.1 2004/05/19 02:50:57 jblough Exp $
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#ifndef _WX_WXZSTREAM_H__
#define _WX_WXZSTREAM_H__

//#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
//#pragma interface "bzipstream.h"
//#endif

#include "wx/defs.h"

#if wxUSE_STREAMS

#include "wx/stream.h"

#ifndef WXBZBS
#define WXBZBS 5000
#endif

class wxBZipInputStream : public wxFilterInputStream
{
public:
	//	if bLessMemory is true, uses decompression alg w/less mem, but less speed also
	wxBZipInputStream(wxInputStream& stream, bool bLessMemory = false); 
	virtual ~wxBZipInputStream();

	wxInputStream& ReadRaw(void* pBuffer, size_t size);
	off_t TellRawI();
	off_t SeekRawI(off_t pos, wxSeekMode sm = wxFromStart);

	void* GetHandleI() {return hZip;}
protected:
	size_t OnSysRead(void *buffer, size_t size);

	void* hZip;
	char pBuffer[WXBZBS];
	int nBufferPos;
};
class wxBZipOutputStream : public wxFilterOutputStream
{
public:
	wxBZipOutputStream(wxOutputStream& stream, wxInt32 nCompressionFactor = 4);
	virtual ~wxBZipOutputStream();

	wxOutputStream& WriteRaw(void* pBuffer, size_t size);
	off_t TellRawO();
	off_t SeekRawO(off_t pos, wxSeekMode sm = wxFromStart);

	void* GetHandleO() {return hZip;}
protected:
	size_t OnSysWrite(const void *buffer, size_t bufsize);
	
	void* hZip;
	char pBuffer[WXBZBS];
};
class wxBZipStream : public wxBZipInputStream, wxBZipOutputStream
{
public:
	wxBZipStream(wxInputStream& istream, wxOutputStream& ostream);
	virtual ~wxBZipStream();
};

#endif
#endif
