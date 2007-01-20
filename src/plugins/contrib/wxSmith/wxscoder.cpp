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

#include "wxscoder.h"

#include <manager.h>
#include <editormanager.h>
#include <configmanager.h>
#include <messagemanager.h>
#include <wx/ffile.h>

static wxsCoder SingletonObject;
wxsCoder* wxsCoder::Singleton = &SingletonObject;

void wxsCoder::AddCode(const wxString& FileName,const wxString& Header,const wxString& End,const wxString& Code,bool Immediately,bool CodeHasHeader,bool CodeHasEnd)
{
    // Searching for file in opened file list
	EditorManager* EM = Manager::Get()->GetEditorManager();
	assert ( EM != NULL );
    cbEditor* Editor = EM->GetBuiltinEditor(FileName);

    if ( Editor )
    {
        ApplyChanges(Editor,Header,End,Code,CodeHasHeader,CodeHasEnd);
    }
    else
    {
        ApplyChanges(FileName,Header,End,Code,CodeHasHeader,CodeHasEnd);
    }
}

bool wxsCoder::ApplyChanges(cbEditor* Editor,const wxString& Header,const wxString& End,wxString Code,bool CodeHasHeader,bool CodeHasEnd)
{
	cbStyledTextCtrl* Ctrl = Editor->GetControl();
	Ctrl->SetSearchFlags(wxSCI_FIND_MATCHCASE);
	Ctrl->SetTargetStart(0);
	int FullLength = Ctrl->GetLength();
	Ctrl->SetTargetEnd(FullLength);
	int Position = Ctrl->SearchInTarget(Header);

	if ( Position == -1 )
	{
	    DBGLOG(_("wxSmith: Couldn't find code with header:\n\t\"%s\"\nin file '%s'"),
			Header.c_str(),
			Editor->GetFilename().c_str());
		return false;
	}

    // Beginning of this code block is in Position, now searching for end
    Ctrl->SetTargetStart(Position);
    Ctrl->SetTargetEnd(FullLength);
    int EndPosition = Ctrl->SearchInTarget(End);
    if ( EndPosition == -1 )
    {
        DBGLOG(_("wxSmith: Unfinished block of auto-generated code with header:\n\t\"%s\"\nin file '%s'"),
            Header.c_str(),
            Editor->GetFilename().c_str());
        return false;
    }

    wxString BaseIndentation;
    int IndentPos = Position;
    while ( --IndentPos >= 0 )
    {
        wxChar ch = Ctrl->GetCharAt(IndentPos);
        if ( (ch == _T('\n')) || (ch == _T('\r')) ) break;
    }
    while ( ++IndentPos < Position )
    {
        wxChar ch = Ctrl->GetCharAt(IndentPos);
        BaseIndentation.Append(
            ( ch == _T('\t') ) ? _T('\t') : _T(' '));
    }

    RebuildCode(BaseIndentation,Code);

    // Fixing up positions to contain or not header / ending sequence
    if ( !CodeHasHeader ) Position += Header.Length();
    if ( CodeHasEnd ) EndPosition += End.Length();

    // Checking of code has really changed
    if ( Ctrl->GetTextRange(Position,EndPosition) == Code )
    {
        return true;
    }

    // Replacing code
    Ctrl->SetTargetStart(Position);
    Ctrl->SetTargetEnd(EndPosition);
    Ctrl->ReplaceTarget(Code);
    Editor->SetModified();

    // TODO: Update fooldings

	return true;
}

