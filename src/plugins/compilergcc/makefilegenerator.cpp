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
#include "makefilegenerator.h" // class's header file
#include <manager.h>
#include <macrosmanager.h>
#include <messagemanager.h>
#include <wx/file.h>
#include <compilerfactory.h>
#include <filefilters.h>

// TODO (mandrav#1#): Fix Makefile for targets using different compilers

// class constructor
MakefileGenerator::MakefileGenerator(CompilerGCC* compiler, cbProject* project, const wxString& makefile, int logIndex)
    : m_Compiler(compiler),
    m_CompilerSet(CompilerFactory::GetCompiler(compiler->GetCurrentCompilerID())),
	m_Project(project),
    m_Makefile(makefile),
    m_LogIndex(logIndex),
    m_GeneratingMakefile(false)
{
}

// class destructor
MakefileGenerator::~MakefileGenerator()
{
}

void MakefileGenerator::UpdateCompiler(ProjectBuildTarget* target)
{
    wxString idx = target
                ? target->GetCompilerID()
                : (m_Project ? m_Project->GetCompilerID() : _T(""));
    if (!idx.IsEmpty())
        m_CompilerSet = CompilerFactory::GetCompiler(idx);
    else
        m_CompilerSet = CompilerFactory::GetDefaultCompiler();
}

wxString MakefileGenerator::ReplaceCompilerMacros(CommandType et,
                                                const wxString& compilerVar,
                                                ProjectBuildTarget* target,
                                                const wxString& file,
                                                const wxString& object,
                                                const wxString& deps)
{
    wxString compilerCmd;
    UpdateCompiler(target);
    compilerCmd = m_CompilerSet->GetCommand(et);

    compilerCmd.Replace(_T("$compiler"), _T("$(") + target->GetTitle() + _T("_") + compilerVar + _T(")"));
    compilerCmd.Replace(_T("$linker"), _T("$(") + target->GetTitle() + _T("_LD)"));
    compilerCmd.Replace(_T("$lib_linker"), _T("$(") + target->GetTitle() + _T("_LIB)"));
    compilerCmd.Replace(_T("$rescomp"), _T("$(") + target->GetTitle() + _T("_RESCOMP)"));
    compilerCmd.Replace(_T("$options"), _T("$(") + target->GetTitle() + _T("_CFLAGS)"));
    compilerCmd.Replace(_T("$link_options"), _T("$(") + target->GetTitle() + _T("_LDFLAGS)"));
    compilerCmd.Replace(_T("$includes"), _T("$(") + target->GetTitle() + _T("_INCS)"));
    compilerCmd.Replace(_T("$libdirs"), _T("$(") + target->GetTitle() + _T("_LIBDIRS)"));
    compilerCmd.Replace(_T("$libs"), _T("$(") + target->GetTitle() + _T("_LIBS)"));
    compilerCmd.Replace(_T("$file"), file);
    compilerCmd.Replace(_T("$objects"), _T("$(") + target->GetTitle() + _T("_OBJS)"));
    compilerCmd.Replace(_T("$dep_object"), deps);
    compilerCmd.Replace(_T("$object"), object);
    compilerCmd.Replace(_T("$link_objects"), _T("$(") + target->GetTitle() + _T("_LINKOBJS)"));
    compilerCmd.Replace(_T("$link_resobjects"), _T("$(") + target->GetTitle() + _T("_RESOURCE)"));
    compilerCmd.Replace(_T("$exe_output"), _T("$(") + target->GetTitle() + _T("_BIN)"));
    if (target->GetTargetType() == ttStaticLib)
        compilerCmd.Replace(_T("$static_output"), _T("$(") + target->GetTitle() + _T("_BIN)"));
    else if (target->GetTargetType() == ttDynamicLib && target->GetCreateStaticLib())
        compilerCmd.Replace(_T("$static_output"), _T("$(") + target->GetTitle() + _T("_STATIC_LIB)"));
    else
        compilerCmd.Replace(_T("-Wl,--out-implib=$static_output"), _T(""));
    if (target->GetTargetType() == ttDynamicLib && target->GetCreateStaticLib())
        compilerCmd.Replace(_T("$def_output"), _T("$(") + target->GetTitle() + _T("_LIB_DEF)"));
    else
        compilerCmd.Replace(_T("-Wl,--output-def=$def_output"), _T(""));
    compilerCmd.Replace(_T("$resource_output"), _T("$(") + target->GetTitle() + _T("_RESOURCE)"));

    int idx = compilerCmd.Find(_T("$res_includes"));
    if (idx != -1)
    {
        wxString incs;
        DoAppendResourceIncludeDirs(incs, 0L, m_CompilerSet->GetSwitches().includeDirs, true);
        DoAppendResourceIncludeDirs(incs, 0L, m_CompilerSet->GetSwitches().includeDirs);
        DoAppendResourceIncludeDirs(incs, target, m_CompilerSet->GetSwitches().includeDirs);
        compilerCmd.Replace(_T("$res_includes"), incs);
    }

    return compilerCmd;
}

wxString MakefileGenerator::CreateSingleFileCompileCmd(CommandType et,
                                                        ProjectBuildTarget* target,
                                                        ProjectFile* pf,
                                                        const wxString& file,
                                                        const wxString& object,
                                                        const wxString& deps)
{
    UpdateCompiler(target);
    return CreateSingleFileCompileCmd(m_CompilerSet->GetCommand(et), target, pf, file, object, deps);
}

