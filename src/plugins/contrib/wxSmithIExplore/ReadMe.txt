/*
* This file is part of a wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith and this file is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
* 
* Ron Collins
* rcoll@theriver.com
* 4-Feb-2010
* 
*/



=============
wxIEHtmlWin
=============

-------------------
General Description
-------------------

wxIEHtmlWin is a MS Windows-only Active-X control that implements the MS
Internet Explorer as a widget.  This allows you to embed a web-browser or an
HTML-display within your application.

You should be familiar with MS Internet Explorer to get the most efficient use
of wxIEHtmlWin, but even the simple use of "LoadUrl()" will allow a novice to
find and load a web-page.



The wxIEHtmlWin build directory has the following structure:
IEHtmlWin\
    bin\                // this dir contains the binary file output of the 
                        // Code::Blocks projects; files include "IETest.exe",
                        // "wxsSmithButton.dll",  "wxsSmithIExplore.zip"
    images\             // empty for this implementation         
    include\            // the include file "IEHtmlWin.h" is here
    lib\                // the library "libIExplore.a" is here
    obj\                // intermediate files for all compiles
    project\            // this is the Code::Bocks project to create the
                        // wxIEHtmlWin library; it creates the file
                        // "libIExplore.a" and places the library file and
                        // the include file in the proper directories 
    src\                // source files for component wxIEHtmlWin
    test\               // this is the Code::Blocks project to create a test
                        // program for wxIEHtmlWin; the output file
                        // "IETest.exe" is placed in the ..\bin\ directory
        wxsmith\        // used by Code::Blocks and wxSmith
    wxs\                // this is the Code::Blocks project to create the
                        // plugin for Code::Blocks and wxSmith, to put the
                        // component wxIEHtmlWin on the wxSmith palette.
                        // the output files "wxsSmithIExplore.dll" and
                        // and "wxsSmithIExplore.zip" are placed in the ..\bin\
                        // directory; they must be manually copied to the
                        // plugin directory of your Code::Blocks installation.
        images\         // images used on the wxSmith palette 
        src\            // source code for the property pages used by
                        // wxSmith for the wxIEHtmlWin component

The Code::Blocks projects used here were created and tested under Windows XP,
using Code::Blocks SVN version 6168, and wxWidgets version 2.8.10 and compiler
TDM GCC 4.4.1.  The correct order to build these projects is:

1) Build the library (project found in the "project\" directory)
2) Build the wxSmith plugin (project found in the "wxs\" directory)
3) Exit Code::Blocks and install the plugins
   copy "bin\wxsSmithIExplore.dll" to "<Code::Blocks>\share\CodeBlocks\plugins"
   copy "bin\wxsSmithIExplore.zip" to "<Code::Blocks>\share\CodeBlocks"
4) Re-start Code::Blocks and build the test program (project found in the
   "test\" directory)

(For Windows XP, all of these projects and programs are already compiled; you
only need to install the plugin files into your own Code::Blocks installation
directory).

---------------------   
Component Description
---------------------

The wxIEHtmlWin component encapsulates a MS Windows Active-X control for
Internet Explorer.  wxIEHtmlWin was originally written by Justin Bradford,
but is no longer actively supported.  The component can be found on the
wxWidgets "wxCode" page at http://wxcode.sourceforge.net/index.php.

Member functions and supported events are pretty much self-explanatory:

Functions
---------

    void     LoadUrl(const wxString&);
    bool     LoadString(wxString html);
    bool     LoadStream(istream *strm);
    bool     LoadStream(wxInputStream *is);

    void     SetCharset(wxString charset);
    void     SetEditMode(bool seton);
    bool     GetEditMode();
    wxString GetStringSelection(bool asHTML = false);
    wxString GetText(bool asHTML = false);

    bool     GoBack();
    bool     GoForward();
    bool     GoHome();
    bool     GoSearch();
    bool     Refresh(wxIEHtmlRefreshLevel level);
    bool     Stop();



Events
------

    EVT_MSHTML_BEFORENAVIGATE2
    EVT_MSHTML_NEWWINDOW2
    EVT_MSHTML_DOCUMENTCOMPLETE
    EVT_MSHTML_PROGRESSCHANGE
    EVT_MSHTML_STATUSTEXTCHANGE
    EVT_MSHTML_TITLECHANGE



  