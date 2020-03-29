/** @mainpage CbLauncher
 * @brief CbLauncher launches the Code::Blocks IDE for portable usage under Windows
 * @authors Biplab, Codeur
 * @version 1.0.1
 * @section intro Introduction
 * @verbinclude  README.txt
 * @section requirements
 * CbLauncher.exe must be located in the Code::Blocks installation root folder.
 * @section recommended
 * If you associate files with extensions .h .hpp .c .cc .cxx .cpp .cbp with CBLauncher.exe
 * on the Windows host machine, Code::Blocks will be portably launched and the file loaded
 * when such a file is selected in Windows Explorer.
 * @section todo [more work to be done]
 * @todo If necessary, prevent loading and later unloading of programmer fonts that
 * already exist in the system at launch.
 * @todo Harden behaviour with fonts when launcher is killed before CB exits by removing
 * the programmer fonts on entry before installing them.
 * <hr>
 * @section History
 *  v-1.0.1 April 2011
 *  - Fixed: Was not setting %AppData% when an instance of CB was already running.
 *           This was interfering when the user wanted to start multiple CB instances.
 *  - This version deals with multiple instances of CB by setting tool path and fonts for
 *    all instances of the launcher. Only unloads fonts when all instances of CB are no
 *    longer running.
 *
 *  v-1.0 March 2011
 *  - Tool path and fonts set up to only occur when Codeblocks is not already running.
 *  - Launcher stable, now tested under a variety of Windows platforms from XP onwards.
 *
 *  v-0.1.6 February 2011
 *  - Fixed: "PATH" environment variable was incorrectly set when long.
 *  - Changed all code to pure unicode strings (no TCHAR and no _T(..))
 *  - Removed --debug-log from Code::Blocks command line (not useful as launcher passes
 *    command line parameters).
 *
 *  v-0.1.5 January 2011
 *  - Fixed: Application directory incorrectly detected if launcher called by selecting a
 *    source file with the mouse (\\AppData was created in wrong directory, etc...).
 *  - A new default icon contains higher resolution icons (requested by olydbg). The cost
 *    is a significantly larger executable size.
 *  - The CBLauncher icon can now be distinguished from the Codeblocks icon.
 *  - Optionally: To reduce the executable size at the cost of a rougher icon, you can
 *    copy "lores.ico" to "launcher.ico" prior to building.
 *  - Optionally: To further reduce the launcher size, compress it with UPX.
 *  - Programmer fonts now also added from any ..\\share\\codeblocks\\fonts\\.. subdirectory.
 *    This enables placing font families and their licence files each in their own directory.
 *  - .otf (postcript OpenType fonts) now also added if present in font directories.
 *  - Some debug messages were modified.
 *  - Pre-built doxygen html documentation is now added to the distribution
 *    (see in ..Others\doc\html\index.html).
 *
 *  v-0.1.4 - November 2010
 *  - Commandline arguments are now passed on to Codeblocks.
 *  - Complete doxygen documentation
 *  - Streamlined code
 *
 *  v-0.1.3 - April 2010
 *  - Added optional portable CB tool path (enables portable programming utilities)
 *  - Launcher icon added
 *  - Console hidden in release build, visible in debug build
 *
 *  v-0.1.2 - April 2010
 *  - Added optional portable CB fonts path (optionally allows fully portable programmer
 *    fonts for the editor).
 *  - Console now hidden
 *
 *  v-0.1.1 - April 2009
 *  - Update on Biplab's blog, see http://biplab.in/2009/04/creating-portable-codeblocks-part-2/
 *
 *  v-0.1 - 5 April 2009
 *  - First release on Biplab's blog http://biplab.in/2009/04/creating-portable-version-of-codeblocks/
 * <hr>
 * @section Licence
 *  Copyright (C) 2009 - Biplab Kumar Modak
 *  Homepage: http://biplab.in
 *  Copyright (C) 2010-2011 - Codeur (codeur at taillecode dot org)
 *
 *  Portable Code::Blocks Launcher is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Portable Code::Blocks Launcher is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */
