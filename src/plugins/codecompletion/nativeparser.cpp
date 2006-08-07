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
* $Revision$
* $Id$
* $HeadURL$
*/

#include <sdk.h>
#include "nativeparser.h"
#include <manager.h>
#include <configmanager.h>
#include <projectmanager.h>
#include <pluginmanager.h>
#include <messagemanager.h>
#include <editormanager.h>
#include <macrosmanager.h>
#include <cbeditor.h>
#include <cbproject.h>
#include <cbexception.h>
#include "classbrowser.h"
#include "parser/parser.h"
#include <compilerfactory.h>
#include <wx/regex.h>
#include <wxFlatNotebook/wxFlatNotebook.h>

#include <wx/wfstream.h>

#include <cctype>

BEGIN_EVENT_TABLE(NativeParser, wxEvtHandler)
//	EVT_MENU(THREAD_START, NativeParser::OnThreadStart)
//	EVT_MENU(THREAD_END, NativeParser::OnThreadEnd)
	EVT_MENU(PARSER_END, NativeParser::OnParserEnd)
END_EVENT_TABLE()

NativeParser::NativeParser()
	: m_Parser(this),
	m_ClassBrowserIsFloating(true)
{
	//ctor
    m_pClassBrowser = 0L;
}

NativeParser::~NativeParser()
{
	RemoveClassBrowser();
	ClearParsers();
}

void NativeParser::CreateClassBrowser()
{
	if (!m_pClassBrowser)
	{
        ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("code_completion"));
        bool isFloating = cfg->ReadBool(_T("/as_floating_window"), false);

        if (!isFloating)
        {
            // make this a tab in projectmanager notebook
            m_pClassBrowser = new ClassBrowser(Manager::Get()->GetProjectManager()->GetNotebook(), this);
            Manager::Get()->GetProjectManager()->GetNotebook()->AddPage(m_pClassBrowser, _("Symbols"));
        }
        else
        {
            m_pClassBrowser = new ClassBrowser(Manager::Get()->GetAppWindow(), this);

            // make this a free floating/docking window
            CodeBlocksDockEvent evt(cbEVT_ADD_DOCK_WINDOW);

            evt.name = _T("SymbolsBrowser");
            evt.title = _("Symbols browser");
            evt.pWindow = m_pClassBrowser;
            evt.dockSide = CodeBlocksDockEvent::dsRight;
            evt.desiredSize.Set(200, 250);
            evt.floatingSize.Set(200, 250);
            evt.minimumSize.Set(150, 150);
            evt.shown = true;
            evt.hideable = true;
            Manager::Get()->GetAppWindow()->ProcessEvent(evt);
        }
        m_ClassBrowserIsFloating = isFloating;
	}
}

void NativeParser::RemoveClassBrowser(bool appShutDown)
{
    if (m_pClassBrowser)
    {
        if (!m_ClassBrowserIsFloating)
        {
            int idx = Manager::Get()->GetProjectManager()->GetNotebook()->GetPageIndex(m_pClassBrowser);
            if (idx != -1)
                Manager::Get()->GetProjectManager()->GetNotebook()->RemovePage(idx);
            m_pClassBrowser->Destroy();
        }
        else if (m_ClassBrowserIsFloating)
        {
            CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
            evt.pWindow = m_pClassBrowser;
            Manager::Get()->GetAppWindow()->ProcessEvent(evt);
            m_pClassBrowser->Destroy();
        }
    }
    m_pClassBrowser = 0L;
}

void NativeParser::UpdateClassBrowser()
{
    if (m_Parser.Done() && !Manager::isappShuttingDown())
    {
        Manager::Get()->GetMessageManager()->DebugLog(_T("Updating class browser..."));
        if (m_pClassBrowser)
        {
            m_pClassBrowser->SetParser(&m_Parser);
            m_pClassBrowser->Update();
        }
        Manager::Get()->GetMessageManager()->DebugLog(_T("Class browser updated."));
    }
}

void NativeParser::RereadParserOptions()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("code_completion"));
	bool needsReparsing = false;

    ParserOptions opts = m_Parser.Options();
    m_Parser.ReadOptions();

    // change class-browser docking settings
    if (m_ClassBrowserIsFloating != cfg->ReadBool(_T("/as_floating_window"), false))
    {
        RemoveClassBrowser();
        CreateClassBrowser();
        // force re-update
        UpdateClassBrowser();
    }

    // reparse if settings changed
    if (opts.followLocalIncludes != m_Parser.Options().followLocalIncludes ||
        opts.followGlobalIncludes != m_Parser.Options().followGlobalIncludes ||
        opts.wantPreprocessor != m_Parser.Options().wantPreprocessor)
    {
        // important options changed... flag for reparsing
        needsReparsing = true;
    }

	if (needsReparsing)
	{
		if (cbMessageBox(_("You changed some class parser options. Do you want to "
						"reparse your projects now, using the new options?"),
						_("Reparse?"),
						wxYES_NO | wxICON_QUESTION) == wxID_YES)
		{
			ClearParsers();
			ProjectsArray* projects = Manager::Get()->GetProjectManager()->GetProjects();
			for (unsigned int i = 0; i < projects->GetCount(); ++i)
			{
				AddParser(projects->Item(i));
			}
			if (m_pClassBrowser)
				m_pClassBrowser->SetParser(&m_Parser);//m_Parsers[proj]);
		}
	}
	if (m_pClassBrowser)
		m_pClassBrowser->Update();
}

void NativeParser::SetClassBrowserProject(cbProject* project)
{
    if (m_pClassBrowser)
		m_pClassBrowser->SetParser(&m_Parser);//m_Parsers[project]);
}