wxString MakefileGenerator::CreateSingleFileCompileCmd(const wxString& command,
                                                        ProjectBuildTarget* target,
                                                        ProjectFile* pf,
                                                        const wxString& file,
                                                        const wxString& object,
                                                        const wxString& deps)
{
    // in case of linking command, deps has resource objects
    UpdateCompiler(target);

    wxString compilerStr;
    if (pf)
    {
        if (pf->compilerVar.Matches(_T("CPP")))
            compilerStr = m_CompilerSet->GetPrograms().CPP;
        else if (pf->compilerVar.Matches(_T("CC")))
            compilerStr = m_CompilerSet->GetPrograms().C;
        else if (pf->compilerVar.Matches(_T("WINDRES")))
            compilerStr = m_CompilerSet->GetPrograms().WINDRES;
        else
            return wxEmptyString; // unknown compiler var
    }
    else
    {
    	wxFileName fname(file);
    	if (fname.GetExt().Lower().Matches(_T("c")))
            compilerStr = m_CompilerSet->GetPrograms().C;
        else
            compilerStr = m_CompilerSet->GetPrograms().CPP;
    }

    wxString cflags;
    wxString global_cflags;
	wxString prj_cflags;
	DoAppendCompilerOptions(global_cflags, 0L, true);
	DoAppendCompilerOptions(prj_cflags, 0L);
    DoGetMakefileCFlags(cflags, target);
    if (target)
    {
        cflags.Replace(_T("$(") + target->GetTitle() + _T("_GLOBAL_CFLAGS)"), global_cflags);
        cflags.Replace(_T("$(") + target->GetTitle() + _T("_PROJECT_CFLAGS)"), prj_cflags);
    }
    else if (!target && !pf) // probably single file compilation
        cflags = global_cflags;

    wxString ldflags;
	wxString global_ldflags;
	wxString prj_ldflags;
	DoAppendLinkerOptions(global_ldflags, 0L, true);
	DoAppendLinkerOptions(prj_ldflags, 0L);
	DoGetMakefileLDFlags(ldflags, target);
    if (target)
    {
        ldflags.Replace(_T("$(") + target->GetTitle() + _T("_GLOBAL_LDFLAGS)"), global_ldflags);
        ldflags.Replace(_T("$(") + target->GetTitle() + _T("_PROJECT_LDFLAGS)"), prj_ldflags);
    }
    else if (!target && !pf) // probably single file compilation
        ldflags = global_ldflags;

    wxString ldadd;
	wxString global_ldadd;
	wxString prj_ldadd;
	DoAppendLinkerLibs(global_ldadd, 0L, true);
	DoAppendLinkerLibs(prj_ldadd, 0L);
	DoGetMakefileLibs(ldadd, target);
    if (target)
    {
        ldadd.Replace(_T("$(") + target->GetTitle() + _T("_GLOBAL_LIBS)"), global_ldadd);
        ldadd.Replace(_T("$(") + target->GetTitle() + _T("_PROJECT_LIBS)"), prj_ldadd);
    }
    else if (!target && !pf) // probably single file compilation
        ldadd = global_ldadd;

	wxString global_res_incs;
	wxString prj_res_incs;
	wxString res_incs;
	DoAppendResourceIncludeDirs(global_res_incs, 0L, m_CompilerSet->GetSwitches().includeDirs, true);
	DoAppendResourceIncludeDirs(prj_res_incs, 0L, m_CompilerSet->GetSwitches().includeDirs);
	res_incs << global_res_incs << _T(" ") << prj_res_incs << _T(" ");
	DoAppendResourceIncludeDirs(res_incs, target, m_CompilerSet->GetSwitches().includeDirs);

    wxString incs;
	wxString global_incs;
	wxString prj_incs;
	DoAppendIncludeDirs(global_incs, 0L, m_CompilerSet->GetSwitches().includeDirs, true);
	DoAppendIncludeDirs(prj_incs, 0L, m_CompilerSet->GetSwitches().includeDirs);
	DoGetMakefileIncludes(incs, target);
    if (target)
    {
        incs.Replace(_T("$(") + target->GetTitle() + _T("_GLOBAL_INCS)"), global_incs);
        incs.Replace(_T("$(") + target->GetTitle() + _T("_PROJECT_INCS)"), prj_incs);
    }
    else if (!target && !pf) // probably single file compilation
        incs = global_incs;

    // for PCH to work, the very first include dir *must* be the object output dir
    // *only* if PCH is generated in the object output dir
    if (target &&
        target->GetParentProject()->GetModeForPCH() == pchObjectDir)
    {
        wxArrayString includedDirs; // avoid adding duplicate dirs...
        wxString sep = wxFILE_SEP_PATH;
        // find all PCH in project
        int count = target->GetParentProject()->GetFilesCount();
        for (int i = 0; i < count; ++i)
        {
            ProjectFile* f = target->GetParentProject()->GetFile(i);
            if (FileTypeOf(f->relativeFilename) == ftHeader &&
                f->compile)
            {
                // it is a PCH; add it's object dir to includes
                wxString dir = wxFileName(target->GetObjectOutput() + sep + f->GetObjName()).GetPath();
                if (includedDirs.Index(dir) == wxNOT_FOUND)
                {
                    includedDirs.Add(dir);
                    incs = m_CompilerSet->GetSwitches().includeDirs +
                            dir +
                            _T(" ") +
                            incs;
                }
            }
        }
    }

    wxString libs;
	wxString global_libs;
	wxString prj_libs;
	DoAppendLibDirs(global_libs, 0L, m_CompilerSet->GetSwitches().libDirs, true);
	DoAppendLibDirs(prj_libs, 0L, m_CompilerSet->GetSwitches().libDirs);
	DoGetMakefileLibDirs(libs, target);
    if (target)
    {
        libs.Replace(_T("$(") + target->GetTitle() + _T("_GLOBAL_LIBDIRS)"), global_libs);
        libs.Replace(_T("$(") + target->GetTitle() + _T("_PROJECT_LIBDIRS)"), prj_libs);
    }
    else if (!target && !pf) // probably single file compilation
        libs = global_libs;

    wxString output;
    if (target)
        output = UnixFilename(target->GetOutputFilename());
    else
    {
        wxString object_unquoted(object);
        if (!object_unquoted.IsEmpty() && object_unquoted.GetChar(0) == '"')
            object_unquoted.Replace(_T("\""), _T(""));
        wxFileName fname(object_unquoted);
        fname.SetExt(FileFilters::EXECUTABLE_EXT);
        output = fname.GetFullPath();
    }
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(output);
    ConvertToMakefileFriendly(output);
    QuoteStringIfNeeded(output);

    wxString linkobjs;

    wxString compilerCmd = command;
    compilerCmd.Replace(_T("$compiler"), compilerStr);
    compilerCmd.Replace(_T("$linker"), m_CompilerSet->GetPrograms().LD);
    compilerCmd.Replace(_T("$lib_linker"), m_CompilerSet->GetPrograms().LIB);
    compilerCmd.Replace(_T("$rescomp"), m_CompilerSet->GetPrograms().WINDRES);
    compilerCmd.Replace(_T("$options"), cflags);
    compilerCmd.Replace(_T("$link_options"), ldflags);
    compilerCmd.Replace(_T("$includes"), incs);
    compilerCmd.Replace(_T("$res_includes"), res_incs);
    compilerCmd.Replace(_T("$libdirs"), libs);
    compilerCmd.Replace(_T("$libs"), ldadd);
    compilerCmd.Replace(_T("$file"), file);
    compilerCmd.Replace(_T("$dep_object"), deps);
    compilerCmd.Replace(_T("$object"), object);
    compilerCmd.Replace(_T("$exe_output"), output);
    compilerCmd.Replace(_T("$resource_output"), object);
    compilerCmd.Replace(_T("$link_resobjects"), deps);
    compilerCmd.Replace(_T("$link_objects"), object);
    // the following were added to support the QUICK HACK
    // at directcommands.cpp:576
    compilerCmd.Replace(_T("$+link_objects"), object);
    compilerCmd.Replace(_T("$-link_objects"), object);
    compilerCmd.Replace(_T("$-+link_objects"), object);
    compilerCmd.Replace(_T("$+-link_objects"), object);

    if (target && (target->GetTargetType() == ttStaticLib || target->GetTargetType() == ttDynamicLib))
    {
        wxFileName fname(target->GetOutputFilename());
        if (!fname.GetName().StartsWith(m_CompilerSet->GetSwitches().libPrefix))
            fname.SetName(m_CompilerSet->GetSwitches().libPrefix + fname.GetName());
        fname.SetExt(m_CompilerSet->GetSwitches().libExtension);
        wxString out = UnixFilename(fname.GetFullPath());
        ConvertToMakefileFriendly(out);
        QuoteStringIfNeeded(out);
        if (target->GetTargetType() == ttStaticLib || target->GetCreateStaticLib())
            compilerCmd.Replace(_T("$static_output"), out);
        else
        {
            compilerCmd.Replace(_T("-Wl,--out-implib=$static_output"), _T("")); // special gcc case
            compilerCmd.Replace(_T("$static_output"), _T(""));
        }
        if (target->GetCreateDefFile())
        {
            fname.SetExt(_T("def"));
            out = UnixFilename(fname.GetFullPath());
            ConvertToMakefileFriendly(out);
            QuoteStringIfNeeded(out);
            compilerCmd.Replace(_T("$def_output"), out);
        }
        else
        {
            compilerCmd.Replace(_T("-Wl,--output-def=$def_output"), _T("")); // special gcc case
            compilerCmd.Replace(_T("$def_output"), _T(""));
        }
    }
#ifndef __WXMSW__
    // run the command in a shell, so backtick'd expressions can be evaluated
//    compilerCmd = m_Compiler->GetConsoleShell() + _T(" '") + compilerCmd + _T("'");
#endif
    return compilerCmd;
}

void MakefileGenerator::DoAppendCompilerOptions(wxString& cmd, ProjectBuildTarget* target, bool useGlobalOptions)
{
    wxArrayString opts;
    if (!m_CompilerSet)
        return;
	if (useGlobalOptions)
		opts = m_CompilerSet->GetCompilerOptions();
	else
	{
		if (target)
			opts = target->GetCompilerOptions();
		else
			opts = m_Project ? m_Project->GetCompilerOptions() : m_CompilerSet->GetCompilerOptions();
	}

    for (unsigned int x = 0; x < opts.GetCount(); ++x)
    {
    	if (!m_GeneratingMakefile)
            Manager::Get()->GetMacrosManager()->ReplaceEnvVars(opts[x]);
        cmd << _T(" ") << opts[x];
    }
}

void MakefileGenerator::DoAppendLinkerOptions(wxString& cmd, ProjectBuildTarget* target, bool useGlobalOptions)
{
    CompileOptionsBase* obj;
    if (!m_CompilerSet)
        return;
	if (useGlobalOptions)
        obj = m_CompilerSet;
	else
        obj = target ? (CompileOptionsBase*)target : (m_Project ? (CompileOptionsBase*)m_Project : m_CompilerSet);

    wxArrayString opts = obj->GetLinkerOptions();
    for (unsigned int x = 0; x < opts.GetCount(); ++x)
    {
    	if (!m_GeneratingMakefile)
            Manager::Get()->GetMacrosManager()->ReplaceEnvVars(opts[x]);
        cmd << _T(" ") << opts[x];
    }
}

void MakefileGenerator::DoAppendLinkerLibs(wxString& cmd, ProjectBuildTarget* target, bool useGlobalOptions)
{
    if (!m_CompilerSet)
        return;
    CompileOptionsBase* obj;
	if (useGlobalOptions)
        obj = m_CompilerSet;
	else
	{
        obj = target ? (CompileOptionsBase*)target : (m_Project ? (CompileOptionsBase*)m_Project : m_CompilerSet);
        wxString index = target ? target->GetCompilerID() : (m_Project ? m_Project->GetCompilerID() : CompilerFactory::GetDefaultCompilerID());
        m_CompilerSet = CompilerFactory::GetCompiler(index);
    }

    wxArrayString libs = obj->GetLinkLibs();
    for (unsigned int x = 0; x < libs.GetCount(); ++x)
    {
        if (libs[x].IsEmpty())
            continue;

        // construct linker option for each lib, based on compiler's settings
        wxString libPrefix = m_CompilerSet->GetSwitches().libPrefix;
        wxString libExt = m_CompilerSet->GetSwitches().libExtension;
        wxString lib = libs[x];
        QuoteStringIfNeeded(lib);
        // run replacements on libs only if no slashes in name (which means it's a relative or absolute path)
        if (lib.Find('/') == -1 && lib.Find('\\') == -1)
        {
            // 'lib' prefix
            bool hadLibPrefix = false;
            if (!m_CompilerSet->GetSwitches().linkerNeedsLibPrefix &&
                !libPrefix.IsEmpty() &&
                lib.StartsWith(libPrefix))
            {
                lib.Remove(0, libPrefix.Length());
                hadLibPrefix = true;
            }
            // extension
            if (!m_CompilerSet->GetSwitches().linkerNeedsLibExtension &&
                lib.Length() > libExt.Length() &&
                lib.Right(libExt.Length() + 1) == _T(".") + libExt)
            {
                // remove the extension only if we had a lib prefix
                if (hadLibPrefix)
                    lib.RemoveLast(libExt.Length() + 1);
            }
            else if (m_CompilerSet->GetSwitches().linkerNeedsLibExtension &&
                    !libExt.IsEmpty())
            {
                if (lib.Length() <= libExt.Length() ||
                    lib.Right(libExt.Length() + 1) != _T(".") + libExt)
                {
                    lib << _T(".") << libExt;
                }
            }
            lib = m_CompilerSet->GetSwitches().linkLibs + lib;
        }
    	if (!m_GeneratingMakefile)
            Manager::Get()->GetMacrosManager()->ReplaceEnvVars(lib);
        cmd << _T(" ") << lib;
    }
}

