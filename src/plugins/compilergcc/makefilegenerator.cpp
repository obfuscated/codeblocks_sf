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

#include "makefilegenerator.h" // class's header file
#include <manager.h>
#include <macrosmanager.h>
#include <messagemanager.h>
#include <wx/file.h>
#include "compilerfactory.h"

// class constructor
MakefileGenerator::MakefileGenerator(CompilerGCC* compiler, cbProject* project, const wxString& makefile, int logIndex)
    : m_Compiler(compiler),
    m_CompilerSet(CompilerFactory::Compilers[compiler->GetCurrentCompilerIndex()]),
    m_Programs(m_CompilerSet->GetPrograms()),
    m_Switches(m_CompilerSet->GetSwitches()),
	m_Project(project),
    m_Makefile(makefile),
    m_LogIndex(logIndex),
    m_Vars(compiler),
    m_GeneratingMakefile(false)
{
}

// class destructor
MakefileGenerator::~MakefileGenerator()
{
}

wxString MakefileGenerator::ReplaceCompilerMacros(CommandType et,
                                                const wxString& compilerVar,
                                                ProjectBuildTarget* target,
                                                const wxString& file,
                                                const wxString& object,
                                                const wxString& deps)
{
    wxString compilerCmd = m_CompilerSet->GetCommand(et);
    
    compilerCmd.Replace("$compiler", "$(" + compilerVar + ")");
    compilerCmd.Replace("$linker", "$(LD)");
    compilerCmd.Replace("$rescomp", "$(RESCOMP)");
    compilerCmd.Replace("$options", "$(" + target->GetTitle() + "_CFLAGS)");
    compilerCmd.Replace("$link_options", "$(" + target->GetTitle() + "_LDFLAGS)");
    compilerCmd.Replace("$includes", "$(" + target->GetTitle() + "_INCS)");
    compilerCmd.Replace("$libdirs", "$(" + target->GetTitle() + "_LIBS)");
    compilerCmd.Replace("$libs", "$(" + target->GetTitle() + "_LDADD)");
    compilerCmd.Replace("$file", file);
    compilerCmd.Replace("$objects", "$(" + target->GetTitle() + "_OBJS)");
    compilerCmd.Replace("$dep_object", deps);
    compilerCmd.Replace("$object", object);
    compilerCmd.Replace("$link_objects", "$(" + target->GetTitle() + "_LINKOBJS)");
    compilerCmd.Replace("$exe_output", "$(" + target->GetTitle() + "_BIN)");
    if (target->GetTargetType() == ttDynamicLib && target->GetCreateStaticLib())
        compilerCmd.Replace("$static_output", "$(" + target->GetTitle() + "_STATIC_LIB)");
    else
        compilerCmd.Replace("-Wl,--out-implib=$static_output", "");
    if ((target->GetTargetType() == ttDynamicLib || target->GetTargetType() == ttStaticLib) && target->GetCreateStaticLib())
        compilerCmd.Replace("$def_output", "$(" + target->GetTitle() + "_LIB_DEF)");
    else
        compilerCmd.Replace("-Wl,--output-def=$def_output", "");
    compilerCmd.Replace("$resource_output", "$(" + target->GetTitle() + "_RESOURCE)");

    int idx = compilerCmd.Find("$res_includes");
    if (idx != -1)
    {
        wxString incs;
        DoAppendIncludeDirs(incs, 0L, m_Switches.includeDirs, true);
        DoAppendIncludeDirs(incs, 0L, m_Switches.includeDirs);
        DoAppendIncludeDirs(incs, target, m_Switches.includeDirs);
        compilerCmd.Replace("$res_includes", incs);
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
    wxString compiler;
    if (pf)
    {
        if (pf->compilerVar.Matches("CPP"))
            compiler = m_Programs.CPP;
        else if (pf->compilerVar.Matches("CC"))
            compiler = m_Programs.C;
        else if (pf->compilerVar.Matches("WINDRES"))
            compiler = m_Programs.WINDRES;
        else
            return wxEmptyString; // unknown compiler var
    }
    
    wxString cflags;
    wxString global_cflags;
	wxString prj_cflags;
	DoAppendCompilerOptions(global_cflags, 0L, true);
	DoAppendCompilerOptions(prj_cflags, 0L);
    DoGetMakefileCFlags(cflags, target);
    cflags.Replace("$(GLOBAL_CFLAGS)", global_cflags);
    cflags.Replace("$(PROJECT_CFLAGS)", prj_cflags);

    wxString ldflags;
	wxString global_ldflags;
	wxString prj_ldflags;
	DoAppendLinkerOptions(global_ldflags, 0L, true);
	DoAppendLinkerOptions(prj_ldflags, 0L);
	DoGetMakefileLDFlags(ldflags, target);
    ldflags.Replace("$(GLOBAL_LDFLAGS)", global_ldflags);
    ldflags.Replace("$(PROJECT_LDFLAGS)", prj_ldflags);
    if (target && target->GetTargetType() == ttExecutable)
        ldflags << " " << m_CompilerSet->GetSwitches().linkerSwitchForGui;

    wxString incs;
	wxString global_incs;
	wxString prj_incs;
	wxString res_incs;
	DoAppendIncludeDirs(global_incs, 0L, m_Switches.includeDirs, true);
	DoAppendIncludeDirs(prj_incs, 0L, m_Switches.includeDirs);
	res_incs  << global_incs << " " << prj_incs << " ";
	DoAppendIncludeDirs(res_incs, target, m_Switches.includeDirs);
	DoGetMakefileIncludes(incs, target);
    incs.Replace("$(GLOBAL_INCS)", global_incs);
    incs.Replace("$(PROJECT_INCS)", prj_incs);

    wxString libs;
	wxString global_libs;
	wxString prj_libs;
	DoAppendLibDirs(global_libs, 0L, m_Switches.libDirs, true);
	DoAppendLibDirs(prj_libs, 0L, m_Switches.libDirs);
	DoGetMakefileLibs(libs, target);
    libs.Replace("$(GLOBAL_LIBS)", global_libs);
    libs.Replace("$(PROJECT_LIBS)", prj_libs);

    wxString output = UnixFilename(target->GetOutputFilename());
    ConvertToMakefileFriendly(output);
    QuoteStringIfNeeded(output);
    
    wxString linkobjs;

    wxString compilerCmd = m_CompilerSet->GetCommand(et);
    compilerCmd.Replace("$compiler", compiler);
    compilerCmd.Replace("$linker", m_Programs.LD);
    compilerCmd.Replace("$rescomp", m_Programs.WINDRES);
    compilerCmd.Replace("$options", cflags);
    compilerCmd.Replace("$link_options", ldflags);
    compilerCmd.Replace("$includes", incs);
    compilerCmd.Replace("$res_includes", res_incs);
    compilerCmd.Replace("$libdirs", libs);
    compilerCmd.Replace("$libs", libs);
    compilerCmd.Replace("$file", file);
    compilerCmd.Replace("$dep_object", deps);
    compilerCmd.Replace("$object", object);
    compilerCmd.Replace("$exe_output", output);
    compilerCmd.Replace("$resource_output", object);
    compilerCmd.Replace("$link_objects", object);

    wxFileName fname(target->GetOutputFilename());
    fname.SetName("lib" + fname.GetName());
    fname.SetExt(STATICLIB_EXT);
    wxString out = UnixFilename(fname.GetFullPath());
    ConvertToMakefileFriendly(out);
    QuoteStringIfNeeded(out);

    if (target->GetTargetType() == ttDynamicLib && target->GetCreateStaticLib())
        compilerCmd.Replace("$static_output", out);
    else
        compilerCmd.Replace("-Wl,--out-implib=$static_output", "");
    if ((target->GetTargetType() == ttDynamicLib || target->GetTargetType() == ttStaticLib) && target->GetCreateStaticLib())
    {
        fname.SetExt("def");
        out = UnixFilename(fname.GetFullPath());
        ConvertToMakefileFriendly(out);
        QuoteStringIfNeeded(out);
        compilerCmd.Replace("$def_output", out);
    }
    else
        compilerCmd.Replace("-Wl,--output-def=$def_output", "");

    return compilerCmd;
}

void MakefileGenerator::DoAppendCompilerOptions(wxString& cmd, ProjectBuildTarget* target, bool useGlobalOptions)
{
    wxArrayString opts;
	if (useGlobalOptions)
		opts = m_CompilerSet->GetCompilerOptions();
	else
	{
		if (target)
			opts = target->GetCompilerOptions();
		else
			opts = m_Project->GetCompilerOptions();
	}
	
    for (unsigned int x = 0; x < opts.GetCount(); ++x)
        cmd << " " << opts[x];
}

void MakefileGenerator::DoAppendLinkerOptions(wxString& cmd, ProjectBuildTarget* target, bool useGlobalOptions)
{
    wxArrayString opts;
	if (useGlobalOptions)
		opts = m_CompilerSet->GetLinkerOptions();
	else
	{
		if (target)
			opts = target->GetLinkerOptions();
		else
			opts = m_Project->GetLinkerOptions();
	}

    for (unsigned int x = 0; x < opts.GetCount(); ++x)
        cmd << " " << opts[x];
}

void MakefileGenerator::DoAppendIncludeDirs(wxString& cmd, ProjectBuildTarget* target, const wxString& prefix, bool useGlobalOptions)
{
    wxArrayString opts;
	if (useGlobalOptions)
		opts = m_CompilerSet->GetIncludeDirs();
	else
	{
		if (target)
			opts = target->GetIncludeDirs();
		else
			opts = m_Project->GetIncludeDirs();
	}
	
    for (unsigned int x = 0; x < opts.GetCount(); ++x)
    {
        wxString out = UnixFilename(opts[x]);
        ConvertToMakefileFriendly(out);
        QuoteStringIfNeeded(out);
        cmd << " " << prefix << out;
    }
}

void MakefileGenerator::DoAppendLibDirs(wxString& cmd, ProjectBuildTarget* target, const wxString& prefix, bool useGlobalOptions)
{
    wxArrayString opts;
	if (useGlobalOptions)
		opts = m_CompilerSet->GetLibDirs();
	else
	{
		if (target)
			opts = target->GetLibDirs();
		else
			opts = m_Project->GetLibDirs();
	}
	
    for (unsigned int x = 0; x < opts.GetCount(); ++x)
    {
        wxString out = UnixFilename(opts[x]);
        ConvertToMakefileFriendly(out);
        QuoteStringIfNeeded(out);
        cmd << " " << prefix << out;
    }
}

void MakefileGenerator::DoGetMakefileIncludes(wxString& buffer, ProjectBuildTarget* target)
{
    wxString prefix = m_Switches.includeDirs;
	buffer << " $(GLOBAL_INCS)";
	
    OptionsRelation relation = target->GetOptionRelation(ortIncludeDirs);
    switch (relation)
    {
        case orUseParentOptionsOnly:
            buffer << " $(PROJECT_INCS)";
            break;
        case orUseTargetOptionsOnly:
            DoAppendIncludeDirs(buffer, target, prefix);
            break;
        case orPrependToParentOptions:
            DoAppendIncludeDirs(buffer, target, prefix);
            buffer << " $(PROJECT_INCS)";
            break;
        case orAppendToParentOptions:
            buffer << " $(PROJECT_INCS)";
            DoAppendIncludeDirs(buffer, target, prefix);
            break;
    }
}

void MakefileGenerator::DoGetMakefileLibs(wxString& buffer, ProjectBuildTarget* target)
{
    wxString prefix = m_Switches.libDirs;
	buffer << " $(GLOBAL_LIBS)";

    OptionsRelation relation = target->GetOptionRelation(ortLibDirs);
    switch (relation)
    {
        case orUseParentOptionsOnly:
			buffer << " $(PROJECT_LIBS)";
            break;
        case orUseTargetOptionsOnly:
            DoAppendLibDirs(buffer, target, prefix);
            break;
        case orPrependToParentOptions:
            DoAppendLibDirs(buffer, target, prefix);
			buffer << " $(PROJECT_LIBS)";
            break;
        case orAppendToParentOptions:
			buffer << " $(PROJECT_LIBS)";
            DoAppendLibDirs(buffer, target, prefix);
            break;
    }
}

void MakefileGenerator::DoGetMakefileCFlags(wxString& buffer, ProjectBuildTarget* target)
{
	buffer << " $(GLOBAL_CFLAGS)";
	
    OptionsRelation relation = target->GetOptionRelation(ortCompilerOptions);
    switch (relation)
    {
        case orUseParentOptionsOnly:
			buffer << " $(PROJECT_CFLAGS)";
            break;
        case orUseTargetOptionsOnly:
            DoAppendCompilerOptions(buffer, target);
            break;
        case orPrependToParentOptions:
            DoAppendCompilerOptions(buffer, target);
			buffer << " $(PROJECT_CFLAGS)";
            break;
        case orAppendToParentOptions:
			buffer << " $(PROJECT_CFLAGS)";
            DoAppendCompilerOptions(buffer, target);
            break;
    }
}

void MakefileGenerator::DoGetMakefileLDFlags(wxString& buffer, ProjectBuildTarget* target)
{
	buffer << " $(GLOBAL_LDFLAGS)";
    
	OptionsRelation relation = target->GetOptionRelation(ortLinkerOptions);
    switch (relation)
    {
        case orUseParentOptionsOnly:
			buffer << " $(PROJECT_LDFLAGS)";
            break;
        case orUseTargetOptionsOnly:
            DoAppendLinkerOptions(buffer, target);
            break;
        case orPrependToParentOptions:
            DoAppendLinkerOptions(buffer, target);
			buffer << " $(PROJECT_LDFLAGS)";
            break;
        case orAppendToParentOptions:
			buffer << " $(PROJECT_LDFLAGS)";
            DoAppendLinkerOptions(buffer, target);
            break;
    }
}

void MakefileGenerator::DoAddMakefileVars(wxString& buffer)
{
    buffer << "### Variables used in this Makefile" << '\n';

    // user vars
	const VarsArray& vars = m_Vars.GetVars();
	for (unsigned int i = 0; i < vars.GetCount(); ++i)
	{
		buffer << vars[i].name << "=" << vars[i].value << '\n';
	}

    // compiler vars
    // defined last so even if the user sets custom vars
    // by these names, ours will have precedence...
    buffer << "CC=" << m_Programs.C << '\n';
    buffer << "CPP=" << m_Programs.CPP << '\n';
    buffer << "LD=" << m_Programs.LD << '\n';
    buffer << "RESCOMP=" << m_Programs.WINDRES << '\n';
	
    buffer << '\n';
}

#ifdef __WXMSW__
void MakefileGenerator::DoAddMakefileResources(wxString& buffer)
{
    buffer << "### Resources used in this Makefile" << '\n';

    int targetsCount = m_Project->GetBuildTargetsCount();
    for (int x = 0; x < targetsCount; ++x)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
        if (!target)
            break;

		// create target's options only if it has at least one linkable file
		if (!IsTargetValid(target))
			continue;

        buffer << target->GetTitle() << "_RESOURCE=";

        if (target->GetTargetType() == ttConsoleOnly)
        {
            buffer << '\n';
            break;
        }

        wxFileName resFile;
        resFile.SetName(target->GetTitle() + "_private");
        resFile.SetExt(RESOURCEBIN_EXT);
        resFile.MakeRelativeTo(m_Project->GetBasePath());

        // now create the resource file...
        bool hasResources = false;
        wxString resBuf;
        resBuf << "#include <windows.h>" << '\n';
        int filesCount = (int)m_Files.GetCount();
        for (int i = 0; i < filesCount; ++i)
        {
            wxFileName file;

            ProjectFile* pf = m_Files[i];
            // if the file is allowed to compile *and* belongs in this target
            if (pf->link && pf->buildTargets.Index(target->GetTitle()) >= 0)
            {
                file.Assign(pf->relativeFilename);
                if (file.GetExt().Lower().Matches("rc"))
                {
                    resBuf << "#include \"" << file.GetFullPath() << "\"" << '\n';
                    hasResources = true;
                }
            }
        }
        
        if (hasResources)
        {
            buffer << UnixFilename(resFile.GetFullPath()) << '\n';
            // write private resource file to disk
            resFile.Normalize(wxPATH_NORM_ALL, m_Project->GetBasePath());
            resFile.Normalize(wxPATH_NORM_ALL, m_Project->GetBasePath());
            resFile.SetExt(RESOURCE_EXT);
            wxFile file(resFile.GetFullPath(), wxFile::write);
            file.Write(resBuf, resBuf.Length());
            file.Flush();
        }
        else
            buffer << '\n';
    }
    buffer << '\n';
}
#endif // __WXMSW__

