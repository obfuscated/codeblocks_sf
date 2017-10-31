File: README.txt
CbLauncher starts the Code::Blocks IDE in a portable manner under Windows.
This is achieved the following way:

Install: CbLauncher.exe must be located in the Code::Blocks installation root folder
together with codeblocks.exe. It's role is to manage the start-up of Code::Blocks.
"CbLauncher.exe" is started by the user instead of "codeblocks.exe".
- On your home machines, where Code::Blocks is the main C/C++ IDE, it is a good idea to
associate .c, .cpp, .cxx, .h, .hpp, .cbp files with the CbLauncher. Double clicking on
a file with one of these extensions in Explorer will open the file or project within
Code::Blocks after launching the IDE.

When CbLauncher.exe is executed:
The launcher performs some "housekeeping work" and only then starts "codeblocks.exe".
It does not noticeably slow down the IDE start-up.

1- APPDATA Environment Variable
The APPDATA environment variable is set to point to the "AppData" subdirectory
of CodeBlocks. CodeBlocks, its plugins, and programs started from CB will write
their ini files and other data there, rather than in the user's "documents and
settings" of the host machine.

2- Programming Fonts
If programmer (ttf) fonts have been located in the "share\CodeBlocks\fonts" subdirectory
of the CodeBlocks installation, or subdirectories of that "fonts" directory, they are
added to the system fonts by the launcher. This enables access to reasonable programming
fonts on any machine where portable CB is used.
- Place your preferred font in the "fonts" directory given above, or a subdirectory.

* The font "DejaVu sans mono" (http://dejavu-fonts.org) is frequently selected.

* "Inconsolata", developed by Ralph Levien is available properly hinted for Windows on
the Google Font Repository (under the SIL Open Font License):
http://code.google.com/webfonts/family?family=Inconsolata&subset=latin
A very legible and elegant fixed-width font, Inconsolata is a brilliant programming font.
However its slanted single and double quotes look silly in code. The same font, but with
straight quotes (Inconsolata-sq.ttf) is available in the "fonts" subdirectory of the
CBLauncher project. It is version 001.011. Beware of previous versions that do not work
well with Windows.

* "Anonymous Pro" is a family of four fixed-width fonts designed especially with coding in
mind. Characters that could be mistaken for one another (O, 0, I, l, 1, etc.) have distinct
shapes to make them easier to tell apart in the context of source code.
Anonymous Pro also features an international, Unicode-based character set, with support
for most Western and European Latin-based languages, Greek, and Cyrillic. It also includes
special "box drawing" characters.
It is available (also under the SIL Open Font License) on the Google Font Repository:
http://code.google.com/webfonts/family?family=Anonymous+Pro&subset=latin

for a wide choice of fonts, see the discussion on preferred  programmers fonts at
http://hivelogic.com/articles/top-10-programming-fonts.

3- Third Party Development Tools
If applications used by CodeBlocks "Tools" or applications used by "Plugins" are
located in a subfolder called "tool" of the CodeBlocks base directory or in
subfolders of "tool", then the "tool" directory and its subdirectories are added to
the system path prior to launching Code::Blocks. If a "tool" subdirectory itself
contains a "bin" subfolder where the tool executables are located, the "bin"
subfolder is added to the path instead.
- This provides a way of carrying around the whole CodeBlocks development
environment, including 3rd party tools.
Handy development tools to have around, together with their associated plugins if
they exist, include AStyle, Cppcheck, cccc, doxygen and Graphviz, resedit,
MS dependency walker, UPX, etc...

4- Cleaning up
When Code::Blocks terminates, the launcher removes the portable CodeBlocks fonts if
they had been added to the system fonts. It then exits.

_____________________________________________________________________________________

DEBUG and RELEASE builds:
The "debug" version of CbLauncher displays various diagnostic messages on a console.
The "release" version of CbLauncher does not show a console (compiled on MinGW with
linker option -mwindows).


