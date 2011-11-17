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


#ifndef __VTKMULTIPLEDATAREADER_H__
#define __VTKMULTIPLEDATAREADER_H__

#include "vtkTemporalDataSetAlgorithm.h"

#include <vector>

class vtkStdString;
class vtkStringArray;
class vtkDataReader;

class vtkMultipleDataReader : public vtkTemporalDataSetAlgorithm
{
public:
  static vtkMultipleDataReader *New();
  vtkTypeMacro( vtkMultipleDataReader, vtkTemporalDataSetAlgorithm );
  void PrintSelf( ostream& os, vtkIndent indent );

  // Description:
  // Specify the directory of the file set to read.
  vtkSetStringMacro(DirectoryName);
  vtkGetStringMacro(DirectoryName);

  // Description:
  // Specify the wildcard of the files set to read.
  vtkSetStringMacro(WildCard);
  vtkGetStringMacro(WildCard);

  // Description:
  // Specify the timesteps to be catched.
  vtkSetMacro(CachedTimeSteps, int);
  vtkGetMacro(CachedTimeSteps, int)

  // Description:
  // Set/Get the time steps the user wants to load
  void SetRequestedTimesTeps( double* timeSteps, int numTimeSteps );
  double* GetRequestedTimesTeps();

  // Description:
  // Read the meta information from the list of files in the directory
  virtual int ReadMetaData( vtkInformation* );

  int GetDataObjectType( const vtkStdString& fileName );
  int GetSeriesDataObjectType( vtkStringArray* series );

  void SetFileNames( vtkStringArray* fileNames );
  vtkStringArray* GetFileNames();

  // Will return the available number of time steps that can be read,
  // not the actually stored in the vtkTemporalDataSet
  int GetAvailableTimeSteps();

protected:
  vtkMultipleDataReader();
  ~vtkMultipleDataReader();

  virtual int ProcessRequest( vtkInformation*, vtkInformationVector**,
                              vtkInformationVector* );
  virtual int RequestData( vtkInformation*, vtkInformationVector**,
                           vtkInformationVector* );

  virtual int RequestUpdateExtent(vtkInformation*, vtkInformationVector**,
                                  vtkInformationVector* );

  virtual int RequestInformation( vtkInformation*, vtkInformationVector**,
                                  vtkInformationVector* );

  // Get a concrete instance of the reader
  virtual vtkDataReader* GetConcreteReaderInstance();

  // Sets in the output data of type vtkTemporalDataSet the time step data read from single file
  virtual void SetOutputTimeStep( vtkDataReader* pDataReader, vtkTemporalDataSet *outputData, int timeStep );

private:
  vtkMultipleDataReader( const vtkMultipleDataReader& );  // Not implemented.
  void operator=( const vtkMultipleDataReader& );  // Not implemented.

protected:
    char* DirectoryName;
    char* WildCard;
    vtkStringArray* FileNames;
    int CachedTimeSteps;	// 0 means all
    double* m_RequestedTimeSteps;
    int m_NumRequestedTimeSteps;
    int m_DataObjectType;
    int m_NumAvailableTimeSteps;

};

#endif	// #ifndef __VTKMULTIPLEDATAREADER_H__