void MakefileGenerator::DoAddMakefileObjs(wxString& buffer)
{
    buffer << "### Objects used in this Makefile" << '\n';

    int targetsCount = m_Project->GetBuildTargetsCount();
    for (int x = 0; x < targetsCount; ++x)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
        if (!target)
            break;

		// create target's options only if it has at least one linkable file
		if (!IsTargetValid(target))
			continue;

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
				wxString depsS = deps_tmp.GetPath(wxPATH_GET_VOLUME);
				if (!depsS.IsEmpty())
                     depsS << "/";
                depsS << target->GetDepsOutput() << "/" << deps_tmp.GetName() << ".d";

				wxFileName objs_tmp = fname;
				wxString objsS = objs_tmp.GetPath(wxPATH_GET_VOLUME);
				if (!objsS.IsEmpty())
                     objsS << "/";
                objsS << target->GetObjectOutput() << "/" << objs_tmp.GetName() << "." << objs_tmp.GetExt();

                objsS = UnixFilename(objsS);
                ConvertToMakefileFriendly(objsS);
                QuoteStringIfNeeded(objsS);
                depsS = UnixFilename(depsS);
                ConvertToMakefileFriendly(depsS);
                QuoteStringIfNeeded(depsS);

				if (pf->compile)
				{
                    deps << depsS << " ";
                    tmp << objsS << " "; // if the file is allowed to compile
                }
				if (pf->link)
					tmpLink << objsS << " "; // if the file is allowed to link
            }
        }
        buffer << target->GetTitle() << "_OBJS=" << tmp << '\n';
        buffer << target->GetTitle() << "_LINKOBJS=";
		if (tmp.Matches(tmpLink))
			buffer << "$(" << target->GetTitle() << "_OBJS)";
		else
			buffer << tmpLink; // only write *_LINKOBJS if different from *_OBJS
		if (target->GetTargetType() != ttConsoleOnly)
			buffer << " $(" << target->GetTitle() << "_RESOURCE)";
        buffer << '\n';
        if (m_CompilerSet->GetSwitches().needDependencies)
        {
            buffer << target->GetTitle() << "_DEPS=" << deps << '\n';
//            buffer << target->GetTitle() << "_DEPS=$(" << target->GetTitle() << "_OBJS:.";
//            buffer << m_CompilerSet->GetSwitches().objectExtension;
//            buffer << "=.d)" << '\n';
        }
    }
    buffer << '\n';
}

