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

#---------------------
#--- Finding GTEST ---
#---------------------

if( NOT REQUIRED_GTEST_VERSION )
  set( REQUIRED_GTEST_VERSION 1.6.0 )
endif()
set( GTEST_FULL_NAME GTEST-${REQUIRED_GTEST_VERSION} )

if( EXISTS ${THIRDPARTY_BUILD_FOLDERS} )
  # First try to find our built version of GTEST
  find_package( ${GTEST_FULL_NAME} NO_MODULE PATHS "${THIRDPARTY_BUILD_FOLDERS}/${GTEST_FULL_NAME}" NO_DEFAULT_PATH )
  if( ${GTEST_FULL_NAME}_FOUND )
    LOG_DEBUG( "Local GTEST was found!" )
    if( NOT ${GTEST_FULL_NAME}_INCLUDE_DIRS )
      LOG_DEBUG( "Setting include" )
      set( ${GTEST_FULL_NAME}_INCLUDE_DIRS ${GTEST_FULL_NAME} )
    endif()
    LOG_DEBUG( "${GTEST_FULL_NAME}_INCLUDE_DIRS ${${GTEST_FULL_NAME}_INCLUDE_DIRS}" )
    LOG_DEBUG( "${GTEST_FULL_NAME}_LIBRARIES ${${GTEST_FULL_NAME}_LIBRARIES}" )
    LOG_DEBUG( "${GTEST_FULL_NAME}_USE_FILE ${${GTEST_FULL_NAME}_USE_FILE} " )

    set( GTEST_INCLUDE_DIRS ${${GTEST_FULL_NAME}_INCLUDE_DIRS} )
    set( GTEST_BOTH_LIBRARIES ${${GTEST_FULL_NAME}_LIBRARIES} )
    
    set( GTEST_FOUND ${${GTEST_FULL_NAME}_FOUND} )

    # Add use file
    include( ${${VTK_FULL_NAME}_USE_FILE} )
    # Add include directories
    include_directories( ${VTK-5.6.1_INCLUDE_DIRS} )
    # Link project library directories
    link_directories( ${VTK-5.6.1_LIBRARY_DIRS} )
    # Link project library
    #LINK_LIBRARIES(${VTK-5.6.1_LIBRARIES})
  else( ${GTEST_FULL_NAME}_FOUND )
    LOG_WARNING( "THIRDPARTY_BUILD_FOLDERS is set to ${THIRDPARTY_BUILD_FOLDERS} but ${GTEST_FULL_NAME} was not found there." )
    # Fallback to CMake module finding
    PUSH_VARIABLE( CMAKE_MODULE_PATH )
    unset( CMAKE_MODULE_PATH  )
    find_package( GTEST ${REQUIRED_GTEST_VERSION} REQUIRED )
    POP_VARIABLE( CMAKE_MODULE_PATH )
    #include( ${GTEST_USE_FILE} )
  endif( ${GTEST_FULL_NAME}_FOUND )
else( EXISTS ${THIRDPARTY_BUILD_FOLDERS} )
  LOG_ERROR( "Needed THIRDPARTY_BUILD_FOLDERS points to ${THIRDPARTY_BUILD_FOLDERS} but it does not exist." )
  FATAL_ERROR()
endif( EXISTS ${THIRDPARTY_BUILD_FOLDERS} )


