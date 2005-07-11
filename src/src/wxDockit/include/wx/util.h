///////////////////////////////////////////////////////////////////////////////
// Name:        wx/util.h
// Purpose:     wxUtil namespace
// Author:      Mark McCormack
// Modified by:
// Created:     07/06/04
// RCS-ID:      
// Copyright:   
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UTIL_H_
#define _WX_UTIL_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <wx/string.h>
#include <wx/stream.h>
#include <wx/wfstream.h>

#include <wx/window.h>
#include <wx/frame.h>

// ----------------------------------------------------------------------------
// wxUtil
// ----------------------------------------------------------------------------

namespace wxUtil {
    
    // wxString <-> wxStream
    void WriteString( wxOutputStream& stream, const wxString& string );
    wxString ReadString( wxInputStream& stream );

    // filename checking
    bool SaveCheckFilename( const wxString& file );
    bool LoadCheckFilename( const wxString& file );

    // wxWindow/wxFrame <-> wxStream
    void WriteWindowLayout( wxOutputStream& stream, wxWindow * pWindow );
    void ReadWindowLayout( wxInputStream& stream, wxWindow * pWindow );

}   // namespace wxUtil

#endif
    // _WX_UTIL_H_
