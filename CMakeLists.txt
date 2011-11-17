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

project( MSVTK_CISTIB_UPF CXX )

# Require CMake 2.6 or higher.
cmake_minimum_required( VERSION 2.8.5 )

include( CMake/Common.cmake )

set( ${PROJECT_NAME}_LIBRARY_DIR ${MSVTK_CISTIB_UPF_BINARY_DIR}/lib )
set( ${PROJECT_NAME}_EXECUTABLE_DIR ${MSVTK_CISTIB_UPF_BINARY_DIR}/bin )
set( PARENT_PROJECT_LIBRARY_DIR ${${PROJECT_NAME}_LIBRARY_DIR} )
set( PARENT_PROJECT_EXECUTABLE_DIR ${${PROJECT_NAME}_EXECUTABLE_DIR} )

add_subdirectory( Examples )
