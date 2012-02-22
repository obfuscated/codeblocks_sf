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

# ===========================================================================
# Add subdirectories containing CMakeLists.txt files or specify projects
# ===========================================================================

set( wxThings_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "Root dir of wxThings" FORCE)

# ---------------------------------------------------------------------------
# CMake build file for wxThings Library
# ---------------------------------------------------------------------------

if (NOT TARGET wxThingsLib)

include_directories( ${wxThings_ROOT_DIR}/include/ )

# We use some functions from GTK directly
if ("${wxWidgets_PLATFORM}" STREQUAL "gtk2")
    find_package(GTK2)
    include_directories( ${GTK2_INCLUDE_DIRS} )
elseif ("${wxWidgets_PLATFORM}" STREQUAL "gtk")
    find_package(GTK)
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

    string(FIND "${WXDIRCTRLG_CPP_STRING}" "// wxGetAvailableDrives, for WINDOWS, DOS, OS2, MAC" WXDIRCTRLG_CPP_STRING_START)
    string(FIND "${WXDIRCTRLG_CPP_STRING}" "// wxIsDriveAvailable"                               WXDIRCTRLG_CPP_STRING_END)

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

# ---------------------------------------------------------------------------

install(FILES
    ${wxThings_ROOT_DIR}/docs/readme.txt
    DESTINATION "doc")

# ---------------------------------------------------------------------------
# CMake build file for wxThings sample
# ---------------------------------------------------------------------------

ADD_EXECUTABLE_FULL( wxThingsDemo WIN32
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

ADD_EXECUTABLE_FULL( wxFileBrowser WIN32
    HEADERS ""
    SOURCES
        ${wxThings_ROOT_DIR}/samples/filebrws/wxfilebrowser.cpp
        ${wxThings_ROOT_DIR}/samples/filebrws/wxfilebrowser.rc
    LINK_LIBRARIES
        wxThingsLib
        ${wxWidgets_LIBRARIES}
    PROPERTIES FOLDER "wxThings")

endif(NOT TARGET wxThingsLib)
