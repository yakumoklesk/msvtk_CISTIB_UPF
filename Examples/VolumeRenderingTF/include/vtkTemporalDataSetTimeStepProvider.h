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


#ifndef __VTKTEMPORALDATASETTIMESTEPPROVIDER_H__
#define __VTKTEMPORALDATASETTIMESTEPPROVIDER_H__

#include "vtkTemporalDataSetCache.h"

#include <vector>

class vtkTemporalDataSetTimeStepProvider : public vtkTemporalDataSetCache
{
public:
  static vtkTemporalDataSetTimeStepProvider *New();
  vtkTypeMacro( vtkTemporalDataSetTimeStepProvider, vtkTemporalDataSetCache );
  void PrintSelf( ostream& os, vtkIndent indent );

  // Description:
  // Specify the next time step to be retrieved
  vtkSetMacro(NextTimeStep, int);
  /*/
  void SetNextTimeStep( int NextTimeStep )
  {
    if( NextTimeStep != this->NextTimeStep )
    {
        this->NextTimeStep = NextTimeStep;

    }
  };
  /*/
  vtkGetMacro(NextTimeStep, int);

protected:
  vtkTemporalDataSetTimeStepProvider();
  ~vtkTemporalDataSetTimeStepProvider();

  int NextTimeStep;

  virtual int RequestUpdateExtent(vtkInformation*, vtkInformationVector** ,
                                  vtkInformationVector* );

  virtual int FillOutputPortInformation(int port, vtkInformation* info);

private:
  vtkTemporalDataSetTimeStepProvider( const vtkTemporalDataSetTimeStepProvider& );  // Not implemented.
  void operator=( const vtkTemporalDataSetTimeStepProvider& );  // Not implemented.
};

#endif	// #ifndef __VTKTEMPORALDATASETTIMESTEPPROVIDER_H__
