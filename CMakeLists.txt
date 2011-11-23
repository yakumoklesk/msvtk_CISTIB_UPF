###########################################################################
#
#  Library: MSVTK
#
#  Copyright (c) Computational Image and Simulation Technologies in Biomedicine (CISTIB),
#                Universitat Pompeu Fabra (UPF), Barcelona, Spain
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0.txt
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
###########################################################################

# Require CMake 2.8 or higher.
cmake_minimum_required( VERSION 2.8.0 )

project( MSVTK CXX )

include( CMake/Common.cmake )


# Set the possible GUI API
set( MSVTK_REQUIRED_GUI_API WXWIDGETS CACHE STRING "Set the desired GUI API to be used for examples" )
set_property( CACHE MSVTK_REQUIRED_GUI_API PROPERTY STRINGS NONE QT WXWIDGETS )
# Set the possible versions of VTK
set( MSVTK_REQUIRED_VTK_VERSION 5.6.1 CACHE STRING "Set the required version of VTK to be used for the examples." )
set_property( CACHE MSVTK_REQUIRED_VTK_VERSION PROPERTY STRINGS 5.6.1 5.8.0 REPOSITORY )
# Set the possible versions of GTEST
set( MSVTK_REQUIRED_GTEST_VERSION 1.6.0 CACHE STRING "Set the required version of GTEST to be used for the examples." )
set_property( CACHE MSVTK_REQUIRED_GTEST_VERSION PROPERTY STRINGS 1.6.0 )

# Choose GUI API
set( GUI_API_NONE_CONFIG 0 )
set( GUI_API_QT_CONFIG 1 )
set( GUI_API_WXWIDGETS_CONFIG 2 )

set( GUI_API_NONE ${GUI_API_NONE_CONFIG} )
set( GUI_API_QT ${GUI_API_QT_CONFIG} )
set( GUI_API_WXWIDGETS ${GUI_API_WXWIDGETS_CONFIG} )

if( MSVTK_REQUIRED_GUI_API STREQUAL NONE )
  # Unset other GUI API
  unset( MSVTK_REQUIRED_WXWIDGETS_VERSION CACHE )
  unset( MSVTK_REQUIRED_QT_VERSION CACHE )
  set( GUI_API_CONFIG ${GUI_API_NONE} )
elseif( MSVTK_REQUIRED_GUI_API STREQUAL QT )
  # Unset other GUI API
  unset( MSVTK_REQUIRED_WXWIDGETS_VERSION CACHE )
  # Set the possible versions of QT
  set( MSVTK_REQUIRED_QT_VERSION 4.7.0 CACHE STRING "Set the required version of Qt to be used for the examples." )
  set_property( CACHE MSVTK_REQUIRED_QT_VERSION PROPERTY STRINGS 4.7.0 )
  # Set the configured value
  set( GUI_API_CONFIG ${GUI_API_QT} )
elseif( MSVTK_REQUIRED_GUI_API STREQUAL WXWIDGETS )
  # Unset other GUI API
  unset( MSVTK_REQUIRED_QT_VERSION CACHE )
  # Set the possible versions of WXWIDGETS
  set( MSVTK_REQUIRED_WXWIDGETS_VERSION 2.8.10 CACHE STRING "Set the required version of wxWidgets to be used for the examples." )
  set_property( CACHE MSVTK_REQUIRED_WXWIDGETS_VERSION PROPERTY STRINGS 2.8.10 )
  # Set the configured value
  set( GUI_API_CONFIG ${GUI_API_WXWIDGETS} )
endif()

set( GUI_API ${GUI_API_CONFIG} )
set( USE_WXVTK_CONFIG 0 )

#define GUI_API_QT 0
#define GUI_API_WXWIDGETS 1

# Setting configuration file source and destination
#configure_file(
#  "${PROJECT_SOURCE_DIR}/config/BuildConfig.h.in"
#  "${PROJECT_BINARY_DIR}/BuildConfig.h"
#  )

#-----------------------------------------------------------------------------
# Superbuild Option - Enabled by default
#-
option(MSVTK_SUPERBUILD "Build MSVTK and the projects it depends on via SuperBuild.cmake." ON)
mark_as_advanced( MSVTK_SUPERBUILD )

