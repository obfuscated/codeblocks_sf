 
 wxChart ReadMe
 -------------
 
 Website: http://wxcode.sourceforge.net/components/wxchart
 Author: Paolo Gava
 Version: 1.0 
 Description:
 [description of this component]
 
 

 Installation - win32
 --------------------
 
 When building on win32, you can use the makefiles in the BUILD folder using the
 WX_DIR, WX_DEBUG, WX_UNICODE and WX_SHARED to match your wxWidgets build.
 Examples:
 
   > nmake -fmakefile.vc WX_DIR=c:\path\to\my\wxWidgets WX_UNICODE=0/1 WX_DEBUG=0/1 WX_SHARED=0/1
   > make -fmakefile.bcc WX_DIR=c:\path\to\my\wxWidgets WX_UNICODE=0/1 WX_DEBUG=0/1 WX_SHARED=0/1


     
 Installation - unix
 -------------------

 When building on a Linux/GNU-based system, type

  cd build
  ./configure
  make

 to build against the default build of wxWidgets (the one listed as 'default' 
 by wx-config --list). Type "./configure --help" for more info.



 ChangeLog
 ---------
 
 0.1 - first version of wxChart at wxCode

 0.2 - This is mainly a bug fixes release.
	The only new feature is the possibility to write the chart to file JPEG, GIF...


 
 
 [optionally: Known bugs, Author's info, ... ]

