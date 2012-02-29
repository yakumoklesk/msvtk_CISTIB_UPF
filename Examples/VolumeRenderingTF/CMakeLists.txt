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

project( VolumeRenderingTF CXX )

# Should not be configured if already configured in Examples/CMakeLists.txt
# Setting configuration file source and destination
configure_file(
  "${PROJECT_SOURCE_DIR}/../config/BuildConfig.h.in"
  "${PROJECT_BINARY_DIR}/BuildConfig.h"
)

# Add the directory where the output configuration file is
include_directories( "${PROJECT_BINARY_DIR}" )

include_directories(
  ${PROJECT_SOURCE_DIR}/../common/include
)

set( SRCS_Common
  ${PROJECT_SOURCE_DIR}/../common/include/VtkExtTypes.h
  ${PROJECT_BINARY_DIR}/BuildConfig.h
)

set( VOLUME_RENDERING_TEST 0 CACHE BOOL "Build all of own tests." )

#Add our own include path
include_directories(
  #${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/GUI
  )

set( SRCS_GUI_WX
  GUI/wxMainFrameUI.cxx
  GUI/wxMainFrameUI.h
  #GUI/VolumeRenderingPanelUI.h
  #GUI/VolumeRenderingPanelUI.cpp
  #VolumeRenderingPanel.h
  #VolumeRenderingPanel.cpp
  GUI/wxVolumeRenderingPanelUI.h
  GUI/wxVolumeRenderingPanelUI.cxx
  GUI/wxSliderDoubleUI.h
  GUI/wxSliderDoubleUI.cxx
  GUI/wxMiscToolsPanelUI.h
  GUI/wxMiscToolsPanelUI.cxx
  include/msvWxApp.h
  src/msvWxApp.cxx
  include/wxMainFrame.h
  src/wxMainFrame.cxx
  include/wxVolumeRenderingPanel.h
  src/wxVolumeRenderingPanel.cxx
  include/wxSliderDoubleWidget.h
  src/wxSliderDoubleWidget.cxx
  include/wxMiscToolsPanel.h
  src/wxMiscToolsPanel.cxx
)

if( GUI_API EQUAL GUI_API_QT )

LOG_DEBUG( "Wrapping all the QT code" )


set( SRCS_CXX_QT
  src/msvQApp.cxx
)

set( SRCS_MOC_QT
  include/msvQApp.h
)

set( SRCS_HEADERS_QT
  ${SRCS_MOC_QT}
)

QT4_WRAP_CPP( SRCS_CXX_QT ${SRCS_MOC_QT} )

set( SRCS_GUI_QT
  ${SRCS_HEADERS_QT}
  ${SRCS_CXX_QT}
)

endif()

if( GUI_API EQUAL GUI_API_WXWIDGETS )
  set( SRCS_GUI ${SRCS_GUI_WX} )
elseif( GUI_API EQUAL GUI_API_QT )
  set( SRCS_GUI ${SRCS_GUI_QT} )
endif()

set( SRCS_VolumeRenderingTF
  ${SRCS_Common}
  #../src/wxVTKRenderWindowInteractor.cxx
  include/msvApp.h
  src/msvApp.cxx
  include/MainApp.h
  src/MainApp.cxx
  ${SRCS_GUI}
  include/msvVTKRTRenderWindowInteractor.h
  src/msvVTKRTRenderWindowInteractor.cxx
  include/msvEntity.h
  src/msvEntity.cxx
  include/msvEntityMgr.h
  src/msvEntityMgr.cxx
  include/msvObjectFactory.h
  src/msvObjectFactory.cxx
  include/vtkMultipleDataReader.h
  src/vtkMultipleDataReader.cxx
  include/vtkMultipleStructuredPointsReader.h
  src/vtkMultipleStructuredPointsReader.cxx
  include/vtkMultiplePolyDataReader.h
  src/vtkMultiplePolyDataReader.cxx
  include/msvThreadSafeGetSet.h
  include/vtkTemporalDataSetTimeStepProvider.h
  src/vtkTemporalDataSetTimeStepProvider.cxx
  include/vtkThreadSafeRendererWrapper.h
  #src/vtkThreadSafeRendererWrapper.cxx
  #include/vtkThreadSafeRenderer.h
  #src/vtkThreadSafeRenderer.cxx
  #include/vtkThreadSafeRenderWindow.h
  #src/vtkThreadSafeRenderWindow.cxx
  #include/vtkThreadSafeRenderWindowInteractor.h
  #src/vtkThreadSafeRenderWindowInteractor.cxx
  #msvVolumeRenderFactory
  #msvGraphicsFactory
  )

LOG_DEBUG( "COMMON_APPLICATION_EXECUTABLE_TYPE_CMAKE ${COMMON_APPLICATION_EXECUTABLE_TYPE_CMAKE}" )
  
add_executable( ${PROJECT_NAME} ${COMMON_APPLICATION_EXECUTABLE_TYPE_CMAKE} ${SRCS_VolumeRenderingTF} )
target_link_libraries( ${PROJECT_NAME} ${QT_LIBRARIES} )

install( TARGETS
  ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib/static
)
  
if( VOLUME_RENDERING_TEST )
  if( GTEST_FOUND )
    LOG_INFO( "Enabled Volume Rendering tests" )
    enable_testing()
    include_directories( ${GTEST_INCLUDE_DIRS} )
    add_subdirectory( tests )
  endif( GTEST_FOUND )
endif( VOLUME_RENDERING_TEST )


