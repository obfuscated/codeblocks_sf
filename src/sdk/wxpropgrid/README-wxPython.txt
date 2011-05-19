
  ************************************************

    wxPropertyGrid for wxPython

    Version:   1.4.16

      By Jaakko Salli (jmsalli@users.sourceforge.net)

  ************************************************

Installation (Windows binaries):

    Run the installer.

    Sample app test_propgrid.py can be found in one of these dirs

    %PYDIR%\Lib\site-packages\wx-2.8-msw-ansi\wx\samples

    -OR-

    %PYDIR%\Lib\site-packages\wx-2.8-msw-unicode\wx\samples

    Of course, replace 2.8 with your wxPython version.

    If your platform+Python combination is not supported by pre-built
    packages, then see bottom of this file for build instructions.


Installation (sources, mainly for POSIX systems):

    NOTE: Probably does not work on Linux, Mac OS X, etc.
          Unfortunately you will need to wait for wxPython 2.9.0 which
          should include wxPropertyGrid.

    First configure and build your wxPython. Currently it is not possible to simply
    use pre-built binaries other than with Windows.

    Unpack sources, including the wxPython archive, into a temporary directory.

    Make sure you have the following installed:
    * Python interpreter, including dev packages.
    * Patched SWIG (see wxPython build instructions page for more info).
    * libxml2 for Python (usually by installed on Linux).
    * bakefile 0.2.3 or newer is required.

    Then, in wxpython subdirectory, run 'build_for_wxpy.py propgrid'. Use --help
    argument to get information about accepted parameters.

    For instance:

    > cd <dir-where-you-downloaded-the-source-archives>
    > tar zxf wxpropgrid-1.4.6-src.tar.gz -C ~/tmp
    > tar zxf wxpropgrid-1.4.6-wxPython-src.tar.gz -C ~/tmp
    > cd ~/tmp/
    > cd wxpython
    > python build_for_wxpy.py propgrid WXDIR=~/wxWidgets/buildgtk2d

    Note how the WXDIR 'must* be the build directory (ie. the directory where
    configure was run), if you want the script to actually build the library
    and the wrappers.


Documentation:

  Included in the latest source archive, available at

  http://sourceforge.net/project/showfiles.php?group_id=133406

  It is for C++ version, but vast majority of it also applies
  to wxPython bindings.


Simple example of use (see test_propgrid.py for more):


    import wx, wx.propgrid

    ...

        pg = wx.propgrid.PropertyGrid(self, wx.ID_ANY,
                style=wx.propgrid.PG_SPLITTER_AUTO_CENTER| \
                      wx.propgrid.PG_AUTO_SORT )

        # Some extra styles are included as well. This particular one
        # causes help strings to appear as tooltips instead of
        # status bar text.
        pg.SetExtraStyle( wx.propgrid.PG_EX_HELP_AS_TOOLTIPS )

        # Bind event that is triggered on every property value change
        # Use event's GetProperty method to get the relevant PGProperty.
        pg.Bind( wx.propgrid.EVT_PG_CHANGED, self.OnPropGridChange )

        #
        # Add simple properties
        #
        # Property constructors usually have argument list like (label,name,value)
        # If name is not given, then label is used.

        pg.Append( wxpg.PropertyCategory("1 - Basic Properties") )
        pg.Append( wxpg.StringProperty("String",value="Some Text") )
        pg.Append( wxpg.IntProperty("Int",value=100) )
        pg.Append( wxpg.FloatProperty("Float",value=100.0) )
        pg.Append( wxpg.BoolProperty("Bool",value=True) )
        pg.Append( wxpg.ArrayStringProperty("ArrayString",value=['A','B','C']) )

        # EnumProperty manages set of value=int_key choices
        pg.Append( wxpg.EnumProperty("Enum","EnumProperty",['A','B','C'],[0,1,2],0) )

        # Example: Automatically fill the grid from dictionary config
        #          Property classes are auto-detected from data types.
        pg.Clear()
        pg.SetPropertyValues( config, autofill=True )

        # Example: Get dictionary d containing values of all properties as strings.
        d = pg.GetPropertyValues( as_strings=True )

        # Example: Set property attribute.
        #   See documentation (goto Modules, and then wxPropertyGrid Property Attribute Identifiers)
        #   Difference is that you use wx.propgrid.PG_XXX instead of wxPG_XXX
        pg.SetPropertyAttribute( "File", wxpg.PG_FILE_SHOW_FULL_PATH, 0 )

        # Example: Set property help string.
        pg.SetPropertyHelpString( "File", "This is a file property." )