void NativeParser::SetCBViewMode(const BrowserViewMode& mode)
{
    m_Parser.ClassBrowserOptions().showInheritance = mode == bvmInheritance;
	UpdateClassBrowser();
}

void NativeParser::ClearParsers()
{
	if (m_pClassBrowser)
	{
		m_pClassBrowser->SetParser(0L);
		m_pClassBrowser->Update();
	}
//    ProjectsArray* projects = Manager::Get()->GetProjectManager()->GetProjects();
//    for (size_t i = 0; i < projects->GetCount(); ++i)
//    {
//        cbProject* prj = projects->Item(i);
//        RemoveParser(prj, false);
//    }
    m_Parser.Clear();
}

void NativeParser::AddCompilerDirs(Parser* parser, cbProject* project)
{
	if (!parser)
		return;

    // do not clean include dirs: we use a single parser for the whole workspace
//    parser->ClearIncludeDirs();
    wxString base = project->GetBasePath();
    parser->AddIncludeDir(base); // add project's base path

    Compiler* compiler = CompilerFactory::GetCompiler(project->GetCompilerID());

    // get project include dirs
    for (unsigned int i = 0; i < project->GetIncludeDirs().GetCount(); ++i)
    {
    	wxString out = project->GetIncludeDirs()[i];
    	Manager::Get()->GetMacrosManager()->ReplaceMacros(out, true);
        wxFileName dir(out);
        if(NormalizePath(dir,base))
        {
            parser->AddIncludeDir(dir.GetFullPath());
//            Manager::Get()->GetMessageManager()->DebugLog(_T("Parser prj dir: ") + dir.GetFullPath());
        }
        else
            Manager::Get()->GetMessageManager()->DebugLog(_T("Error normalizing path: '%s' from '%s'"),out.c_str(),base.c_str());

    }

	// alloc array for target's compilers and project compiler
	int nCompilers = 1 + project->GetBuildTargetsCount();
	Compiler** Compilers = new Compiler* [nCompilers];
	memset(Compilers, 0, sizeof(Compiler*) * nCompilers);
	nCompilers = 0; // reset , use as insert index in the next for loop

    // get targets include dirs
    for (int i = 0; i < project->GetBuildTargetsCount(); ++i)
    {
        ProjectBuildTarget* target = project->GetBuildTarget(i);
        if (target)
        {
        	// apply target vars
//            target->GetCustomVars().ApplyVarsToEnvironment();
            for (unsigned int ti = 0; ti < target->GetIncludeDirs().GetCount(); ++ti)
            {
                wxString out = target->GetIncludeDirs()[ti];
                Manager::Get()->GetMacrosManager()->ReplaceMacros(out, true);
                wxFileName dir(out);
                wxLogNull ln; // hide the error log about "too many ..", if the relative path is invalid
                if(NormalizePath(dir,base))
                {
                    parser->AddIncludeDir(dir.GetFullPath());
//                    Manager::Get()->GetMessageManager()->DebugLog(_T("Parser tgt dir: ") + dir.GetFullPath());
                }
                else
                    Manager::Get()->GetMessageManager()->DebugLog(_T("Error normalizing path: '%s' from '%s'"),out.c_str(),base.c_str());
            }
            // get the compiler
            wxString CompilerIndex = target->GetCompilerID();
            Compiler* myc = CompilerFactory::GetCompiler(CompilerIndex);
			if (myc)
			{
				Compilers[nCompilers] = myc;
				++nCompilers;
			}
        }
    } // end loop over the targets
	// add the project compiler to the array of compilers
	if(compiler)
	{ // note it might be possible that this compiler is already in the list
		// no need to worry since the compiler list of the parser will filter out duplicate
		// entries in the include dir list
		Compilers[nCompilers++] = compiler;
	}
    // add compiler include dirs
	for (int idxCompiler = 0; idxCompiler < nCompilers; ++idxCompiler)
	{
		const wxArrayString& dirs = (Compilers[idxCompiler])->GetIncludeDirs();
		for (unsigned int i = 0; i < dirs.GetCount(); ++i)
		{
			//Manager::Get()->GetMessageManager()->Log(mltDevDebug, "Adding %s", dirs[i].c_str());
            wxString out = dirs[i];
            Manager::Get()->GetMacrosManager()->ReplaceMacros(out, true);
            wxFileName dir(out);
            wxLogNull ln; // hide the error log about "too many ..", if the relative path is invalid
            if (NormalizePath(dir,base))
            {
                parser->AddIncludeDir(dir.GetFullPath());
//                Manager::Get()->GetMessageManager()->DebugLog(_T("Parser cmp dir: ") + dir.GetFullPath());
			}
            else
                Manager::Get()->GetMessageManager()->DebugLog(_T("Error normalizing path: '%s' from '%s'"),out.c_str(),base.c_str());
		}
		// find out which compiler, if gnu, do the special trick
		// to find it's internal include paths
		wxString CompilerID = (Compilers[idxCompiler])->GetID();
		if(CompilerID == _T("gcc"))
		{ // for starters , only do this for gnu compiler
		    wxLogNull ln; // spare us the error messages; we 'll deal with them on our own
//			Manager::Get()->GetMessageManager()->DebugLog(_T("CompilerID ") + CompilerID);
			//	wxString Command("mingw32-g++ -v -E -x c++ - < nul");
			// specifying "< nul", does not seem to work
			// workaround : create a dummy file (let's hope it does not exist)
			// do the trick only for c++, not needed then for C (since this is a subset of C++)
			wxString DummyFileName = wxFileName::CreateTempFileName(_T("Dummy_z4hsdkl9nf7ba3L9nv41"));
			if(!DummyFileName.IsEmpty())
			{
				// let's construct the command
				wxString Command = ((Compilers[idxCompiler])->GetPrograms()).CPP;
				Command += _T(" -v -E -x c++ ") + DummyFileName;
				// action time  (everything shows up on the error stream
				wxArrayString Output, Errors;
				wxExecute(Command, Output, Errors, wxEXEC_NODISABLE);
				int nCount = Errors.GetCount();
				// the include dir (1 per line) show up between the lines
				// #include <...> search starts here:
				// End of search list
				//   let's hope this does not change too quickly, otherwise we need
				// to adjust our search code (for several versions ...)
				bool bStart = false;
				for(int idxCount = 0; idxCount < nCount; ++idxCount)
				{
					if (!bStart && Errors[idxCount] == _("#include <...> search starts here:"))
					{
						bStart = true;
					}
					else if (bStart && Errors[idxCount] == _("End of search list."))
					{
						bStart = false; // could jump out of for loop if we want
					}
					else if (bStart)
					{
//						 Manager::Get()->GetMessageManager()->DebugLog("include dir " + Errors[idxCount]);
						// get rid of the leading space (more general : any whitespace)in front
						wxRegEx reg(_T("^[ \t]*(.*)"));
						if(reg.Matches(Errors[idxCount]))
						{
							wxString out = reg.GetMatch(Errors[idxCount], 1);
							if(!out.IsEmpty())
							{
								wxFileName dir(out);
								wxLogNull ln; // hide the error log about "too many ..", if the relative path is invalid
								if (NormalizePath(dir,base))
								{
									parser->AddIncludeDir(dir.GetFullPath());
//									Manager::Get()->GetMessageManager()->DebugLog(_T("Parser internal cmp dir: ") + dir.GetFullPath());
								}
                                else
                                    Manager::Get()->GetMessageManager()->DebugLog(_T("Error normalizing path: '%s' from '%s'"),out.c_str(),base.c_str());
							}
						}
					}
				} // end for : idx : idxCount
				// clean up our temp file
				::wxRemoveFile(DummyFileName);
			} // Dummy is open
		} // GNU GCC compiler
	} // end of while loop over the found compilers
	if(!nCompilers)
	{
		Manager::Get()->GetMessageManager()->DebugLog(_T("No compilers found!"));
	}
	delete [] Compilers;
} // end of AddCompilerDirs

