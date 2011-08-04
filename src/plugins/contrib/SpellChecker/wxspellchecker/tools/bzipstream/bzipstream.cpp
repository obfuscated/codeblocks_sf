/////////////////////////////////////////////////////////////////////////////
// Name:        bzipstream.cpp
// Purpose:     BZip stream classes
// Author:      Ryan Norton
// Modified by:
// Created:     10/11/03
// RCS-ID:      $Id: bzipstream.cpp,v 1.1 2004/05/19 02:50:57 jblough Exp $
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//                          HEADERS
//===========================================================================

//#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
//#pragma implementation "zipstream.h"
//#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif


#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_STREAMS

#include "bzipstream.h"
#include "wx/utils.h"
#include "wx/intl.h"
#include "wx/log.h"

#define BZ_NO_STDIO
#include "bzip/bzlib.h"

#ifndef BZ_MAX_UNUSED
#define BZ_MAX_UNUSED 5000
#endif

//===========================================================================
//                          IMPLEMENTATION
//===========================================================================

/*
typedef 
   struct {
      char *next_in;
      unsigned int avail_in;
      unsigned int total_in_lo32;
      unsigned int total_in_hi32;

      char *next_out;
      unsigned int avail_out;
      unsigned int total_out_lo32;
      unsigned int total_out_hi32;

      void *state;

      void *(*bzalloc)(void *,int,int);
      void (*bzfree)(void *,void *);
      void *opaque;
   } 
   bz_stream;
*/

// ----------------------------------------------------------------------------
// wxBZipInputStream
// ----------------------------------------------------------------------------

wxBZipInputStream::wxBZipInputStream(wxInputStream& Stream, 
									 bool bLessMemory) : wxFilterInputStream(Stream),
														 nBufferPos(0)
{
	hZip = new bz_stream;
	if (hZip == NULL)
	{
		wxLogSysError(wxT("OUT OF MEMORY!"));
		return;
	}

	((bz_stream*&)hZip)->bzalloc = NULL;
	((bz_stream*&)hZip)->bzfree = NULL;
	((bz_stream*&)hZip)->opaque = NULL;

	//param 2 - verbosity = 0-4, 4 more stuff to stdio
	//param 3 - small = non-zero means less memory and more time
	if (BZ2_bzDecompressInit((bz_stream*&)hZip, 0, bLessMemory)!= BZ_OK)
	{
		delete (bz_stream*&) hZip;
		wxLogSysError(wxT("Could not initialize bzip decompression engine!"));
	}
}

wxBZipInputStream::~wxBZipInputStream()
{
	BZ2_bzDecompressEnd((bz_stream*&)hZip);
	delete (bz_stream*&) hZip;
}

wxInputStream& wxBZipInputStream::ReadRaw(void* pBuffer, size_t size)
{
	return m_parent_i_stream->Read(pBuffer, size);
}

off_t wxBZipInputStream::TellRawI()
{
	return m_parent_i_stream->TellI();
}

off_t wxBZipInputStream::SeekRawI(off_t pos, wxSeekMode sm)
{
	return 0;
}

size_t wxBZipInputStream::OnSysRead(void* buffer, size_t bufsize)
{
	wxInt32 nRead = 0;

	((bz_stream*&)hZip)->next_out = &(((char*&)buffer)[nRead]);
	((bz_stream*&)hZip)->avail_out = bufsize - nRead;

	while (((bz_stream*&)hZip)->avail_out != 0)
	{
		//wxMessageBox(wxString::Format("%i %i", nRead, ((bz_stream*&)hZip)->avail_out));

		if (nBufferPos == 0 || nBufferPos == WXBZBS)
		{
			ReadRaw(pBuffer, WXBZBS);
			nBufferPos = 0;
			((bz_stream*&)hZip)->next_in = &pBuffer[nBufferPos];
			((bz_stream*&)hZip)->avail_in = WXBZBS - nBufferPos;
			if (m_parent_i_stream->LastRead() != WXBZBS)
			{
				((bz_stream*&)hZip)->avail_in = m_parent_i_stream->LastRead();

				int nRet = BZ2_bzDecompress((bz_stream*&)hZip);

				if (nRet == BZ_OK || nRet == BZ_STREAM_END)
					return bufsize - ((bz_stream*&)hZip)->avail_out;
				else
					return 0;
			}
		}
		((bz_stream*&)hZip)->next_in = &pBuffer[nBufferPos];
		((bz_stream*&)hZip)->avail_in = WXBZBS - nBufferPos;

		int nRet = BZ2_bzDecompress((bz_stream*&)hZip);

		if (nRet == BZ_OK)	
		{
			nBufferPos += -(nRead - (
			nRead += (WXBZBS - nBufferPos - ((bz_stream*&)hZip)->avail_in)
			));
		}
		else if(nRet == BZ_STREAM_END)
			return bufsize - ((bz_stream*&)hZip)->avail_out;
		else
			return 0;	
	}
 
	return bufsize - ((bz_stream*&)hZip)->avail_out;	
}