- SetPropertyValues and GetPropertyValues work with Python objects and dictionaries.

  SetPropertyValues accepts Python dictionary or object with
  __dict__ attribute (ie. normal object).

    SetPropertyValues(dict_)

  Sets property values according to key=value pairs in dictionary or
  object dict_.

    GetPropertyValues(dict_=None, as_strings=False)

  GetPropertyValues fills a dictionary with values of all properties
  and returns it. If optional argument dict_ is given, then that
  dictionary or object is filled (otherwise empty one is created).
  If as_strings is True, then all values will be string representations
  instead of native types (useful for config and such).
  Return value is always the relevant dictionary (ie. if dict_ is object,
  then its __dict__ is returned).

    AutoFill(dict_, parent=None)

  Deletes all children of parent and then re-creates them by
  auto-detecting property to use from value types.

  If parent is not given, then root of the grid (or current page,
  if using PropertyGridManager) is used.



Notes (known limitations, differences to C++ version)
 ----------------------------------

- Following additional properties are available (previously only
  as examples in C++ sample app):
    FontDataProperty: Same as FontProperty, but with colour as well.
    PointProperty: Edits wx.Point.
    SizeProperty: Edits wx.Size.
    DirsProperty: Edits a list of strings representing file system
      paths.

- Editors are referred by their names as string. For instance,
  property's editor can be changed to TextCtrlAndButton by
  calling pg.SetPropertyEditor(property,"TextCtrlAndButton").

- wx.propgrid.PGProperty instances are used instead of
  wx.propgrid.PGId. Because of this, unlike in C++, methods
  return None instead of invalid property object, and as such,
  IsOk method cannot be used to detect whether return value
  is valid.

    Right:
        p = pg.GetFirstChild(parent)
        if p:
            ....

    WRONG:
        p = pg.GetFirstChild(parent)
        if p.IsOk():
            ....

- You can use GetPropertyValue method to return Python object
  representation of a property value. In C++ there were specific
  getters for specific values, but of those only
  GetPropertyValueAsString should be of any interest, as it is
  used to return string representation of value.

- Currently you cannot use wxPG_LABEL as name to indicate that
  value of label should be used (and frankly, it makes much
  less sense in Python). With custom properties, wx.propgrid.LABEL_AS_NAME
  should be used as a default value for name argument of
  property constructor.

