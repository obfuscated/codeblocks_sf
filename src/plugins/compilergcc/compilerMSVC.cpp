#include "compilerMSVC.h"

CompilerMSVC::CompilerMSVC()
    : Compiler(_("Microsoft Visual C++"))
{
	m_MasterPath = "C:\\Program Files\\Microsoft Visual Studio .NET\\Vc7";
	
	m_Programs.C = "cl.exe";
	m_Programs.CPP = "cl.exe";
	m_Programs.LD = "link.exe";
	m_Programs.WINDRES = "rc.exe";
	m_Programs.MAKE = "mingw32-make.exe";
	
	m_Switches.includeDirs = "/I";
	m_Switches.libDirs = "/L";
	m_Switches.linkLibs = "/l";
	m_Switches.defines = "/D";
	m_Switches.genericSwitch = "/";
	
	m_Options.AddOption(_("Produce debugging symbols"),
				"/Zi",
				_("Debugging"),
				"",
				true, 
				"/Og /O1 /O2 /Os", 
				_("You have optimizations enabled. This is Not A Good Thing(tm) when producing debugging symbols..."));
	m_Options.AddOption(_("Enable all compiler warnings"), "/Wall", _("Warnings"));
	m_Options.AddOption(_("Enable global optimization"), "/Og", _("Optimization"));
	m_Options.AddOption(_("Maximum optimization (no need for other options)"), "/Ox", _("Optimization"));
	m_Options.AddOption(_("Minimize space"), "/O1", _("Optimization"));
	m_Options.AddOption(_("Maximize speed"), "/O2", _("Optimization"));
	m_Options.AddOption(_("Favor code space"), "/Os", _("Optimization"));
	m_Options.AddOption(_("Favor code speed"), "/Ot", _("Optimization"));

    m_Commands[(int)ctCompileObjectCmd] = "$compiler $options $includes /c $file /o $object";
    m_Commands[(int)ctCompileResourceCmd] = "$(RESCOMP) -i $file -J rc -o $resource_output -O coff $includes";
    m_Commands[(int)ctLinkExeCmd] = "$(LD) $libdirs /o $exe_output $link_objects $libs";
    m_Commands[(int)ctLinkDynamicCmd] = "$(LD) -shared -Wl,--output-def=$def_output -Wl,--out-implib=$static_output -Wl,--dll $libdirs $link_objects $libs -o $dynamic_output";
    m_Commands[(int)ctLinkStaticCmd] = "ar -r $output $link_objects\n\tranlib $static_output";
}

CompilerMSVC::~CompilerMSVC()
{
	//dtor
}

Compiler * CompilerMSVC::CreateCopy()
{
    return new CompilerMSVC(*this);
}
