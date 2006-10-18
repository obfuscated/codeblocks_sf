#include "wxsheaders.h"
#include "wxscoder.h"

#include <manager.h>
#include <editormanager.h>
#include <configmanager.h>
#include <messagemanager.h>
#include <wx/ffile.h>

#include "wxsglobals.h"

wxsCoder* wxsCoder::Singleton = NULL;

wxsCoder::wxsCoder():
	Enteries(NULL),
	BlockProcess(false)
{
    if ( Singleton==NULL ) Singleton = this;
}

wxsCoder::~wxsCoder()
{
	ProcessCodeQueue();
	if ( Singleton==this ) Singleton = NULL;
}

void wxsCoder::AddCode(const wxString& FileName,const wxString& BlockHeader,const wxString& Code,bool Immediately)
{
	DataMutex.Lock();

	// Iterating through all enteries searching for currently associated code

	CodeEntry* Entry;
	CodeEntry* Previous;
	for ( Entry = Enteries, Previous = NULL;
	      Entry != NULL;
	      Previous = Entry, Entry = Entry->Next )
	{
		if ( Entry->FileName == FileName && Entry->BlockHeader == BlockHeader )
			break;
	}

	if ( Entry )
	{
		( Previous ? Previous->Next : Enteries ) = Entry->Next;
		Previous = Entry;
		if ( Previous ) while ( Previous->Next ) Previous = Previous->Next;
	}
	else
	{
		Entry = new CodeEntry;
		Entry->FileName = FileName;
		Entry->BlockHeader = BlockHeader;
	}

	Entry->Code = Code;

	if ( Immediately )
	{
		ApplyChanges(Entry);
		delete Entry;
	}
	else
	{
        time(&Entry->TimeStamp);
        Entry->Next = NULL;
        ( Previous ? Previous->Next : Enteries ) = Entry;
	}
	DataMutex.Unlock();

// TODO (SpOoN#1#): Add sheduling and replace direct processing

	ProcessCodeQueue();
}

bool wxsCoder::ProcessCodeQueue()
{
	wxMutexLocker Locker(DataMutex);

	if ( BlockProcess ) return false;
	BlockProcess = true;

	bool Result = true;
	time_t now;
	time(&now);

// TODO (SpOoN#1#): Process requests which are staying inside queue long enough
	while ( Enteries )
	{
		CodeEntry* Entry = Enteries;
		ApplyChanges(Entry);
		Enteries = Enteries->Next;
		delete Entry;
	}

	BlockProcess = false;
	return Result;
}

void wxsCoder::DropQueue()
{
	wxMutexLocker Locker(DataMutex);

	for ( CodeEntry* Entry = Enteries, *Next; Entry; Entry = Next )
	{
		Next = Entry->Next;
		delete Entry;
	}
}

bool wxsCoder::ApplyChanges(wxsCoder::CodeEntry* Entry)
{
	EditorManager* EM = Manager::Get()->GetEditorManager();
	assert ( EM != NULL );

    cbEditor* Editor = EM->GetBuiltinEditor(Entry->FileName);
    if ( Editor )
    {
        return ApplyChanges(Entry,Editor);
    }

    return ApplyChanges(Entry,Entry->FileName);
}