void NativeParser::AddParser(cbProject* project, bool useCache)
{
	if (!project)
		return;

	Manager::Get()->GetMessageManager()->DebugLog(_T("Add project %s in parsing queue"), project->GetTitle().c_str());
	Parser* parser = &m_Parser;//new Parser(this);
	AddCompilerDirs(parser, project);

    wxArrayString files;

	// parse header files first
	for (int i = 0; i < project->GetFilesCount(); ++i)
	{
		ProjectFile* pf = project->GetFile(i);
		FileType ft = FileTypeOf(pf->relativeFilename);
		if (ft == ftHeader) // only parse header files
		{
			files.Add(pf->file.GetFullPath());
        }
	}
	// next, parse source files
	for (int i = 0; i < project->GetFilesCount(); ++i)
	{
		ProjectFile* pf = project->GetFile(i);
		FileType ft = FileTypeOf(pf->relativeFilename);
		if (ft == ftSource) // only parse source files
		{
			files.Add(pf->file.GetFullPath());
        }
	}
	if (!files.IsEmpty())
    {
        Manager::Get()->GetMessageManager()->DebugLog(_T("Passing list of files to parse"));
        parser->BatchParse(files);
    }
}

void NativeParser::RemoveParser(cbProject* project, bool useCache)
{
    if (!project)
        return;
	Manager::Get()->GetMessageManager()->DebugLog(_T("Removing project %s from parsed projects"), project->GetTitle().c_str());
    for (int i = 0; i < project->GetFilesCount(); ++i)
    {
        ProjectFile* pf = project->GetFile(i);
        m_Parser.RemoveFile(pf->file.GetFullPath());
    }
    UpdateClassBrowser();
}

void NativeParser::AddFileToParser(cbProject* project, const wxString& filename)
{
	m_Parser.Parse(filename, true);
}

void NativeParser::RemoveFileFromParser(cbProject* project, const wxString& filename)
{
	m_Parser.RemoveFile(filename);
}

void NativeParser::ForceReparseActiveProject()
{
    cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (prj)
    {
        RemoveParser(prj, false);
        AddParser(prj, false);
    }
}

Parser* NativeParser::FindParserFromEditor(cbEditor* editor)
{
    return &m_Parser;
}

Parser* NativeParser::FindParserFromActiveEditor()
{
    return &m_Parser;
}

Parser* NativeParser::FindParserFromActiveProject()
{
    return &m_Parser;
}

Parser* NativeParser::FindParserFromProject(cbProject* project)
{
    return &m_Parser;
}

// UNUSED
bool NativeParser::LoadCachedData(Parser* parser, cbProject* project)
{
    if (!parser || !project)
        return false;

    wxFileName projectCache = project->GetFilename();
    projectCache.SetExt(_T("cbCache"));

    wxLogNull ln;
    wxFile f(projectCache.GetFullPath(), wxFile::read);
    if (!f.IsOpened())
        return false;

    // read cache file
    Manager::Get()->GetMessageManager()->DebugLog(_T("Using parser's existing cache: %s"), projectCache.GetFullPath().c_str());
    bool ret = false;
    try
    {
        wxFileInputStream fs(f);
        wxBufferedInputStream fb(fs);

        ret = parser->ReadFromCache(&fb);
    }
    catch (cbException& ex)
    {
        ex.ShowErrorMessage(true);
        ret = false;
    }
    catch (...)
    {
        // eat it
        wxSafeShowMessage(_("Exception thrown!"),_("ERROR"));
        ret = false;
    }
    if(!ret)
        Manager::Get()->GetMessageManager()->DebugLog(_T("Error reading Cache! Re-parsing from scratch."));
//    else
//        DisplayStatus(parser, project);
    return ret;
}