void MakefileGenerator::DoAddMakefileOptions(wxString& buffer)
{
    buffer << "### Compiler/linker options" << '\n';

	buffer << "GLOBAL_CFLAGS=";
	DoAppendCompilerOptions(buffer, 0L, true);
	buffer << '\n';
	
	buffer << "PROJECT_CFLAGS=";
	DoAppendCompilerOptions(buffer, 0L);
    buffer << '\n';

	buffer << "GLOBAL_LDFLAGS=";
	DoAppendLinkerOptions(buffer, 0L, true);
	buffer << '\n';
	
	buffer << "PROJECT_LDFLAGS=";
	DoAppendLinkerOptions(buffer, 0L);
    buffer << '\n';

	buffer << "GLOBAL_INCS=";
	DoAppendIncludeDirs(buffer, 0L, m_Switches.includeDirs, true);
	buffer << '\n';
	
	buffer << "PROJECT_INCS=";
	DoAppendIncludeDirs(buffer, 0L, m_Switches.includeDirs);
    buffer << '\n';

	buffer << "GLOBAL_LIBS=";
	DoAppendLibDirs(buffer, 0L, m_Switches.libDirs, true);
	buffer << '\n';
	
	buffer << "PROJECT_LIBS=";
	DoAppendLibDirs(buffer, 0L, m_Switches.libDirs);
    buffer << '\n';

	buffer << '\n';
}

