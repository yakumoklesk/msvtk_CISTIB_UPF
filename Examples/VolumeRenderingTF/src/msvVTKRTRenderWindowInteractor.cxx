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

#include "msvVTKRTRenderWindowInteractor.h"

#include <vtkObjectFactory.h>

vtkStandardNewMacro( msvVTKRTRenderWindowInteractor );


msvVTKRTRenderWindowInteractor::msvVTKRTRenderWindowInteractor()
{

}

msvVTKRTRenderWindowInteractor::~msvVTKRTRenderWindowInteractor()
{

}

void msvVTKRTRenderWindowInteractor::Start()
{
    MSG msg;
    //while( !exit )
    while( 1 )
    {
        //if( PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE )
        if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
    }
}

void msvVTKRTRenderWindowInteractor::PrintSelf( ostream& os, vtkIndent indent )
{
    this->Superclass::PrintSelf( os, indent );
}