// UNUSED
bool NativeParser::SaveCachedData(Parser* parser, const wxString& projectFilename)
{
    bool result = false;
    if (!parser)
        return false;

    wxFileName projectCache = projectFilename;
    projectCache.SetExt(_T("cbCache"));

    wxLogNull ln;
    wxFile f(projectCache.GetFullPath(), wxFile::write);
    if (!f.IsOpened())
    {
        Manager::Get()->GetMessageManager()->DebugLog(_T("Failed updating parser's cache: %s"), projectCache.GetFullPath().c_str());
        return false;
    }

    Manager::Get()->GetMessageManager()->DebugLog(_T("Updating parser's cache: %s"), projectCache.GetFullPath().c_str());

    // write cache file
    wxFileOutputStream fs(f);
    {
        wxBufferedOutputStream fb(fs);
        result = parser->WriteToCache(&fb);
    }
    return result;
}

void NativeParser::DisplayStatus(Parser* parser)
{
    if (!parser)
        return;
    long int tim = parser->LastParseTime();
    Manager::Get()->GetMessageManager()->DebugLog(_T("Parsing stage done (%d total parsed files, %d tokens in %d minute(s), %d.%d seconds)."),
                    parser->GetFilesCount(),
                    parser->GetTokens()->realsize(),
                    (tim / 60000),
                    ((tim / 1000) %60),
                    tim % 1000);
}

int NativeParser::MarkItemsByAI(bool reallyUseAI)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
	if (!ed)
		return 0;

	Parser* parser = FindParserFromActiveEditor();
	if (!parser)
		return 0;

	if (!parser->Done())
		Manager::Get()->GetMessageManager()->DebugLog(_T("C++ Parser is still parsing files..."));
	else
	{
		bool sort = false;

		// parse function's arguments
		wxString _namespace;
		wxString _procedure;
		if (FindFunctionNamespace(ed, &_namespace, &_procedure))
		{
			Token* token = parser->FindTokenByName(_procedure, false, tkFunction);
			if (token)
			{
				 if (!token->m_Args.IsEmpty() && !token->m_Args.Matches(_T("()")))
				{
					wxString buffer = token->m_Args;
					buffer.Remove(0, 1); // remove (
					buffer.RemoveLast(); // remove )
					buffer.Replace(_T(","), _T(";")); // replace commas with semi-colons
					buffer << _T(';'); // aid parser ;)
					Manager::Get()->GetMessageManager()->DebugLog(_T("Parsing arguments: \"%s\""), buffer.c_str());
					if (!parser->ParseBuffer(buffer, false))
						Manager::Get()->GetMessageManager()->DebugLog(_T("ERROR parsing block:\n%s"), buffer.c_str());
//					sort = true;
				}
			}
		}
		else
			Manager::Get()->GetMessageManager()->DebugLog(_T("Could not find current function's namespace..."));

		// parse current code block
		int blockStart = FindCurrentBlockStart(ed);
		if (blockStart != -1)
		{
			++blockStart; // skip {
			int blockEnd = ed->GetControl()->GetCurrentPos();
			wxString buffer = ed->GetControl()->GetTextRange(blockStart, blockEnd);
			if (!parser->ParseBuffer(buffer, false))
				Manager::Get()->GetMessageManager()->DebugLog(_T("ERROR parsing block:\n%s"), buffer.c_str());
			sort = true;
		}
		else
			Manager::Get()->GetMessageManager()->DebugLog(_T("Could not find current block start..."));

//		if (sort)
//			parser->SortAllTokens();

		// clear previously marked tokens
		TokensTree* tokens = parser->GetTokens();
		for (unsigned int i = 0; i < tokens->size(); ++i)
		{
			Token* token = tokens->at(i);
			if(!token)
                continue;
			token->m_Bool = !reallyUseAI;
		}

        if (!reallyUseAI)
            return tokens->size();

        // AI will mark (m_Bool == true) every token we should include in list
        return AI(ed, parser);
	}
	return 0;
}

const wxString& NativeParser::GetCodeCompletionItems()
{
    m_CCItems.Clear();

	Parser* parser = FindParserFromActiveEditor();
	if (!parser)
		return m_CCItems;

	int count = MarkItemsByAI();
	if (count)
	{
		TokensTree* tokens = parser->GetTokens();
		for (unsigned int i = 0; i < tokens->size(); ++i)
		{
			Token* token = tokens->at(i);
			if(!token)
                continue;
			if (!token->m_Bool)
				continue; // not marked by AI
			token->m_Bool = false; // reset flag for next run
			if (!m_CCItems.IsEmpty())
				 m_CCItems << _T(";");
			m_CCItems << token->m_Name << token->m_Args;//" " << token->m_Filename << ":" << token->m_Line;
		}
	}

	return m_CCItems;
}