- SetPropertyValue with int array does not seem to be working
  (does not crash, but doesn't set the value either).

- Property client data is a python object instead of untyped C pointer.

- Automatic converters of wxVariant -> PyObject (for instance,
  PyObjectToVariant()) return new reference, ie. they increment the
  referenc count.


    Custom Property Construction (EXPERIMENTAL!)
  ------------------------------------------------

    - Derive a custom class from wx.propgrid.PyProperty, or
      any of the derivative classes, such as wx.propgrid.PyStringProperty
      or wx.propgrid.PyEnumProperty. *You must use the version
      of the class that starts with Py*. Note that there is no Py-class
      for following properties (due to code line savings needed to
      have SWIG-generated .cpp file compile with VC7):
          CursorProperty, DateProperty, MultiChoiceProperty, FontDataProperty,
          PointProperty, SizeProperty, DirsProperty.

    - To call method of a parent class, self.CallSuperMethod("$METHOD$",
      ...args...) instead of wx.propgrid.$PARENTCLASS$.$METHOD$(...args...)
      (or anything else that achieves the same thing). Doing otherwise will
      result in an infinite recursion. So, for instance:

        Right:
            self.CallSuperMethod("CreateControls", propgrid, property, pos, sz2)

        WRONG:
            wx.propgrid.PyStringProperty.CreateControls(propgrid, property, pos, sz2)

      And yes, it is only possible to call method of immediate parent class.

    - wx.propgrid.LABEL_AS_NAME should be used as a default value for
      name argument of property constructor

    - A new variant data type type for "PyObject*" is introduced.

    - test_propgrid.py includes some sample custom properties.



    Custom Editor Construction (EXPERIMENTAL!)
  ----------------------------------------------

    - Derive a custom class from wx.propgrid.PyEditor, or
      any of the derivative classes, such as wx.propgrid.PyChoiceEditor
      or wx.propgrid.PyComboBoxEditor. *You must use the version
      of the class that starts with Py*.

    - To call method of a parent class, self.CallSuperMethod("$METHOD$",
      ...args...) instead of wx.propgrid.$PARENTCLASS$.$METHOD$(...args...)
      (or anything else that achieves the same thing). Doing otherwise will
      result in an infinite recursion. So, for instance:

        Right:
            self.CallSuperMethod("CreateControls", propgrid, property, pos, sz2)

        WRONG:
            wx.propgrid.PyStringProperty.CreateControls(propgrid, property, pos, sz2)

      And yes, it is only possible to call method of immediate parent class.

    - Before using the editor, it must be registered. Use code like this
      (pg is PropertyGrid or PropertyGridManager instance):

        pg.RegisterEditor(MyPropertyEditor())

      RegisterEditor also accepts editor name as second argument. If it
      is not given, editor's class name is used.

    - After editor has been registered, property can be set to use
      it with:

        pg.SetPropertyEditor(property, "MyPropertyEditor")

    - Following methods can be implemented. Any differences from the
      C++ version are noted.

      CreateControls(self, propgrid, property, pos, sz):
        Creates the editor controls. Returns the control, or tuple
        with two controls, of which the second is usually a wx.Button.

      For the rest, there should not be anything different - see the C++ class
      reference or propdev.h include file.

    - test_propgrid.py contains LargeImageEditor, which is a from-the-scratch
      implementation of a text editor which has a large image in front of it.
      ImageFile property is set to use this editor.



Building wxPropertyGrid for wxPython on Windows
 -----------------------------------------------
 
!!! UNDER CONSTRUCTION !!!

NOTE: This mostly applies to Windows platforms. For Linux/*nix systems, you
  probably need to rebuild the entire wxPython (see top of this document
  for more information).

REQUIRED SOFTWARE

* WINDOWS ONLY:
  Your default Visual C++ compiler must be VC 6.0 for Python 2.3,
  or VC 7.x for Python 2.4 and Python 2.5. It must be properly
  configured for Python distutils to call it (you may need to
  set environment variables DISTUTILS_USE_SDK=1 and MSSdk=<SDK_Dir>).


* Get patched SWIG.

  A) Get the right version (1.3.29 for wxPython 2.7 or newer
     1.3.27 for wxPython 2.6.3.2 and newer,
     1.3.24 for older ones):

       http://sourceforge.net/project/showfiles.php?group_id=1645

  B) Build it (requires MingW+msys or equivalent on Windows). For
     instance:

       cd $(WHEREVER_SWIG_SRC_IS)
       patch -p0 < $(WXDIR)\wxPython\SWIG\swig-1.3.29.patch
       ./configure
       make


* Get libxml2 for Python:

    Windows: http://users.skynet.be/sbi/libxml-python/
    Non-Windows users: google for it. However, there is
      a good chance that it has already been pre-installed
      on a Linux system.


* Get bakefile (optional, but recommended):

    You may need to have correct version of bakefile
    installed. Note that version may have to be
    *exactly* correct. Not all bakefile versions
    are backwards compatible.

    wxPython 2.7.1.3 and earlier: Bakefile 0.2.0
    wxPython 2.7.2 and later: Bakefile 0.2.1

    Path to bakefile is defined when running the build
    the script at the command line (see below).



* If you do not have already built wxPython from the
  sources, then either do that -OR- get yourself the
  appropriate wxPython source distribution and matching
  development archive on top of that. This will make sure
  that the correct import libraries are used.

  For details, see:

    http://www.wxpython.org/download.php



After all the above work is done, uncompress both normal and
wxPython-related source archive into same location. Then do 
following at the command prompt:

cd %WXPROPGRID%/wxpython
python build_for_wxpy.py propgrid WXDIR=<my_wx_dir> SWIG=<swig_path> BAKEFILE=<bakefile_base_dir>

Additionally, if you have both ANSI and unicode version installed,
you can define UNICODE=1 or UNICODE=0 to select which version to
target.

