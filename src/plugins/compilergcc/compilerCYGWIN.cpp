/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "compilerCYGWIN.h"

#ifdef __WXMSW__
    #include "globals_cygwin.h"
#endif // __WXMSW__

CompilerCYGWIN::CompilerCYGWIN()
    : CompilerMINGW(_("Cygwin GCC"), _T("cygwin"))
{
    m_Weight = 32;
    Reset();
}

CompilerCYGWIN::~CompilerCYGWIN()
{
}

Compiler * CompilerCYGWIN::CreateCopy()
{
    return (new CompilerCYGWIN(*this));
}

AutoDetectResult CompilerCYGWIN::AutoDetectInstallationDir()
{
#ifdef __WXMSW__
    // Only detect on Windows!!!!
    if (isDetectedCygwinCompiler())
    {
        m_MasterPath = getCygwinCompilerPathRoot();
        return adrDetected;
    }
    else
#endif // __WXMSW__
    {
        return adrGuessed;
    }
}
