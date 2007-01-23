/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxseventseditor.h"
#include "wxsitem.h"
#include "wxsitemresdata.h"
#include "../wxscoder.h"

#include <editormanager.h>

namespace
{
    const wxString NoneStr   = _("-- None --");
    const wxString AddNewStr = _("-- Add new handler --");
}

wxsEventsEditor::wxsEventsEditor():
    m_Item(NULL),
    m_Events(NULL),
    m_Source(),
    m_Header(),
    m_Class(),
    m_Ids()
{
}

wxsEventsEditor::~wxsEventsEditor()
{
    m_Item = NULL;
    m_Events = NULL;
    m_Ids.Clear();
}

wxsEventsEditor& wxsEventsEditor::Get()
{
    static wxsEventsEditor Singleton;
    return Singleton;
}

void wxsEventsEditor::BuildEvents(wxsItem* Item,wxsPropertyGridManager* Grid)
{
    m_Item = Item;
    m_Data = NULL;
    m_Events = NULL;
    m_Ids.Clear();
    m_Source.Clear();
    m_Header.Clear();
    m_Class.Clear();

    int PageIndex = 1;              // TODO: Do not use fixed page number
    Grid->ClearPage(PageIndex);
    Grid->SetTargetPage(PageIndex);

    if ( !m_Item )
    {
        return;
    }

    m_Events = &m_Item->GetEvents();
    m_Data   = m_Item->GetResourceData();
    m_Source = m_Data->GetSrcFileName();
    m_Header = m_Data->GetHdsFileName();
    m_Class  = m_Data->GetClassName();
    m_Language = m_Data->GetLanguage();

	int Cnt = m_Events->GetCount();
	for ( int i=0; i<Cnt; i++ )
	{
		const wxsEventDesc* Event = m_Events->GetDesc(i);
		const wxString& FunctionName = m_Events->GetHandler(i);

        // TODO: Create new group
		if ( Event->ET == wxsEventDesc::Category ) continue;

		wxArrayString Functions;
		FindFunctions(Event->ArgType,Functions);

        wxPGConstants Const;
        Const.Add(NoneStr,0);
        Const.Add(AddNewStr,1);

        int Index = 0;
        for ( int j=0; j<(int)Functions.Count(); j++ )
        {
        	Const.Add(Functions[j],j+2);
        	if ( Functions[j] == FunctionName )
        	{
        		Index = j+2;
        	}
        }
        if ( Index == 0 )
        {
            m_Events->SetHandler(i,_T(""));
        }

        m_Ids.Add(Grid->Append(wxEnumProperty(Event->Entry,wxPG_LABEL,Const,Index)));
	}
}

void wxsEventsEditor::PGChanged(wxsItem* Item,wxsPropertyGridManager* Grid,wxPGId Id)
{
    // Just small check to avoid some invalid updates
    if ( Item != m_Item )
    {
        return;
    }

    int Index;
	for ( Index=0; Index<(int)m_Ids.Count(); Index++ )
	{
	    if ( m_Ids[Index] == Id ) break;
	}

	if ( Index>=(int)m_Ids.Count() )
	{
	    return;
	}

	wxString Selection = Grid->GetPropertyValueAsString(Id);

    if ( Selection == NoneStr )
    {
        m_Events->SetHandler(Index,_T(""));
    }
    else if ( Selection == AddNewStr )
    {
        m_Events->SetHandler(Index,GetNewFunction(m_Events->GetDesc(Index)));
        BuildEvents(m_Item,Grid);
    }
    else
    {
        m_Events->SetHandler(Index,Selection);
        // TODO: Jump to selected function
    }

    m_Data->NotifyChange(m_Item);
}