void MakefileGenerator::DoAddMakefileIncludes(wxString& buffer)
{
    buffer << "### Targets include directories" << '\n';


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

        buffer << target->GetTitle() << "_INCS=" << tmp << '\n';
    }
    buffer << '\n';
}

void MakefileGenerator::DoAddMakefileLibs(wxString& buffer)
{
    buffer << "### Targets library directories" << '\n';

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

        buffer << target->GetTitle() << "_LDADD=" << tmp << '\n';
    }
    buffer << '\n';
}

void MakefileGenerator::DoAddMakefileCFlags(wxString& buffer)
{
    buffer << "### Targets compiler flags" << '\n';

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

        buffer << target->GetTitle() << "_CFLAGS=" << tmp << '\n';
    }
    buffer << '\n';
}

void MakefileGenerator::DoAddMakefileLDFlags(wxString& buffer)
{
    buffer << "### Targets linker flags" << '\n';

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

        buffer << target->GetTitle() << "_LDFLAGS=" << tmp;
        if (target->GetTargetType() == ttExecutable)
            buffer << " " << m_CompilerSet->GetSwitches().linkerSwitchForGui;
        buffer << '\n';
    }
    buffer << '\n';
}

void MakefileGenerator::DoAddMakefileTargets(wxString& buffer)
{
    buffer << "### The targets of this project" << '\n';

    int targetsCount = m_Project->GetBuildTargetsCount();
    for (int x = 0; x < targetsCount; ++x)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
        if (!target)
            break;

		// create target's options only if it has at least one linkable file
		if (!IsTargetValid(target))
			continue;

        // add commands to create the target directory (if it does not exist)
        wxFileName fname(target->GetOutputFilename());
        fname.MakeRelativeTo(m_Project->GetBasePath());
		wxString out = UnixFilename(fname.GetPath(wxPATH_GET_VOLUME));
		if (out.IsEmpty())
			out = ".";
        ConvertToMakefileFriendly(out);
        QuoteStringIfNeeded(out);
        buffer << target->GetTitle() << "_OUTDIR=" << out << '\n';

        // the filename is already adapted based on the project type
        out = UnixFilename(target->GetOutputFilename());
        ConvertToMakefileFriendly(out);
        QuoteStringIfNeeded(out);
		buffer << target->GetTitle() << "_BIN=" << out << '\n';
        if (target->GetTargetType() == ttDynamicLib)
        {
            wxFileName fname(target->GetOutputFilename());
            fname.SetName("lib" + fname.GetName());
            fname.SetExt(STATICLIB_EXT);
            out = UnixFilename(fname.GetFullPath());
            ConvertToMakefileFriendly(out);
            QuoteStringIfNeeded(out);
            buffer << target->GetTitle() << "_STATIC_LIB=" << out << '\n';
            fname.SetExt("def");
            out = UnixFilename(fname.GetFullPath());
            ConvertToMakefileFriendly(out);
            QuoteStringIfNeeded(out);
            buffer << target->GetTitle() << "_LIB_DEF=" << out << '\n';
        }
    }
    buffer << '\n';
}

