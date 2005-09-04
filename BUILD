These notes are for developers wishing to build Code::Blocks from source.

The only external library needed to build Code::Blocks is wxWidgets.
You must compile wxWidgets as a DLL. You must also compile contrib/stc and contrib/xrc under the wxWidgets source dir. Refer to the build documentation in the wxWidgets sources on how to do it.

Win32 build instructions:
-------------------------
If you already have Code::Blocks installed, then just open the provided project file and hit "Compile". Else read on...
Currently Code::Blocks' provided Makefile is for use with the MinGW/Cygwin GCC. Later support will be added for more compilers.
For now, you must have MinGW/Cygwin installed in order to build Code::Blocks.

After you get hold of the Code::Blocks sources (either by downloading a source package or getting it from CVS), enter the directory where you unpacked the sources and type "make" or "mingw32-make" (depends on what compiler you have installed) without the quotes. This should build the core IDE and the plugins.
When building is done, type "make update" or "mingw32-make update". This should create a working environment for Code::Blocks, by copying needed files under folders "devel" and "output".

Working on Code::Blocks sources from within Code::Blocks!
---------------------------------------------------------
These two folders will contain the same files and directory structure and you can use the IDE from either of these two directories. This structure has been created so that you can work in Code::Blocks while editing Code::Blocks' sources ;).
Basically, you 'll be using the "output/CodeBlocks.exe" executable. Code::Blocks' project settings are such that all output goes under "devel". So you can edit Code::Blocks' sources inside Code::Blocks and, when pressing "Run", it will run the "devel/CodeBlocks.exe" executable ;). This way, you can't ruin the main executable you 're using (under "output"). When your changes satisfy you and all works well, quit Code::Blocks, run "make update" from command line and re-launch "output/CodeBlocks.exe". You 'll be working on your brand new IDE!