// ----------------------------------------------------------------------------
// wxBZipOutputStream
// ----------------------------------------------------------------------------

wxBZipOutputStream::wxBZipOutputStream(wxOutputStream& Stream,
									   wxInt32 nCompressionFactor) : 
																wxFilterOutputStream(Stream)
{
	hZip = new bz_stream;
	if (hZip == NULL)
	{
		wxLogSysError(wxT("OUT OF MEMORY!"));
		return;
	}

	((bz_stream*&)hZip)->bzalloc = NULL;
	((bz_stream*&)hZip)->bzfree = NULL;
	((bz_stream*&)hZip)->opaque = NULL;

	//param 2 - compression factor = 1-9 9 more compression but slower
	//param 3 - verbosity = 0-4, 4 more stuff to stdio (ignored)
	//param 4 - workfactor = reliance on standard comp alg, 0-250, 0==30 default
	if (BZ2_bzCompressInit((bz_stream*&)hZip, nCompressionFactor, 0, 0)!= BZ_OK)
	{
		delete (bz_stream*&) hZip;
		wxLogSysError(wxT("Could not initialize bzip compression engine!"));
	}
}

wxBZipOutputStream::~wxBZipOutputStream()
{
	BZ2_bzCompressEnd((bz_stream*&)hZip);
	delete (bz_stream*&) hZip;
}

wxOutputStream& wxBZipOutputStream::WriteRaw(void* pBuffer, size_t size)
{
	return m_parent_o_stream->Write(pBuffer, size);
}

off_t wxBZipOutputStream::TellRawO()
{
	return m_parent_o_stream->TellO();
}

off_t wxBZipOutputStream::SeekRawO(off_t pos, wxSeekMode sm)
{
	return 0;
}

size_t wxBZipOutputStream::OnSysWrite(const void* buffer, size_t bufsize)
{
	size_t nWrote = 0;
	int n;

	
	((bz_stream*&)hZip)->next_in = &(((char*&)buffer)[nWrote]);
	((bz_stream*&)hZip)->avail_in = bufsize - nWrote;
	((bz_stream*&)hZip)->next_out = &pBuffer[0];
	((bz_stream*&)hZip)->avail_out = WXBZBS;

	do {
	if ((n=BZ2_bzCompress((bz_stream*&)hZip, BZ_RUN)) != BZ_RUN_OK)
	{

		wxMessageBox(wxString::Format("BrokeC %i", n));
		break;
	}
	if (((bz_stream*&)hZip)->avail_out < WXBZBS)
	{
		((bz_stream*&)hZip)->next_out = &pBuffer[0];
		((bz_stream*&)hZip)->avail_out = WXBZBS;
		size_t nCurWrite = WXBZBS - ((bz_stream*&)hZip)->avail_out;
	//	wxMessageBox(wxString::Format("%i", nCurWrite));
		WriteRaw(pBuffer, nCurWrite);
		nWrote += m_parent_o_stream->LastWrite();


		if (m_parent_o_stream->LastWrite() != nCurWrite)
		{
			wxMessageBox("Broke");
			break;
		}
	}
	} while(((bz_stream*&)hZip)->avail_in != 0);

	while (nWrote != bufsize)
	{
		((bz_stream*&)hZip)->next_out = &pBuffer[0];
		((bz_stream*&)hZip)->avail_out = WXBZBS;

		int nRet = BZ2_bzCompress((bz_stream*&)hZip, BZ_FINISH);
		
		if (nRet != BZ_FINISH_OK && nRet != BZ_STREAM_END)
		{
			wxMessageBox("ErrorFK");
			break;
		}

		
		size_t nCurWrite = WXBZBS - ((bz_stream*&)hZip)->avail_out;
	//	wxMessageBox(wxString::Format("%i", nCurWrite));
		if (nCurWrite != 0)
		{
			WriteRaw(pBuffer, nCurWrite);
			nWrote += m_parent_o_stream->LastWrite();


			if (m_parent_o_stream->LastWrite() != nCurWrite)
			{
				wxMessageBox("Broke");
				break;
			}
		}

		if (nRet == BZ_STREAM_END)
		{
			wxMessageBox("GOOD");
			break;
		}

	}
 
//	wxMessageBox(wxString::Format("%i %i", nWrote, bufsize);
	return nWrote;	
}

// ----------------------------------------------------------------------------
// wxBZipStream
// ----------------------------------------------------------------------------

wxBZipStream::wxBZipStream(wxInputStream& i, wxOutputStream& o) : 
	wxBZipInputStream(i), wxBZipOutputStream(o) {}

wxBZipStream::~wxBZipStream(){}

#endif  //wxUSE_ZLIB && wxUSE_STREAMS