void MakefileGenerator::DoAddPhonyTargets(wxString& buffer)
{
	wxString tmp;
	tmp << "all all-before all-custom all-after clean clean-custom distclean distclean-custom ";
    int targetsCount = m_Project->GetBuildTargetsCount();
    for (int x = 0; x < targetsCount; ++x)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
        if (!target)
            break;

        tmp << target->GetTitle() << "-before " << target->GetTitle() << "-after ";
    }
    buffer << ".PHONY: " << tmp << '\n';
    buffer << '\n';
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
            break;

		if (target->GetIncludeInTargetAll())
		{
			// create target's options only if it has at least one linkable file
            // or custom commands...
			if (IsTargetValid(target))
			{
				tmp << target->GetTitle() << " ";
                // to include dependencies, the target must have linkable files...
                if (m_LinkableTargets.Index(target) != -1 && m_CompilerSet->GetSwitches().needDependencies)
                    deps << "-include $(" << target->GetTitle() << "_DEPS)" << '\n';
			}
		}
    }

    if (!tmp.IsEmpty()) // include target "all" first, so it is the default target
        buffer << "all: all-before " << tmp << "all-after" << '\n';
    if (!deps.IsEmpty()) // include dependencies too
        buffer << deps;
    buffer << '\n';
}

void MakefileGenerator::DoAddMakefileCommands(const wxString& prefix, const wxArrayString& commands, wxString& buffer)
{
	if (commands.GetCount())
	{
		// run any user-defined commands *before* build
		if (!prefix.IsEmpty())
			buffer << prefix << ": " << '\n';
		for (unsigned int i = 0; i < commands.GetCount(); ++i)
		{
			wxString tmp = commands[i];
			Manager::Get()->GetMacrosManager()->ReplaceMacros(tmp);
			buffer << '\t' << tmp << '\n';
		}
		buffer << '\n';
	}
}

void MakefileGenerator::DoAddMakefileTargets_BeforeAfter(wxString& buffer)
{
	DoAddMakefileCommands("all-before", m_Project->GetCommandsBeforeBuild(), buffer);
	DoAddMakefileCommands("all-after", m_Project->GetCommandsAfterBuild(), buffer);
	
    wxString tmp;
    int targetsCount = m_Project->GetBuildTargetsCount();
    for (int x = 0; x < targetsCount; ++x)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
        if (!target)
            break;

		tmp.Clear();
		tmp << target->GetTitle() << "-before";
		DoAddMakefileCommands(tmp, target->GetCommandsBeforeBuild(), buffer);
		tmp << target->GetTitle() << "-after";
		DoAddMakefileCommands(tmp, target->GetCommandsAfterBuild(), buffer);
    }
	buffer << '\n';
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

        buffer << "clean_" << target->GetTitle() << ":" << '\n';
		if (m_CompilerSet->GetSwitches().logging == clogSimple)
			buffer << '\t' << "@echo Cleaning target \"" << target->GetTitle() << "\"..." << '\n';
        buffer << '\t' << m_Quiet << "$(RM) $(" << target->GetTitle() << "_BIN) ";
        buffer << "$(" << target->GetTitle() << "_OBJS) ";
        buffer << "$(" << target->GetTitle() << "_RESOURCE) ";
        if (target->GetTargetType() == ttDynamicLib)
        {
            buffer << "$(" << target->GetTitle() << "_STATIC_LIB) ";
            buffer << "$(" << target->GetTitle() << "_LIB_DEF) ";
        }
        buffer << '\n' << '\n';
        tmp << "clean_" << target->GetTitle() << " ";

        buffer << "distclean_" << target->GetTitle() << ":" << '\n';
		if (m_CompilerSet->GetSwitches().logging == clogSimple)
			buffer << '\t' << "@echo Dist-cleaning target \"" << target->GetTitle() << "\"..." << '\n';
        buffer << '\t' << m_Quiet << "$(RM) $(" << target->GetTitle() << "_BIN) ";
        buffer << "$(" << target->GetTitle() << "_OBJS) ";
        buffer << "$(" << target->GetTitle() << "_DEPS) ";
        buffer << "$(" << target->GetTitle() << "_RESOURCE) ";
        if (target->GetTargetType() == ttDynamicLib)
        {
            buffer << "$(" << target->GetTitle() << "_STATIC_LIB) ";
            buffer << "$(" << target->GetTitle() << "_LIB_DEF) ";
        }
        buffer << '\n' << '\n';
        tmp1 << "distclean_" << target->GetTitle() << " ";
    }
    buffer << "clean: " << tmp << '\n';
    buffer << '\n';
    buffer << "distclean: " << tmp1 << '\n';
    buffer << '\n';
}

