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

#ifndef __VTKMULTIPLEPOLYDATAREADER_H__
#define __VTKMULTIPLEPOLYDATAREADER_H__

#include "vtkMultipleDataReader.h"

#include <vector>

class vtkPolyData;
class vtkTemporalDataSet;

class vtkMultiplePolyDataReader : public vtkMultipleDataReader
{
public:
    static vtkMultiplePolyDataReader *New();
    vtkTypeMacro( vtkMultiplePolyDataReader, vtkMultipleDataReader );
    void PrintSelf( ostream& os, vtkIndent indent );

    // Description:
    // Set/Get the output of this reader.
    void SetOutput( vtkTemporalDataSet* output );
    vtkTemporalDataSet* GetOutput( int idx );
    vtkTemporalDataSet* GetOutput();

protected:
    vtkMultiplePolyDataReader();
    virtual ~vtkMultiplePolyDataReader();


    virtual vtkDataReader* GetConcreteReaderInstance();

    virtual void SetOutputTimeStep( vtkDataReader* pDataReader, vtkTemporalDataSet *outputData, int timeStep );

private:
    vtkMultiplePolyDataReader( const vtkMultiplePolyDataReader& );  // Not implemented.
    void operator=( const vtkMultiplePolyDataReader& );  // Not implemented.

private:

};

#endif	// #ifndef __VTKMULTIPLESTRUCTUREDPOINTSREADER_H__
