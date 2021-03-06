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

if( THIRDPARTY_BUILD_FOLDERS )
  if( THIRDPARTY_BUILD_FOLDERS STREQUAL "" )
    unset( THIRDPARTY_BUILD_FOLDERS )
  endif()
endif()

# Give priority if VTK_DIR is defined ( probably by a superbuild script )
if( VTK_DIR )
  LOG_DEBUG( "VTK_DIR is set" )
  LOG_DEBUG( "VTK_DIR ${VTK_DIR}" )
  set( VTK_FULL_NAME VTK )
  LOG_DEBUG( "CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}" )
  PUSH_VARIABLE( CMAKE_MODULE_PATH )
  unset( CMAKE_MODULE_PATH  )
  LOG_DEBUG( "CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}" )
  find_package( VTK REQUIRED )
  POP_VARIABLE( CMAKE_MODULE_PATH )
  LOG_DEBUG( "CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}" )
  if( VTK_FOUND )
    LOG_DEBUG( "VTK was found in specified VTK_DIR ( ${VTK_DIR} )!" )
    LOG_DEBUG( "${VTK_FULL_NAME}_USE_FILE ${${VTK_FULL_NAME}_USE_FILE} " )
    #LOG_DEBUG( "${VTK_FULL_NAME}_INCLUDE_DIRS ${${VTK_FULL_NAME}_INCLUDE_DIRS}" )
    LOG_DEBUG( "${VTK_FULL_NAME}_LIBRARIES ${${VTK_FULL_NAME}_LIBRARIES}" )
    LOG_DEBUG( "${VTK_FULL_NAME}_LIBRARY_DIRS ${${VTK_FULL_NAME}_LIBRARY_DIRS} " )

    #LOG_DEBUG( "CMAKE_C_FLAGS             ${CMAKE_C_FLAGS}" )
    #LOG_DEBUG( "CMAKE_CXX_FLAGS           ${CMAKE_CXX_FLAGS}" )
    #LOG_DEBUG( "CMAKE_EXE_LINKER_FLAGS    ${CMAKE_EXE_LINKER_FLAGS}" )
    #LOG_DEBUG( "CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS}" )
    #LOG_DEBUG( "CMAKE_MODULE_LINKER_FLAGS ${CMAKE_MODULE_LINKER_FLAGS}" )

    include( ${${VTK_FULL_NAME}_USE_FILE} )
    link_libraries( ${${VTK_FULL_NAME}_LIBRARIES} )
    link_directories( ${${VTK_FULL_NAME}_LIBRARY_DIRS} )
    
    #LOG_DEBUG( "VTK_REQUIRED_C_FLAGS             ${VTK_REQUIRED_C_FLAGS}" )
    #LOG_DEBUG( "VTK_REQUIRED_CXX_FLAGS           ${VTK_REQUIRED_CXX_FLAGS}" )
    #LOG_DEBUG( "VTK_REQUIRED_EXE_LINKER_FLAGS    ${VTK_REQUIRED_EXE_LINKER_FLAGS}" )
    #LOG_DEBUG( "VTK_REQUIRED_SHARED_LINKER_FLAGS ${VTK_REQUIRED_SHARED_LINKER_FLAGS}" )
    #LOG_DEBUG( "VTK_REQUIRED_MODULE_LINKER_FLAGS ${VTK_REQUIRED_MODULE_LINKER_FLAGS}" )

    return()
  else()
   LOG_WARNING( "VTK_DIR was specified ( ${VTK_DIR} ) but find_package could not find it" )
  endif()
else( VTK_DIR )
    if( NOT REQUIRED_VTK_VERSION )
	  #set( DEFAULT_REQUIRED_VTK_VERSION 5.6.1 )
	  set( DEFAULT_REQUIRED_VTK_VERSION 5.8.0 )
	  LOG_DEBUG( "--------- REQUIRED_VTK_VERSION not specified, defaulting to ${DEFAULT_REQUIRED_VTK_VERSION}-----------" )
	  # Beware when compiling examples, that the correct include files are set in the project
      set( REQUIRED_VTK_VERSION ${DEFAULT_REQUIRED_VTK_VERSION} )
    endif()
    set( VTK_FULL_NAME VTK-${REQUIRED_VTK_VERSION} )
endif( VTK_DIR )

#LOG_DEBUG( "VTK_DIR ${VTK_DIR}" )
#LOG_DEBUG( "REQUIRED_VTK_VERSION ${REQUIRED_VTK_VERSION}" )
#LOG_DEBUG( "VTK_FULL_NAME ${VTK_FULL_NAME}" )

if( THIRDPARTY_BUILD_FOLDERS )
  if( EXISTS ${THIRDPARTY_BUILD_FOLDERS} )
    # First try to find our built version of VTK
    find_package( ${VTK_FULL_NAME} NO_MODULE PATHS "${THIRDPARTY_BUILD_FOLDERS}/${VTK_FULL_NAME}" NO_DEFAULT_PATH )
    if( ${VTK_FULL_NAME}_FOUND )
      LOG_DEBUG( "Local VTK was found!" )
      if( NOT ${VTK_FULL_NAME}_INCLUDE_DIRS )
        LOG_DEBUG( "Setting include" )
        set( ${VTK_FULL_NAME}_INCLUDE_DIRS ${VTK_INCLUDE_DIRS} )
      endif()
      LOG_DEBUG( "${VTK_FULL_NAME}_INCLUDE_DIRS ${${VTK_FULL_NAME}_INCLUDE_DIRS}" )
      LOG_DEBUG( "${VTK_FULL_NAME}_LIBRARIES ${${VTK_FULL_NAME}_LIBRARIES}" )
      LOG_DEBUG( "${VTK_FULL_NAME}_USE_FILE ${${VTK_FULL_NAME}_USE_FILE} " )
      # Add use file
      include( ${${VTK_FULL_NAME}_USE_FILE} )
      # Add include directories
      include_directories( ${VTK-5.6.1_INCLUDE_DIRS} )
      # Link project library directories
      link_directories( ${VTK-5.6.1_LIBRARY_DIRS} )
      # Link project library
      #LINK_LIBRARIES(${VTK-5.6.1_LIBRARIES})
    else( ${VTK_FULL_NAME}_FOUND )
      LOG_WARNING( "THIRDPARTY_BUILD_FOLDERS is set to ${THIRDPARTY_BUILD_FOLDERS} but ${VTK_FULL_NAME} was not found there." )
      # Fallback to CMake module finding
      PUSH_VARIABLE( CMAKE_MODULE_PATH )
      unset( CMAKE_MODULE_PATH  )
      find_package( VTK ${REQUIRED_VTK_VERSION} REQUIRED )
      POP_VARIABLE( CMAKE_MODULE_PATH )
      include( ${VTK_USE_FILE} )
    endif( ${VTK_FULL_NAME}_FOUND )
  else( EXISTS ${THIRDPARTY_BUILD_FOLDERS} )
    LOG_ERROR( "Needed THIRDPARTY_BUILD_FOLDERS points to ${THIRDPARTY_BUILD_FOLDERS} but it does not exist." )
    FATAL_ERROR()
  endif( EXISTS ${THIRDPARTY_BUILD_FOLDERS} )
endif( THIRDPARTY_BUILD_FOLDERS )