void MakefileGenerator::DoAddMakefileTarget_Dist(wxString& buffer)
{
    wxString tmp = "${PROJECT_FILENAME} ${MAKEFILE} ${ALL_PROJECT_FILES}";
    Manager::Get()->GetMacrosManager()->ReplaceMacros(tmp);
    wxFileName fname(m_Project->GetFilename());
    
    buffer << "dist:" << '\n';
    buffer << '\t' << "@$(ZIP) " << UnixFilename(fname.GetFullName()) << ".$(ZIP_EXT) " << tmp << '\n';
    buffer << '\n';
}

void MakefileGenerator::DoAddMakefileTarget_Depend(wxString& buffer)
{
    if (!m_CompilerSet->GetSwitches().needDependencies)
        return;

    wxString tmp;
    int targetsCount = m_Project->GetBuildTargetsCount();
    for (int x = 0; x < targetsCount; ++x)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
        if (!target)
            break;

		// create target's options only if it has at least one linkable file
		if (!IsTargetValid(target))
			continue;

        buffer << "depend_" << target->GetTitle() << ": $(" << target->GetTitle() << "_DEPS)" << '\n' << '\n';
        tmp << " depend_" << target->GetTitle();
    }
    buffer << "depend:" << tmp << '\n';
    buffer << '\n';
}

void MakefileGenerator::DoAddMakefileTarget_Link(wxString& buffer)
{
    int targetsCount = m_Project->GetBuildTargetsCount();
    for (int x = 0; x < targetsCount; ++x)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
        if (!target)
            break;

		buffer << target->GetTitle() << ": " << target->GetTitle() << "-before ";
		if (IsTargetValid(target))
        {
			buffer << "$(" << target->GetTitle() << "_BIN) ";
            // add all custom-built files that do *not* link
            int filesCount = (int)m_Files.GetCount();
            for (int i = 0; i < filesCount; ++i)
            {
                ProjectFile* pf = m_Files[i];
                if (pf->useCustomBuildCommand && !pf->link)
                    buffer << pf->relativeFilename << " ";
            }
        }
		buffer << target->GetTitle() << "-after" << '\n' << '\n';

		// create target's options only if it has at least one linkable file
		if (!IsTargetValid(target))
			continue;

		buffer << "$(" << target->GetTitle() << "_BIN): " << "$(" << target->GetTitle() << "_LINKOBJS)";
        // add external deps
        wxArrayString array = GetArrayFromString(target->GetExternalDeps());
        for (unsigned int i = 0; i < array.GetCount(); ++i)
        {
            buffer << ' ' << UnixFilename(array[i]);
        }
		buffer << '\n';

        // command to create the target dir
#ifdef __WXMSW__
        buffer << "\t-@if not exist \"$(" << target->GetTitle() << "_OUTDIR)\" mkdir \"$(" << target->GetTitle() << "_OUTDIR)\"\n";
#else
		buffer << "\t-@if ! test -d $(" << target->GetTitle() << "_OUTDIR); then mkdir $(" << target->GetTitle() << "_OUTDIR); fi\n";
#endif


		// run any user-defined commands *before* build
		DoAddMakefileCommands(wxEmptyString, target->GetCommandsBeforeBuild(), buffer);
		
		// change link stage command based on target type
		switch (target->GetTargetType())
		{
			case ttConsoleOnly:
			case ttExecutable:
            {
                if (m_CompilerSet->GetSwitches().logging == clogSimple)
					buffer << '\t' << "@echo Linking executable \"" << target->GetOutputFilename() << "\"..." << '\n';
				wxString compilerCmd = ReplaceCompilerMacros(ctLinkExeCmd, "", target, "", "", "");
				buffer << '\t' << m_Quiet << compilerCmd<< '\n';
				break;
			}

			case ttStaticLib:
			{
                if (m_CompilerSet->GetSwitches().logging == clogSimple)
					buffer << '\t' << "@echo Linking static library \"" << target->GetOutputFilename() << "\"..." << '\n';
				wxString compilerCmd = ReplaceCompilerMacros(ctLinkStaticCmd, "", target, "", "", "");
				buffer << '\t' << m_Quiet << compilerCmd<< '\n';
				break;
            }

			case ttDynamicLib:
			{
                if (m_CompilerSet->GetSwitches().logging == clogSimple)
					buffer << '\t' << "@echo Linking shared library \"" << target->GetOutputFilename() << "\"..." << '\n';
				wxString compilerCmd = ReplaceCompilerMacros(ctLinkDynamicCmd, "", target, "", "", "");
				buffer << '\t' << m_Quiet << compilerCmd<< '\n';
				break;
            }
		}

		// run any user-defined commands *after* build
		DoAddMakefileCommands(wxEmptyString, target->GetCommandsAfterBuild(), buffer);
        buffer << '\n';
    }
    buffer << '\n';
}