void MakefileGenerator::DoAppendIncludeDirs(wxString& cmd, ProjectBuildTarget* target, const wxString& prefix, bool useGlobalOptions)
{
    wxArrayString opts;
    if (!m_CompilerSet)
        return;
	if (useGlobalOptions)
		opts = m_CompilerSet->GetIncludeDirs();
	else
	{
		if (target)
			opts = target->GetIncludeDirs();
		else
			opts = m_Project ? m_Project->GetIncludeDirs() : m_CompilerSet->GetIncludeDirs();
	}

    for (unsigned int x = 0; x < opts.GetCount(); ++x)
    {
        if (opts[x].IsEmpty())
            continue;
        wxString out = UnixFilename(opts[x]);
    	if (!m_GeneratingMakefile)
            Manager::Get()->GetMacrosManager()->ReplaceEnvVars(out);
        ConvertToMakefileFriendly(out);
        QuoteStringIfNeeded(out);
        cmd << _T(" ") << prefix << out;
    }
}

void MakefileGenerator::DoAppendResourceIncludeDirs(wxString& cmd, ProjectBuildTarget* target, const wxString& prefix, bool useGlobalOptions)
{
    wxArrayString opts;
    if (!m_CompilerSet)
        return;
	if (useGlobalOptions)
		opts = m_CompilerSet->GetResourceIncludeDirs();
	else
	{
		if (target)
			opts = target->GetResourceIncludeDirs();
		else
			opts = m_Project ? m_Project->GetResourceIncludeDirs() : m_CompilerSet->GetResourceIncludeDirs();
	}

    for (unsigned int x = 0; x < opts.GetCount(); ++x)
    {
        if (opts[x].IsEmpty())
            continue;
        wxString out = UnixFilename(opts[x]);
    	if (!m_GeneratingMakefile)
            Manager::Get()->GetMacrosManager()->ReplaceEnvVars(out);
        ConvertToMakefileFriendly(out);
        QuoteStringIfNeeded(out);
        cmd << _T(" ") << prefix << out;
    }
}

void MakefileGenerator::DoAppendLibDirs(wxString& cmd, ProjectBuildTarget* target, const wxString& prefix, bool useGlobalOptions)
{
    wxArrayString opts;
    if (!m_CompilerSet)
        return;
	if (useGlobalOptions)
		opts = m_CompilerSet->GetLibDirs();
	else
	{
		if (target)
			opts = target->GetLibDirs();
		else
			opts = m_Project ? m_Project->GetLibDirs() : m_CompilerSet->GetLibDirs();
	}

    for (unsigned int x = 0; x < opts.GetCount(); ++x)
    {
        if (opts[x].IsEmpty())
            continue;
        wxString out = UnixFilename(opts[x]);
    	if (!m_GeneratingMakefile)
            Manager::Get()->GetMacrosManager()->ReplaceEnvVars(out);
        ConvertToMakefileFriendly(out);
        QuoteStringIfNeeded(out);
        cmd << _T(" ") << prefix << out;
    }
}

void MakefileGenerator::DoGetMakefileIncludes(wxString& buffer, ProjectBuildTarget* target)
{
    UpdateCompiler(target);
    if (!m_CompilerSet || !target)
        return;
    wxString prefix = m_CompilerSet->GetSwitches().includeDirs;
    OptionsRelation relation = target->GetOptionRelation(ortIncludeDirs);
    switch (relation)
    {
        case orUseParentOptionsOnly:
            buffer << _T(" $(") + target->GetTitle() + _T("_PROJECT_INCS)");
            break;
        case orUseTargetOptionsOnly:
            DoAppendIncludeDirs(buffer, target, prefix);
            break;
        case orPrependToParentOptions:
            DoAppendIncludeDirs(buffer, target, prefix);
            buffer << _T(" $(") + target->GetTitle() + _T("_PROJECT_INCS)");
            break;
        case orAppendToParentOptions:
            buffer << _T(" $(") + target->GetTitle() + _T("_PROJECT_INCS)");
            DoAppendIncludeDirs(buffer, target, prefix);
            break;
    }
	buffer << _T(" $(") + target->GetTitle() + _T("_GLOBAL_INCS)");
}

void MakefileGenerator::DoGetMakefileLibs(wxString& buffer, ProjectBuildTarget* target)
{
	UpdateCompiler(target);
    if (!m_CompilerSet || !target)
        return;
    OptionsRelation relation = target->GetOptionRelation(ortLinkerOptions);
    switch (relation)
    {
        case orUseParentOptionsOnly:
			buffer << _T(" $(") + target->GetTitle() + _T("_PROJECT_LIBS)");
            break;
        case orUseTargetOptionsOnly:
            DoAppendLinkerLibs(buffer, target);
            break;
        case orPrependToParentOptions:
            DoAppendLinkerLibs(buffer, target);
			buffer << _T(" $(") + target->GetTitle() + _T("_PROJECT_LIBS)");
            break;
        case orAppendToParentOptions:
			buffer << _T(" $(") + target->GetTitle() + _T("_PROJECT_LIBS)");
            DoAppendLinkerLibs(buffer, target);
            break;
    }
	buffer << _T(" $(") + target->GetTitle() + _T("_GLOBAL_LIBS)");
}

void MakefileGenerator::DoGetMakefileLibDirs(wxString& buffer, ProjectBuildTarget* target)
{
    UpdateCompiler(target);
    if (!m_CompilerSet || !target)
        return;
    wxString prefix = m_CompilerSet->GetSwitches().libDirs;
    OptionsRelation relation = target->GetOptionRelation(ortLibDirs);
    switch (relation)
    {
        case orUseParentOptionsOnly:
			buffer << _T(" $(") + target->GetTitle() + _T("_PROJECT_LIBDIRS)");
            break;
        case orUseTargetOptionsOnly:
            DoAppendLibDirs(buffer, target, prefix);
            break;
        case orPrependToParentOptions:
            DoAppendLibDirs(buffer, target, prefix);
			buffer << _T(" $(") + target->GetTitle() + _T("_PROJECT_LIBDIRS)");
            break;
        case orAppendToParentOptions:
			buffer << _T(" $(") + target->GetTitle() + _T("_PROJECT_LIBDIRS)");
            DoAppendLibDirs(buffer, target, prefix);
            break;
    }
	buffer << _T(" $(") + target->GetTitle() + _T("_GLOBAL_LIBDIRS)");
}

void MakefileGenerator::DoGetMakefileCFlags(wxString& buffer, ProjectBuildTarget* target)
{
	UpdateCompiler();
    if (!m_CompilerSet || !target)
        return;
    OptionsRelation relation = target->GetOptionRelation(ortCompilerOptions);
    switch (relation)
    {
        case orUseParentOptionsOnly:
			buffer << _T(" $(") + target->GetTitle() + _T("_PROJECT_CFLAGS)");
            break;
        case orUseTargetOptionsOnly:
            DoAppendCompilerOptions(buffer, target);
            break;
        case orPrependToParentOptions:
            DoAppendCompilerOptions(buffer, target);
			buffer << _T(" $(") + target->GetTitle() + _T("_PROJECT_CFLAGS)");
            break;
        case orAppendToParentOptions:
			buffer << _T(" $(") + target->GetTitle() + _T("_PROJECT_CFLAGS)");
            DoAppendCompilerOptions(buffer, target);
            break;
    }
	buffer << _T(" $(") + target->GetTitle() + _T("_GLOBAL_CFLAGS)");
}

void MakefileGenerator::DoGetMakefileLDFlags(wxString& buffer, ProjectBuildTarget* target)
{
	UpdateCompiler(target);
    if (!m_CompilerSet || !target)
        return;
	OptionsRelation relation = target->GetOptionRelation(ortLinkerOptions);
    switch (relation)
    {
        case orUseParentOptionsOnly:
			buffer << _T(" $(") + target->GetTitle() + _T("_PROJECT_LDFLAGS)");
            break;
        case orUseTargetOptionsOnly:
            DoAppendLinkerOptions(buffer, target);
            break;
        case orPrependToParentOptions:
            DoAppendLinkerOptions(buffer, target);
			buffer << _T(" $(") + target->GetTitle() + _T("_PROJECT_LDFLAGS)");
            break;
        case orAppendToParentOptions:
			buffer << _T(" $(") + target->GetTitle() + _T("_PROJECT_LDFLAGS)");
            DoAppendLinkerOptions(buffer, target);
            break;
    }
	buffer << _T(" $(") + target->GetTitle() + _T("_GLOBAL_LDFLAGS)");
}

