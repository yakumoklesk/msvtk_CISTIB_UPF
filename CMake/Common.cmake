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

#-----------------------------------------------------------------------------
# Update CMake module path
# Note: FindXXX.cmake script specific to utility should be copied into Utilities/CMake
#
#"${CMAKE_CURRENT_SOURCE_DIR}/Utilities/CMake"



#message( "CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}" )

if( NOT CMAKE_MODULE_PATH )
  set( CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake"
    ${CMAKE_MODULE_PATH}
  )
endif()

include( Logging )


set( COMMON_APPLICATION_EXECUTABLE_TYPE_NUMERIC_CONSOLE 0 )
set( COMMON_APPLICATION_EXECUTABLE_TYPE_NUMERIC_GUI 1 )

set( COMMON_APPLICATION_EXECUTABLE_TYPE_CONSOLE "CONSOLE" )
set( COMMON_APPLICATION_EXECUTABLE_TYPE_GUI "GUI" )
set( GUI_API_WXWIDGETS ${GUI_API_WXWIDGETS_CONFIG} )

set( APPLICATION_EXECUTABLE_TYPE_CONSOLE_CONFIG 0 )
set( APPLICATION_EXECUTABLE_TYPE_GUI_CONFIG     1 )
set( APPLICATION_EXECUTABLE_TYPE_CONSOLE        ${APPLICATION_EXECUTABLE_TYPE_CONSOLE_CONFIG} )
set( APPLICATION_EXECUTABLE_TYPE_GUI            ${APPLICATION_EXECUTABLE_TYPE_GUI_CONFIG} )

set( COMMON_APPLICATION_EXECUTABLE_TYPE ${COMMON_APPLICATION_EXECUTABLE_TYPE_GUI} CACHE STRING "Set the type of application executable to be built" )
set_property( CACHE COMMON_APPLICATION_EXECUTABLE_TYPE PROPERTY STRINGS ${COMMON_APPLICATION_EXECUTABLE_TYPE_CONSOLE} ${COMMON_APPLICATION_EXECUTABLE_TYPE_GUI} )

if( WIN32 )
  if( COMMON_APPLICATION_EXECUTABLE_TYPE STREQUAL COMMON_APPLICATION_EXECUTABLE_TYPE_CONSOLE )
    set( APPLICATION_EXECUTABLE_TYPE_CONFIG ${APPLICATION_EXECUTABLE_TYPE_CONSOLE} )
    set( COMMON_APPLICATION_EXECUTABLE_TYPE_CMAKE "" )
  elseif( COMMON_APPLICATION_EXECUTABLE_TYPE STREQUAL COMMON_APPLICATION_EXECUTABLE_TYPE_GUI )
    set( APPLICATION_EXECUTABLE_TYPE_CONFIG ${APPLICATION_EXECUTABLE_TYPE_GUI} )
    set( COMMON_APPLICATION_EXECUTABLE_TYPE_CMAKE WIN32 )
  endif()
endif()

LOG_DEBUG( "Common.cmake: APPLICATION_EXECUTABLE_TYPE_CONFIG ${APPLICATION_EXECUTABLE_TYPE_CONFIG}" )
LOG_DEBUG( "Common.cmake: COMMON_APPLICATION_EXECUTABLE_TYPE_CMAKE ${COMMON_APPLICATION_EXECUTABLE_TYPE_CMAKE}" )

set( _EMPTY_STACK_TOP_ "Stack_Top" )

set( COMMON_DEBUG_PUSH_VARIABLE FALSE )

if( COMMON_DEBUG_PUSH_VARIABLE )
  FUNCTION( PUSH_VARIABLE variable )
