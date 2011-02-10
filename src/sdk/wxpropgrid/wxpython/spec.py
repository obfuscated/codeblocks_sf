#
# wxPropertyGrid project specifications
#

projname = 'propgrid'
longprojname = 'wxPropertyGrid'
branch = 'stable'  # Must be either '', 'stable' or 'dev'
version = '1.4.15'
author = 'Jaakko Salli'
author_email = 'jmsalli@users.sourceforge.net'
homepage = 'http://wxpropgrid.sourceforge.net/'

# Set to True to pregenerate makefiles for all supported versions, if possible.
# If False, only makefiles for the latest version are pregenerated.
pregen_all_makefiles = True

# Contents of each sub-directory listed here will be placed into a separate archive.
separate_dir_archives = ['wxpython']

# Version number entry format: (filename,identifier,format)
#   In format, A is major version, B is minor version, C is release, and D
#   sub-release.
# NOT YET IMPLEMENTED!
auto_version_numbers = [('propgrid.h','wxPG_VERSION','ABC0')]

# List of supported wxWidgets versions
# Let's only supply makefiles for the two latest versions of the dev branch.
# After new stable branch comes out,
wx_version_list = ['2.6.4', '2.8.10']

required_wx_libs = ['base','core','adv']

sample_additional_required_wx_libs = ['xml','html','xrc']

lib_exclude_source_files = ['xh_propgrid.cpp']

sample_additional_source_files = ['src/xh_propgrid.cpp']

# File list - tuples have format (subdir,filespecs)
#   subdir: directory relative to contrib or components
#   filespec: list of strings. May be filespecs understood by fnmatch, or special
#     symbols (currently only $makefiles is supported).
#   * Can have format filespec>>target_dir, where the target_dir is the alternate location
#     of file in the distribution.
#   * both subdir and filespecs understand that %(projname)s is the short project name.
#   * subdirs support $(dirname), where dirname is src, build, docs/html, wxPython etc.:
#     for contrib mode, it is translated to dirname/projname and for component mode
#     it is translated to projname/dirname. System is somewhat smart, so certain special
#     cases are handled differently(ie. more correctly).
dir_infos = [
    #( '$()', ['*.txt','*.html','install-%(projname)s.sh>>..','uninstall-%(projname)s.sh','wxcdist_config.py>>wxPython'] ),
    ( '$()', ['*.txt','*.html','*.in','configure'] ),
    ( '$(build)', ['*.bkl','*.bkgen','$makefiles','*.sh','*.m4','*.ac','*.guess','*.sub','GNUMakefile'] ),
    ( '$(build_wx26)', ['$makefiles'] ),
    ( '$(lib)', ['*.txt'] ),
    ( '$(locale)', ['*.po', '*.pot', '*.mo'] ),
    ( '$(docs)', ['Doxyfile'] ),
    ( '$(docs/html)', ['*.htm','*.html','*.css','*.jpg','*.gif','*.png'] ),
    ( '$(include/wx/propgrid)', ['*.h'] ),
    ( '$(samples)', ['*.cpp','*.h','*.bkl','*.xpm','*.ico','$makefiles','*.sh','*.xrc','*.rc'] ),
    ( '$(src)', ['*.cpp','$makefiles'] ),
    ( '$(patch_wx26)', ['*.txt'] ),
    ( '$(patch_wx26/include/wx)', ['*.h'] ),
    ( '$(patch_wx26/src/common)', ['*.cpp'] ),
    ( '$(distscript)', ['libwxcdist.py>>wxPython','build_for_wxpy.py>>wxPython'] ),
    ( '$(wxpython)', ['*.i','*.py','*.cpp','*.h','*.txt','*.bkl'] ),
    ]


extra_uninstall = [
#'distscript/libwxcdist.py',
#'distscript/build_for_wxpy.py',
#'distscript',
#'$(wxPython)/custom_build.py',
#'$(wxPython)',
#'wxPython/build_for_wxpy_config.py',
#'wxcdist_config.py',
]


#
# Extra functionality for building the wxPython bindings
#

"""\

Most locals from build_for_wxpy.py main() are available here as well. No import required.

projname: Project name.
wx_dir_contrib: Contrib dir with the component source distribution.
target_contrib: Target contrib dir (may be different from source contrib dir).

opj: os.path.join
copyfile: os.copyfile
copytree: os.copytree

"""

define_wxpython_injection = "// Temporary define -- REMOVE IF PERSISTS --\n#undef __WXPYHON__\n#define __WXPYTHON\n\n"


def wxpy_before_build():
    """\
    Called just before building the DLL or SO.
    """

    print ". copying sample properties into main library"
    extra_source_file = 'sampleprops'
    src_path = opj(full_samples_dir,extra_source_file)
    dst_path_src = opj(target_contrib,'src',projname,'extras')
    dst_path_inc = opj(target_contrib,'include','wx',projname,'extras')
    copyfile( src_path+'.cpp', dst_path_src+'.cpp' )
    copyfile( src_path+'.h', dst_path_inc+'.h' )


def wxpy_after_build():
    """\
    Called after the build process has finished.
    """
    pass


def wxpy_get_extra_swig_args():
    """\
    Return extra parameters needed by SWIG.
    """
    return ["-noextern"]


def wxpy_get_culled_swig_args():
    """\
    Returns list of parameters which should *not* be passed to SWIG.
    """
    #return ["-fvirtual"]
    return []


def wxpy_get_extra_redist_files():
    """\
    Returns list of file names (relative to contrib base path) that should be added
    to the redistributable. File name can be tuple so that the first item is
    source and second destination.
    """
    return [(opj('README-wxPython.txt'), opj('README-wxPropertyGrid.txt')),
            (opj('wxpython','test_propgrid.py'),opj('PY_WX_LIB','samples','test_propgrid.py'))
           ]

