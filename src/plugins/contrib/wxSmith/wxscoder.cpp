/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxscoder.h"

#include <manager.h>
#include <editormanager.h>
#include <configmanager.h>
#include <logmanager.h>
#include <encodingdetector.h>
#include <globals.h>
#include <wx/file.h>
#include <wx/intl.h>
#include "cbstyledtextctrl.h"

namespace
{
    bool ReadFileContentWithProperEncoding(const wxString& FileName,wxString& Content,wxFontEncoding& Encoding,bool& UseBOM)
    {
        EncodingDetector Detector(FileName);
        if ( !Detector.IsOK() ) return false;
        Encoding = Detector.GetFontEncoding();
        if ( Encoding == wxFONTENCODING_ISO8859_1 )
        {
            wxString enc_name = Manager::Get()->GetConfigManager(_T("editor"))->Read(_T("/default_encoding"), wxLocale::GetSystemEncodingName());
            Encoding = wxFontMapper::GetEncodingFromName(enc_name);
        }
        UseBOM = Detector.UsesBOM();
        wxFile Fl(FileName,wxFile::read);
        if ( !Fl.IsOpened() ) return false;
        if ( !cbRead(Fl,Content,Encoding) ) return false;
        Content.Remove(0,Detector.GetBOMSizeInBytes() / sizeof(wxChar));
        return true;
    }
}

static wxsCoder SingletonObject;
wxsCoder* wxsCoder::Singleton = &SingletonObject;

wxsCoder::wxsCoder()
{
    FlushTimer.SetOwner(this,1);
    Connect(wxEVT_TIMER,(wxObjectEventFunction)&wxsCoder::FlushTimerEvent);
}

wxsCoder::~wxsCoder()
{
    FlushAll();
}

void wxsCoder::AddCode(const wxString& FileName,const wxString& Header,const wxString& End,const wxString& Code,bool Immediately,bool CodeHasHeader,bool CodeHasEnd)
{
    wxMutexLocker Lock(DataMutex);

    wxString FixedFileName = NormalizeFileName(FileName);
    if ( FixedFileName.IsEmpty() )
    {
        return;
    }

    // Find changing file
    int Index = CodeChangesFiles.Index(FileName);
    if ( Index==wxNOT_FOUND )
    {
        Index = CodeChangesFiles.Count();
        CodeChangesFiles.Add(FileName);
        CodeChanges.Add(0);
    }

    // Add entry to list of waiting changes
    CodeChange* Change = new CodeChange;
    Change->Header = Header;
    Change->End = End;
    Change->Code = Code;
    Change->CodeHasHeader = CodeHasHeader;
    Change->CodeHasEnd = CodeHasEnd;
    Change->Next = CodeChanges[Index];
    CodeChanges[Index] = Change;

    // If the change has already been put onto queue, delete it
    for ( CodeChange *Prev=Change, *This=Prev->Next; This; Prev=This, This=This->Next )
    {
        if ( This->Header==Header && This->End==End )
        {
            Prev->Next = This->Next;
            delete This;
            This = Prev;
        }
    }

    if ( Immediately )
    {
        FlushFile(FixedFileName);
    }
}

wxString wxsCoder::GetCode(const wxString& FileName,const wxString& Header,const wxString& End,bool IncludeHeader,bool IncludeEnd)
{
    wxMutexLocker Lock(DataMutex);

    wxString FixedFileName = NormalizeFileName(FileName);
    FlushFile(FixedFileName);

    int TabSize = Manager::Get()->GetConfigManager(_T("editor"))->ReadInt(_T("/tab_size"), 4);

    // Checking if editor is opened
	EditorManager* EM = Manager::Get()->GetEditorManager();
	assert ( EM != 0 );
    cbEditor* Editor = EM->GetBuiltinEditor(FixedFileName);

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
        wxString Content;
        wxFontEncoding Encoding;
        bool UseBOM;
        if ( !ReadFileContentWithProperEncoding(FixedFileName,Content,Encoding,UseBOM) ) return _T("");

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

wxString wxsCoder::GetFullCode(const wxString& FileName,wxFontEncoding& Encoding,bool &UseBOM)
{
    wxMutexLocker Lock(DataMutex);

    wxString FixedFileName = NormalizeFileName(FileName);
    FlushFile(FixedFileName);

    // Checking if editor is opened
	EditorManager* EM = Manager::Get()->GetEditorManager();
	assert ( EM != 0 );
    cbEditor* Editor = EM->GetBuiltinEditor(FixedFileName);

    if ( Editor )
    {
        Encoding = Editor->GetEncoding();
        UseBOM = Editor->GetUseBom();
        cbStyledTextCtrl* Ctrl = Editor->GetControl();
        return Ctrl->GetText();
    }
    else
    {
        wxString Content;
        if ( !ReadFileContentWithProperEncoding(FixedFileName,Content,Encoding,UseBOM) ) return _T("");
        return Content;
    }
}

void wxsCoder::PutFullCode(const wxString& FileName,const wxString& Code,wxFontEncoding Encoding,bool UseBOM)
{
    wxMutexLocker Lock(DataMutex);

    wxString FixedFileName = NormalizeFileName(FileName);
    int Index = CodeChangesFiles.Index(FixedFileName);
    if ( Index != wxNOT_FOUND )
    {
        for ( CodeChange* Change=CodeChanges[Index]; Change; )
        {
            CodeChange* Next = Change->Next;
            delete Change;
            Change = Next;
        }
        CodeChanges[Index] = 0;
    }

    // Searching for file in opened file list
	EditorManager* EM = Manager::Get()->GetEditorManager();
	assert ( EM != 0 );
    cbEditor* Editor = EM->GetBuiltinEditor(FixedFileName);

    if ( Editor )
    {
        Editor->GetControl()->SetText(Code);
    }
    else
    {
        cbSaveToFile(FixedFileName,Code,Encoding,UseBOM);
    }
}

void wxsCoder::FlushFile(const wxString& FileName)
{
    int Index = CodeChangesFiles.Index(FileName);
    if ( Index == wxNOT_FOUND ) return;

    CodeChange* Changes = CodeChanges[Index];
    if ( !Changes ) return;

    // Searching for file in opened file list
	EditorManager* EM = Manager::Get()->GetEditorManager();
	assert ( EM != 0 );
    cbEditor* Editor = EM->GetBuiltinEditor(FileName);

    if ( Editor )
    {
        wxString EOL;
        while ( Changes )
        {
            CodeChange* Next = Changes->Next;
            ApplyChangesEditor(Editor,Changes->Header,Changes->End,Changes->Code,Changes->CodeHasHeader,Changes->CodeHasEnd,EOL);
            delete Changes;
            Changes = Next;
        }
    }
    else
    {
        // Reading file content
        wxString Content;
        wxFontEncoding Encoding;
        wxString EOL;
        bool UseBOM;
        bool HasChanged = false;

        //wxStopWatch SW;
        if ( !ReadFileContentWithProperEncoding(FileName,Content,Encoding,UseBOM) )
        {
            Manager::Get()->GetLogManager()->DebugLog(F(_("wxSmith: Couldn't open file '%s'"),FileName.c_str()));
            return;
        }
        //Manager::Get()->GetLogManager()->DebugLog(F(_T("File read time: %d ms"),SW.Time()));

        while ( Changes )
        {
            CodeChange* Next = Changes->Next;
            ApplyChangesString(Content,Changes->Header,Changes->End,Changes->Code,Changes->CodeHasHeader,Changes->CodeHasEnd,HasChanged,EOL);
            delete Changes;
            Changes = Next;
        }

        if ( HasChanged )
        {
            // Storing the result
            //wxStopWatch SW;
            cbSaveToFile(FileName,Content,Encoding,UseBOM);
            //Manager::Get()->GetLogManager()->DebugLog(F(_T("File write time: %d ms"),SW.Time()));
        }
    }

    CodeChanges[Index] = 0;
}

bool wxsCoder::ApplyChangesEditor(cbEditor* Editor,const wxString& Header,const wxString& End,wxString& Code,bool CodeHasHeader,bool CodeHasEnd,wxString& EOL)
{
	cbStyledTextCtrl* Ctrl = Editor->GetControl();
	int FullLength = Ctrl->GetLength();

    if ( EOL.IsEmpty() )
    {
        // Detecting EOL style in source
        for ( int i=0; i<FullLength; i++ )
        {
            wxChar ch = Ctrl->GetCharAt(i);
            if ( ch==_T('\n') || ch==_T('\r') )
            {
                EOL = ch;
                if ( ++i < FullLength )
                {
                    wxChar ch2 = Ctrl->GetCharAt(i);
                    if ( (ch2==_T('\n') || ch2==_T('\r')) && ch!=ch2 )
                    {
                        EOL.Append(ch2);
                    }
                }
                break;
            }
        }
    }

    // Searching for beginning of section to replace
	Ctrl->SetSearchFlags(wxSCI_FIND_MATCHCASE);
	Ctrl->SetTargetStart(0);
	Ctrl->SetTargetEnd(FullLength);
	int Position = Ctrl->SearchInTarget(Header);

	if ( Position == -1 )
	{
	    Manager::Get()->GetLogManager()->DebugLog(F(_("wxSmith: Couldn't find code with header:\n\t\"%s\"\nin file '%s'"),
			Header.c_str(),
			Editor->GetFilename().c_str()));
		return false;
	}

    // Beginning of this code block is in Position, now searching for end
    Ctrl->SetTargetStart(Position);
    Ctrl->SetTargetEnd(FullLength);
    int EndPosition = Ctrl->SearchInTarget(End);
    if ( EndPosition == -1 )
    {
        Manager::Get()->GetLogManager()->DebugLog(F(_("wxSmith: Unfinished block of auto-generated code with header:\n\t\"%s\"\nin file '%s'"),
            Header.c_str(),
            Editor->GetFilename().c_str()));
        return false;
    }

    // Fetching indentation
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

    Code = RebuildCode(BaseIndentation,Code.c_str(),(int)Code.Length(),EOL);

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

bool wxsCoder::ApplyChangesString(wxString& BaseContent,const wxString& Header,const wxString& End,wxString& Code,bool CodeHasHeader,bool CodeHasEnd,bool& HasChanged,wxString& EOL)
{
    wxString Content = BaseContent;
    if ( EOL.IsEmpty() )
    {
        // Detecting EOL in this sources
        for ( size_t i=0; i<Content.Length(); i++ )
        {
            wxChar ch = Content.GetChar(i);
            if ( ch==_T('\n') || ch==_T('\r') )
            {
                EOL = ch;
                if ( ++i < Content.Length() )
                {
                    wxChar ch2 = Content.GetChar(i);
                    if ( (ch2==_T('\n') || ch2==_T('\r')) && ch!=ch2 )
                    {
                        EOL.Append(ch2);
                    }
                }
                break;
            }
        }
    }

    // Search for header
    int Position = Content.First(Header);

    if ( Position == -1 )
    {
    	Manager::Get()->GetLogManager()->DebugLog(F(_("wxSmith: Couldn't find code with header:\n\t\"%s\""),Header.c_str()));
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
        Manager::Get()->GetLogManager()->DebugLog(F(_("wxSmith: Unfinished block of auto-generated code with header:\n\t\"%s\""),Header.c_str()));
        return false;
    }

    // Including ending sequence if necessary
    if ( CodeHasEnd ) EndPosition += End.Length();

    // Fetching indentation
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

    Code = RebuildCode(BaseIndentation,Code.c_str(),Code.Length(),EOL);

    // Checking if code has really changed
    if ( Content.Mid(0,EndPosition) == Code )
    {
        return true;
    }

    HasChanged = true;
    Result += Code;
    Result += Content.Remove(0,EndPosition);
    BaseContent = Result;

    return true;
}

wxString wxsCoder::RebuildCode(wxString& BaseIndentation,const wchar_t* Code,int CodeLen,wxString& EOL)
{
    wxString Tab;
    bool UseTab = Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/use_tab"), false);
    int TabSize = Manager::Get()->GetConfigManager(_T("editor"))->ReadInt(_T("/tab_size"), 4);
    if ( !UseTab )
    {
        Tab.Append(_T(' '),TabSize);
    }

    if ( EOL.IsEmpty() )
    {
        int EolMode = Manager::Get()->GetConfigManager(_T("editor"))->ReadInt(_T("/eol/eolmode"), 0);
        switch ( EolMode )
        {
            case 1:  EOL = _T("\r"); break;
            case 2:  EOL = _T("\n"); break;
            default: EOL = _T("\r\n");
        }
    }

    BaseIndentation.Prepend(EOL);

    wxString Result;
    Result.reserve(CodeLen+10);

    while ( *Code )
    {
        switch ( *Code )
        {
            case _T('\n'): Result << BaseIndentation; break;
            case _T('\t'): if ( UseTab ) { Result << Tab; break; }
            default:       Result << *Code;
        }
        Code++;
    }

    return Result;
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

wxString wxsCoder::NormalizeFileName(const wxString& FileName)
{
    // Updating the file name in case there are some ".." or "." enteries which prevents from finding
    // opened editor for file
    wxFileName FixedNameObject(FileName);
    FixedNameObject.Normalize(wxPATH_NORM_DOTS);
    return FixedNameObject.GetFullPath();
}

void wxsCoder::Flush(int Delay)
{
    if ( Delay<=0 )
    {
        FlushTimer.Stop();
        FlushAll();
    }
    else
    {
        FlushTimer.Start(Delay,true);
    }
}

void wxsCoder::FlushAll()
{
    wxStopWatch SW;
    for ( int i=0; i<(int)CodeChangesFiles.Count(); i++ )
    {
        FlushFile(CodeChangesFiles[i]);
    }
    CodeChanges.Clear();
    CodeChangesFiles.Clear();
    //Manager::Get()->GetLogManager()->DebugLog(F(_T("wxSmith: Flushing of code done in %d ms"),SW.Time()));
}

void wxsCoder::FlushTimerEvent(wxTimerEvent& event)
{
    FlushAll();
}