const wxArrayString& NativeParser::GetCallTips()
{
    m_CallTips.Clear();
    Parser* parser = 0;
    int end = 0;
    wxString lineText = _T("");
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    wxCriticalSectionLocker* lock = 0;
    do
    {
        if(!ed)
            break;
        parser = FindParserFromActiveEditor();
        if(!parser)
            break;
        if(!parser->Done())
            break;
        int line = ed->GetControl()->GetCurrentLine();
        lineText = ed->GetControl()->GetLine(line);
        end = lineText.Length();
        int nest = 0;
        while (end > 0)
        {
            --end;
            if (lineText.GetChar(end) == ')')
                --nest;
            else if (lineText.GetChar(end) == '(')
            {
                ++nest;
                if (nest != 0)
                    break;
            }
        }
        if (!end)
            break;
        lineText.Remove(end);
        Manager::Get()->GetMessageManager()->DebugLog(_T("Sending \"%s\" for call-tip"), lineText.c_str());
        // clear previously marked tokens
        TokensTree* tokens = parser->GetTokens();
        lock = new wxCriticalSectionLocker(s_MutexProtection);

        for (unsigned int i = 0; i < tokens->size(); ++i)
        {
            Token* token = tokens->at(i);
            if(token)
                token->m_Bool = false;
        }

        // AI will mark (m_Bool == true) every token we should include in list
        if (!AI(ed, parser, lineText, true, true))
            break;
        for (size_t i = 0; i < tokens->size(); ++i)
        {
            Token* token = tokens->at(i);
            if(!token)
                continue;
            if (token->m_Bool && token->m_Args != _T("()"))
            {
                m_CallTips.Add(token->m_Args);
                token->m_Bool = false; // reset flag for next run
            }
        }
    }while(false);
    if(lock)
        delete lock;
	return m_CallTips;
}

// helper funcs

unsigned int NativeParser::FindCCTokenStart(const wxString& line)
{
	int x = line.Length() - 1;
	int nest = 0;

    bool repeat = true;
    while (repeat)
    {
        repeat = false;
        while (x >= 0 && (isalnum(line.GetChar(x)) || line.GetChar(x) == '_'))
            --x;

        if (x > 0 &&
            (line.GetChar(x) == '>' && line.GetChar(x - 1) == '-') ||
            (line.GetChar(x) == ':' && line.GetChar(x - 1) == ':'))
        {
            x -= 2;
            repeat = true;
        }
        else if (x >= 0 && line.GetChar(x) == '.')
        {
            --x;
            repeat = true;
        }

        if (repeat)
        {
            // check for function/array/cast ()
            if (x >= 0 && (line.GetChar(x) == ')' || line.GetChar(x) == ']'))
            {
                ++nest;
                while (--x >= 0 && nest != 0)
                {
                    switch (line.GetChar(x))
                    {
                        case ']':
                        case ')': ++nest; break;

                        case '[':
                        case '(': --nest; break;
                    }
                }
                if (x > 0 && (isalnum(line.GetChar(x - 1)) || line.GetChar(x - 1) == '_'))
                    --x;
            }
        }
    }
    ++x;

	if (x < 0)
		x = 0;

    while (line.GetChar(x) == ' ' || line.GetChar(x) == '\t')
        ++x;

	//Manager::Get()->GetMessageManager()->DebugLog("Starting at %d \"%s\"", x, line.Mid(x).c_str());
	return x;
}

wxString NativeParser::GetNextCCToken(const wxString& line, unsigned int& startAt)
{
	wxString res;
	int nest = 0;

    if (startAt < line.Length() && line.GetChar(startAt) == '(')
    {
        while (startAt < line.Length() &&
                (line.GetChar(startAt) == '*' ||
                line.GetChar(startAt) == '&' ||
                line.GetChar(startAt) == '('))
        {
            if (line.GetChar(startAt) == '(')
                ++nest;
            ++startAt;
        }
    }

//    Manager::Get()->GetMessageManager()->DebugLog(_T("at %d (%c): res=%s"), startAt, line.GetChar(startAt), res.c_str());
    while (startAt < line.Length() && (isalnum(line.GetChar(startAt)) || line.GetChar(startAt) == '_'))
    {
        res << line.GetChar(startAt);
        ++startAt;
    }

    while (nest > 0 && startAt < line.Length())
    {
        if (line.GetChar(startAt) == ')')
            --nest;
        ++startAt;
    }
    //Manager::Get()->GetMessageManager()->DebugLog("Done nest: at %d (%c): res=%s", startAt, line.GetChar(startAt), res.c_str());

    if (startAt < line.Length() && (line.GetChar(startAt) == '(' || line.GetChar(startAt) == '['))
    {
        ++nest;
        while (startAt < line.Length() - 1 && nest != 0)
        {
            ++startAt;
            switch (line.GetChar(startAt))
            {
                case ']':
                case ')': --nest; break;

                case '[':
                case '(': ++nest; break;
            }
        }
        ++startAt;
    }

    //Manager::Get()->GetMessageManager()->DebugLog("Return at %d (%c): res=%s", startAt, line.GetChar(startAt), res.c_str());
	return res;
}

wxString NativeParser::GetCCToken(wxString& line, ParserTokenType& tokenType)
{
	// line contains a string on the following form:
	// "    char* mychar = SomeNamespace::m_SomeVar.SomeMeth"
	// first we locate the first non-space char starting from the *end*:
	//
	// "    char* mychar = SomeNamespace::m_SomeVar.SomeMeth"
	//                     ^
	// then we remove everything before it.
	// after it, what we do here, is (by this example) return "SomeNamespace"
	// *and* modify line to become:
	// m_SomeVar.SomeMeth
	// so that if we 're called again with the (modified) line,
	// we 'll return "m_SomeVar" and modify line (again) to become:
	// SomeMeth
	// and so on and so forth until we return an empty string...
	// NOTE: if we find () args or [] arrays in our way, we skip them (done in GetNextCCToken)...

	tokenType = pttSearchText;
	if (line.IsEmpty())
		return wxEmptyString;

	unsigned int x = FindCCTokenStart(line);
	wxString res = GetNextCCToken(line, x);
	//Manager::Get()->GetMessageManager()->DebugLog("FindCCTokenStart returned %d \"%s\"", x, line.c_str());
	//Manager::Get()->GetMessageManager()->DebugLog("GetNextCCToken returned %d \"%s\"", x, res.c_str());

	if (x == line.Length())
		line.Clear();
	else
	{
		//Manager::Get()->GetMessageManager()->DebugLog("Left \"%s\"", line.Mid(x).c_str());
		if (line.GetChar(x) == '.')
		{
			tokenType = pttClass;
			line.Remove(0, x + 1);
		}
		else if ((x < line.Length() - 1 && line.GetChar(x) == '-' && line.GetChar(x + 1) == '>') ||
			(x < line.Length() - 1 && line.GetChar(x) == ':' && line.GetChar(x + 1) == ':'))
		{
			if (line.GetChar(x) == ':')
				tokenType = pttNamespace;
			else
				tokenType = pttClass;
			line.Remove(0, x + 2);
		}
		else
			line.Clear();
	}
	return res;
}