void MakefileGenerator::DoAddVarsSet(wxString& buffer, CustomVars& vars)
{
//    const VarsArray& v = vars.GetVars();
//    for (unsigned int i = 0; i < v.GetCount(); ++i)
//    {
//    	wxString out = v[i].value;
//        Manager::Get()->GetMacrosManager()->ReplaceEnvVars(out);
//        ConvertToMakefileFriendly(out);
//        QuoteStringIfNeeded(out);
//        buffer << v[i].name << _T("=") << out << _T('\n');
//    }
}

void MakefileGenerator::DoAddMakefileVars(wxString& buffer)
{
//    buffer << _T("### Variables used in this Makefile") << _T('\n');
//
//    // compiler global vars
//    DoAddVarsSet(buffer, CompilerFactory::GetCompiler(m_Project->GetCompilerID())->GetCustomVars());
//    // project vars
//    DoAddVarsSet(buffer, m_Project->GetCustomVars());
//    int targetsCount = m_Project->GetBuildTargetsCount();
//    for (int x = 0; x < targetsCount; ++x)
//    {
//        ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
//        if (!IsTargetValid(target))
//            continue;
//        Compiler* compilerSet = CompilerFactory::GetCompiler(target->GetCompilerID());
//
//        // target vars
//        DoAddVarsSet(buffer, compilerSet->GetCustomVars());
//
//        // compiler vars
//        // defined last so even if the user sets custom vars
//        // by these names, ours will have precedence...
//        buffer << target->GetTitle() << _T("_CC=") << compilerSet->GetPrograms().C << _T('\n');
//        buffer << target->GetTitle() << _T("_CPP=") << compilerSet->GetPrograms().CPP << _T('\n');
//        buffer << target->GetTitle() << _T("_LD=") << compilerSet->GetPrograms().LD << _T('\n');
//        buffer << target->GetTitle() << _T("_LIB=") << compilerSet->GetPrograms().LIB << _T('\n');
//        buffer << target->GetTitle() << _T("_RESCOMP=") << compilerSet->GetPrograms().WINDRES << _T('\n');
//    }
//
//    buffer << _T('\n');
}

void MakefileGenerator::DoAddMakefileResources(wxString& buffer)
{
    if(platform::windows)
    {
        buffer << _T("### Resources used in this Makefile") << _T('\n');

        int targetsCount = m_Project->GetBuildTargetsCount();
        for (int x = 0; x < targetsCount; ++x)
        {
            ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
            if (!target)
                break;

            // create target's options only if it has at least one linkable file
            if (!IsTargetValid(target))
                continue;

            buffer << target->GetTitle() << _T("_RESOURCE=");

            if (target->GetTargetType() == ttConsoleOnly)
            {
                buffer << _T('\n');
                break;
            }

            wxFileName resFile;
            resFile.SetName(target->GetTitle() + _T("_private"));
            resFile.SetExt(FileFilters::RESOURCEBIN_EXT);
            resFile.MakeRelativeTo(m_Project->GetBasePath());

            // now create the resource file...
            bool hasResources = false;
            wxString resBuf;
            resBuf << _T("#include <windows.h>") << _T('\n');
            int filesCount = (int)m_Files.GetCount();
            for (int i = 0; i < filesCount; ++i)
            {
                wxFileName file;

                ProjectFile* pf = m_Files[i];
                // if the file is allowed to compile *and* belongs in this target
                if (pf->link && pf->buildTargets.Index(target->GetTitle()) >= 0)
                {
                    file.Assign(pf->relativeFilename);
                    if (file.GetExt().Lower().Matches(_T("rc")))
                    {
                        resBuf << _T("#include \"") << file.GetFullPath() << _T("\"") << _T('\n');
                        hasResources = true;
                    }
                }
            }

            if (hasResources)
            {
                wxString out = UnixFilename(resFile.GetFullPath());
                ConvertToMakefileFriendly(out);
                QuoteStringIfNeeded(out);
                buffer << out << _T('\n');
                // write private resource file to disk
                resFile.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, m_Project->GetBasePath());
                resFile.SetExt(FileFilters::RESOURCE_EXT);
                wxFile file(resFile.GetFullPath(), wxFile::write);
                cbWrite(file,resBuf);
            }
            else
                buffer << _T('\n');
        }
        buffer << _T('\n');
    }
}

void MakefileGenerator::DoAddMakefileCreateDirs(wxString& buffer, ProjectBuildTarget* target, bool obj, bool dep, bool bin)
{
    if (!target)
        return;

    // create target's options only if it has at least one linkable file
    if (!IsTargetValid(target))
        return;

    wxArrayString addedDirs; // avoid creating multiple commands for the same dir
    int filesCount = (int)m_Files.GetCount();

    if (obj)
    {
        // object output dirs
        addedDirs.Clear();
        for (int i = 0; i < filesCount; ++i)
        {
            ProjectFile* pf = m_Files[i];
            // if the file belongs in this target
            if (pf->buildTargets.Index(target->GetTitle()) >= 0)
            {
                wxString sep = wxFileName::GetPathSeparator();
                wxString o_out = target->GetObjectOutput();
                wxString object_file = (!o_out.IsEmpty() ? o_out : _T(".")) +
                                       sep +
                                       pf->GetObjName();
                wxFileName o_file(object_file);
                wxFileName o_dir(o_file.GetPath(wxPATH_GET_SEPARATOR));
                RecursiveCreateDir(buffer, o_dir.GetDirs(), addedDirs);
            }
        }
    }

    if (dep)
    {
        // deps output dirs
        addedDirs.Clear();
        for (int i = 0; i < filesCount; ++i)
        {
            ProjectFile* pf = m_Files[i];
            // if the file belongs in this target
            if (pf->buildTargets.Index(target->GetTitle()) >= 0)
            {
                wxString sep = wxFileName::GetPathSeparator();
                wxString o_out = target->GetDepsOutput();
                wxString object_file = (!o_out.IsEmpty() ? o_out : _T(".")) +
                                       sep +
                                       pf->GetObjName();
                wxFileName o_file(object_file);
                wxFileName o_dir(o_file.GetPath(wxPATH_GET_SEPARATOR));
                RecursiveCreateDir(buffer, o_dir.GetDirs(), addedDirs);
            }
        }
    }

    if (bin)
    {
        // add output dir also
        addedDirs.Clear();
        wxFileName fname(target->GetOutputFilename());
        if (fname.IsAbsolute())
            fname.MakeRelativeTo(m_Project->GetBasePath());
        wxString out = UnixFilename(fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR));
        if (!out.IsEmpty())
        {
            ConvertToMakefileFriendly(out);
            QuoteStringIfNeeded(out);
            wxFileName o_file(out);
            wxFileName o_dir(o_file.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR));
            RecursiveCreateDir(buffer, o_dir.GetDirs(), addedDirs);
        }
    }
}

void MakefileGenerator::RecursiveCreateDir(wxString& buffer, const wxArrayString& subdirs, wxArrayString& guardList)
{
    wxString currdir;
    for (size_t i = 0; i < subdirs.GetCount(); ++i)
    {
    	wxString sub = subdirs[i];

    	if (platform::windows && m_GeneratingMakefile)
    	{
            // Can't do it differently here...
            // We *must* replace the env vars if we 're running under windows
            // because the windows command shell is *really* dumb...
            // If we use an env. var in output and this env. var contains
            // path separators, it breaks under windows...
            Manager::Get()->GetMacrosManager()->ReplaceEnvVars(sub);
    	}

        currdir << sub;
        if (guardList.Index(currdir) != wxNOT_FOUND)
        {
            currdir << wxFileName::GetPathSeparator();
            continue;
        }
        guardList.Add(currdir);

        if(platform::windows)
        {
            buffer << _T("\t-@if not exist \"") << currdir << wxFileName::GetPathSeparator() << _T(".\" mkdir \"") << currdir << _T("\"\n");
        }
        else
        {
            wxString out = currdir;
            ConvertToMakefileFriendly(out);
            QuoteStringIfNeeded(out);
            buffer << _T("\t-@if ! test -d ") << out << _T("; then mkdir ") << out << _T("; fi\n");
        }

        currdir << wxFileName::GetPathSeparator();
    }
}

