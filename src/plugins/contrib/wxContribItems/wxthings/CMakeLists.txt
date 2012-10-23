# ---------------------------------------------------------------------------
# CMake build file
# ---------------------------------------------------------------------------

# ---------------------------------------------------------------------------
# Project name, sets ${wxThings_SOURCE_DIR} and ${wxThings_BINARY_DIR}

project( wxThings )

# ===========================================================================
# General settings for CMake
# ===========================================================================

# ---------------------------------------------------------------------------
# Use a minimum version of CMake of 2.8, >= 2.8.3 is prefered

cmake_minimum_required( VERSION 2.8 )

# ---------------------------------------------------------------------------
# Setup the CMake environment

include( build/CMakewxAppLib.cmake )
include( build/CMakeFunctions.txt )

# Specify what wxWidgets libs we need to link to. Note: 'core' must be before 'base'.
# If you call this CMakeLists.txt from another one you may have already called FIND_WXWIDGETS()
if (NOT DEFINED wxWidgets_COMPONENTS)
    set(wxWidgets_COMPONENTS adv core base) # minimal set for static lib/dll
    #set(wxWidgets_COMPONENTS gl xrc xml net media propgrid richtext aui stc html adv core base) # for multilib/dll
    #set(wxWidgets_COMPONENTS stc mono) # for monolithic
endif()

FIND_WXWIDGETS(wxWidgets_COMPONENTS) # Ok to call multiple times
# wxWidgets include (this will do all the magic to configure everything)
include( "${wxWidgets_USE_FILE}" )

# ===========================================================================
# Add subdirectories containing CMakeLists.txt files or specify projects
# ===========================================================================

