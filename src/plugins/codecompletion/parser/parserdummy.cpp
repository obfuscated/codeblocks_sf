/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#include "parser.h"

BEGIN_EVENT_TABLE(Parser, wxEvtHandler)
END_EVENT_TABLE()

wxString Parser::GetFullFileName(const wxString & Filename,const wxString& filename,bool isGlobal)
{
    return wxEmptyString;
}

void Parser::DoParseFile(const wxString & filename, bool flag)
{
}

Parser::Parser(wxEvtHandler* pEvt, cbProject* project):m_Pool(NULL,0,0)
{
}

Parser::~Parser()
{
}