bool wxsCoder::ApplyChanges(const wxString& FileName,const wxString& Header,const wxString& End,wxString Code,bool CodeHasHeader,bool CodeHasEnd)
{
    wxFFile File(FileName,_T("rb"));
    if ( !File.IsOpened() )
    {
    	DBGLOG(_("wxSmith: Couldn't open file '%s' for reading"),FileName.c_str());
    	return false;
    }

    // TODO: Fix encoding issues
    wxString Content;
    if ( !File.ReadAll(&Content) )
    {
        DBGLOG(_("wxSmith: Couldn't read from file '%s'"),FileName.c_str());
    	return false;
    }
    File.Close();

    int Position = Content.First(Header);

    if ( Position == -1 )
    {
    	DBGLOG(_("wxSmith: Couldn't find code with header:\n\t\"%s\"\nin file '%s'"),
			Header.c_str(),
			FileName.c_str());
		return false;
    }

    // Skipping header if necessary
    int IndentPos = Position;
    int IndentMax = Position;
    if ( !CodeHasHeader ) Position += Header.Length();

    wxString Result = Content.Left(Position);
    Content.Remove(0,Position);

    int EndPosition = Content.First(End);
    if ( EndPosition == -1 )
    {
        DBGLOG(_("wxSmith: Unfinished block of auto-generated code with header:\n\t\"%s\"\nin file '%s'"),
            Header.c_str(),
            FileName.c_str());
        return false;
    }

    // Including ending sequence if necessary
    if ( CodeHasEnd ) EndPosition += End.Length();

    wxString BaseIndentation;
    while ( --IndentPos >= 0 )
    {
        wxChar ch = Result.GetChar(IndentPos);
        if ( (ch == _T('\n')) || (ch == _T('\r')) ) break;
    }
    while ( ++IndentPos < IndentMax )
    {
        wxChar ch = Result.GetChar(IndentPos);
        BaseIndentation.Append(
            ( ch == _T('\t') ) ? _T('\t') : _T(' '));
    }

    RebuildCode(BaseIndentation,Code);

    // Checking if code has really changed
    if ( Content.Mid(0,EndPosition) == Code )
    {
        return true;
    }

    Result += Code;
    Result += Content.Remove(0,EndPosition);

    if ( !File.Open(FileName,_T("wb")) )
    {
    	DBGLOG(_("wxSmith: Couldn't open file '%s' for writing"),FileName.c_str());
    	return false;
    }

    // TODO: Fix encoding issues
    if ( !File.Write(Result) )
    {
    	DBGLOG(_("Couldn't write to file '%s'"),FileName.c_str());
    	return false;
    }

	return true;
}

wxString wxsCoder::GetCode(const wxString& FileName,const wxString& Header,const wxString& End,bool IncludeHeader,bool IncludeEnd)
{
    int TabSize = Manager::Get()->GetConfigManager(_T("editor"))->ReadInt(_T("/tab_size"), 4);

    // Checking if editor is opened
	EditorManager* EM = Manager::Get()->GetEditorManager();
	assert ( EM != NULL );
    cbEditor* Editor = EM->GetBuiltinEditor(FileName);

    if ( Editor )
    {
        cbStyledTextCtrl* Ctrl = Editor->GetControl();
        Ctrl->SetSearchFlags(wxSCI_FIND_MATCHCASE);
        Ctrl->SetTargetStart(0);
        Ctrl->SetTargetEnd(Ctrl->GetLength());
        int Position = Ctrl->SearchInTarget(Header);
        if ( Position == -1 ) return _T("");

        // Counting number of indentation spaces which will be removed at
        // the beginning of each line
        int SpacesCut = 0;
        int SpacesPos = Position;
        while ( --SpacesPos >= 0 )
        {
            wxChar ch = Ctrl->GetCharAt(SpacesPos);
            if ( ch == _T('\t') ) SpacesCut += TabSize;
            else if ( (ch==_T('\n')) || (ch==_T('\r')) ) break;
            else SpacesCut++;
        }

        Ctrl->SetTargetStart(Position);
        Ctrl->SetTargetEnd(Ctrl->GetLength());
        int EndPosition = Ctrl->SearchInTarget(End);
        if ( EndPosition == -1 ) return _T("");

        // Fixing up positions to include / exclude header and/or ending sequence
        if ( !IncludeHeader ) Position += Header.Length();
        if ( IncludeEnd ) EndPosition += End.Length();
        return CutSpaces(Ctrl->GetTextRange(Position,EndPosition),SpacesCut);
    }
    else
    {
        wxFFile File(FileName,_T("r"));
        wxString Content;
        if ( !File.IsOpened() ) return _T("");
        if ( !File.ReadAll(&Content) ) return _T("");
        int Position = Content.First(Header);
        if ( Position == -1 ) return _T("");
        int SpacesCut = 0;
        int SpacesPos = Position;
        while ( --SpacesPos >= 0 )
        {
            wxChar ch = Content.GetChar(SpacesPos);
            if ( ch == _T('\t') ) SpacesCut += TabSize;
            else if ( (ch==_T('\n')) || (ch==_T('\r')) ) break;
            else SpacesCut++;
        }

        if ( !IncludeHeader ) Position += Header.Length();
        Content.Remove(0,Position);
        int EndPosition = Content.First(End);
        if ( EndPosition == -1 ) return _T("");
        if ( IncludeEnd ) EndPosition += End.Length();
        Content.Remove(EndPosition);
        return CutSpaces(Content,SpacesCut);
    }
}