void wxsEventsEditor::FindFunctions(const wxString& ArgType,wxArrayString& Array)
{
	wxString Code = wxsCoder::Get()->GetCode(m_Header,
        wxsCodeMarks::Beg(m_Language,_T("Handlers"),m_Class),
        wxsCodeMarks::End(m_Language),
        false,false);

    switch ( m_Language )
    {
        case wxsCPP:
        {
            // Basic parsing

            for(;;)
            {
                // Searching for void statement - it may begin new fuunction declaration

                int Pos = Code.Find(_T("void"));
                if ( Pos == -1 ) break;

                // Removing all before function name
                Code.Remove(0,Pos+4).Trim(false);

                // Getting function name
                Pos  = 0;
                while ( (int)Code.Length() > Pos )
                {
                    wxChar First = Code.GetChar(Pos);
                    if ( ( First<_T('A') || First>_T('Z') ) &&
                         ( First<_T('a') || First>_T('z') ) &&
                         ( First<_T('0') || First>_T('9') ) &&
                         ( First!=_T('_') ) )
                    {
                        break;
                    }
                    Pos++;
                }
                wxString NewFunctionName = Code.Mid(0,Pos);
                Code.Remove(0,Pos).Trim(false);
                if ( !Code.Length() ) break;

                // Parsing arguments
                if ( Code.GetChar(0) != _T('(') ) continue;
                Code.Remove(0,1).Trim(false);
                if ( !Code.Length() ) break;

                // Getting argument type
                Pos  = 0;
                while ( (int)Code.Length() > Pos )
                {
                    wxChar First = Code.GetChar(Pos);
                    if ( ( First<_T('A') || First>_T('Z') ) &&
                         ( First<_T('a') || First>_T('z') ) &&
                         ( First<_T('0') || First>_T('9') ) &&
                         ( First!=_T('_') ) )
                    {
                        break;
                    }
                    Pos++;
                }
                wxString NewEventType = Code.Mid(0,Pos);
                Code.Remove(0,Pos).Trim(false);
                if ( !Code.Length() ) break;

                // Checking if the rest of declaratin is valid
                if ( Code.GetChar(0) != _T('&') ) continue;
                Code.Remove(0,1).Trim(false);
                if ( !Code.Length() ) break;

                // Skipping argument name
                Pos  = 0;
                while ( (int)Code.Length() > Pos )
                {
                    wxChar First = Code.GetChar(Pos);
                    if ( ( First<_T('A') || First>_T('Z') ) &&
                         ( First<_T('a') || First>_T('z') ) &&
                         ( First<_T('0') || First>_T('9') ) &&
                         ( First!=_T('_') ) )
                    {
                        break;
                    }
                    Pos++;
                }
                Code.Remove(0,Pos).Trim(false);
                if ( !Code.Length() ) break;

                if ( Code.GetChar(0) != _T(')') ) continue;
                Code.Remove(0,1).Trim(false);
                if ( !Code.Length() ) break;

                if ( Code.GetChar(0) != _T(';') ) continue;
                Code.Remove(0,1).Trim(false);

                if ( NewFunctionName.Length() == 0 || NewEventType.Length() == 0 ) continue;

                // We got new function, checking event type and adding to array

                if ( !ArgType.Length() || ArgType == NewEventType )
                {
                    Array.Add(NewFunctionName);
                }
            }
            break;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsEventsEditor::FindFunctions"),m_Language);
        }
    }

}

wxString wxsEventsEditor::GetFunctionProposition(const wxsEventDesc* Event)
{
    // Creating proposition of new function name

    wxString NewNameBase;
    NewNameBase.Printf(_T("On%s%s"),m_Item->GetVarName().c_str(),Event->NewFuncNameBase.c_str());

    int Suffix = 0;
    wxArrayString Functions;
    FindFunctions(_T(""),Functions);
    wxString NewName = NewNameBase;

    while ( Functions.Index(NewName) != wxNOT_FOUND )
    {
    	NewName = NewNameBase;
    	NewName.Append(wxString::Format(_T("%d"),++Suffix));
    }

    return NewName;
}

wxString wxsEventsEditor::GetNewFunction(const wxsEventDesc* Event)
{
	wxString Name = GetFunctionProposition(Event);

	for (;;)
	{
		Name = ::wxGetTextFromUser(_("Enter name for new handler:"),_("New handler"),Name);
		if ( !Name.Length() ) return _T("");

        if ( !wxsCodeMarks::ValidateIdentifier(m_Language,Name) )
        {
        	wxMessageBox(_("Invalid name"));
        	continue;
        }

        wxArrayString Functions;
        FindFunctions(_T(""),Functions);

        if ( Functions.Index(Name) != wxNOT_FOUND )
        {
        	wxMessageBox(_("Handler with this name already exists"));
        	continue;
        }

        break;
	}

	// Creating new function

	if ( !CreateNewFunction(Event,Name) )
	{
		wxMessageBox(_("Couldn't add new handler"));
		return _T("");
	}

	return Name;
}

bool wxsEventsEditor::CreateNewFunction(const wxsEventDesc* Event,const wxString& NewFunctionName)
{
    switch ( m_Language )
    {
        case wxsCPP:
        {
            wxString Declarations = wxsCoder::Get()->GetCode(
                m_Header,
                wxsCodeMarks::Beg(wxsCPP,_T("Handlers"),m_Class),
                wxsCodeMarks::End(wxsCPP),
                false,false);

            if ( Declarations.Length() == 0 )
            {
                return false;
            }

            Declarations << _T("void ") << NewFunctionName << _T('(');
            Declarations << Event->ArgType << _T("& event);\n");

            wxsCoder::Get()->AddCode(
                m_Header,
                wxsCodeMarks::Beg(wxsCPP,_T("Handlers"),m_Class),
                wxsCodeMarks::End(wxsCPP),
                Declarations,
                true, false, false);

            cbEditor* Editor = Manager::Get()->GetEditorManager()->Open(m_Source);
            if ( !Editor )
            {
                return false;
            }

            wxString NewFunctionCode;
            NewFunctionCode <<
                _T("\n")
                _T("void ") << m_Class << _T("::") << NewFunctionName << _T("(") << Event->ArgType << _T("& event)\n")
                _T("{\n")
                _T("}\n");

            cbStyledTextCtrl* Ctrl = Editor->GetControl();
            Ctrl->DocumentEnd();
            Ctrl->AddText(NewFunctionCode);
            Ctrl->LineUp();
            Ctrl->LineUp();
            Ctrl->LineEnd();

            Editor->SetModified();
            return true;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsEventsEditor::CreateNewFunction"),m_Language);
        }
    }
    return true;
}
