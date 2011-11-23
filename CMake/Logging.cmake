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

set( LOG_VERBOSELEVEL_ALL "All" )
set( LOG_VERBOSELEVEL_INFO "Info" )
set( LOG_VERBOSELEVEL_DEBUG "Debug" )
set( LOG_VERBOSELEVEL_WARNING "Warning" )
set( LOG_VERBOSELEVEL_ERROR "Error" )

set( LOG_VERBOSELEVEL_NUMERIC_ALL 0 )
set( LOG_VERBOSELEVEL_NUMERIC_INFO 1 )
set( LOG_VERBOSELEVEL_NUMERIC_DEBUG 2 )
set( LOG_VERBOSELEVEL_NUMERIC_WARNING 3 )
set( LOG_VERBOSELEVEL_NUMERIC_ERROR 4 )

set( LOG_VERBOSELEVEL ${LOG_VERBOSELEVEL_ALL} CACHE STRING "Set the LOG verbose level." )
set_property( CACHE LOG_VERBOSELEVEL PROPERTY STRINGS ${LOG_VERBOSELEVEL_ALL} ${LOG_VERBOSELEVEL_INFO} ${LOG_VERBOSELEVEL_DEBUG} ${LOG_VERBOSELEVEL_WARNING} ${LOG_VERBOSELEVEL_ERROR} )


FUNCTION( LOG_VERBOSELEVEL_TO_NUMERIC verboseLevel numericVerboseLevel )

  if( verboseLevel STREQUAL LOG_VERBOSELEVEL_ALL )
    set( ${numericVerboseLevel} ${LOG_VERBOSELEVEL_NUMERIC_ALL} PARENT_SCOPE )
  elseif( verboseLevel STREQUAL LOG_VERBOSELEVEL_INFO )
    set( ${numericVerboseLevel} ${LOG_VERBOSELEVEL_NUMERIC_INFO} PARENT_SCOPE )
  elseif( verboseLevel STREQUAL LOG_VERBOSELEVEL_DEBUG )
    set( ${numericVerboseLevel} ${LOG_VERBOSELEVEL_NUMERIC_DEBUG} PARENT_SCOPE )
  elseif( verboseLevel STREQUAL LOG_VERBOSELEVEL_WARNING )
    set( ${numericVerboseLevel} ${LOG_VERBOSELEVEL_NUMERIC_WARNING} PARENT_SCOPE )
  elseif( verboseLevel STREQUAL LOG_VERBOSELEVEL_ERROR )
    set( ${numericVerboseLevel} ${LOG_VERBOSELEVEL_NUMERIC_ERROR} PARENT_SCOPE )
  else()
    message( "WARNING: Log verbose level not specified, defaulting to ${LOG_VERBOSELEVEL_ERROR}" )
    set( ${numericVerboseLevel} ${LOG_VERBOSELEVEL_NUMERIC_ERROR} PARENT_SCOPE )
  endif()

ENDFUNCTION()


#------------------------
#--- _LOG ( Private ) ---
#------------------------
FUNCTION( _LOG outputString verboseLevel )

  LOG_VERBOSELEVEL_TO_NUMERIC( ${verboseLevel} numericVerboseLevel )
  LOG_VERBOSELEVEL_TO_NUMERIC( ${LOG_VERBOSELEVEL} numericConfiguredVerboseLevel )

  #message( "verboseLevel ${verboseLevel}" )
  #message( "numericVerboseLevel ${numericVerboseLevel}" )
  #message( "numericConfiguredVerboseLevel ${numericConfiguredVerboseLevel}" )
  
  if( numericVerboseLevel LESS numericConfiguredVerboseLevel )
    return()
  endif()
  
  if( numericVerboseLevel EQUAL LOG_VERBOSELEVEL_NUMERIC_ALL )
    set( LOG_START_STRING "Log:" )
  elseif( numericVerboseLevel EQUAL LOG_VERBOSELEVEL_NUMERIC_INFO )
    set( LOG_START_STRING "Info:" )
  elseif( numericVerboseLevel EQUAL LOG_VERBOSELEVEL_NUMERIC_DEBUG )
    set( LOG_START_STRING "Debug:" )
  elseif( numericVerboseLevel EQUAL LOG_VERBOSELEVEL_NUMERIC_WARNING )
    set( LOG_START_STRING "Warning:" )
  elseif( numericVerboseLevel EQUAL LOG_VERBOSELEVEL_NUMERIC_ERROR )
    set( LOG_START_STRING "Error:" )
  endif()

  set( LOG_OUTPUT_STRING "${LOG_START_STRING} ${outputString}" )
  if( numericVerboseLevel GREATER LOG_VERBOSELEVEL_NUMERIC_DEBUG )
    message( "${LOG_OUTPUT_STRING}" )
  else()
    message( STATUS "${LOG_OUTPUT_STRING}" )
  endif()

ENDFUNCTION()


#-----------
#--- LOG ---
#-----------
FUNCTION( LOG outputString )
  _LOG( "${outputString}" ${LOG_VERBOSELEVEL_ALL} )
ENDFUNCTION()

#----------------
#--- LOG_INFO ---
#----------------
FUNCTION( LOG_INFO outputString )
  _LOG( "${outputString}" ${LOG_VERBOSELEVEL_INFO} )
ENDFUNCTION()

#-----------------
#--- LOG_DEBUG ---
#-----------------
FUNCTION( LOG_DEBUG outputString )
  _LOG( "${outputString}" ${LOG_VERBOSELEVEL_DEBUG} )
ENDFUNCTION()

#-------------------
#--- LOG_WARNING ---
#-------------------
FUNCTION( LOG_WARNING outputString )
  _LOG( "${outputString}" ${LOG_VERBOSELEVEL_WARNING} )
ENDFUNCTION()

#-----------------
#--- LOG_ERROR ---
#-----------------
FUNCTION( LOG_ERROR outputString )
  _LOG( "${outputString}" ${LOG_VERBOSELEVEL_ERROR} )
ENDFUNCTION()

# Examples
#LOG( "This is log message" )
#LOG_INFO( "This is an information log message" )
#LOG_DEBUG( "This is a debug log message" )
#LOG_WARNING( "Tbis is a warning log message" )
#LOG_ERROr( "Tbis is an error log message" )


