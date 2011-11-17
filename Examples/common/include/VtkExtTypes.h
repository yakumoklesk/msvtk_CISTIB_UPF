/*==============================================================================

  Library: MSVTK

  Copyright (c) Computational Image and Simulation Technologies in Biomedicine (CISTIB),
                Universitat Pompeu Fabra (UPF), Barcelona, Spain

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

#ifndef _VTKEXTYPES_H_
#define _VTKEXTYPES_H_

class vtkStructuredPoints;
class vtkStructuredPointsReader;

class vtkSphereSource;
class vtkConeSource;

class vtkActor;
class vtkPolyData;
class vtkPolyDataMapper;

class vtkVolume;
class vtkVolumeMapper;
class vtkVolumeReader;
class vtkVolumeTextureMapper3D;
class vtkGPUVolumeRayCastMapper;
class vtkVolumeProperty;
class vtkColorTransferFunction;
class vtkPiecewiseFunction;

class vtkAnimationScene;
class vtkAnimationCue;
class AnimationSceneObserver;

class vtkRenderer;
class vtkRenderWindow;
class vtkRenderWindowInteractor;


class wxVTKRenderWindowInteractor;

#include <vtkSmartPointer.h>

typedef vtkSmartPointer<vtkStructuredPoints>        vtkStructuredPointsSP;
typedef vtkSmartPointer<vtkStructuredPointsReader>  vtkStructuredPointsReaderSP;

typedef vtkSmartPointer<vtkSphereSource>            vtkSphereSourceSP;
typedef vtkSmartPointer<vtkConeSource>              vtkConeSourceSP;

typedef vtkSmartPointer<vtkActor>                   vtkActorSP;
typedef vtkSmartPointer<vtkPolyData>                vtkPolyDataSP;
typedef vtkSmartPointer<vtkPolyDataMapper>          vtkPolyDataMapperSP;

typedef vtkSmartPointer<vtkVolume>                  vtkVolumeSP;
typedef vtkSmartPointer<vtkVolumeMapper>            vtkVolumeMapperSP;
typedef vtkSmartPointer<vtkVolumeTextureMapper3D>   vtkVolumeTextureMapper3DSP;
typedef vtkSmartPointer<vtkGPUVolumeRayCastMapper>  vtkGPUVolumeRayCastMapperSP;
typedef vtkSmartPointer<vtkVolumeProperty>          vtkVolumePropertySP;
typedef vtkSmartPointer<vtkColorTransferFunction>   vtkColorTransferFunctionSP;
typedef vtkSmartPointer<vtkPiecewiseFunction>       vtkPiecewiseFunctionSP;

typedef vtkSmartPointer<vtkAnimationScene>          vtkAnimationSceneSP;
typedef vtkSmartPointer<vtkAnimationCue>            vtkAnimationCueSP;
typedef vtkSmartPointer<AnimationSceneObserver>     AnimationSceneObserverSP;

typedef vtkSmartPointer<vtkRenderer>                vtkRendererSP;
typedef vtkSmartPointer<vtkRenderWindow>            vtkRenderWindowSP;
typedef vtkSmartPointer<vtkRenderWindowInteractor>  vtkRenderWindowInteractorSP;



#endif  // #ifndef _VTKEXTYPES_H_