bool wxsCoder::ApplyChanges(wxsCoder::CodeEntry* Entry,cbEditor* Editor)
{
	assert ( Editor != NULL );
	cbStyledTextCtrl* Ctrl = Editor->GetControl();
	assert ( Ctrl != NULL );

	Ctrl->SetSearchFlags(wxSCI_FIND_MATCHCASE);

	Ctrl->SetTargetStart(0);
	Ctrl->SetTargetEnd(Ctrl->GetLength());

	int Position = Ctrl->SearchInTarget(Entry->BlockHeader);

	if ( Position == -1 )
	{
	    DBGLOG(_("wxSmith: Couldn't find code with header:\n\t\"%s\"\nin file '%s'"),
			Entry->BlockHeader.c_str(),
			Editor->GetFilename().c_str());
		return false;
	}

    // Beginning of this code block is in Position, now searching for end
    Ctrl->SetTargetStart(Position);
    Ctrl->SetTargetEnd(Ctrl->GetLength());
    int End = Ctrl->SearchInTarget(wxsBEnd());
    if ( End == -1 )
    {
        DBGLOG(_("wxSmith: Unfinished block of auto-generated code with header:\n\t\"%s\"\nin file '%s'"),
            Entry->BlockHeader.c_str(),
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

    RebuildCode(BaseIndentation,Entry->Code);

    if ( Ctrl->GetTextRange(Position,End) == Entry->Code ) return true;

    Ctrl->SetTargetStart(Position);
    Ctrl->SetTargetEnd(End);
    Ctrl->ReplaceTarget(Entry->Code);
    Editor->SetModified();

	return true;
}

bool wxsCoder::ApplyChanges(wxsCoder::CodeEntry* Entry,const wxString& FileName)
{
    wxFFile File(FileName,_T("rb"));
    if ( !File.IsOpened() )
    {
    	DBGLOG(_("wxSmith: Couldn't open file '%s' for reading"),FileName.c_str());
    	return false;
    }

    wxString Content;
    if ( !File.ReadAll(&Content) )
    {
        DBGLOG(_("wxSmith: Couldn't read from file '%s'"),FileName.c_str());
    	return false;
    }
    File.Close();

    int Position = Content.First(Entry->BlockHeader);

    if ( Position == -1 )
    {
    	DBGLOG(_("wxSmith: Couldn't find code with header:\n\t\"%s\"\nin file '%s'"),
			Entry->BlockHeader.c_str(),
			FileName.c_str());
		return false;
    }

    wxString Result = Content.Left(Position);
    Content.Remove(0,Position);
    Position = Content.First(wxsBEnd());
    if ( Position == -1 )
    {
        DBGLOG(_("wxSmith: Unfinished block of auto-generated code with header:\n\t\"%s\"\nin file '%s'"),
            Entry->BlockHeader.c_str(),
            FileName.c_str());
        return false;
    }

    wxString BaseIndentation;
    int IndentPos = Result.Length();
    while ( --IndentPos >= 0 )
    {
        wxChar ch = Result.GetChar(IndentPos);
        if ( (ch == _T('\n')) || (ch == _T('\r')) ) break;
    }
    while ( ++IndentPos < (int)Result.Length() )
    {
        wxChar ch = Result.GetChar(IndentPos);
        BaseIndentation.Append(
            ( ch == _T('\t') ) ? _T('\t') : _T(' '));
    }

    RebuildCode(BaseIndentation,Entry->Code);

    if ( Content.Mid(0,Position) == Entry->Code ) return true;

    Result += Entry->Code;
    Result += Content.Remove(0,Position);

    if ( !File.Open(FileName,_T("wb")) )
    {
    	DBGLOG(_("wxSmith: Couldn't open file '%s' for writing"),FileName.c_str());
    	return false;
    }

    if ( !File.Write(Result) )
    {
    	DBGLOG(_("Couldn't write to file '%s'"),FileName.c_str());
    	return false;
    }

	return true;
}

bool wxsCoder::ProcessCodeForFile(const wxString& FileName)
{
	wxMutexLocker Locker(DataMutex);

	bool Result = true;

	for ( CodeEntry* Entry = Enteries, *Previous = NULL; Entry; )
	{
		if ( Entry->FileName == FileName )
		{
			ApplyChanges(Entry);
			CodeEntry* Next = ( Previous ? Previous->Next : Enteries ) = Entry->Next;
			delete Entry;
			Entry = Next;
		}
		else
		{
			Previous = Entry;
			Entry = Entry->Next;
		}
	}

	return Result;
}

wxString wxsCoder::GetCode(const wxString& FileName,const wxString& BlockHeader)
{
	EditorManager* EM = Manager::Get()->GetEditorManager();
	assert ( EM != NULL );
    cbEditor* Editor = EM->GetBuiltinEditor(FileName);
    int TabSize = Manager::Get()->GetConfigManager(_T("editor"))->ReadInt(_T("/tab_size"), 4);
    if ( Editor )
    {
        cbStyledTextCtrl* Ctrl = Editor->GetControl();
        Ctrl->SetSearchFlags(wxSCI_FIND_MATCHCASE);
        Ctrl->SetTargetStart(0);
        Ctrl->SetTargetEnd(Ctrl->GetLength());
        int Position = Ctrl->SearchInTarget(BlockHeader);
        if ( Position == -1 ) return _T("");
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
        int End = Ctrl->SearchInTarget(wxsBEnd());
        if ( End == -1 ) return _T("");
        return CutSpaces(Ctrl->GetTextRange(Position,End),SpacesCut);
    }

    wxFFile File(FileName,_T("r"));
    wxString Content;
    if ( !File.IsOpened() ) return _T("");
    if ( !File.ReadAll(&Content) ) return _T("");
    int Position = Content.First(BlockHeader);
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

    Content.Remove(0,Position);
    Position = Content.First(wxsBEnd());
    if ( Position == -1 ) return _T("");
    Content.Remove(Position);
	return CutSpaces(Content,SpacesCut);
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