set( wxThings_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Root dir of wxThings" FORCE)

# ---------------------------------------------------------------------------
# CMake build file for wxThings Library
# ---------------------------------------------------------------------------

if (NOT TARGET wxThingsLib)

set( wxThings_VERSION         "1.0.0" )
set( wxThings_MAJOR_VERSION   "1" )
set( wxThings_MINOR_VERSION   "0" )
set( wxThings_RELEASE_VERSION "0" )

# ---------------------------------------------------------------------------

ADD_CPPCHECK_TEST(wxThings_CppCheck)

set( DOXYGEN_PROJECT_NAME        "wxThings" )
set( DOXYGEN_PROJECT_NUMBER      "${wxThings_VERSION}" )
set( DOXYGEN_OUTPUT_DIRECTORY    "${CMAKE_BINARY_DIR}/doc-wxThings" )
set( DOXYGEN_STRIP_FROM_PATH     "${wxThings_ROOT_DIR}/include" )
set( DOXYGEN_STRIP_FROM_INC_PATH "${wxThings_ROOT_DIR}/include" )
set( DOXYGEN_INPUT               "${wxThings_ROOT_DIR}/include" )
set( DOXYGEN_FILE_PATTERNS       "*.h *.hpp" )
set( DOXYGEN_PREDEFINED          "${DOXYGEN_PREDEFINED_WXWIDGETS}  WXDLLIMPEXP_DATA_THINGS(x)=x")

ADD_DOXYGEN( wxThings_doxygen
             ${wxThings_ROOT_DIR}/build/CMake-doxygen.in
             ${CMAKE_BINARY_DIR}/doxygen_wxThings.cfg
             ${CMAKE_BINARY_DIR} )

# ---------------------------------------------------------------------------


include_directories( ${wxThings_ROOT_DIR}/include/ )

# We use some functions from GTK directly
if ("${wxWidgets_PORTNAME}" STREQUAL "gtk2")
    find_package(GTK2)

    if (NOT GTK2_FOUND)
        MESSAGE(WARNING "Unable to find GTK2, you might need to install the gtk2-devel package")
    endif()

    include_directories( ${GTK2_INCLUDE_DIRS} )
elseif ("${wxWidgets_PORTNAME}" STREQUAL "gtk")
    find_package(GTK)

    if (NOT GTK_FOUND)
        MESSAGE(WARNING "Unable to find GTK, you might need to install the gtk-devel package")
    endif()

    include_directories( ${GTK_INCLUDE_DIRS} )
endif()

# ---------------------------------------------------------------------------
# wxFileBrowser uses /wx/generic/filedlgg.h which is not available in MSW.
# For MSW DLLs we need to replace WXDLLEXPORT with WXDLLIMPEXP_THINGS in
# wx/include/wx/generic/filedlgg.h

if (BUILDING_DLLS)

    # Use configure since it'll create the dirs for us too.
    configure_file(${wxWidgets_ROOT_DIR}/include/wx/generic/filedlgg.h
                   ${CMAKE_CURRENT_BINARY_DIR}/include/wx/generic/filedlgg.h_ COPYONLY)

    # Include this dir first so our filedlgg.h is found first.
    include_directories( BEFORE ${CMAKE_CURRENT_BINARY_DIR}/include/ )

    file(READ ${wxWidgets_ROOT_DIR}/include/wx/generic/filedlgg.h WXFILEDLGG_H_STRING)


    # wxWidgets 2.9

    string(REPLACE "class WXDLLIMPEXP_CORE wxGenericFileDialog"
                   "class WXDLLIMPEXP_THINGS wxGenericFileDialog"
                   WXFILEDLGG_H_STRING "${WXFILEDLGG_H_STRING}")

    string(REPLACE "class WXDLLIMPEXP_CORE wxFileData"
                   "class WXDLLIMPEXP_THINGS wxFileData"
                   WXFILEDLGG_H_STRING "${WXFILEDLGG_H_STRING}")

    string(REPLACE "class WXDLLIMPEXP_CORE wxFileCtrl"
                   "class WXDLLIMPEXP_THINGS wxFileCtrl"
                   WXFILEDLGG_H_STRING "${WXFILEDLGG_H_STRING}")

    # wxWidgets 2.8

    string(REPLACE "class WXDLLEXPORT wxGenericFileDialog"
                   "class WXDLLIMPEXP_THINGS wxGenericFileDialog"
                   WXFILEDLGG_H_STRING "${WXFILEDLGG_H_STRING}")

    string(REPLACE "class WXDLLEXPORT wxFileData"
                   "class WXDLLIMPEXP_THINGS wxFileData"
                   WXFILEDLGG_H_STRING "${WXFILEDLGG_H_STRING}")

    string(REPLACE "class WXDLLEXPORT wxFileCtrl"
                   "class WXDLLIMPEXP_THINGS wxFileCtrl"
                   WXFILEDLGG_H_STRING "${WXFILEDLGG_H_STRING}")

    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/include/wx/generic/filedlgg.h_ "${WXFILEDLGG_H_STRING}")

    configure_file(${CMAKE_CURRENT_BINARY_DIR}/include/wx/generic/filedlgg.h_
                   ${CMAKE_CURRENT_BINARY_DIR}/include/wx/generic/filedlgg.h COPYONLY)
endif()

# ---------------------------------------------------------------------------
# wxFileBrowser uses /wx/generic/filedlgg.h which is not available in MSW.
# For MSW DLLs we cannot get at wxGetAvailableDrives() in wx/src/generic/dirctrlg.cpp
# which wx/src/generic/filedlgg.cpp requires. We simply cut out that function
# and paste it into our code to get it.
# If this breaks we can resort to some other means, but this works for wx 2.8 and 2.9

