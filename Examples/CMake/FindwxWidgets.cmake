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

#-------------------
#--- Finding VTK ---
#-------------------

if( NOT REQUIRED_WXWIDGETS_VERSION )
  set( REQUIRED_WXWIDGETS_VERSION 2.8.10 )
endif()
set( WXWIDGETS_FULL_NAME WXWIDGETS-${REQUIRED_WXWIDGETS_VERSION} )

if( EXISTS ${THIRDPARTY_BUILD_FOLDERS} )
  # First try to find our built version of WXWIDGETS
  find_package( ${WXWIDGETS_FULL_NAME} NO_MODULE PATHS "${THIRDPARTY_BUILD_FOLDERS}/${WXWIDGETS_FULL_NAME}" NO_DEFAULT_PATH )
  if( ${WXWIDGETS_FULL_NAME}_FOUND )
    LOG_DEBUG( "Local WXWIDGETS was found!" )
    if( NOT ${WXWIDGETS_FULL_NAME}_INCLUDE_DIRS )
      LOG_DEBUG( "Setting include" )
      set( ${WXWIDGETS_FULL_NAME}_INCLUDE_DIRS ${WXWIDGETS_INCLUDE_DIRS} )
    endif()
    LOG_DEBUG( "${WXWIDGETS_FULL_NAME}_INCLUDE_DIRS ${${WXWIDGETS_FULL_NAME}_INCLUDE_DIRS}" )
    LOG_DEBUG( "${WXWIDGETS_FULL_NAME}_LIBRARIES ${${WXWIDGETS_FULL_NAME}_LIBRARIES}" )
    LOG_DEBUG( "${WXWIDGETS_FULL_NAME}_USE_FILE ${${WXWIDGETS_FULL_NAME}_USE_FILE} " )
    # Add use file
    include( ${${WXWIDGETS_FULL_NAME}_USE_FILE} )
    # Add include directories
    include_directories( ${WXWIDGETS_FULL_NAME_INCLUDE_DIRS} )
    # Link project library directories
    link_directories( ${WXWIDGETS_FULL_NAME_LIBRARY_DIRS} )
    # Link project library
    #ling_libraries(${WXWIDGETS_FULL_NAME_LIBRARIES})
  else( ${WXWIDGETS_FULL_NAME}_FOUND )
    LOG_WARNING( "THIRDPARTY_BUILD_FOLDERS is set to ${THIRDPARTY_BUILD_FOLDERS} but ${WXWIDGETS_FULL_NAME} was not found there." )
    # Fallback to CMake module finding
    PUSH_VARIABLE( CMAKE_MODULE_PATH )
    unset( CMAKE_MODULE_PATH  )
    find_package( wxWidgets ${REQUIRED_WXWIDGETS_VERSION} REQUIRED )
    POP_VARIABLE( CMAKE_MODULE_PATH )
    include( ${WXWIDGETS_USE_FILE} )
  endif( ${WXWIDGETS_FULL_NAME}_FOUND )
else( EXISTS ${THIRDPARTY_BUILD_FOLDERS} )
  LOG_ERROR( "Needed THIRDPARTY_BUILD_FOLDERS points to ${THIRDPARTY_BUILD_FOLDERS} but it does not exist." )
  FATAL_ERROR()
endif( EXISTS ${THIRDPARTY_BUILD_FOLDERS} )