int NativeParser::AI(cbEditor* editor, Parser* parser, const wxString& lineText, bool noPartialMatch, bool caseSensitive)
{
	int count = 0;
    int pos = editor->GetControl()->GetCurrentPos();
	m_EditorStartWord = editor->GetControl()->WordStartPosition(pos, true);
	m_EditorEndWord = pos;//editor->GetControl()->WordEndPosition(pos, true);
	int line = editor->GetControl()->GetCurrentLine();

	wxString searchtext;
	//Manager::Get()->GetMessageManager()->DebugLog("********* START **********");

    TokensTree* tree = parser->GetTokens();
    if(!tree)
        return 0;
	Token* parentToken = 0L;
	ParserTokenType tokenType;
	wxString actual;
	int col;
	wxString tabwidth;
	tabwidth.Pad(editor->GetControl()->GetTabWidth(), ' ');
	if (lineText.IsEmpty())
	{
		actual = editor->GetControl()->GetLine(line);
		col = editor->GetControl()->GetColumn(pos);
		// replace tabs in line by equal-count spaces because col is in spaces!
		actual.Replace(_T("\t"), tabwidth);
		actual.Remove(col);
		actual.Trim();
	}
	else
	{
		actual = lineText;
		col = actual.Length() - 1;
	}
	Manager::Get()->GetMessageManager()->DebugLog(_T("Doing AI for '%s':"), actual.c_str());

	// find current function's namespace
	wxString procName;
	wxString scopeName;
	FindFunctionNamespace(editor, &scopeName, &procName);
	Token* scopeToken = 0L;
	if (!scopeName.IsEmpty())
		scopeToken = parser->FindTokenByName(scopeName, false, tkNamespace | tkClass);

	while (1)
	{
//		wxString bef = actual;
		wxString tok = GetCCToken(actual, tokenType);
//		wxString aft = actual;
//		Manager::Get()->GetMessageManager()->DebugLog("before='%s', token='%s', after='%s', namespace=%d", bef.c_str(), tok.c_str(), aft.c_str(), tokenType);
		Manager::Get()->GetMessageManager()->DebugLog(_T("tok='%s'"), tok.c_str());
		if (tok.IsEmpty())
			break;
		if (actual.IsEmpty() && tokenType == pttSearchText)
		{
			searchtext = tok;
			break;
		}

		if (tokenType == pttNamespace)
		{
			//parentToken = parser->FindTokenByName(tok);
			parentToken = parser->FindChildTokenByName(parentToken, tok, true);
			if (!parentToken)
				parentToken = parser->FindChildTokenByName(scopeToken, tok, true); // try local scope
			Manager::Get()->GetMessageManager()->DebugLog(_T("Namespace '%s'"), parentToken ? parentToken->m_Name.c_str() : _("unknown"));
			if (!parentToken)
			{
				Manager::Get()->GetMessageManager()->DebugLog(_T("Bailing out: namespace not found"));
				return 0; // fail deliberately
			}
		}
		else
		{
			Token* token = 0L;
			// try #1 - special case
			if (tok==_T("this")) // <-- special case
			{
                token = scopeToken;
                parentToken = scopeToken;
			}
			// try #2 - function's class member
			if (!token)
			{
                Manager::Get()->GetMessageManager()->DebugLog(_T("Looking for %s under %s"), tok.c_str(), scopeToken ? scopeToken->m_Name.c_str() : _("Unknown"));
				token = parser->FindChildTokenByName(scopeToken, tok, true); // try local scope
            }
            // try #3 - everything else
            if (!token)
            {
                Manager::Get()->GetMessageManager()->DebugLog(_T("Looking for %s under %s"), tok.c_str(), parentToken ? parentToken->m_Name.c_str() : _("Unknown"));
                token = parser->FindChildTokenByName(parentToken, tok, true);
            }
            // NOTE: Now that #2 is checked before #3, class member supersedes similarly named
            //       function argument (if any). But if we put #3 before #2, we 'll be checking
            //       global tokens first, which is not what we want...

			if (token)
                Manager::Get()->GetMessageManager()->DebugLog(_T("Token found %s, type '%s'"), token->m_Name.c_str(), token->m_ActualType.c_str());
			if (token && !token->m_ActualType.IsEmpty())
			{
				Manager::Get()->GetMessageManager()->DebugLog(_T("actual type is %s"), token->m_ActualType.c_str());
                wxString srch = token->m_ActualType;
				int colon = srch.Find(_T("::"));
				if (colon != -1)
				{
                    // type has namespace; break it down and search for it
                    while (!srch.IsEmpty())
                    {
                        parentToken = parser->FindChildTokenByName(parentToken, srch.Left(colon), true);
                        if (!parentToken)
                            break;
                        Manager::Get()->GetMessageManager()->DebugLog(_T("searching under %s"), parentToken->DisplayName().c_str());
                        srch.Remove(0, colon + 2); // plus two to compensate for "::"
                        colon = srch.Find(_T("::"));
                        if (colon == -1)
                            colon = wxSTRING_MAXLEN;
                    }
				}
				else
				{
                    Manager::Get()->GetMessageManager()->DebugLog(_T("Locating %s"), token->m_ActualType.c_str());
                    parentToken = parser->FindTokenByName(token->m_ActualType, true, tkClass | tkNamespace | tkEnum);
                    if (!parentToken) // one more, global, try (might be under a namespace)
                        parentToken = parser->FindTokenByName(token->m_ActualType, false, tkClass | tkNamespace | tkEnum);
                }
			}
			Manager::Get()->GetMessageManager()->DebugLog(_T("Class '%s'"), parentToken ? parentToken->m_Name.c_str() : _("unknown"));

			if (!parentToken)
			{
				Manager::Get()->GetMessageManager()->DebugLog(_T("Bailing out: class not found"));
				return 0; // fail deliberately
			}
		}
		//Manager::Get()->GetMessageManager()->Log(mltDevDebug, "parentToken=0x%8.8x", parentToken);
	}

    if (noPartialMatch && searchtext.IsEmpty())
        return 0;

	bool bCaseSensitive = parser->Options().caseSensitive || caseSensitive;
	if (!bCaseSensitive)
		searchtext.MakeLower();

	Manager::Get()->GetMessageManager()->DebugLog(_T("Scope='%s'"), scopeToken ? scopeToken->m_Name.c_str() : _("Unknown"));
    TokenIdxSet::iterator it,it2;
    Token *token,*ctok;
	if (parentToken)
	{
		Manager::Get()->GetMessageManager()->DebugLog(_T("Final parent: '%s' (count=%d, search=%s)"), parentToken->m_Name.c_str(), parentToken->m_Children.size(), searchtext.c_str());
		for (it = parentToken->m_Children.begin();it != parentToken->m_Children.end();it++)
		{
            token = tree->at(*it);
            if (!token)
                continue;
			if (token->m_IsOperator)
				continue; // ignore operators

			wxString name = token->m_Name;
			if (!bCaseSensitive)
				name.MakeLower();

			bool textCondition;
            if (noPartialMatch)
                textCondition = searchtext.IsEmpty() ? false : name.Matches(searchtext);
            else
            {
                if (lineText.IsEmpty())
                    textCondition = searchtext.IsEmpty() ? true : name.StartsWith(searchtext);
                else
                    textCondition = searchtext.IsEmpty() ? true : name.Matches(searchtext);
            }

            if (token->m_TokenKind == tkEnum)
            {
                // enums children (enumerators), are added by default

                for (it2 = token->m_Children.begin();it2 != token->m_Children.end();it2++)
                {
                    ctok = tree->at(*it2);
                    if (!ctok)
                        continue;
                    ctok->m_Bool = true;
                    ++count;
                }
            }

			// scope-based member access :)
			// display public members
			// private/protected are displayed only if in same scope...
			bool scopeCondition =
						// we can access public members
						token->m_Scope == tsPublic ||
						// we can access private/protected members of current scope only
						scopeToken == parentToken;

			token->m_Bool = textCondition && scopeCondition;// &&
//							token->m_TokenKind != tkConstructor && // ignore constructors
//							token->m_TokenKind != tkDestructor; // and destructors too
			if (token->m_Bool)
				++count;
		}
		// look for inheritance
		count += DoInheritanceAI(parentToken, scopeToken, searchtext, bCaseSensitive);
	}
	else
	{
		//Manager::Get()->GetMessageManager()->Log(mltDevDebug, "no token");
		// just globals and current function's parent class members here...
		Manager::Get()->GetMessageManager()->DebugLog(_T("Procedure of class '%s' (0x%8.8x), name='%s'"), scopeName.c_str(), scopeToken, procName.c_str());

		for (unsigned int i = 0; i < tree->size(); ++i)
		{
			Token* token = tree->at(i);
			if(!token)
                continue;
			if (token->m_IsOperator)
				continue; // ignore operators

			wxString name = token->m_Name;
			if (!bCaseSensitive)
				name.MakeLower();

			bool textCondition;
			if (!noPartialMatch && lineText.IsEmpty())
				textCondition = searchtext.IsEmpty() ? true : name.StartsWith(searchtext);
			else
				textCondition = searchtext.IsEmpty() ? false : name.Matches(searchtext);

			token->m_Bool = textCondition &&
							(!token->GetParentToken() || // globals
							token->m_TokenKind == tkEnumerator || // enumerators
							token->GetParentToken() == scopeToken || // child of procToken
							!lineText.IsEmpty()); // locals too
			if (token->m_Bool)
				++count;
		}
		// look for inheritance
		count += DoInheritanceAI(scopeToken, scopeToken, searchtext, bCaseSensitive);
	}
	return count;
}

