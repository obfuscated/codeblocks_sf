WARNING: Outdated

This Code::Blocks plugin was created to automatically find libraries installed in system.
Each library is represented as global variable. If this global variable is configured by
this plugin, all necessary steps to use such library is to:

  - add $(#GLOBAL_VAR_NAME.include) in include directories
  - add $(#GLOBAL_VAR_NAME.lib) in lib include directories
  - add $(#GLOBAL_VAR_NAME.obj) in lib and resoure compiler directories
  - add $(#GLOBAL_VAR_NAME.cflags) in other compiler options
  - add $(#GLOBAL_VAR_NAME.lflags) in other linker options
  (where GLOBAL_VAR_NAME should be changed to variable associated with given library,
   f.ex. WX will be used for wxWidgets, cb for Code::Blocks SDK. There's no rule describing
   how these global variables are named. But it should be short and easy to guess ;)

These variables define all necessary directories and libraries needed during linkage.

WARNING:
Because C::B allow only one directory for include / lib / obj paths, LibFinder will try to simulate
others by adding them to LFLAGS and CFLAGS


Libraries are configured in xml files (see lib_finder/README.txt for more details).
LibFinder will search for these files in $(CB_DIR)/share/CodeBlocks/lib_finder.

There are 3 libraries currently supported - wxWidgets (only wx 2.6 monolithic dll version),
c::B sdk and OpenGL Framework.