#---------------------------------------------------------------------------
# Will contain a list of sub-directory without option ON or OFF
#-
set( MSVTK_PLUGINS_SUBDIRS )
set( MSVTK_APPLICATIONS_SUBDIRS )
set( MSVTK_EXAMPLE_SUBDIRS )

#option( MSVTK_APP_ECG "ECG application to demonstrate vtkButtons" ON )
#list( APPEND MSVTK_APPLICATIONS_SUBDIRS Examples )

option( MSVTK_EXAMPLE_VolumeRenderingTF "VolumeRenderingTF example" ON )
list( APPEND MSVTK_EXAMPLES_SUBDIRS VolumeRenderingTF )

#-----------------------------------------------------------------------------
# QT
#-
include( msvMacroSetupQt )
msvMacroSetupQt()

#-----------------------------------------------------------------------------
# Superbuild script
#-
if(MSVTK_SUPERBUILD)
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/CMake/SuperBuild")
  return()
endif()

# VTK_DIR should be set by Superbuild
if( VTK_DIR )
  find_package( VTK REQUIRED )
else()
  if( MSVTK_REQUIRED_VTK_VERSION STREQUAL REPOSITORY )
    include( DownloadVTK )
  else()
    set( REQUIRED_VTK_VERSION ${MSVTK_REQUIRED_VTK_VERSION} )
    include( FindVTK )
  endif()
endif()

#set( REQUIRED_GTEST_VERSION ${MSVTK_REQUIRED_GTEST_VERSION} )
#include( FindGTEST )

#if( GUI_API EQUAL GUI_API_WXWIDGETS )
#  set( REQUIRED_WXWIDGETS_VERSION ${MSVTK_REQUIRED_WXWIDGETS_VERSION} )
#  include( FindWXWIDGETS )
#endif()

#-----------------------------------------------------------------------------
# MSVTK_SUPERBUILD_BINARY_DIR

# If MSVTK_SUPERBUILD_BINARY_DIR isn't defined, it means MSVTK is *NOT* build using Superbuild.
# In that specific case, MSVTK_SUPERBUILD_BINARY_DIR should default to MSVTK_BINARY_DIR
if(NOT DEFINED MSVTK_SUPERBUILD_BINARY_DIR)
  set(MSVTK_SUPERBUILD_BINARY_DIR ${MSVTK_BINARY_DIR})
endif()

#-----------------------------------------------------------------------------
# Set C/CXX Flags
#-
set(CMAKE_CXX_FLAGS ${MSVTK_CXX_FLAGS} CACHE STRING "CMake C Flags" FORCE)
set(CMAKE_C_FLAGS ${MSVTK_C_FLAGS} CACHE STRING "CMake CXX Flags" FORCE)

#-----------------------------------------------------------------------------
# Set the header template which defines custom export/import macros
# for shared libraries
#-
set(MSVTK_EXPORT_HEADER_TEMPLATE "${MSVTK_SOURCE_DIR}/Libs/msvExport.h.in")

#-----------------------------------------------------------------------------
# Add MSVTK library subdirectories
#-
#add_subdirectory( Libs )

#-----------------------------------------------------------------------------
# Add MSVTK plugin subdirectories
#-
foreach( plugin ${MSVTK_PLUGINS_SUBDIRS} )
  if( MSVTK_PLUGIN_${plugin} )
    add_subdirectory( Plugins/${plugin} )
  endif()
endforeach()

#-----------------------------------------------------------------------------
# Add MSVTK application subdirectories
#-
foreach( app ${MSVTK_APPLICATIONS_SUBDIRS} )
  if( MSVTK_APP_${app} )
    add_subdirectory( Applications/${app} )
  endif()
endforeach()

#-----------------------------------------------------------------------------
# Add MSVTK examples subdirectories
#-
foreach( app ${MSVTK_EXAMPLES_SUBDIRS} )
  if( MSVTK_EXAMPLE_${app} )
    add_subdirectory( Examples/${app} )
  endif()
endforeach()



#-----------------------------------------------------------------------------
# Style Checking configuration
#-
#if( MSVTK_USE_KWSTYLE )
#  include( Utilities/KWStyle/KWStyle.cmake )
#endif()

#---------------------------------------------------------------------------
# Documentation
#-
#add_subdirectory( Utilities/Documentation )
