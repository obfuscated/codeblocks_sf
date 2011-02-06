These notes are for developers wishing to build Code::Blocks from source.

The only external library needed to build Code::Blocks is wxWidgets.
You must compile wxWidgets as a monolithic DLL. Refer to the build documentation in the wxWidgets sources on how to do it.

Win32 build instructions:
-------------------------
To build these sources, you must have a recent Code::Blocks nightly build installed (http://www.codeblocks.org/nightly).
Open the src/CodeBlocks.cbp and hit "Build". This should build the IDE and the core plugins.
Close the project now and open src/ContribPlugins.workspace. Hit "Build". This builds all the contributed plugins.

When you 're done with the above, close Code::Blocks and run src/update.bat. This creates a working environment for Code::Blocks, by copying needed files under folders "devel" and "output".

Unix build instructions:
------------------------
You need a working autotools environment (autoconf, automake, libtool, make, etc).
In a terminal, go to the top level folder.
If you fetched the sources from SVN, you need to bootstrap the program first. So type:

./bootstrap

This will adapt the project's configuration file to your environment. This only needs to be done once: the first time you checkout the SVN version.

After this, type the following:

./configure --with-contrib-plugins=all
make
make install

For the last step you must be root.
That's it.

or:

./configure --prefix=/usr --with-contrib-plugins=all,-help
make
make install

"all" compiles all contrib plugins
"all,-help" compiles all contrib plugins except the help plugin
By default, no contrib plugins are compiled
Plugin names are: AutoVersioning, BrowseTracker,byogames,Cccc,CppCheck,cbkoders,codesnippets,
		     codestat, copystrings, Cscope, DoxyBlocks, dragscroll, EditorTweaks, envvars,
		     FileManager, headerfixup, help, hexeditor, incsearch, keybinder, libfinder, MouseSap,
		     NassiShneiderman, profiler, regex, ReopenEditor, exporter, symtab, ThreadSearch,
		     ToolsPlus, Valgrind, wxsmith, wxsmithcontrib,wxsmithaui

Working on Code::Blocks sources from within Code::Blocks!
---------------------------------------------------------
The following apply for all platforms where you have Code::Blocks installed and working.

These two folders will contain the same files and directory structure and you can use the IDE from either of these two directories. This structure has been created so that you can work in Code::Blocks while editing Code::Blocks' sources ;).
Basically, you 'll be using the "output/CodeBlocks.exe" executable. Code::Blocks' project settings are such that all output goes under "devel". So you can edit Code::Blocks' sources inside Code::Blocks and, when pressing "Run", it will run the "devel/CodeBlocks.exe" executable ;). This way, you can't ruin the main executable you 're using (under "output"). When your changes satisfy you and all works well, quit Code::Blocks, run "make update" from command line and re-launch "output/CodeBlocks.exe". You 'll be working on your brand new IDE!
