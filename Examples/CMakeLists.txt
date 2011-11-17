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

project( Examples CXX )


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