void MakefileGenerator::DoAddMakefileObjs(wxString& buffer)
{
    buffer << _T("### Objects used in this Makefile") << _T('\n');

    int targetsCount = m_Project->GetBuildTargetsCount();
    for (int x = 0; x < targetsCount; ++x)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
        if (!target)
            break;

		// create target's options only if it has at least one linkable file
		if (!IsTargetValid(target))
			continue;
        UpdateCompiler(target);

        wxString deps;
        wxString tmp;
		wxString tmpLink;
        int filesCount = (int)m_Files.GetCount();
        for (int i = 0; i < filesCount; ++i)
        {
            wxFileName file;

            ProjectFile* pf = m_Files[i];
            // if the file belongs in this target
            if (pf->buildTargets.Index(target->GetTitle()) >= 0)
            {
                if (FileTypeOf(pf->relativeFilename) == ftResource)
                    continue; // resource file are treated differently

				wxString fname = UnixFilename(pf->GetObjName());
//				ConvertToMakefileFriendly(fname);

				wxFileName deps_tmp = fname;
				deps_tmp.SetExt(_T("d"));
				wxString depsS;
                depsS << target->GetDepsOutput() << _T("/") << deps_tmp.GetFullPath();

				wxFileName objs_tmp = fname;
				wxString objsS;
                objsS << target->GetObjectOutput() << _T("/") << fname;

                objsS = UnixFilename(objsS);
                ConvertToMakefileFriendly(objsS);
                QuoteStringIfNeeded(objsS);
                depsS = UnixFilename(depsS);
                ConvertToMakefileFriendly(depsS);
                QuoteStringIfNeeded(depsS);

				if (pf->compile)
				{
                    deps << depsS << _T(" ");
                    tmp << objsS << _T(" "); // if the file is allowed to compile
                }
				if (pf->link)
					tmpLink << objsS << _T(" "); // if the file is allowed to link
            }
        }
        buffer << target->GetTitle() << _T("_OBJS=") << tmp << _T('\n');
        buffer << target->GetTitle() << _T("_LINKOBJS=");
		if (tmp.Matches(tmpLink))
			buffer << _T("$(") << target->GetTitle() << _T("_OBJS)");
		else
			buffer << tmpLink; // only write *_LINKOBJS if different from *_OBJS
//		if (target->GetTargetType() != ttConsoleOnly)
//			buffer << _T(" $(") << target->GetTitle() << _T("_RESOURCE)";
        buffer << _T('\n');
        if (m_CompilerSet->GetSwitches().needDependencies)
        {
            buffer << target->GetTitle() << _T("_DEPS=") << deps << _T('\n');
//            buffer << target->GetTitle() << _T("_DEPS=$(") << target->GetTitle() << _T("_OBJS:.";
//            buffer << m_CompilerSet->GetSwitches().objectExtension;
//            buffer << _T("=.d)") << _T('\n');
        }
    }
    buffer << _T('\n');
}

void MakefileGenerator::DoAddMakefileOptions(wxString& buffer)
{
    buffer << _T("### Compiler/linker options") << _T('\n');
    for (int i = 0; i < m_Project->GetBuildTargetsCount(); ++i)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(i);
        UpdateCompiler(target);
        if (!m_CompilerSet)
            continue;

        buffer << target->GetTitle() + _T("_GLOBAL_CFLAGS=");
        DoAppendCompilerOptions(buffer, 0L, true);
        buffer << _T('\n');

        buffer << target->GetTitle() + _T("_PROJECT_CFLAGS=");
        DoAppendCompilerOptions(buffer, 0L);
        buffer << _T('\n');

        buffer << target->GetTitle() + _T("_GLOBAL_LDFLAGS=");
        DoAppendLinkerOptions(buffer, 0L, true);
        buffer << _T('\n');

        buffer << target->GetTitle() + _T("_PROJECT_LDFLAGS=");
        DoAppendLinkerOptions(buffer, 0L);
        buffer << _T('\n');

        buffer << target->GetTitle() + _T("_GLOBAL_INCS=");
        DoAppendIncludeDirs(buffer, 0L, m_CompilerSet->GetSwitches().includeDirs, true);
        buffer << _T('\n');

        buffer << target->GetTitle() + _T("_PROJECT_INCS=");
        DoAppendIncludeDirs(buffer, 0L, m_CompilerSet->GetSwitches().includeDirs);
        buffer << _T('\n');

        buffer << target->GetTitle() + _T("_GLOBAL_LIBDIRS=");
        DoAppendLibDirs(buffer, 0L, m_CompilerSet->GetSwitches().libDirs, true);
        buffer << _T('\n');

        buffer << target->GetTitle() + _T("_PROJECT_LIBDIRS=");
        DoAppendLibDirs(buffer, 0L, m_CompilerSet->GetSwitches().libDirs);
        buffer << _T('\n');

        buffer << target->GetTitle() + _T("_GLOBAL_LIBS=");
        DoAppendLinkerLibs(buffer, 0L, true);
        buffer << _T('\n');

        buffer << target->GetTitle() + _T("_PROJECT_LIBS=");
        DoAppendLinkerLibs(buffer, 0L);
        buffer << _T('\n');
    }
	buffer << _T('\n');
}

void MakefileGenerator::DoAddMakefileIncludes(wxString& buffer)
{
    buffer << _T("### Targets include directories") << _T('\n');


    int targetsCount = m_Project->GetBuildTargetsCount();
    for (int x = 0; x < targetsCount; ++x)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
        if (!target)
            break;

		// create target's options only if it has at least one linkable file
		if (!IsTargetValid(target))
			continue;

        wxString tmp;
        DoGetMakefileIncludes(tmp, target);

        buffer << target->GetTitle() << _T("_INCS=") << tmp << _T('\n');
    }
    buffer << _T('\n');
}

void MakefileGenerator::DoAddMakefileLibs(wxString& buffer)
{
    buffer << _T("### Targets libraries") << _T('\n');

    int targetsCount = m_Project->GetBuildTargetsCount();
    for (int x = 0; x < targetsCount; ++x)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
        if (!target)
            break;

		// create target's options only if it has at least one linkable file
		if (!IsTargetValid(target))
			continue;

        wxString tmp;
        DoGetMakefileLibs(tmp, target);

        buffer << target->GetTitle() << _T("_LIBS=") << tmp << _T('\n');
    }
    buffer << _T('\n');
}

void MakefileGenerator::DoAddMakefileLibDirs(wxString& buffer)
{
    buffer << _T("### Targets library directories") << _T('\n');

    int targetsCount = m_Project->GetBuildTargetsCount();
    for (int x = 0; x < targetsCount; ++x)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
        if (!target)
            break;

		// create target's options only if it has at least one linkable file
		if (!IsTargetValid(target))
			continue;

        wxString tmp;
        DoGetMakefileLibDirs(tmp, target);

        buffer << target->GetTitle() << _T("_LIBDIRS=") << tmp << _T('\n');
    }
    buffer << _T('\n');
}
void MakefileGenerator::DoAddMakefileCFlags(wxString& buffer)
{
    buffer << _T("### Targets compiler flags") << _T('\n');

    int targetsCount = m_Project->GetBuildTargetsCount();
    for (int x = 0; x < targetsCount; ++x)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
        if (!target)
            break;

		// create target's options only if it has at least one linkable file
		if (!IsTargetValid(target))
			continue;

        wxString tmp;
        DoGetMakefileCFlags(tmp, target);

        buffer << target->GetTitle() << _T("_CFLAGS=") << tmp << _T('\n');
    }
    buffer << _T('\n');
}

void MakefileGenerator::DoAddMakefileLDFlags(wxString& buffer)
{
    buffer << _T("### Targets linker flags") << _T('\n');

    int targetsCount = m_Project->GetBuildTargetsCount();
    for (int x = 0; x < targetsCount; ++x)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
        if (!target)
            break;

		// create target's options only if it has at least one linkable file
		if (!IsTargetValid(target))
			continue;

        wxString tmp;
        DoGetMakefileLDFlags(tmp, target);

        buffer << target->GetTitle() << _T("_LDFLAGS=") << tmp;
        buffer << _T('\n');
    }
    buffer << _T('\n');
}

void MakefileGenerator::DoAddMakefileTargets(wxString& buffer)
{
    buffer << _T("### The targets of this project") << _T('\n');

    int targetsCount = m_Project->GetBuildTargetsCount();
    for (int x = 0; x < targetsCount; ++x)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
        if (!target)
            break;

		// create target's options only if it has at least one linkable file
		if (!IsTargetValid(target))
			continue;
        UpdateCompiler(target);

        // the filename is already adapted based on the project type
        wxString out = UnixFilename(target->GetOutputFilename());
    	if (!m_GeneratingMakefile)
            Manager::Get()->GetMacrosManager()->ReplaceEnvVars(out);
        ConvertToMakefileFriendly(out);
//        QuoteStringIfNeeded(out);
		buffer << target->GetTitle() << _T("_BIN=") << out << _T('\n');
        if (target->GetTargetType() == ttDynamicLib)
        {
            wxFileName fname(target->GetOutputFilename());
            if (!fname.GetName().StartsWith(m_CompilerSet->GetSwitches().libPrefix))
                fname.SetName(m_CompilerSet->GetSwitches().libPrefix + fname.GetName());
            fname.SetExt(m_CompilerSet->GetSwitches().libExtension);
            out = UnixFilename(fname.GetFullPath());
            Manager::Get()->GetMacrosManager()->ReplaceEnvVars(out);
            ConvertToMakefileFriendly(out);
            QuoteStringIfNeeded(out);
            buffer << target->GetTitle() << _T("_STATIC_LIB=") << out << _T('\n');
            fname.SetExt(_T("def"));
            out = UnixFilename(fname.GetFullPath());
            Manager::Get()->GetMacrosManager()->ReplaceEnvVars(out);
            ConvertToMakefileFriendly(out);
            QuoteStringIfNeeded(out);
            buffer << target->GetTitle() << _T("_LIB_DEF=") << out << _T('\n');
        }
    }
    buffer << _T('\n');
}

