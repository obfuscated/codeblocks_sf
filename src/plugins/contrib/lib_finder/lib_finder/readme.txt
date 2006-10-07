This folder contains configuration files for libraries.
Configurations are stored in XML format and must have .xml extension.

The main skeleton of library configuration is as follows:


<library name="Library Name" global_var="Global Var Name">

    <!--... Global configuration here ...-->

    <config>
        <!--... Library configurtion 1 ...-->
    </config>

    <config new_name="Changed library name">
        <!--... Library configurtion 2 ...-->
    </config>

    <!--... more configurations here ...-->


</library>

LibFinder assumes that each library may have different number of configurations 
(static library / dll / debug version etc. ), each of these configurations is
set inside <config> </config> node. It will inherit all global settings defined
outsidee these blocks. If there's no special configuration, LibFinder will use
global settings as main configuration.

Configurable options are described below.


* File option

file option is used to to notify that library must have specified file. File names are
relative to base library directory. There's no need to check all files included with library.
But given list of files must not be ambiguous. Here's example of file option from wxWidgets library:

    <file name="include/wx/wx.h"/>
    <file name="include/wx/wxprec.h"/>

* Path option

path option is used to add one of paths for library configuration. Like in global vars, there are
three types of paths: include, lib, obj. These paths should be global, what can be done easily by using variables (it will be describedd later). Global Variable allow only one path to be defined
but LibFinder tries to set additional directories through CFLAGS and LFLAGS. Paths can be defined
with following tags:

    <path include="include path"/>
    <path lib="lib path"/>
    <path obj="obj path"/>

* Flags option

flags option can be used to set compiler / linker flags (options). Good example is to set 
automatically linked librarues with LFLAGS and required dedfines with CFLAGS. Tag describing
flags can be defined like in following forms (example from wxWidgets configuration):

    <flags cflags="-DHAVE_W32API_H -D__WXMSW__ -DWXUSINGDLL"/>
    <flags lflags="-lwxmsw26"/>

* Variables

LibFinder allow to use variables what gives more flexibility.
In order to use variable, You can use $(VARIABLE_NAME) anywhere in path, flags options and 
in library name. If variable named VARIABLE_NAME is declared, it will be replace by
corresponding value.

There's one variable always defined: $(BASE_DIR) and it contains global path to library root folder
(paths of required files are relative to this base path). And it should be always used when declaring
paths for this library, like:

    <path include="$(BASE_DIR)/include"/>

Additional variables may be produced from names of directories in required files. If instead
of directory name there is *$(VARIABLE_NAME), it does mean that any directory here will match
and that variable named VARIABLE_NAME wil be set to this directory name. F.ex. :

        <file name="lib/*$(CONFIG_NAME)/msw/wx/setup.h"/>

will set variable CONFIG_NAME with names of all directories inside lib, containing msw/wx/setup.h.
(OF course, this path will be used when other file requirements are met).

If variable is declared in more than one path, is must be same in all paths, otherwise, this will
be threated like file name mismatch.

