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

# David Lucena: I want to allow to build the examples directories by providing my
# own external libraries sources, but also make it compatible with the normal MSVTK
# build, this is why I test if I am being included or not

get_directory_property( LISTFILE_STACK_VALUE LISTFILE_STACK )
message( "LISTFILE_STACK_VALUE ${LISTFILE_STACK_VALUE}" )
message( "CMAKE_PARENT_LIST_FILE ${CMAKE_PARENT_LIST_FILE}" )
get_directory_property( PARENT_DIRECTORY_VALUE PARENT_DIRECTORY )
message( "PARENT_DIRECTORY_VALUE ${PARENT_DIRECTORY_VALUE}" )

list( LENGTH LISTFILE_STACK_VALUE NUMBER_OF_CURRENT_PROCESSING_FILES )
#if( NUMBER_OF_CURRENT_PROCESSING_FILES EQUAL 1 )
if( NOT PARENT_DIRECTORY_VALUE )
  list( GET LISTFILE_STACK_VALUE 0 CURRENT_PROCESSING_FILE )
  get_filename_component( CURRENT_PROCESSING_FILE_PATH ${CURRENT_PROCESSING_FILE} PATH )
  message( "CURRENT_PROCESSING_FILE ${CURRENT_PROCESSING_FILE}" )
  message( "CURRENT_PROCESSING_FILE_PATH ${CURRENT_PROCESSING_FILE_PATH}" )
  set( CMAKE_MODULE_PATH
    "${CURRENT_PROCESSING_FILE_PATH}/../CMake"
    ${CMAKE_MODULE_PATH}
  )
  message( "CMAKE_MODULE_PATH being set" )
  # To import a module, you must not specify the extension
  #include( ${CURRENT_PROCESSING_FILE_PATH}/../CMake/Common.cmake )
  include( Common )
  #message( "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}" )
  set( Examples_STANDALONE_BUILD 1 )
endif()

# Removed the CXX, or QT will fail on initialization (uses try_compile on c files)
project( Examples )
#project( Examples CXX )

# Set some variables to substitute in config file
set( Examples_VERSION_MAJOR 1 )
set( Examples_VERSION_MINOR 0 )

# Set the possible GUI API
set( Examples_GUI_API WXWIDGETS CACHE STRING "Set the desired GUI API to be used for examples" )
set_property( CACHE Examples_GUI_API PROPERTY STRINGS NONE QT WXWIDGETS )
# Set the possible versions of VTK
set( Examples_REQUIRED_VTK_VERSION 5.6.1 CACHE STRING "Set the required version of VTK to be used for the examples." )
set_property( CACHE Examples_REQUIRED_VTK_VERSION PROPERTY STRINGS 5.6.1 5.8.0 REPOSITORY )
# Set the possible versions of GTEST
set( Examples_REQUIRED_GTEST_VERSION 1.6.0 CACHE STRING "Set the required version of GTEST to be used for the examples." )
set_property( CACHE Examples_REQUIRED_GTEST_VERSION PROPERTY STRINGS 1.6.0 )

# Choose GUI API
set( GUI_API_NONE_CONFIG 0 )
set( GUI_API_QT_CONFIG 1 )
set( GUI_API_WXWIDGETS_CONFIG 2 )

set( GUI_API_NONE ${GUI_API_NONE_CONFIG} )
set( GUI_API_QT ${GUI_API_QT_CONFIG} )
set( GUI_API_WXWIDGETS ${GUI_API_WXWIDGETS_CONFIG} )

if( Examples_GUI_API STREQUAL NONE )
  # Unset other GUI API
  unset( Examples_REQUIRED_WXWIDGETS_VERSION CACHE )
  unset( Examples_REQUIRED_QT_VERSION CACHE )
  set( GUI_API_CONFIG ${GUI_API_NONE} )
elseif( Examples_GUI_API STREQUAL QT )
  # Unset other GUI API
  unset( Examples_REQUIRED_WXWIDGETS_VERSION CACHE )
  # Set the possible versions of QT
  set( Examples_REQUIRED_QT_VERSION 4.7.0 CACHE STRING "Set the required version of Qt to be used for the examples." )
  set_property( CACHE Examples_REQUIRED_QT_VERSION PROPERTY STRINGS 4.7.0 )
  # Set the configured value
  set( GUI_API_CONFIG ${GUI_API_QT} )