if (BUILDING_DLLS)

    file(READ ${wxWidgets_ROOT_DIR}/src/generic/dirctrlg.cpp WXDIRCTRLG_CPP_STRING)

    if (${CMAKE_VERSION} VERSION_GREATER "2.8.4")
        string(FIND "${WXDIRCTRLG_CPP_STRING}" "// wxGetAvailableDrives, for WINDOWS, DOS, OS2, MAC" WXDIRCTRLG_CPP_STRING_START)
        string(FIND "${WXDIRCTRLG_CPP_STRING}" "// wxIsDriveAvailable"                               WXDIRCTRLG_CPP_STRING_END)
    else()
        # string(FIND ...) is not in CMake < 2.8.5, really ugly hack to duplicate it

        string(LENGTH "${WXDIRCTRLG_CPP_STRING}" WXDIRCTRLG_CPP_STRING_LEN)
        set(pos_ 1)

        message("Thinking... please wait...")

        while (${pos_} LESS ${WXDIRCTRLG_CPP_STRING_LEN})
            math(EXPR pos_ "${pos_} + 1")

            string(SUBSTRING "${WXDIRCTRLG_CPP_STRING}" "${pos_}" "21" sub_str)

            if ("${sub_str}" STREQUAL "// wxGetAvailableDriv")
                set(WXDIRCTRLG_CPP_STRING_START ${pos_})
                break()
            endif()
        endwhile()

        while (${pos_} LESS ${WXDIRCTRLG_CPP_STRING_LEN})
            math(EXPR pos_ "${pos_} + 1")

            string(SUBSTRING "${WXDIRCTRLG_CPP_STRING}" "${pos_}" "21" sub_str)

            if ("${sub_str}" STREQUAL "// wxIsDriveAvailable")
                set(WXDIRCTRLG_CPP_STRING_END ${pos_})
                break()
            endif()
        endwhile()
    endif()

    math(EXPR WXDIRCTRLG_CPP_STRING_LEN "${WXDIRCTRLG_CPP_STRING_END} - ${WXDIRCTRLG_CPP_STRING_START}")
    string(SUBSTRING "${WXDIRCTRLG_CPP_STRING}" "${WXDIRCTRLG_CPP_STRING_START}" "${WXDIRCTRLG_CPP_STRING_LEN}" CMAKE_CONFIGURE_WXTHINGS_FILEDLGG)

else()

    set(CMAKE_CONFIGURE_WXTHINGS_FILEDLGG "Non DLL linking : wxGetAvailableDrives() is available in dirctrlg.cpp")

endif()

configure_file(${wxThings_ROOT_DIR}/src/filedlgg.cpp
               ${CMAKE_CURRENT_BINARY_DIR}/src/filedlgg.cpp @ONLY)

configure_file(${wxThings_ROOT_DIR}/src/precomp.h
               ${CMAKE_CURRENT_BINARY_DIR}/src/precomp.h COPYONLY)

# ---------------------------------------------------------------------------

ADD_LIBRARY_FULL(wxThingsLib
    HEADERS
        ${wxThings_ROOT_DIR}/include/wx/things/block.h
        ${wxThings_ROOT_DIR}/include/wx/things/bmpcombo.h
        ${wxThings_ROOT_DIR}/include/wx/things/dropdown.h
        ${wxThings_ROOT_DIR}/include/wx/things/filebrws.h
        ${wxThings_ROOT_DIR}/include/wx/things/genergdi.h
        ${wxThings_ROOT_DIR}/include/wx/things/geometry.h
        ${wxThings_ROOT_DIR}/include/wx/things/matrix2d.h
        ${wxThings_ROOT_DIR}/include/wx/things/medsort.h
        ${wxThings_ROOT_DIR}/include/wx/things/menubtn.h
        ${wxThings_ROOT_DIR}/include/wx/things/optvalue.h
        ${wxThings_ROOT_DIR}/include/wx/things/range.h
        ${wxThings_ROOT_DIR}/include/wx/things/spinctld.h
        ${wxThings_ROOT_DIR}/include/wx/things/thingdef.h
        ${wxThings_ROOT_DIR}/include/wx/things/toggle.h

        ${wxThings_ROOT_DIR}/src/precomp.h
    SOURCES
        ${wxThings_ROOT_DIR}/src/block.cpp
        ${wxThings_ROOT_DIR}/src/bmpcombo.cpp
        ${wxThings_ROOT_DIR}/src/dropdown.cpp
        ${wxThings_ROOT_DIR}/src/filebrws.cpp

        # This file is configured before compliation, see above.
        #${wxThings_ROOT_DIR}/src/filedlgg.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/src/filedlgg.cpp

        ${wxThings_ROOT_DIR}/src/genergdi.cpp
        ${wxThings_ROOT_DIR}/src/geometry.cpp
        ${wxThings_ROOT_DIR}/src/matrix2d.cpp
        ${wxThings_ROOT_DIR}/src/menubtn.cpp
        ${wxThings_ROOT_DIR}/src/optvalue.cpp
        ${wxThings_ROOT_DIR}/src/precomp.cpp
        ${wxThings_ROOT_DIR}/src/range.cpp
        ${wxThings_ROOT_DIR}/src/spinctld.cpp
        ${wxThings_ROOT_DIR}/src/toggle.cpp
    PRECOMPILED_INCLUDE precomp.h
    PRECOMPILED_HEADER  ${wxThings_ROOT_DIR}/src/precomp.h
    PRECOMPILED_SOURCE  ${wxThings_ROOT_DIR}/src/precomp.cpp
    LINK_LIBRARIES
        ${wxWidgets_LIBRARIES}
    PROPERTIES DEFINE_SYMBOL "WXMAKINGDLL_THINGS"
    PROPERTIES FOLDER "wxThings")

