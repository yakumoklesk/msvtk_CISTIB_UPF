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
set( GUI_API_CONSOLE_ENABLED_CONFIG 1 )

set( GUI_API_NONE ${GUI_API_NONE_CONFIG} )
set( GUI_API_QT ${GUI_API_QT_CONFIG} )
set( GUI_API_WXWIDGETS ${GUI_API_WXWIDGETS_CONFIG} )
set( GUI_API_CONSOLE_ENABLED ${GUI_API_CONSOLE_ENABLED} )

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

# Add some global options
option( MSVTK_BUILD_SHARED_LIBS "" ON )


# Add some VTK options
option( MSVTK_VTK_DEBUG_LEAKS "" ON )
option( MSVTK_VTK_LEGACY_REMOVE "" ON )
set( MSVTK_VTK_BUILD_OPTIONS )
list( APPEND MSVTK_VTK_BUILD_OPTIONS -DVTK_DEBUG_LEAKS:BOOL=${MSVTK_VTK_DEBUG_LEAKS} )
list( APPEND MSVTK_VTK_BUILD_OPTIONS -DVTK_LEGACY_REMOVE:BOOL=${MSVTK_VTK_LEGACY_REMOVE} )

LOG_DEBUG( "MSVTK_VTK_BUILD_OPTIONS ${MSVTK_VTK_BUILD_OPTIONS}" )

