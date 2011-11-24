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

set( SRCS_GUI_QT
)

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
  include/vtkTemporalDataSetTimeStepProvider.h
  src/vtkTemporalDataSetTimeStepProvider.cxx
  #msvVolumeRenderFactory
  #msvGraphicsFactory
  )

add_executable( wxVolumeRenderingTF ${COMMON_APPLICATION_EXECUTABLE_TYPE_CMAKE} ${SRCS_VolumeRenderingTF} )

install( TARGETS
  wxVolumeRenderingTF
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


