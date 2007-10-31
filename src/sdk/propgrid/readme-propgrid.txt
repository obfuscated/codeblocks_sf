
  ************************************************

    wxPropertyGrid

      By Jaakko Salli (jmsalli79@hotmail.com)
                      (jaakko.salli@users.sourceforge.net)

  ************************************************

    Version:   1.2.10
    Requires:  wxWidgets 2.6.0 or later
               ( 1.0.x branch is also compatible with wxWidgets 2.5.2 - 2.6.2 )
    Platforms: Windows - OK; Linux GTK 2.0 - OK;
               Linux GTK 1.2, Mac OS X, Windows CE - builds and runs. Other platforms remain untested.
    License:   wxWidgets license

    Home Pages:     http://wxpropgrid.sourceforge.net/
                    http://www.geocities.com/jmsalli/propertygrid/index.html
                    At SourceForge: http://sourceforge.net/projects/wxpropgrid/

    wxPropertyGrid is a specialized two-column grid for editing properties
    such as strings, numbers, flagsets, fonts, and colours. wxPropertySheet
    used to do the very same thing, but it hasn't been updated for a while
    and it is currently deprecated.

    wxPropertyGrid is modeled after .NET propertygrid (hence the name),
    and thus features are similar.


  HOW TO INSTALL AND BUILD
  ------------------------

  NOTE: For wxPython instructions and notes, see README-propgrid-wxPython.txt.

  WINDOWS

  Run the installer. wxPropertyGrid will be installed as a contrib.

  Installer uses the NSIS-system (by Nullsoft) and it is able to auto-detect wxWidgets
  dir from WXWIN and WXDIR environment variables, plus from dir stored to registry by
  wxWidgets installer. wxWidgets version is auto-detected from include/wx/version.h
  and only the relevant makefiles are copied.

  Altough I have tried to test the installer well enough, there may still be problems
  at this point. If you find any, then please report them.


  !!! COMPILER SPECIFIC NOTES !!!

  MINGW. Using configure to build is recommened over using makefile.gcc directly
    (see wxWidgets install.txt file for details).

  VISUAL STUDIO 2005 (VC8). Various problems have been reported with this IDE.
     Here are potential solutions to some of them.

     Problem: File XXX.pch is missing.
     Fix:     Change the "Use precompiled header" option to
              "Create precompiled header" and build the solution.

     Problem: Where did the lib file go? It's not in wxWidgets\lib.
     Fix:     Try C:\lib\vc_lib. If it cannot found from there, you can of course
              search your file system.

 
  POSIX SYSTEMS (Linux, OS X, MingW+MSys, etc.)

  Extract the tar.gz archive to a temporary directory and run install-propgrid.sh.
  Note that you need to have setup wxWidgets using configure.

  For example:

  > cd <dir-where-you-downloaded-the-source-archive>
  > tar zxf wxpropgrid-1.2.8-src.tar.gz -C ~/tmp
  > cd ~/tmp
  > bash install-propgrid.sh
    - OR -
  > bash install-propgrid.sh WXDIR BUILDSUBDIR

  As a result:

  "$WXDIR/$BUILDSUBDIR/contrib/src/propgrid" has library Makefile
  "$WXDIR/$BUILDSUBDIR/contrib/samples/propgrid" has sample app Makefile

  NOTE: Install script should be compatible with ash as well as bash, but
    still I recommend using bash where possible.

  NOTE: If you encounter "lone zero block" error when extracting files
    with tar, then be aware that it is actually only a warning and doesn't
    mean that the archive is corrupt.

  NOTE: "make install" may copy propgrid include files to include/wx instead
    of include/wx/propgrid. Right now you will have to manually copy the
    files to the correct location.

  NOTE: You may want to manually add propgrid to CONTRIB_SUBDIRS list in
  "$WXDIR/contrib/src/Makefile.in" and
  "$WXDIR/contrib/samples/Makefile.in"


  DOCUMENTATION
  -------------

  See contrib/docs/html/propgrid/index.htm

  for documentation generated with doxygen. There should also be a shortcut
  index-propgrid.html at $WXDIR/contrib.


  ACKNOWLEDGEMENTS
  ----------------

    Following people have provided plenty of helpful feedback:
      Vladimir Vainer, Andrew B, Chris Garrett, Dave Moor, Bernard Duggan,
      Joachim Hönig.

    Also thanks to Julian Smart, Vadim Zeitlin, Robert Roebling, Robin Dunn,
    et al. for wxWidgets itself!


  INSTALLATION TROUBLESHOOTING
  ----------------------------

  Script:

  In the likely case that my script doesn't take your system-configuration
  into account, then you can just manually copy files from the archive. Just copy
  contrib/build/propgrid to $WXDIR/contrib/build, contrib/include/wx/propgrid to
  $WXDIR/contrib/include/wx, and so on. If you are using wxWidgets version earlier
  than 2.5.4 (or whatever the propgrid docs say is the most recent supported),
  then you will need to replace Makefile.in files with
  Makefile.<your-version>.in files (located in contrib/src/propgrid and
  contrib/samples/propgrid).

  LIBRARY VERSION NOTE

  There are no makefiles supplied for wxWidgets versions other than those reported
  on the propgrid homepage - in this case you will need to get your hands on
  bakefile and run bakefile_gen in $WXDIR/build/bakefiles/.

  INSTALLING TO A DIFFERENT SUB-DIRECTORY

  If you do not wish to install into 'contrib' directory, then you can uncompress the
  archive into a temporary directory first, rename the base dir, and copy it into
  wxWidgets root (or enter something else than 'contrib' when running install script).
  Supplied (Windows) makefiles and project files should still work, but if you wish
  for configure script to generate a corresponding makefile, then you need to add your
  dir to the following line in configure (install script also tells you to do this):

  SUBDIRS="samples demos utils contrib"

  (it is line 41397 in 2.5.3).

  If you need to (re)generate makefiles to a non-contrib directory, run bakefile_gen,
  as instructed above, but be sure to have modified 'Bakefiles.bkgen' so that it contains
  a version of every directive intended for 'contrib' for your dir as well (to make it
  simple, replace all instances of 'contrib' with name of your dir - but since this
  disables makefile generation for actual contrib dir, you might want to backup the file
  first).