ADD_TARGET_INCLUDE_PATHS(wxThingsLib ${CMAKE_CURRENT_BINARY_DIR}/include/)

WXLIKE_LIBRARY_NAMES( wxThingsLib wxthings "${wxThings_VERSION}")

# ---------------------------------------------------------------------------

install(TARGETS wxThingsLib
        EXPORT  wxThings_export
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(EXPORT wxThings_export
        DESTINATION share/wxthings/
        FILE wxThingsConfig.cmake)

export( TARGETS wxThingsLib
        FILE "${CMAKE_BINARY_DIR}/build/wxThingsConfig.cmake")

install(FILES
        ${wxThings_ROOT_DIR}/docs/readme.txt
        DESTINATION "doc")

install(FILES
        ${wxThings_ROOT_DIR}/include/wx/things/block.h
        ${wxThings_ROOT_DIR}/include/wx/things/bmpcombo.h
        ${wxThings_ROOT_DIR}/include/wx/things/dropdown.h
        ${wxThings_ROOT_DIR}/include/wx/things/filebrws.h
        ${wxThings_ROOT_DIR}/include/wx/things/genergdi.h
        ${wxThings_ROOT_DIR}/include/wx/things/geometry.h
        ${wxThings_ROOT_DIR}/include/wx/things/matrix2d.h
        ${wxThings_ROOT_DIR}/include/wx/things/medsort.h
        ${wxThings_ROOT_DIR}/include/wx/things/menubtn.h
        ${wxThings_ROOT_DIR}/include/wx/things/optvalue.h
        ${wxThings_ROOT_DIR}/include/wx/things/range.h
        ${wxThings_ROOT_DIR}/include/wx/things/spinctld.h
        ${wxThings_ROOT_DIR}/include/wx/things/thingdef.h
        ${wxThings_ROOT_DIR}/include/wx/things/toggle.h
        DESTINATION "include/wx/things")

# ---------------------------------------------------------------------------
# CMake build file for wxThings sample
# ---------------------------------------------------------------------------

ADD_EXECUTABLE_FULL( wxThingsDemo WIN32 MACOSX_BUNDLE
    HEADERS ""
    SOURCES
        ${wxThings_ROOT_DIR}/samples/things/thingsdemo.cpp
        ${wxThings_ROOT_DIR}/samples/things/thingsdemo.rc
    LINK_LIBRARIES
        wxThingsLib
        ${wxWidgets_LIBRARIES}
    PROPERTIES FOLDER "wxThings")

# ---------------------------------------------------------------------------
# CMake build file for wxThings FileBrowser sample
# ---------------------------------------------------------------------------

ADD_EXECUTABLE_FULL( wxFileBrowser WIN32 MACOSX_BUNDLE
    HEADERS ""
    SOURCES
        ${wxThings_ROOT_DIR}/samples/filebrws/wxfilebrowser.cpp
        ${wxThings_ROOT_DIR}/samples/filebrws/wxfilebrowser.rc
    LINK_LIBRARIES
        wxThingsLib
        ${wxWidgets_LIBRARIES}
    PROPERTIES FOLDER "wxThings")

endif(NOT TARGET wxThingsLib)