int NativeParser::DoInheritanceAI(Token* parentToken, Token* scopeToken, const wxString& searchText, bool caseSensitive)
{
	int count = 0;
	if (!parentToken)
		return 0;
	TokensTree* tree = parentToken->GetTree();
	if(!tree)
        return 0;
	Manager::Get()->GetMessageManager()->DebugLog(_T("Checking inheritance of %s"), parentToken->m_Name.c_str());
	Manager::Get()->GetMessageManager()->DebugLog(_T("- Has %d ancestor(s)"), parentToken->m_Ancestors.size());
	TokenIdxSet::iterator it,x;
	for (it = parentToken->m_Ancestors.begin();it != parentToken->m_Ancestors.end(); it++)
	{
		int ancestor_idx = *it;
		Token* ancestor = tree->at(ancestor_idx);
		if(!ancestor)
            continue;
		Manager::Get()->GetMessageManager()->DebugLog(_T("- Checking ancestor %s"), ancestor->m_Name.c_str());
		int bak = count;
		for (x = ancestor->m_Children.begin(); x != ancestor->m_Children.end(); x++)
		{
			Token* token = tree->at(*x);
			if(!token)
                continue;
			wxString name = token->m_Name;
			if (!caseSensitive)
				name.MakeLower();
			bool textCondition = searchText.IsEmpty() ? true : name.StartsWith(searchText);
			//Manager::Get()->GetMessageManager()->DebugLog("- [%s] %s: %s", searchText.c_str(), name.c_str(), textCondition ? "match" : "no match");
			token->m_Bool = textCondition &&
							(token->m_Scope == tsPublic || (scopeToken && scopeToken->InheritsFrom(ancestor_idx)));
			if (token->m_Bool)
				count++;
		}
		Manager::Get()->GetMessageManager()->DebugLog(_T("- %d matches"), count - bak);
		count += DoInheritanceAI(ancestor, scopeToken, searchText, caseSensitive);
	}
	return count;
}