wxString wxsCoder::GetFullCode(const wxString& FileName)
{
    // Checking if editor is opened
	EditorManager* EM = Manager::Get()->GetEditorManager();
	assert ( EM != NULL );
    cbEditor* Editor = EM->GetBuiltinEditor(FileName);

    if ( Editor )
    {
        cbStyledTextCtrl* Ctrl = Editor->GetControl();
        return Ctrl->GetText();
    }
    else
    {
        wxFFile File(FileName,_T("r"));
        wxString Content;
        if ( !File.IsOpened() ) return _T("");
        if ( !File.ReadAll(&Content) ) return _T("");
        return Content;
    }
}

void wxsCoder::PutFullCode(const wxString& FileName,const wxString& Code)
{
    // Searching for file in opened file list
	EditorManager* EM = Manager::Get()->GetEditorManager();
	assert ( EM != NULL );
    cbEditor* Editor = EM->GetBuiltinEditor(FileName);

    if ( Editor )
    {
        Editor->GetControl()->SetText(Code);
    }
    else
    {
        wxFile Fl(FileName,wxFile::write);
        Fl.Write(Code);
    }
}

void wxsCoder::RebuildCode(wxString& BaseIndentation,wxString& Code)
{
    bool UseTab = Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/use_tab"), false);
    int TabSize = Manager::Get()->GetConfigManager(_T("editor"))->ReadInt(_T("/tab_size"), 4);
    int EolMode = Manager::Get()->GetConfigManager(_T("editor"))->ReadInt(_T("/eol/eolmode"), 0);

    if ( !UseTab )
    {
        Code.Replace(_T("\t"),wxString(_T(' '),TabSize));
    }

    switch ( EolMode )
    {
        case 1:
            BaseIndentation.Prepend(_T("\r"));
            break;

        case 2:
            BaseIndentation.Prepend(_T("\n"));
            break;

        default:
            BaseIndentation.Prepend(_T("\r\n"));
    }


    Code.Replace(_T("\n"),BaseIndentation);
}

wxString wxsCoder::CutSpaces(wxString Code,int Count)
{
    int TabSize = Manager::Get()->GetConfigManager(_T("editor"))->ReadInt(_T("/tab_size"), 4);
    if ( TabSize < 1 ) TabSize = 4;

    // Changing to \n line end mode
    wxString Result;

    for(;;)
    {
        int PosN = Code.Find(_T("\n"));
        int PosR = Code.Find(_T("\r"));

        if ( ( PosN < 0 ) && ( PosR < 0 ) ) break;

        int Pos;
        if ( PosN < 0 ) Pos = PosR;
        else if ( PosR < 0 ) Pos = PosN;
        else Pos = (PosN < PosR) ? PosN : PosR;

        Result.Append(Code.Left(Pos));
        Code.Remove(0,Pos);
        while ( Code.Length() )
        {
            if ( ( Code[0] != _T('\n') ) &&
                 ( Code[0] != _T('\r') ) ) break;
            Code.Remove(0,1);
        }
        int LeftSpaces = Count;
        while ( Code.Length() && LeftSpaces > 0 )
        {
            if ( Code[0] == _T(' ') ) LeftSpaces--;
            else if ( Code[0] == _T('\t') ) LeftSpaces -= TabSize;
            else break;
            Code.Remove(0,1);
        }
        Result.Append(_T('\n'));
    }

    Result.Append(Code);
    return Result;
}
