This folder contains configuration files for libraries.
Configurations are stored in XML format and must have .xml extension.

The main skeleton of library configuration is as follows:


<library name="Library Name" global_var="Global Var Name">

    <!--... Global configuration here ...-->

    <config>
        <!--... Library configurtion 1 ...-->

    </config>

    <config description="Some description">
        <!--... Library configurtion 2 ...-->
    </config>

    <config>
        <!--... Some shared configuration ...-->
        <config>
            <!--... Library configurtion 3 ...-->
        </config>

        <config>
            <!--... Library configurtion 4 ...-->
        </config>
    </config>

    <!--... more configurations here ...-->


</library>

LibFinder assumes that each library may have different number of configurations
(static library / dll / debug version etc. ), each of these configurations is
set inside <config> </config> node. If configuration is in nested block (
inside other configuration node), it will inherit settings of all parent nodes
and global configuration. Final configurations are made from <config> nodes which
don't have sub-configurations.

Configurable options are described below.


* Filters

Filters are used to show some requirements for the library before it's detected.
All filters are located inside <filters> node in <config> one. There are following
types of filters:

  - File filter:

        file option is used to to notify that library must have specified file. File names are
        relative to base library directory. There's no need to check all files included with library.
        But given list of files must not be ambiguous. Here's example of file option from wxWidgets library:

            <file name="include/wx/wx.h"/>
            <file name="include/wx/wxprec.h"/>

   - Platform filter:

        platform filter may be used to allow specific configuration for one platform only: for example:

            <platform name="win"/>

        will allow this cofiguration (all all sub-configurations) to be added for windows hosts only.

        There are following names which can be used:

           win, windows, lin, linux, unix, un*x, mac, macosx,
           bsd, freebsd, netbsd, openbsd, darwin, solaris

        More than one platform may be specified in one filter, for example:

            <platform name="win|lin"/>

        will allow this configuration on windows and linux hosts

    - Compiler filter

        compiler filter may be used to notify that given configuration is dedicated for one compiler only.
        Similarily to platform, you may specify more than one compiler. Names of compilers are declared in
        compiler plugin. In the time of wrigting this section the full list of supported compilers was:

           cygwin, dmc, dmd, gdc, arm-elf-gcc, avr-gcc, msp430-gcc, ppc-gcc,
           tricore-gcc, icc, msvctk, msvc8, ow, sdcc, tcc, gcc, mingw(?)

        Example of compiler filter:
            <compiler name="dmc|gcc"/>

    - Executable filter:

        this filter may be used to check whether there's specified executable available on host system.
        It may be used to check whether there's some configuration-script available. For example:

            <exec name="wx-config"/>

        checks whether we can incoke wx-config which is required on unix host to fetch configuration of
        wxWidgets.

* Compiler Settings

In compiler settings section you may set configuration which will be added into current compilation options
for the compile time. All settings should be placed inside <settings> node.

There are following settings which can be added:

    - Path option

        path option is used to add one of paths for library configuration. Like in global vars, there are
        three types of paths: include, lib, obj. These paths should be global, what can be done easily by using variables (it will be describedd later). Global Variable allow only one path to be defined
        but LibFinder tries to set additional directories through CFLAGS and LFLAGS. Paths can be defined
        with following tags:

            <path include="include path"/>
            <path lib="lib path"/>
            <path obj="obj path"/>

    - Flags options

        flags option can be used to set compiler / linker flags (options). Good example is to set
        automatically linked librarues with LFLAGS and required dedfines with CFLAGS. Tag describing
        flags can be defined like in following forms (example from wxWidgets configuration):

            <flags cflags="-DHAVE_W32API_H"/>
            <flags lflags="-lwxmsw26"/>

* Pkg-config support

NOTE: Pkg-config support is not yet finished so pkg-config-related stuff won't work.

    There may be special switch dedicated to pkg-config available in <config> section.
    This switch is both filter and setting so it should be placed directly inside <config>
    node. It's main purpose is to check whether pkg-config does have configuration of
    some library and add pkg-config queries while compiling sources, for example:

    <config description="OpenGL Framework (pkg-config)">
        <pkgconfig name="libglfw"/>
    </config>

    checks whether there's libglfw available in pkg-config and will let use it when such
    configuration will be choosen.

    Such extra configuration dedicated to pkg-config entry is also generated automatically
    when name of global variable is same as pkg-config entry.

* Variables

LibFinder allow to use variables what gives more flexibility.
In order to use variable, You can use $(VARIABLE_NAME) anywhere in path, flags options and
in library name. If variable named VARIABLE_NAME is declared, it will be replace by
corresponding value.

There's one variable always defined: $(BASE_DIR) and it contains global path to library root folder
(paths of required files are relative to this base path). And it should be always used when declaring
paths for this library, like:

    <path include="$(BASE_DIR)/include"/>

Additional variables may be produced from names of directories in required files. If there is
*$(VARIABLE_NAME) instead of directory, it does mean that any directory will match here
and that variable named VARIABLE_NAME wil be set to this directory name. F.ex. :

        <file name="lib/*$(CONFIG_NAME)/msw/wx/setup.h"/>

will set variable CONFIG_NAME with names of all directories inside lib, containing msw/wx/setup.h.
(Of course, this path will be used when other file requirements are met).

If variable is declared in more than one path, is must be same in all paths, otherwise, this will
be threated like file name mismatch.