# When pushing a variable, if that variable is a list, then the lenght of the list must be
# also pushed
	LOG_DEBUG( "PUSHING ${variable}" )
    LOG_DEBUG( "_STACK_${variable}_ ${_STACK_${variable}_}" )
    if( NOT _STACK_${variable}_ )
      set( CURRENT_STACK ${_STACK_${variable}_} )
      list( APPEND CURRENT_STACK ${_EMPTY_STACK_TOP_} )
    else()
      set( CURRENT_STACK ${_STACK_${variable}_} )
    endif()
	LOG_DEBUG( "CURRENT_STACK ${CURRENT_STACK}" )
    list( APPEND CURRENT_STACK  ${${variable}} )
	list( LENGTH ${variable} CURRENT_VARIABLE_LIST_LENGTH )
	LOG_DEBUG( "CURRENT_VARIABLE_LIST_LENGTH ${CURRENT_VARIABLE_LIST_LENGTH}" )
	list( APPEND CURRENT_STACK  ${CURRENT_VARIABLE_LIST_LENGTH} )
	LOG_DEBUG( "CURRENT_STACK ${CURRENT_STACK}" )
    set( _STACK_${variable}_ ${CURRENT_STACK} PARENT_SCOPE )
  ENDFUNCTION()
  FUNCTION( POP_VARIABLE variable )
    LOG_DEBUG( "POPPING ${variable}" )
	LOG_DEBUG( "_STACK_${variable}_ ${_STACK_${variable}_}" )
    if( NOT _STACK_${variable}_ )
      LOG_ERROR( "_STACK_${variable}_ is not defined" )
      return()
      set( ${variable} "" PARENT_SCOPE )
    endif()
    set( CURRENT_STACK ${_STACK_${variable}_} )
    list( LENGTH CURRENT_STACK STACK_LENGTH )
    LOG_DEBUG( "STACK_LENGTH ${STACK_LENGTH}" )
    math( EXPR STACK_ELEMENT_INDEX "${STACK_LENGTH} - 1" )
    LOG_DEBUG( "STACK_ELEMENT_INDEX ${STACK_ELEMENT_INDEX}" )
    list( GET CURRENT_STACK ${STACK_ELEMENT_INDEX} ELEMENTS_TO_POP )
    LOG_DEBUG( "ELEMENTS_TO_POP ${ELEMENTS_TO_POP}" )
	list( REMOVE_AT CURRENT_STACK ${STACK_ELEMENT_INDEX} )
	set( POPPED_ELEMENTS "" )
	while( ELEMENTS_TO_POP GREATER 0 )
	  math( EXPR STACK_ELEMENT_INDEX "${STACK_ELEMENT_INDEX} - 1" )
	  list( GET CURRENT_STACK ${STACK_ELEMENT_INDEX} ELEMENT_POPPED )
	  list( INSERT POPPED_ELEMENTS 0 ${ELEMENT_POPPED} )
	  math( EXPR ELEMENTS_TO_POP "${ELEMENTS_TO_POP} - 1" )
	endwhile()
	LOG_DEBUG( "POPPED_ELEMENTS ${POPPED_ELEMENTS}" )
    
    #message( "CURRENT_STACK ${CURRENT_STACK}" )
    set( _STACK_${variable}_ ${CURRENT_STACK} PARENT_SCOPE )
    set( ${variable} ${POPPED_ELEMENTS} PARENT_SCOPE )
  ENDFUNCTION()

else( COMMON_DEBUG_PUSH_VARIABLE )  

  FUNCTION( PUSH_VARIABLE variable )
# When pushing a variable, if that variable is a list, then the lenght of the list must be
# also pushed
    if( NOT _STACK_${variable}_ )
      set( CURRENT_STACK ${_STACK_${variable}_} )
      list( APPEND CURRENT_STACK ${_EMPTY_STACK_TOP_} )
    else()
      set( CURRENT_STACK ${_STACK_${variable}_} )
    endif()
    list( APPEND CURRENT_STACK  ${${variable}} )
	list( LENGTH ${variable} CURRENT_VARIABLE_LIST_LENGTH )
	list( APPEND CURRENT_STACK  ${CURRENT_VARIABLE_LIST_LENGTH} )
    set( _STACK_${variable}_ ${CURRENT_STACK} PARENT_SCOPE )
  ENDFUNCTION()
  FUNCTION( POP_VARIABLE variable )
    if( NOT _STACK_${variable}_ )
      return()
      set( ${variable} "" PARENT_SCOPE )
    endif()
    set( CURRENT_STACK ${_STACK_${variable}_} )
    list( LENGTH CURRENT_STACK STACK_LENGTH )
    math( EXPR STACK_ELEMENT_INDEX "${STACK_LENGTH} - 1" )
    list( GET CURRENT_STACK ${STACK_ELEMENT_INDEX} ELEMENTS_TO_POP )
	list( REMOVE_AT CURRENT_STACK ${STACK_ELEMENT_INDEX} )
	set( POPPED_ELEMENTS "" )
	while( ELEMENTS_TO_POP GREATER 0 )
	  math( EXPR STACK_ELEMENT_INDEX "${STACK_ELEMENT_INDEX} - 1" )
	  list( GET CURRENT_STACK ${STACK_ELEMENT_INDEX} ELEMENT_POPPED )
	  list( INSERT POPPED_ELEMENTS 0 ${ELEMENT_POPPED} )
	  math( EXPR ELEMENTS_TO_POP "${ELEMENTS_TO_POP} - 1" )
	endwhile()
    
    #message( "CURRENT_STACK ${CURRENT_STACK}" )
    set( _STACK_${variable}_ ${CURRENT_STACK} PARENT_SCOPE )
    set( ${variable} ${POPPED_ELEMENTS} PARENT_SCOPE )
  ENDFUNCTION()