void MakefileGenerator::ConvertToMakefileFriendly(wxString& str)
{
    if (!m_GeneratingMakefile)
        return;

    if (str.IsEmpty())
        return;

    for (unsigned int i = 0; i < str.Length(); ++i)
    {
        if (str[i] == ' ' && (i > 0 && str[i - 1] != '\\'))
            str.insert(i, '\\');
    }
    str.Replace("\\\\", "/");
}

void MakefileGenerator::QuoteStringIfNeeded(wxString& str)
{
    if (!m_GeneratingMakefile ||
        ((m_CompilerSet->GetSwitches().forceCompilerUseQuotes ||
        m_CompilerSet->GetSwitches().forceLinkerUseQuotes) &&
        str.GetChar(0) != '"'))
    {
        str = '"' + str + '"';
    }
}

void MakefileGenerator::AddCreateSubdir(wxString& buffer, const wxString& basepath, const wxString& filename, const wxString& subdir)
{
    wxChar sep = wxFileName::GetPathSeparator();
#ifdef __WXMSW__
    wxFileName d_filename_tmp = filename;
    wxFileName d_filename = d_filename_tmp.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + subdir + sep + d_filename_tmp.GetFullName();
    buffer << "\t-@if not exist ";
    buffer << "\"" << basepath << sep << d_filename.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) << ".\" ";
    buffer << "mkdir ";
    buffer << "\"" << basepath << sep << d_filename.GetPath(wxPATH_GET_VOLUME) << "\"";
#else
    wxFileName d_filename_tmp = UnixFilename(filename);
    wxFileName d_filename = d_filename_tmp.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + subdir + sep + d_filename_tmp.GetFullName();
    buffer << "\t-@if ! test -d ";
    buffer << basepath << sep << d_filename.GetPath(wxPATH_GET_VOLUME);
    buffer << "; then mkdir ";
    buffer << basepath << sep << d_filename.GetPath(wxPATH_GET_VOLUME);
    buffer << "; fi";
#endif
    buffer << "\n";
}

void MakefileGenerator::DoAddMakefileTarget_Objs(wxString& buffer)
{
    m_ObjectFiles.Clear();
    wxString tmp;
    int targetsCount = m_Project->GetBuildTargetsCount();
    for (int x = 0; x < targetsCount; ++x)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
        if (!target)
            break;

#ifdef __WXMSW__
        wxString resources;
#endif // __WXMSW__
        wxChar sep = wxFileName::GetPathSeparator();
        int filesCount = (int)m_Files.GetCount();
        for (int i = 0; i < filesCount; ++i)
        {
            ProjectFile* pf = m_Files[i];
            if (pf->compile &&
                !pf->compilerVar.IsEmpty() &&
                pf->buildTargets.Index(target->GetTitle()) >= 0 &&
                m_ObjectFiles.Index(pf) == wxNOT_FOUND)
            {
                m_ObjectFiles.Add(pf); // mark it as included in the Makefile

                wxFileName d_filename_tmp = UnixFilename(pf->GetObjName());
				wxFileName d_filename = d_filename_tmp.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + target->GetDepsOutput() + sep + d_filename_tmp.GetFullName();
                d_filename.SetExt("d");
                wxFileName o_filename_tmp = UnixFilename(pf->GetObjName());
				wxFileName o_filename = o_filename_tmp.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + target->GetObjectOutput() + sep + o_filename_tmp.GetFullName();
                // vars to make easier reading the following code
                wxString o_file = UnixFilename(o_filename.GetFullPath());
                ConvertToMakefileFriendly(o_file);
                QuoteStringIfNeeded(o_file);
                wxString d_file;
                if (m_CompilerSet->GetSwitches().needDependencies)
                {
                    d_file = UnixFilename(d_filename.GetFullPath());
                    ConvertToMakefileFriendly(d_file);
                    QuoteStringIfNeeded(d_file);
                }
                wxString c_file = UnixFilename(pf->relativeFilename);
                ConvertToMakefileFriendly(c_file);
                QuoteStringIfNeeded(c_file);
                wxString targetName = target->GetTitle();

                bool isResource = FileTypeOf(pf->relativeFilename) == ftResource;
                if (!isResource)
                {
                    if (m_CompilerSet->GetSwitches().needDependencies)
                    {
                        if (pf->autoDeps)
                        {
                            // depend rule
                            buffer << d_file << ": " << c_file << '\n';
                            if (m_CompilerSet->GetSwitches().logging == clogSimple)
                                buffer << '\t' << "@echo Calculating dependencies for \"" << pf->relativeFilename << "\"..." << '\n';
                            AddCreateSubdir(buffer, target->GetBasePath(), pf->GetObjName(), target->GetDepsOutput());
                            wxString compilerCmd = ReplaceCompilerMacros(ctGenDependenciesCmd, pf->compilerVar, target, c_file, o_file, d_file);
                            if (!compilerCmd.IsEmpty())
                                buffer << '\t' << m_Quiet << compilerCmd << '\n';
                            buffer << '\n';
                        }
                        else if (!pf->customDeps.IsEmpty())
                        {
                            // custom depend rule
                            wxString customDeps = pf->customDeps;
                            ReplaceMacros(pf, customDeps);
    
                            buffer << d_file << ": " << c_file << '\n';
                            if (m_CompilerSet->GetSwitches().logging == clogSimple)
                                buffer << '\t' << "@echo Generating dependencies for \"" << pf->relativeFilename << "\"... (custom dependencies)" << '\n';
                            buffer << '\t' << m_Quiet << customDeps << '\n';
                            buffer << '\n';
                        }
                    }
                    else
                        d_file = UnixFilename(pf->relativeFilename); // for compilers that don't need deps, use .cpp file
					
					if (pf->useCustomBuildCommand)
					{
						// custom build command
                        wxString customBuild = pf->buildCommand;
                        ReplaceMacros(pf, customBuild);
						buffer << pf->GetObjName() << ": " << d_file << '\n';
                        if (m_CompilerSet->GetSwitches().logging == clogSimple)
							buffer << '\t' << "@echo Compiling \"" << pf->relativeFilename << "\" (custom command)..." << '\n';
						buffer << '\t' << m_Quiet << customBuild << '\n';
						buffer << '\n';
					}
					else
					{	
						// compile rule
						buffer << o_file << ": " << d_file << '\n';
                        if (m_CompilerSet->GetSwitches().logging == clogSimple)
							buffer << '\t' << "@echo Compiling \"" << pf->relativeFilename << "\"..." << '\n';
                        AddCreateSubdir(buffer, target->GetBasePath(), pf->GetObjName(), target->GetObjectOutput());
						wxString compilerCmd = ReplaceCompilerMacros(ctCompileObjectCmd, pf->compilerVar, target, c_file, o_file, d_file);
						if (!compilerCmd.IsEmpty())
                            buffer << '\t' << m_Quiet << compilerCmd << '\n';
						buffer << '\n';
					}
                }
                else
                {
#ifdef __WXMSW__
                    if (pf->compile && FileTypeOf(pf->relativeFilename) == ftResource)
                        resources << UnixFilename(c_file) << " ";
#endif // __WXMSW__
                }
            }
        }
#ifdef __WXMSW__
        if (!resources.IsEmpty())
        {
            wxFileName resFile;
            resFile.SetName(target->GetTitle() + "_private");
            resFile.SetExt(RESOURCE_EXT);
            resFile.MakeRelativeTo(m_Project->GetBasePath());
            buffer << "$(" << target->GetTitle() << "_RESOURCE): " << resources << '\n';
            if (m_CompilerSet->GetSwitches().logging == clogSimple)
				buffer << '\t' << "@echo Compiling resources..." << '\n';
            wxString compilerCmd = ReplaceCompilerMacros(ctCompileResourceCmd, "", target, UnixFilename(resFile.GetFullPath()), "", "");
            if (!compilerCmd.IsEmpty())
                buffer << '\t' << m_Quiet << compilerCmd << '\n';
            /*buffer << '\t' << m_Quiet << "$(RESCOMP) -i " << UnixFilename(resFile.GetFullPath()) << " -J rc ";
            buffer << "-o $(" << target->GetTitle() << "_RESOURCE) -O coff ";

			DoAppendIncludeDirs(buffer, 0L, "--include-dir=", true);
			DoAppendIncludeDirs(buffer, 0L, "--include-dir=");
			DoAppendIncludeDirs(buffer, target, "--include-dir=");*/
			buffer << '\n';
        }
        buffer << '\n';
#endif // __WXMSW__
    }
    buffer << '\n';
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
    bool hasCmds = !target->GetCommandsAfterBuild().IsEmpty() ||
                    !target->GetCommandsBeforeBuild().IsEmpty();
	return hasCmds || m_LinkableTargets.Index(target) != -1;
}

// WARNING: similar function in DirectCommands too.
// If you change this, change that too
void MakefileGenerator::ReplaceMacros(ProjectFile* pf, wxString& text)
{
    wxFileName fname(pf->relativeFilename);
    text.Replace("$DIR", UnixFilename(fname.GetPath(wxPATH_GET_VOLUME)));
    text.Replace("$NAME", UnixFilename(fname.GetName()));
    text.Replace("$BASE", pf->GetBaseName());
    text.Replace("$OBJECT", pf->GetObjName());
    text.Replace("$FILENAME", pf->relativeFilename);
    text.Replace("\n", "\n\t" + m_Quiet);
}

bool MakefileGenerator::CreateMakefile()
{
    m_GeneratingMakefile = true;

    if (m_CompilerSet->GetSwitches().logging != clogFull)
        m_Quiet = "@";
    else
        m_Quiet = wxEmptyString;
    DoPrepareFiles();
	DoPrepareValidTargets();
	
    wxString buffer;
    buffer << "###############################################################################" << '\n';
    buffer << "# Makefile automatically generated by Code::Blocks IDE                        #" << '\n';
    buffer << "###############################################################################" << '\n';
    buffer << '\n';
    buffer << "# Project:          " << m_Project->GetTitle() << '\n';
    buffer << "# Project filename: " << m_Project->GetFilename() << '\n';
    buffer << "# Date:             " << wxDateTime::Now().Format("%c", wxDateTime::Local) << '\n';
    buffer << "# Compiler used:    " << m_CompilerSet->GetName() << '\n';
    buffer << '\n';

    DoAddMakefileVars(buffer);
    DoAddMakefileOptions(buffer);
    DoAddMakefileCFlags(buffer);
    DoAddMakefileLDFlags(buffer);
    DoAddMakefileIncludes(buffer);
    DoAddMakefileLibs(buffer);
    buffer << "###############################################################################" << '\n';
    buffer << "#         You shouldn't need to modify anything beyond this point             #" << '\n';
    buffer << "###############################################################################" << '\n';
    buffer << '\n';
#ifdef __WXMSW__
    DoAddMakefileResources(buffer);
#endif // __WXMSW__
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
    file.Write(buffer, buffer.Length());
    file.Flush();

    m_GeneratingMakefile = false;
    return true;
}
