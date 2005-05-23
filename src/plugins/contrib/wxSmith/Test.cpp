/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Id$
* $Date$
*/

#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/fontutil.h>
#include <wx/notebook.h>

#include "test.h" // class's header file

// class constructor
_cbEditor::_cbEditor(wxMDIParentFrame* parent, const wxString& filename)
    : wxMDIChildFrame(parent, -1, filename, wxDefaultPosition, wxDefaultSize, wxMAXIMIZE | wxDEFAULT_FRAME_STYLE | wxCLIP_CHILDREN)
 {
}

// class destructor
_cbEditor::~_cbEditor()
{
}