endif()



#set( test_var "1" )
#PUSH_VARIABLE( test_var )
#message( "1: test_var ${test_var}" )
#message( "1: _STACK_test_var_ ${_STACK_test_var_}" )
#set( test_var "2" )
#PUSH_VARIABLE( test_var )
#message( "2: test_var ${test_var}" )
#message( "2: _STACK_test_var_ ${_STACK_test_var_}" )
#set( test_var "3" )
#message( "3: test_var ${test_var}" )
#message( "3: _STACK_test_var_ ${_STACK_test_var_}" )
#POP_VARIABLE( test_var )
#message( "4: test_var ${test_var}" )
#message( "4: _STACK_test_var_ ${_STACK_test_var_}" )
#POP_VARIABLE( test_var )
#message( "5: test_var ${test_var}" )
#message( "5: _STACK_test_var_ ${_STACK_test_var_}" )
  
FUNCTION( FATAL_ERROR )
  message( FATAL_ERROR "" )
ENDFUNCTION()
# Configure THIRDPARTY_BUILD_FOLDERS -->

#UNSET( THIRDPARTY_BUILD_FOLDERS CACHE FORCE )

set( BUILD_FROM_UNKNOWN -1 )
set( BUILD_FROM_WORK 0 )
set( BUILD_FROM_HOME 1 )

if( $ENV{USERNAME} STREQUAL "DLucena" )
    set( DEFAULT_THIRDPARTY_BUILD_FOLDERS "F:/dev/prj/UsingVTK_bin/build32/thirdParty" )
    set( Build_Location_CONFIG ${BUILD_FROM_WORK} )
    LOG_DEBUG( "Common.cmake: You're at work!" )
elseif( $ENV{USERNAME} STREQUAL "yakumo" )
    set( DEFAULT_THIRDPARTY_BUILD_FOLDERS "C:/dev/prj/job/UsingVTK_bin/build32/thirdParty" )
    set( Build_Location_CONFIG ${BUILD_FROM_HOME} )
    LOG_DEBUG( "Common.cmake: You're at home!" )
else( $ENV{USERNAME} STREQUAL "DLucena" )
    LOG_DEBUG( "Common.cmake: Where the fuck are you!" )
    LOG_DEBUG( "Common.cmake: Username environment variable is $ENV{USERNAME}" )
    set( Build_Location_CONFIG ${BUILD_FROM_UNKNOWN} )
endif()

if( NOT THIRDPARTY_BUILD_FOLDERS )
  LOG_DEBUG( "Common.cmake: Setting THIRDPARTY_BUILD_FOLDERS to default value ${DEFAULT_THIRDPARTY_BUILD_FOLDERS}" )
  set( THIRDPARTY_BUILD_FOLDERS ${DEFAULT_THIRDPARTY_BUILD_FOLDERS} )
endif( NOT THIRDPARTY_BUILD_FOLDERS )

if( NOT DEFINED THIRDPARTY_BUILD_FOLDERS )
  LOG_ERROR( "Common.cmake: Setting THIRDPARTY_BUILD_FOLDERS defaulting to ${DEFAULT_THIRDPARTY_BUILD_FOLDERS}" )
  set( THIRDPARTY_BUILD_FOLDERS ${DEFAULT_THIRDPARTY_BUILD_FOLDERS} )
endif()

# Configure THIRDPARTY_BUILD_FOLDERS <--