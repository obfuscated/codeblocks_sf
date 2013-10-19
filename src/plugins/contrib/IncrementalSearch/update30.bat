@echo off
rem
rem This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
rem http://www.gnu.org/licenses/gpl-3.0.html
rem
rem Copyright: 2008 Jens Lody
rem
rem $Revision: 7443 $
rem $Id: update.bat 7443 2011-09-01 16:30:16Z mortenmacfly $
rem $HeadURL: https://jenslody@svn.berlios.de/svnroot/repos/codeblocks/trunk/src/plugins/contrib/IncrementalSearch/update.bat $
rem

md ..\..\..\devel30\share\CodeBlocks\images\settings > nul 2>&1
md ..\..\..\output30\share\CodeBlocks\images\settings > nul 2>&1
copy .\*.png ..\..\..\devel30\share\CodeBlocks\images\settings\ > nul 2>&1
copy .\*.png ..\..\..\output30\share\CodeBlocks\images\settings\ > nul 2>&1
exit 0