void MakefileGenerator::DoAddPhonyTargets(wxString& buffer)
{
	wxString tmp;
	tmp << _T("all all-before all-custom all-after clean clean-custom distclean distclean-custom ");
    int targetsCount = m_Project->GetBuildTargetsCount();
    for (int x = 0; x < targetsCount; ++x)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
        if (!IsTargetValid(target))
            continue;

        tmp << _T("depend_") << target->GetTitle() << _T(" ")
            << target->GetTitle() << _T("-before ")
            << target->GetTitle() << _T("-after ");
    }
    buffer << _T(".PHONY: ") << tmp << _T('\n');
    buffer << _T('\n');
}

void MakefileGenerator::DoAddMakefileTarget_All(wxString& buffer)
{
    wxString tmp;
	wxString deps;
    int targetsCount = m_Project->GetBuildTargetsCount();
    for (int x = 0; x < targetsCount; ++x)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
        if (!target)
            continue;
        UpdateCompiler(target);

		if (target->GetIncludeInTargetAll())
		{
			// create target's options only if it has at least one linkable file
            // or custom commands...
			if (IsTargetValid(target))
			{
				tmp << target->GetTitle() << _T(" ");
                // to include dependencies, the target must have linkable files...
//                if (m_LinkableTargets.Index(target) != -1 && m_CompilerSet->GetSwitches().needDependencies)
//                    deps << _T("-include $(") << target->GetTitle() << _T("_DEPS)") << _T('\n');
			}
		}
    }

    if (!tmp.IsEmpty()) // include target "all" first, so it is the default target
        buffer << _T("all: all-before ") << tmp << _T("all-after") << _T('\n');
    if (!deps.IsEmpty()) // include dependencies too
        buffer << deps;
    buffer << _T('\n');
}

void MakefileGenerator::DoAddMakefileCommands(const wxString& desc, const wxString& prefix, const wxArrayString& commands, wxString& buffer)
{
    if (!m_CompilerSet)
        return;
	if (commands.GetCount())
	{
		// run any user-defined commands *before* build
        if (!prefix.IsEmpty())
            buffer << prefix << _T(": ") << _T('\n');
        if (m_CompilerSet->GetSwitches().logging == clogSimple)
            buffer << _T('\t') << _T("@echo ") << desc << _T('\n');
		for (unsigned int i = 0; i < commands.GetCount(); ++i)
		{
			wxString tmp = commands[i];
			Manager::Get()->GetMacrosManager()->ReplaceMacros(tmp);
			buffer << _T('\t') << m_Quiet << tmp << _T('\n');
		}
        buffer << _T('\n');
	}
}

void MakefileGenerator::DoAddMakefileTargets_BeforeAfter(wxString& buffer)
{
	DoAddMakefileCommands(_T("Running project pre-build step"), _T("all-before"), m_Project->GetCommandsBeforeBuild(), buffer);
	DoAddMakefileCommands(_T("Running project post-build step"), _T("all-after"), m_Project->GetCommandsAfterBuild(), buffer);

    wxString tmp;
    int targetsCount = m_Project->GetBuildTargetsCount();
    for (int x = 0; x < targetsCount; ++x)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
        if (!target || !IsTargetValid(target))
            continue;

		tmp.Clear();
		tmp << target->GetTitle() << _T("-before");
		DoAddMakefileCommands(_("Running pre-build step"), tmp, target->GetCommandsBeforeBuild(), buffer);
		tmp.Clear();
		tmp << target->GetTitle() << _T("-after");
		DoAddMakefileCommands(_("Running post-build step"), tmp, target->GetCommandsAfterBuild(), buffer);
    }
	buffer << _T('\n');
}

void MakefileGenerator::DoAddMakefileTarget_Clean(wxString& buffer)
{
    wxString tmp;
    wxString tmp1;
    int targetsCount = m_Project->GetBuildTargetsCount();
    for (int x = 0; x < targetsCount; ++x)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
        if (!target)
            break;

		// create target's options only if it has at least one linkable file
		if (!IsTargetValid(target))
			continue;
        UpdateCompiler(target);

        buffer << _T("clean_") << target->GetTitle() << _T(":") << _T('\n');
		if (m_CompilerSet->GetSwitches().logging == clogSimple)
			buffer << _T('\t') << _T("@echo Cleaning target \"") << target->GetTitle() << _T("\"...") << _T('\n');
        buffer << _T('\t') << m_Quiet << _T("$(RM) $(") << target->GetTitle() << _T("_BIN) ");
        buffer << _T("$(") << target->GetTitle() << _T("_OBJS) ");
        buffer << _T("$(") << target->GetTitle() << _T("_RESOURCE) ");
        if (target->GetTargetType() == ttDynamicLib)
        {
            buffer << _T("$(") << target->GetTitle() << _T("_STATIC_LIB) ");
            buffer << _T("$(") << target->GetTitle() << _T("_LIB_DEF) ");
        }
        buffer << _T('\n') << _T('\n');
        tmp << _T("clean_") << target->GetTitle() << _T(" ");

        buffer << _T("distclean_") << target->GetTitle() << _T(":") << _T('\n');
		if (m_CompilerSet->GetSwitches().logging == clogSimple)
			buffer << _T('\t') << _T("@echo Dist-cleaning target \"") << target->GetTitle() << _T("\"...") << _T('\n');
        buffer << _T('\t') << m_Quiet << _T("$(RM) $(") << target->GetTitle() << _T("_BIN) ");
        buffer << _T("$(") << target->GetTitle() << _T("_OBJS) ");
        buffer << _T("$(") << target->GetTitle() << _T("_DEPS) ");
        buffer << _T("$(") << target->GetTitle() << _T("_RESOURCE) ");
        if (target->GetTargetType() == ttDynamicLib)
        {
            buffer << _T("$(") << target->GetTitle() << _T("_STATIC_LIB) ");
            buffer << _T("$(") << target->GetTitle() << _T("_LIB_DEF) ");
        }
        buffer << _T('\n') << _T('\n');
        tmp1 << _T("distclean_") << target->GetTitle() << _T(" ");
    }
    buffer << _T("clean: ") << tmp << _T('\n');
    buffer << _T('\n');
    buffer << _T("distclean: ") << tmp1 << _T('\n');
    buffer << _T('\n');
}

void MakefileGenerator::DoAddMakefileTarget_Dist(wxString& buffer)
{
    wxString tmp = _T("${PROJECT_FILENAME} ${MAKEFILE} ${ALL_PROJECT_FILES}");
    Manager::Get()->GetMacrosManager()->ReplaceMacros(tmp);

    wxFileName fname(m_Project->GetFilename());
    wxString projname = UnixFilename(fname.GetFullName());
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(projname);
    ConvertToMakefileFriendly(projname);
    QuoteStringIfNeeded(projname);

    buffer << _T("dist:") << _T('\n');
    buffer << _T('\t') << _T("@zip ") << projname << _T(".zip ") << tmp << _T('\n');
    buffer << _T('\n');
}

void MakefileGenerator::DoAddMakefileTarget_Depend(wxString& buffer)
{
    wxString tmp;
    int targetsCount = m_Project->GetBuildTargetsCount();
    for (int x = 0; x < targetsCount; ++x)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
        if (!target)
            continue;

		// create target's options only if it has at least one linkable file
		if (!IsTargetValid(target))
			continue;
        UpdateCompiler(target);
        if (!m_CompilerSet->GetSwitches().needDependencies)
            continue;

        buffer << _T("depend_") << target->GetTitle() << _T("_DIRS:") << _T('\n');
        DoAddMakefileCreateDirs(buffer, target, false, true, false);
        buffer << _T('\n');

        buffer << _T("depend_") << target->GetTitle() << _T(": depend_") << target->GetTitle() << _T("_DIRS $(") << target->GetTitle() << _T("_DEPS)") << _T('\n');
        buffer << _T('\n');

        tmp << _T(" depend_") << target->GetTitle();
    }
    buffer << _T("depend:") << tmp << _T('\n');
    buffer << _T('\n');
}

void MakefileGenerator::DoAddMakefileTarget_Link(wxString& buffer)
{
    int targetsCount = m_Project->GetBuildTargetsCount();
    for (int x = 0; x < targetsCount; ++x)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
        UpdateCompiler(target);
        if (!IsTargetValid(target))
            continue;

		buffer << target->GetTitle() << _T("_DIRS:") << _T('\n');
        DoAddMakefileCreateDirs(buffer, target, true, false, true);
		buffer << _T('\n');

		buffer << target->GetTitle() << _T(": depend_") << target->GetTitle() << _T(" ") << target->GetTitle() << _T("_DIRS ") << target->GetTitle() << _T("-before ");
		if (IsTargetValid(target))
        {
			buffer << _T("$(") << target->GetTitle() << _T("_BIN) ");
            // add all custom-built files that do *not* link
//            int filesCount = (int)m_Files.GetCount();
//            for (int i = 0; i < filesCount; ++i)
//            {
//                ProjectFile* pf = m_Files[i];
//                if (pf->useCustomBuildCommand && !pf->link)
//                    buffer << pf->relativeFilename << _T(" ");
//            }
        }
		buffer << target->GetTitle() << _T("-after") << _T('\n');
		buffer << _T('\n');

		// create target's options only if it has at least one linkable file
		if (!IsTargetValid(target))
			continue;

		buffer << _T("$(") << target->GetTitle() << _T("_BIN): ") << _T("$(") << target->GetTitle() << _T("_LINKOBJS) $(") << target->GetTitle() << _T("_RESOURCE)");
        // add external deps
        wxArrayString array = GetArrayFromString(target->GetExternalDeps());
        for (unsigned int i = 0; i < array.GetCount(); ++i)
        {
            buffer << _T(' ') << UnixFilename(array[i]);
        }
		buffer << _T('\n');

		// change link stage command based on target type
		switch (target->GetTargetType())
		{
			case ttConsoleOnly:
			case ttExecutable:
            {
                CommandType ct = target->GetTargetType() == ttConsoleOnly ? ctLinkConsoleExeCmd : ctLinkExeCmd;
                if (m_CompilerSet->GetSwitches().logging == clogSimple)
					buffer << _T('\t') << _T("@echo Linking executable \"") << target->GetOutputFilename() << _T("\"...") << _T('\n');
				wxString compilerCmd = ReplaceCompilerMacros(ct, _T(""), target, _T(""), _T(""), _T(""));
				buffer << _T('\t') << m_Quiet << compilerCmd<< _T('\n');
				break;
			}

			case ttStaticLib:
			{
                if (m_CompilerSet->GetSwitches().logging == clogSimple)
					buffer << _T('\t') << _T("@echo Linking static library \"") << target->GetOutputFilename() << _T("\"...") << _T('\n');
				wxString compilerCmd = ReplaceCompilerMacros(ctLinkStaticCmd, _T(""), target, _T(""), _T(""), _T(""));
				buffer << _T('\t') << m_Quiet << compilerCmd<< _T('\n');
				break;
            }

			case ttDynamicLib:
			{
                if (m_CompilerSet->GetSwitches().logging == clogSimple)
					buffer << _T('\t') << _T("@echo Linking shared library \"") << target->GetOutputFilename() << _T("\"...") << _T('\n');
				wxString compilerCmd = ReplaceCompilerMacros(ctLinkDynamicCmd, _T(""), target, _T(""), _T(""), _T(""));
				buffer << _T('\t') << m_Quiet << compilerCmd<< _T('\n');
				break;
            }
            default: break;
		}
        buffer << _T('\n');
    }
    buffer << _T('\n');
}

void MakefileGenerator::ConvertToMakefileFriendly(wxString& str, bool force)
{
    if (!force && !m_GeneratingMakefile)
        return;

    if (str.IsEmpty())
        return;

    str.Replace(_T("\\"), _T("/"));
    for (unsigned int i = 0; i < str.Length(); ++i)
    {
        if (str[i] == _T(' ') && (i > 0 && str[i - 1] != _T('\\')))
            str.insert(i, _T('\\'));
    }
//    str.Replace("\\\\", "/");
}

void MakefileGenerator::QuoteStringIfNeeded(wxString& str, bool force)
{
    if (!force && m_GeneratingMakefile)
        return;
    if (m_CompilerSet->GetSwitches().forceCompilerUseQuotes ||
        m_CompilerSet->GetSwitches().forceLinkerUseQuotes ||
        (str.Find(' ') != -1 && str.GetChar(0) != '"'))
    {
        str = _T('"') + str + _T('"');
    }
}

wxString MakefileGenerator::GetObjectFile(ProjectFile* pf, ProjectBuildTarget* target)
{
    wxFileName o_filename_tmp = UnixFilename(pf->GetObjName());
    wxFileName o_filename = target->GetObjectOutput() + wxFILE_SEP_PATH + o_filename_tmp.GetFullPath();
    // vars to make easier reading the following code
    wxString o_file = UnixFilename(o_filename.GetFullPath());
    ConvertToMakefileFriendly(o_file);
    QuoteStringIfNeeded(o_file);
    return o_file;
}

wxString MakefileGenerator::GetDependencyFile(ProjectFile* pf, ProjectBuildTarget* target)
{
    wxFileName d_filename_tmp = UnixFilename(pf->GetObjName());
    wxFileName d_filename = target->GetDepsOutput() + wxFILE_SEP_PATH + d_filename_tmp.GetFullPath();
    d_filename.SetExt(_T("d"));
    wxString d_file;
    UpdateCompiler(target);
    if (!m_CompilerSet)
        return d_file;
    if (m_CompilerSet->GetSwitches().needDependencies)
    {
        d_file = UnixFilename(d_filename.GetFullPath());
        ConvertToMakefileFriendly(d_file);
        QuoteStringIfNeeded(d_file);
    }
    return d_file;
}

void MakefileGenerator::DoAddMakefileTarget_Objs(wxString& buffer)
{
    m_ObjectFiles.Clear();
    wxString tmp;
    wxArrayString depfiles; // one occurrence per dep (case where the same file is used in more than one target)
    int targetsCount = m_Project->GetBuildTargetsCount();
    for (int x = 0; x < targetsCount; ++x)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
        if (!target)
            break;
        UpdateCompiler(target);
        if (!IsTargetValid(target))
            continue;

        wxString resources;

        int filesCount = (int)m_Files.GetCount();
        for (int i = 0; i < filesCount; ++i)
        {
            ProjectFile* pf = m_Files[i];
            if (pf->compile &&
                !pf->compilerVar.IsEmpty() &&
                pf->buildTargets.Index(target->GetTitle()) >= 0)
            {
                // vars to make easier reading the following code
                wxString o_file = GetObjectFile(pf, target);
                wxString d_file = GetDependencyFile(pf, target);
                wxString c_file = UnixFilename(pf->relativeFilename);
                ConvertToMakefileFriendly(c_file);
                QuoteStringIfNeeded(c_file);
                wxString targetName = target->GetTitle();

                bool isResource = FileTypeOf(pf->relativeFilename) == ftResource;
                if (!isResource)
                {
                    if (m_CompilerSet->GetSwitches().needDependencies &&
                        depfiles.Index(d_file) == wxNOT_FOUND)
                    {
                        depfiles.Add(d_file);
//                        if (pf->autoDeps)
//                        {
//                            // depend rule
//                            buffer << d_file << _T(": ") << c_file << _T('\n');
//                            if (m_CompilerSet->GetSwitches().logging == clogSimple)
//                                buffer << _T('\t') << _T("@echo Calculating dependencies for \"") << pf->relativeFilename << _T("\"...") << _T('\n');
//                            // gather all object files generated from this source file (multiple targets case)
//                            wxString tmpdep;
//                            for (unsigned int i = 0; i < pf->buildTargets.GetCount(); ++i)
//                            {
//                                ProjectBuildTarget* tmptarget = m_Project->GetBuildTarget(pf->buildTargets[i]);
//                                if (tmptarget)
//                                    tmpdep << GetObjectFile(pf, tmptarget) << _T(',');
//                            }
//                            if (tmpdep.Last() == _T(','))
//                                tmpdep.RemoveLast();
//                            wxString compilerCmd = ReplaceCompilerMacros(ctGenDependenciesCmd, pf->compilerVar, target, c_file, tmpdep, d_file);
//                            if (!compilerCmd.IsEmpty())
//                                buffer << _T('\t') << m_Quiet << compilerCmd << _T('\n');
//                            buffer << _T('\n');
//                        }
//                        else if (!pf->customDeps.IsEmpty())
//                        {
//                            // custom depend rule
//                            wxString customDeps = pf->customDeps;
//                            ReplaceMacros(target, pf, customDeps);
//
//                            buffer << d_file << _T(": ") << c_file << _T('\n');
//                            if (m_CompilerSet->GetSwitches().logging == clogSimple)
//                                buffer << _T('\t') << _T("@echo Generating dependencies for \"") << pf->relativeFilename << _T("\"... (custom dependencies)") << _T('\n');
//                            buffer << _T('\t') << m_Quiet << customDeps << _T('\n');
//                            buffer << _T('\n');
//                        }
                    }
                    else
                        d_file = UnixFilename(pf->relativeFilename); // for compilers that don't need deps, use .cpp file

//					if (pf->useCustomBuildCommand)
//					{
//						// custom build command
//                        wxString customBuild = pf->buildCommand;
//                        ReplaceMacros(target, pf, customBuild);
//                        wxString obj_file = target->GetObjectOutput() + wxFILE_SEP_PATH + pf->GetObjName();
//                        ConvertToMakefileFriendly(obj_file);
//						buffer << obj_file << _T(": ") << d_file << _T('\n');
//                        if (m_CompilerSet->GetSwitches().logging == clogSimple)
//							buffer << _T('\t') << _T("@echo Compiling \"") << pf->relativeFilename << _T("\" (custom command)...") << _T('\n');
//						buffer << _T('\t') << m_Quiet << customBuild << _T('\n');
//						buffer << _T('\n');
//					}
//					else
//					{
//						// compile rule
//						buffer << o_file << _T(": ") << d_file << _T('\n');
//                        if (m_CompilerSet->GetSwitches().logging == clogSimple)
//							buffer << _T('\t') << _T("@echo Compiling \"") << pf->relativeFilename << _T("\"...") << _T('\n');
////                        AddCreateSubdir(buffer, target->GetBasePath(), pf->GetObjName(), target->GetObjectOutput());
//						wxString compilerCmd = ReplaceCompilerMacros(ctCompileObjectCmd, pf->compilerVar, target, c_file, o_file, d_file);
//						if (!compilerCmd.IsEmpty())
//                            buffer << _T('\t') << m_Quiet << compilerCmd << _T('\n');
//						buffer << _T('\n');
//					}
                }
                else
                {
                    if (platform::windows && pf->compile && FileTypeOf(pf->relativeFilename) == ftResource)
                    {
                        wxString out = pf->relativeFilename;
                        ConvertToMakefileFriendly(out);
                        resources << out << _T(" ");
                    }
                }
            }
        }

        if (platform::windows && !resources.IsEmpty())
        {
            wxFileName resFile;
            resFile.SetName(target->GetTitle() + _T("_private"));
            resFile.SetExt(FileFilters::RESOURCE_EXT);
            resFile.MakeRelativeTo(m_Project->GetBasePath());
            buffer << _T("$(") << target->GetTitle() << _T("_RESOURCE): ");
            if (m_CompilerSet->GetSwitches().needDependencies)
                 buffer << resources;
            buffer << _T('\n');
            if (m_CompilerSet->GetSwitches().logging == clogSimple)
				buffer << _T('\t') << _T("@echo Compiling resources...") << _T('\n');
            wxString compilerCmd = ReplaceCompilerMacros(ctCompileResourceCmd, _T(""), target, UnixFilename(resFile.GetFullPath()), _T(""), _T(""));
            if (!compilerCmd.IsEmpty())
                buffer << _T('\t') << m_Quiet << compilerCmd << _T('\n');
            /*buffer << _T('\t') << m_Quiet << _T("$(RESCOMP) -i ") << UnixFilename(resFile.GetFullPath()) << _T(" -J rc ");
            buffer << _T("-o $(") << target->GetTitle() << _T("_RESOURCE) -O coff ");

			DoAppendIncludeDirs(buffer, 0L, _T("--include-dir="), true);
			DoAppendIncludeDirs(buffer, 0L, _T("--include-dir=");
			DoAppendIncludeDirs(buffer, target, _T("--include-dir=");*/
			buffer << _T('\n');
        }
        buffer << _T('\n');
    }
    buffer << _T('\n');
}

