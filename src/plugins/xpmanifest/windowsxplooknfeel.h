/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef WINDOWSXPLOOKNFEEL_H
#define WINDOWSXPLOOKNFEEL_H

#include "cbplugin.h" // the base class we 're inheriting

#ifndef __WXMSW__
    #error This plugin is for use under Win32 only!
#endif

class WindowsXPLookNFeel : public cbToolPlugin
{
	public:
		WindowsXPLookNFeel();
		~WindowsXPLookNFeel();
		int Execute();
};

#endif // WINDOWSXPLOOKNFEEL_H