bool NativeParser::SkipWhitespaceForward(cbEditor* editor, int& pos)
{
    if (!editor)
        return false;
    wxChar ch = editor->GetControl()->GetCharAt(pos);
    int len = editor->GetControl()->GetLength() - 1;
    if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
    {
        while (pos < len && (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n'))
        {
            ++pos;
            ch = editor->GetControl()->GetCharAt(pos);
        }
        return true;
    }
    return false;
}

bool NativeParser::SkipWhitespaceBackward(cbEditor* editor, int& pos)
{
    if (!editor)
        return false;
    wxChar ch = editor->GetControl()->GetCharAt(pos);
    if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
    {
        while (pos > 0 && (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n'))
        {
            --pos;
            ch = editor->GetControl()->GetCharAt(pos);
        }
        return true;
    }
    return false;
}

int NativeParser::FindCurrentBlockStart(cbEditor* editor)
{
	int pos = -1;
	int line = editor->GetControl()->GetCurrentLine();
	while (line >= 0)
	{
		int level = editor->GetControl()->GetFoldLevel(line);
		if ((level & wxSCI_FOLDLEVELHEADERFLAG) &&
			(wxSCI_FOLDLEVELBASE == (level & wxSCI_FOLDLEVELNUMBERMASK)))
		{
			// we 're at block start (maybe '{')
			wxString lineStr = editor->GetControl()->GetLine(line);
			pos = lineStr.Find(_T("{"));
			if (pos != wxNOT_FOUND)
			{
				pos += editor->GetControl()->PositionFromLine(line);
				break;
			}
		}
		--line;
	}
	return pos;
}

bool NativeParser::FindFunctionNamespace(cbEditor* editor, wxString* nameSpace, wxString* procName)
{
	if (!nameSpace && !procName)
		return false;

	int posOf = FindCurrentBlockStart(editor);
	if (posOf != wxNOT_FOUND)
	{
		// look for :: right before procname and procargs
		// if we find a }, we failed (probably no class member this function)...
		bool done = false;
		int nest = 0;
		bool passedArgs = false;
		bool hasNS = false;
		while (posOf > 0)
		{
			--posOf;
			char ch = editor->GetControl()->GetCharAt(posOf);
			switch (ch)
			{
				case ')' : --nest; passedArgs = false; break;
                case '(' :
                    ++nest;
                    passedArgs = nest == 0;
                    if (passedArgs)
                    {
                        --posOf;
                        SkipWhitespaceBackward(editor, posOf);
                    }
                    break;
			}
			if (passedArgs)
			{
                if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n' || ch == ':')
                {
                    int bkp = posOf;
                    SkipWhitespaceBackward(editor, posOf);
                    done = true;
                    hasNS = ch == ':' && editor->GetControl()->GetCharAt(posOf - 1) == ':';
                    posOf = bkp;
                }
            }
			if (done || ch == '}' || ch == ';')
				break;
		}
        Manager::Get()->GetMessageManager()->DebugLog(_T("Pos=%d"), posOf);
		if (done)
		{
			if (procName)
			{
				int procEnd = editor->GetControl()->WordEndPosition(posOf + 1, true);
				*procName = editor->GetControl()->GetTextRange(posOf + 1, procEnd);
			}
			if (nameSpace && hasNS)
			{
                nameSpace->Clear();
				posOf -= 2;
				int scopeStart = editor->GetControl()->WordStartPosition(posOf, true);
				*nameSpace = editor->GetControl()->GetTextRange(scopeStart, posOf + 1);
			}
            Manager::Get()->GetMessageManager()->DebugLog(_T("NS: '%s', PROC: '%s'"), nameSpace ? nameSpace->c_str() : _T(""), procName ? procName->c_str() : _T(""));
			return true;
		}
	}
	else
        Manager::Get()->GetMessageManager()->DebugLog(_T("Can't find block start."));
	return false;
}

// events

void NativeParser::OnThreadStart(wxCommandEvent& event)
{
//	 nothing for now
}

void NativeParser::OnThreadEnd(wxCommandEvent& event)
{
//	 nothing for now
}

void NativeParser::OnParserEnd(wxCommandEvent& event)
{
	Parser* parser = (Parser*)event.GetClientData();
	if (parser)// && parser->Done())
	{
        DisplayStatus(parser);
        UpdateClassBrowser();
	}
}

void NativeParser::OnEditorActivated(EditorBase* editor)
{
    cbEditor* ed = editor && editor->IsBuiltinEditor() ? static_cast<cbEditor*>(editor) : 0;
    if (ed)
    {
        Parser* parser = FindParserFromEditor(ed);
        if (parser && !parser->ClassBrowserOptions().showAllSymbols)
            m_pClassBrowser->Update();
    }
}