elseif( Examples_GUI_API STREQUAL WXWIDGETS )
  # Unset other GUI API
  unset( Examples_REQUIRED_QT_VERSION CACHE )
  # Set the possible versions of WXWIDGETS
  set( Examples_REQUIRED_WXWIDGETS_VERSION 2.8.10 CACHE STRING "Set the required version of wxWidgets to be used for the examples." )
  set_property( CACHE Examples_REQUIRED_WXWIDGETS_VERSION PROPERTY STRINGS 2.8.10 )
  # Set the configured value
  set( GUI_API_CONFIG ${GUI_API_WXWIDGETS} )
endif()

set( GUI_API ${GUI_API_CONFIG} )
set( USE_WXVTK_CONFIG 0 )

#define GUI_API_QT 0
#define GUI_API_WXWIDGETS 1

# Setting configuration file source and destination
configure_file(
  "${PROJECT_SOURCE_DIR}/config/BuildConfig.h.in"
  "${PROJECT_BINARY_DIR}/BuildConfig.h"
)

# Add the directory where the output configuration file is
include_directories( "${PROJECT_BINARY_DIR}" )


if( Examples_REQUIRED_VTK_VERSION STREQUAL REPOSITORY )
  include( DownloadVTK )
else()
  set( REQUIRED_VTK_VERSION ${Examples_REQUIRED_VTK_VERSION} )
  include( FindVTK )
endif()

set( REQUIRED_GTEST_VERSION ${Examples_REQUIRED_GTEST_VERSION} )
include( FindGTEST )

if( GUI_API EQUAL GUI_API_WXWIDGETS )
  set( REQUIRED_WXWIDGETS_VERSION ${Examples_REQUIRED_WXWIDGETS_VERSION} )
  include( FindWXWIDGETS )
elseif( GUI_API EQUAL GUI_API_QT )
  set( REQUIRED_QT_VERSION ${Examples_REQUIRED_QT_VERSION} )
  if( QT4_FOUND )
    LOG_DEBUG( "QT4 FOUND!!!!!!!!!!" )
  else()
	include( msvMacroSetupQt )
	msvMacroSetupQt()
  endif()
  
endif()

include_directories(
  ${PROJECT_SOURCE_DIR}/common/include
)

set( SRCS_Common
  ${PROJECT_SOURCE_DIR}/common/include/VtkExtTypes.h
  ${PROJECT_BINARY_DIR}/BuildConfig.h
)

# If the parent has configure libary or executable dir, inherit
if( PARENT_PROJECT_LIBRARY_DIR )
  set( LIBRARY_OUTPUT_PATH ${PARENT_PROJECT_LIBRARY_DIR} )
endif()
if( PARENT_PROJECT_EXECUTABLE_DIR )
  set( EXECUTABLE_OUTPUT_PATH ${PARENT_PROJECT_EXECUTABLE_DIR} )
endif()

set( Examples_BINARY_DIR "bin" )

add_subdirectory( VolumeRenderingTF )
add_subdirectory( GPUPolyDataMapper )
add_subdirectory( ExampleSkeleton )


if( NOT Examples_STANDALONE_BUILD )

  project(msvExamples)

  cmake_minimum_required(VERSION 2.8)
  include_regular_expression("^.*$")

  # -----------------------------------------------------------------------------
  # Disable deprecation warnings for standard C and STL functions in VS2005 and
  # later (no, we don't need IF(CMAKE_COMPILER_2005) ... )
  # -----------------------------------------------------------------------------
  IF(MSVC_VERSION EQUAL 1400 OR MSVC_VERSION GREATER 1400 OR MSVC10)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS)
    ADD_DEFINITIONS(-D_SCL_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)
  ENDIF(MSVC_VERSION EQUAL 1400 OR MSVC_VERSION GREATER 1400 OR MSVC10)

  IF(NOT MSVTK_BINARY_DIR)
    find_package(MSVTK REQUIRED)
    include(${MSVTK_USE_FILE})
  ENDIF(NOT MSVTK_BINARY_DIR)

  set(dirs
  # FileWatcher
    )

  list(APPEND dirs
    Time
    )

  #-----------------------------------------------------------------------------
  # Loop over list of directories
  #-----------------------------------------------------------------------------
  foreach(dir ${dirs})
    message(STATUS "Configuring examples: msv${dir}")
    add_subdirectory(${dir})
  endforeach()

endif( NOT Examples_STANDALONE_BUILD )