int SortProjectFilesByWeight(ProjectFile** one, ProjectFile** two)
{
    return (*one)->weight - (*two)->weight;
}

void MakefileGenerator::DoPrepareFiles()
{
    m_Files.Clear();

    for (int i = 0; i < m_Project->GetFilesCount(); ++i)
    {
        ProjectFile* pf = m_Project->GetFile(i);
        m_Files.Add(pf);
    }
    m_Files.Sort(SortProjectFilesByWeight);
}

void MakefileGenerator::DoPrepareValidTargets()
{
	m_LinkableTargets.Clear();
    int targetsCount = m_Project->GetBuildTargetsCount();
    for (int x = 0; x < targetsCount; ++x)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
        if (!target)
            continue;

		// create link target only if it has at least one linkable file
		bool hasFiles = false;
		for (unsigned int i = 0; i < m_Files.GetCount(); ++i)
		{
			ProjectFile* pf = m_Files[i];
			if (pf->link && pf->buildTargets.Index(target->GetTitle()) >= 0)
			{
				hasFiles = true;
				break;
			}
		}
		if (hasFiles)
			m_LinkableTargets.Add(target);
	}
}

bool MakefileGenerator::IsTargetValid(ProjectBuildTarget* target)
{
    UpdateCompiler(target);
    if (!m_CompilerSet || !target)
        return false;
    bool hasBin = target->GetTargetType() != ttCommandsOnly; // is not "commands-only" target
    bool hasCmds = !target->GetCommandsAfterBuild().IsEmpty() ||
                    !target->GetCommandsBeforeBuild().IsEmpty();
	return hasBin && (hasCmds || m_LinkableTargets.Index(target) != -1);
}

void MakefileGenerator::ReplaceMacros(ProjectBuildTarget* bt, ProjectFile* pf, wxString& text)
{
	wxString o_dir = bt ? bt->GetObjectOutput() + wxFILE_SEP_PATH : _T("");
	wxString d_dir = bt ? bt->GetDepsOutput() + wxFILE_SEP_PATH : _T("");
	wxFileName d_filename = d_dir + pf->GetObjName();
	d_filename.SetExt(_T("d"));
	wxString d_file = d_filename.GetFullPath();
	ConvertToMakefileFriendly(o_dir);
	ConvertToMakefileFriendly(d_dir);
	ConvertToMakefileFriendly(d_file);
	QuoteStringIfNeeded(o_dir);
	QuoteStringIfNeeded(d_dir);
	QuoteStringIfNeeded(d_file);

    wxFileName fname(pf->relativeFilename);
    text.Replace(_T("$DIR"), UnixFilename(fname.GetPath(wxPATH_GET_VOLUME)));
    if (bt)
        text.Replace(_T("$INCLUDES"),_T("$(") + bt->GetTitle() + _T("_INCS)"));
    if (bt)
        text.Replace(_T("$CFLAGS"),_T("$(") + bt->GetTitle() + _T("_CFLAGS)"));
    if (bt)
        text.Replace(_T("$LDFLAGS"),_T("$(") + bt->GetTitle() + _T("_LDFLAGS)"));
    if (bt)
        text.Replace(_T("$LIBS"),_T("$(") + bt->GetTitle() + _T("_LIBS)"));
    if (bt)
        text.Replace(_T("$LIBDIRS"),_T("$(") + bt->GetTitle() + _T("_LIBDIRS)"));
    text.Replace(_T("$NAME"), UnixFilename(fname.GetName()));
    text.Replace(_T("$BASE"), pf->GetBaseName());
    text.Replace(_T("$DEPEND_DIR"), d_dir);
    text.Replace(_T("$OBJECT_DIR"), o_dir);
    text.Replace(_T("$DEPEND"), d_file);
    text.Replace(_T("$OBJECT"), o_dir + pf->GetObjName());
    text.Replace(_T("$FILENAME"), UnixFilename(pf->relativeFilename));
    text.Replace(_T("\n"), _T("\n\t") + m_Quiet);
}

bool MakefileGenerator::CreateMakefile()
{
    m_GeneratingMakefile = true;

    if (m_CompilerSet->GetSwitches().logging != clogFull)
        m_Quiet = _T("@");
    else
        m_Quiet = wxEmptyString;
    DoPrepareFiles();
	DoPrepareValidTargets();

    wxString buffer;
    buffer << _T("###############################################################################") << _T('\n');
    buffer << _("# Makefile automatically generated by Code::Blocks IDE                        #") << _T('\n');
    buffer << _T("###############################################################################") << _T('\n');
    buffer << _T('\n');
    buffer << _("# Project:          ") << m_Project->GetTitle() << _T('\n');
    buffer << _("# Project filename: ") << m_Project->GetFilename() << _T('\n');
//    buffer << _T("# Date:             ") << wxDateTime::Now().Format("%c", wxDateTime::Local) << _T('\n');
    buffer << _("# Compiler used:    ") << m_CompilerSet->GetName() << _T('\n');
    buffer << _T('\n');

    DoAddMakefileVars(buffer);
    DoAddMakefileOptions(buffer);
    DoAddMakefileCFlags(buffer);
    DoAddMakefileLDFlags(buffer);
    DoAddMakefileIncludes(buffer);
    DoAddMakefileLibDirs(buffer);
    DoAddMakefileLibs(buffer);
    buffer << _T("###############################################################################") << _T('\n');
    buffer << _("#         You shouldn't need to modify anything beyond this point             #") << _T('\n');
    buffer << _T("###############################################################################") << _T('\n');
    buffer << _T('\n');

    if(platform::windows)
        DoAddMakefileResources(buffer);

    DoAddMakefileObjs(buffer);
    DoAddMakefileTargets(buffer);
	DoAddPhonyTargets(buffer);
    DoAddMakefileTarget_All(buffer);
	DoAddMakefileTargets_BeforeAfter(buffer);
    DoAddMakefileTarget_Dist(buffer);
    DoAddMakefileTarget_Clean(buffer);
    DoAddMakefileTarget_Depend(buffer);
    DoAddMakefileTarget_Link(buffer);
    DoAddMakefileTarget_Objs(buffer);

    // write Makefile to disk
    wxFile file(m_Makefile, wxFile::write);
    cbWrite(file,buffer);

    m_GeneratingMakefile = false;
    return true;
}
