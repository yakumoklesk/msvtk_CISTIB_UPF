/*==============================================================================

  Library: MSVTK

  Copyright (c) Kitware Inc.

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

/*=========================================================================

  Program:   ParaView
  Module:    vtkFileSeriesReader.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

/*
 * Copyright 2008 Sandia Corporation.
 * Under the terms of Contract DE-AC04-94AL85000, there is a non-exclusive
 * license for use of this work by or on behalf of the
 * U.S. Government. Redistribution and use in source and binary forms, with
 * or without modification, are permitted provided that this Notice and any
 * statement of authorship are reproduced on all copies.
 */

#include "msvVTKFileSeriesReader.h"

#include "vtkGenericDataObjectReader.h"
#include "vtkInformation.h"
#include "vtkInformationIntegerKey.h"
#include "vtkInformationVector.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkStdString.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkStringArray.h"
#include "vtkTypeTraits.h"

#include "vtkSmartPointer.h"
#define VTK_CREATE(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

#include <vtkstd/algorithm>
#include <vtkstd/map>
#include <vtkstd/set>
#include <vtkstd/string>
#include <vtkstd/vector>

//=============================================================================
vtkCxxSetObjectMacro(msvVTKFileSeriesReader,Reader,vtkAlgorithm);

//=============================================================================
// Internal class for holding time ranges.
class msvVTKFileSeriesReaderTimeRanges
{
public:
  msvVTKFileSeriesReaderTimeRanges();
  void Reset();
  void AddTimeRange(int index, vtkInformation *srcInfo);
  void SetTimeRange(int index, double* range);
  void GetTimeRange(double timeRange[2]);
  void SetOutputTimeRange(double range[2]);
  void GetOutputTimeRange(double range[2]);
  void ResetRangeMap();
  int GetAggregateTimeInfo(vtkInformation *outInfo);
  int GetInputTimeInfo(int index, vtkInformation *outInfo);
  int GetIndexForTime(double time);
  vtkstd::set<int> ChooseInputs(vtkInformation *outInfo);
  vtkstd::vector<double> GetTimesForInput(int inputId, vtkInformation *outInfo);

private:
  static vtkInformationIntegerKey *INDEX();
  typedef vtkstd::map<double, vtkSmartPointer<vtkInformation> > RangeMapType;
  RangeMapType RangeMap;
  double outputTimeRange[2];
  vtkstd::map<int, vtkSmartPointer<vtkInformation> > InputLookup;
};

vtkInformationKeyMacro(msvVTKFileSeriesReaderTimeRanges, INDEX, Integer);

//-----------------------------------------------------------------------------
msvVTKFileSeriesReaderTimeRanges::msvVTKFileSeriesReaderTimeRanges()
{
  this->outputTimeRange[0] = 0.;
  this->outputTimeRange[1] = -1.;
}

//-----------------------------------------------------------------------------
void msvVTKFileSeriesReaderTimeRanges::Reset()
{
  this->ResetRangeMap();
  this->InputLookup.clear();
}

//-----------------------------------------------------------------------------
void msvVTKFileSeriesReaderTimeRanges::AddTimeRange(int index,
                                                 vtkInformation *srcInfo)
{
  VTK_CREATE(vtkInformation, info);
  info->Set(msvVTKFileSeriesReaderTimeRanges::INDEX(), index);

  this->InputLookup[index] = info;

  if (srcInfo->Has(vtkStreamingDemandDrivenPipeline::TIME_STEPS()))
    {
    info->CopyEntry(srcInfo, vtkStreamingDemandDrivenPipeline::TIME_STEPS());
    if (srcInfo->Has(vtkStreamingDemandDrivenPipeline::TIME_RANGE()))
      {
      info->CopyEntry(srcInfo, vtkStreamingDemandDrivenPipeline::TIME_RANGE());
      }
    else
      {
      double *timeSteps
        = info->Get(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
      int numTimeSteps
        = info->Length(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
      double timeRange[2];
      timeRange[0] = timeSteps[0];
      timeRange[1] = timeSteps[numTimeSteps-1];
      info->Set(vtkStreamingDemandDrivenPipeline::TIME_RANGE(), timeRange, 2);
      }
    }
  else if (srcInfo->Has(vtkStreamingDemandDrivenPipeline::TIME_RANGE()))
    {
    info->CopyEntry(srcInfo, vtkStreamingDemandDrivenPipeline::TIME_RANGE());
    }
  else
    {
    vtkGenericWarningMacro(<< "Input with index " << index
                           << " has no time information.");
    return;
    }

  this->RangeMap[info->Get(vtkStreamingDemandDrivenPipeline::TIME_RANGE())[0]]
    = info;
}

//------------------------------------------------------------------------------
void msvVTKFileSeriesReaderTimeRanges::SetTimeRange(int index,
                                                    double* range)
{
  if (this->InputLookup.empty())
    {
    return;
    }

  vtkInformation* info;
  info = this->InputLookup[index];
  info->Remove(vtkStreamingDemandDrivenPipeline::TIME_RANGE());
  info->Set(vtkStreamingDemandDrivenPipeline::TIME_RANGE(), range, 2);
  this->RangeMap[info->Get(vtkStreamingDemandDrivenPipeline::TIME_RANGE())[0]]
    = info;
}

//------------------------------------------------------------------------------
void msvVTKFileSeriesReaderTimeRanges::GetTimeRange(double timeRange[2])
{
  timeRange[0] = this->RangeMap.begin()->second
                       ->Get(vtkStreamingDemandDrivenPipeline::TIME_RANGE())[0];
  timeRange[1] = (--this->RangeMap.end())->second
                       ->Get(vtkStreamingDemandDrivenPipeline::TIME_RANGE())[1];
}

//------------------------------------------------------------------------------
void msvVTKFileSeriesReaderTimeRanges::SetOutputTimeRange(double timeRange[2])
{
  this->outputTimeRange[0] = timeRange[0];
  this->outputTimeRange[1] = timeRange[1];
}

//------------------------------------------------------------------------------
void msvVTKFileSeriesReaderTimeRanges::GetOutputTimeRange(double timeRange[2])
{
  timeRange[0] = this->outputTimeRange[0];
  timeRange[1] = this->outputTimeRange[1];
}

//------------------------------------------------------------------------------
void msvVTKFileSeriesReaderTimeRanges::ResetRangeMap()
{
  this->RangeMap.clear();
}

//-----------------------------------------------------------------------------
int msvVTKFileSeriesReaderTimeRanges::GetAggregateTimeInfo(vtkInformation *outInfo)
{
  if (this->RangeMap.empty())
    {
    vtkGenericWarningMacro(<< "No inputs with time information.");
    return 0;
    }

  double timeRange[2];
  timeRange[0] = this->RangeMap.begin()->second
                       ->Get(vtkStreamingDemandDrivenPipeline::TIME_RANGE())[0];
  timeRange[1] = (--this->RangeMap.end())->second
                       ->Get(vtkStreamingDemandDrivenPipeline::TIME_RANGE())[1];

  // Special case: if the time range is a single value, supress it.  This is
  // most likely from a data set that is a single file with no time anyway.
  // Even if it is not, how much value added is there for a single time value?
  if (timeRange[0] >= timeRange[1])
    {
    outInfo->Remove(vtkStreamingDemandDrivenPipeline::TIME_RANGE());
    outInfo->Remove(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
    return 1;
    }

  outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_RANGE(), timeRange, 2);

  vtkstd::vector<double> timeSteps;

  RangeMapType::iterator itr = this->RangeMap.begin();
  while (itr != this->RangeMap.end())
    {
    // First, get all the time steps for this input.
    double *localTimeSteps
      = itr->second->Get(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
    int numLocalSteps
      = itr->second->Length(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
    double localEndTime = vtkTypeTraits<double>::Max();
    // Second, check where the time range for the next section begins.
    itr++;
    if (itr != this->RangeMap.end())
      {
      localEndTime
        = itr->second->Get(vtkStreamingDemandDrivenPipeline::TIME_RANGE())[0];
      }
    // Third, copy the appropriate time steps to the aggregate time steps.
    for (int i = 0; (i<numLocalSteps) && (localTimeSteps[i]<localEndTime); i++)
      {
      timeSteps.push_back(localTimeSteps[i]);
      }
    }

  if (timeSteps.size() > 0)
    {
    outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_STEPS(),
               &timeSteps[0], static_cast<int>(timeSteps.size()));
    }
  else
    {
    outInfo->Remove(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
    }
  return 1;
}

//-----------------------------------------------------------------------------
int msvVTKFileSeriesReaderTimeRanges::GetInputTimeInfo(int index,
                                                    vtkInformation *outInfo)
{
  if (this->InputLookup.find(index) == this->InputLookup.end())
    {
    // if there are no files specified, there's no time information to provide.
    return 1;
    }

  vtkInformation *storedInfo = this->InputLookup[index];
  outInfo->CopyEntry(storedInfo,vtkStreamingDemandDrivenPipeline::TIME_RANGE());
  if (storedInfo->Has(vtkStreamingDemandDrivenPipeline::TIME_STEPS()))
    {
    outInfo->CopyEntry(storedInfo,
                       vtkStreamingDemandDrivenPipeline::TIME_STEPS());
    return 1;
    }
  else
    {
    return 0;
    }
}

//-----------------------------------------------------------------------------
int msvVTKFileSeriesReaderTimeRanges::GetIndexForTime(double time)
{
  if (this->RangeMap.empty())
    {
    // It would make sense to give a warning here, but we should have already
    // warned in GetAggregateTimeInfo.  Warning here would just be annoying.
    return 0;
    }

  // This returns the item _after_ the one we want.
  RangeMapType::iterator itr = this->RangeMap.upper_bound(time);
  if (itr == this->RangeMap.begin())
    {
    // The requested time step is before any available time.  We will use it by
    // doing nothing here.
    }
  else
    {
    // Back up one to the item we really want.
    itr--;
    }

  return itr->second->Get(msvVTKFileSeriesReaderTimeRanges::INDEX());
}

//-----------------------------------------------------------------------------
vtkstd::set<int> msvVTKFileSeriesReaderTimeRanges::ChooseInputs(
                                                        vtkInformation *outInfo)
{
  vtkstd::set<int> indices;
  if(outInfo->Has(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS()))
    {
    // get the update times
    int numUpTimes =
      outInfo->Length(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS());
    double *upTimes =
      outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS());

    for (int i = 0; i < numUpTimes; i++)
      {
      indices.insert(this->GetIndexForTime(upTimes[i]));
      }
    }
  else
    {
    indices.insert(0);
    }

  return indices;
}

//-----------------------------------------------------------------------------
vtkstd::vector<double> msvVTKFileSeriesReaderTimeRanges::GetTimesForInput(
                                                        int inputId,
                                                        vtkInformation *outInfo)
{
  // Get the saved info for this input.
  vtkInformation *inInfo = this->InputLookup[inputId];

  // This is the time range that is supported by this input.
  double *supportedTimeRange
    = inInfo->Get(vtkStreamingDemandDrivenPipeline::TIME_RANGE());

  // Get the time range from which we "allow" data from this input.  The lower
  // bound is simply the bottom part of the time range of the input, unless it
  // has the smallest time values, in which case it also defines all times less
  // than that.  The upper bound is defined where the input with the next
  // highest times starts.
  double allowedTimeRange[2];
  allowedTimeRange[0] = supportedTimeRange[0];

  // Find the input with the next times.
  RangeMapType::iterator itr = this->RangeMap.upper_bound(allowedTimeRange[0]);
  if (itr != this->RangeMap.end())
    {
    allowedTimeRange[1] = itr->first;
    }
  else
    {
    // There is no next time.
    allowedTimeRange[1] = vtkTypeTraits<double>::Max();
    }

  // Adjust the begining time if we are the first time.
  if (this->RangeMap.find(allowedTimeRange[0]) == this->RangeMap.begin())
    {
    allowedTimeRange[0] = -vtkTypeTraits<double>::Max();
    }

  // Now we are finally ready to identify the times
  vtkstd::vector<double> times;

  // Get the update times
  int numUpTimes =
    outInfo->Length(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS());
  double *upTimes =
    outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS());

  for (int i = 0; i < numUpTimes; i++)
    {
    if ((upTimes[i]>=allowedTimeRange[0]) && (upTimes[i]<allowedTimeRange[1]))
      {
      // Add the time.  Clamp it to the input's supported time range in
      // case that input is clipping based on the time.
      times.push_back(vtkstd::max(supportedTimeRange[0],
                                  vtkstd::min(supportedTimeRange[1],
                                              upTimes[i])));
      }
    }

  return times;
}

//=============================================================================
struct msvVTKFileSeriesReaderInternals
{
  vtkstd::vector<vtkstd::string> FileNames;
  bool FileNameIsSet;
  msvVTKFileSeriesReaderTimeRanges *TimeRanges;
};

//=============================================================================
msvVTKFileSeriesReader::msvVTKFileSeriesReader()
{
  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(1);

  this->Reader = 0;

  this->HiddenReaderModification = 0;
  this->SavedReaderModification = 0;

  this->Internal = new msvVTKFileSeriesReaderInternals;
  this->Internal->FileNameIsSet = false;
  this->Internal->TimeRanges = new msvVTKFileSeriesReaderTimeRanges;

  this->FileNameMethod = NULL;
  //this->SetFileNameMethod("SetFileName");

  this->MetaFileName = NULL;
  this->UseMetaFile = 0;
  this->CurrentFileName = 0;

  this->IgnoreReaderTime = 0;

  this->LastRequestInformationIndex = -1;
}

//-----------------------------------------------------------------------------
msvVTKFileSeriesReader::~msvVTKFileSeriesReader()
{
  this->SetCurrentFileName(NULL);
  this->SetMetaFileName(NULL);
  this->SetReader(NULL);
  delete this->Internal->TimeRanges;
  delete this->Internal;
  this->SetFileNameMethod(0);
}

//----------------------------------------------------------------------------
// Overload standard modified time function. If the internal reader is
// modified, then this object is modified as well.
unsigned long msvVTKFileSeriesReader::GetMTime()
{
  unsigned long mTime=this->vtkObject::GetMTime();

  if ( this->Reader )
    {
    // In general, we want Modifieds in Reader to be reflected in this object's
    // MTime.  However, we will also be making modifications to the Reader (such
    // as changing the filename) that we want to suppress from the reporting.
    // When this happens, we save the timestamp before our modification into
    // this->SavedReaderModification and capture the resulting MTime in
    // this->HiddenReaderModification.  If we run into that modification,
    // suppress it by reporting the saved modification.
    unsigned long readerMTime;
    if (this->Reader->GetMTime() == this->HiddenReaderModification)
      {
      readerMTime = this->SavedReaderModification;
      }
    else
      {
      readerMTime = this->Reader->GetMTime();
      }
    mTime = ( readerMTime > mTime ? readerMTime : mTime );
    }

  return mTime;
}

//----------------------------------------------------------------------------
void msvVTKFileSeriesReader::AddFileName(const char* name)
{
  this->Internal->FileNames.push_back(name);
  this->Modified();
}

//----------------------------------------------------------------------------
void msvVTKFileSeriesReader::RemoveAllFileNames()
{
  this->Internal->FileNames.clear();
  this->Modified();
}

//----------------------------------------------------------------------------
unsigned int msvVTKFileSeriesReader::GetNumberOfFileNames()
{
  return static_cast<unsigned int>(this->Internal->FileNames.size());
}

//----------------------------------------------------------------------------
const char* msvVTKFileSeriesReader::GetFileName(unsigned int idx)
{
  if (idx >= this->Internal->FileNames.size())
    {
    return 0;
    }
  return this->Internal->FileNames[idx].c_str();
}

//----------------------------------------------------------------------------
int msvVTKFileSeriesReader::ProcessRequest(vtkInformation* request,
                                        vtkInformationVector** inputVector,
                                        vtkInformationVector* outputVector)
{
  this->UpdateMetaData();

  if (this->Reader)
    {
    // Make sure that there is a file to get information from.
    if (request->Has(vtkDemandDrivenPipeline::REQUEST_DATA_OBJECT()))
      {
      if (!this->Internal->FileNameIsSet && (this->GetNumberOfFileNames() > 0))
        {
        this->SetReaderFileName(this->GetFileName(0));
        this->Internal->FileNameIsSet = true;
        }
      }
    // Our handling of these requests will call the reader's request in turn.
    if (request->Has(vtkDemandDrivenPipeline::REQUEST_INFORMATION()))
      {
      return this->RequestInformation(request, inputVector, outputVector);
      }
    if (request->Has(vtkDemandDrivenPipeline::REQUEST_DATA()))
      {
      return this->RequestData(request, inputVector, outputVector);
      }
    // Let the reader process anything we did not handle ourselves.
    int retVal = this->Reader->ProcessRequest(request,
                                              inputVector,
                                              outputVector);
    // Additional processing requried by us.
    if (request->Has(vtkStreamingDemandDrivenPipeline::REQUEST_UPDATE_EXTENT()))
      {
      this->RequestUpdateExtent(request, inputVector, outputVector);
      }

    return retVal;
    }
  vtkErrorMacro("No reader is defined. Cannot perform pipeline pass.");
  return 0;
}

//----------------------------------------------------------------------------
int msvVTKFileSeriesReader::RequestInformation(
                                 vtkInformation* request,
                                 vtkInformationVector** vtkNotUsed(inputVector),
                                 vtkInformationVector* outputVector)
{
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  this->Internal->TimeRanges->Reset();

  int numFiles = static_cast<int>(this->GetNumberOfFileNames());
  if (numFiles < 1)
    {
    // This can happen in special cases, like Plot3DReader where the
    // msvVTKFileSeriesReader is actually controlling a non-essential file-name
    // property. In which case, we simply pass the RequestInformation call to
    // the reader.
    outInfo->Remove(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
    outInfo->Remove(vtkStreamingDemandDrivenPipeline::TIME_RANGE());
    this->RequestInformationForInput(-1, request, outputVector);

    //vtkErrorMacro("Expecting at least 1 file.  Cannot proceed.");
    return 1;
    }

  // Run RequestInformation on the reader for the first file.  Use that info to
  // determine if the inputs have time information
  outInfo->Remove(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
  outInfo->Remove(vtkStreamingDemandDrivenPipeline::TIME_RANGE());
  this->RequestInformationForInput(0, request, outputVector);

  // Does the reader have time?
  if (   this->IgnoreReaderTime
      || (   !outInfo->Has(vtkStreamingDemandDrivenPipeline::TIME_STEPS())
          && !outInfo->Has(vtkStreamingDemandDrivenPipeline::TIME_RANGE()) ) )
    {
    // Input files have no time steps.  Fake a time step for each equal to the
    // index.
    outInfo->Remove(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
    outInfo->Remove(vtkStreamingDemandDrivenPipeline::TIME_RANGE());
    for (int i = 0; i < numFiles; i++)
      {
      double time = static_cast<double>(i);
      outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_STEPS(), &time, 1);
      this->Internal->TimeRanges->AddTimeRange(i, outInfo);
      }

    this->UpdateOutputTimeRange();
    }
  else
    {
    // Record the reported file time info.
    this->Internal->TimeRanges->AddTimeRange(0, outInfo);

    // Query all the other files for time info.
    for (int i = 1; i < numFiles; i++)
      {
      this->RequestInformationForInput(i, request, outputVector);
      this->Internal->TimeRanges->AddTimeRange(i, outInfo);
      }
    }

  // Now that we have collected all of the time information, set the aggregate
  // time steps in the output.
  this->Internal->TimeRanges->GetAggregateTimeInfo(outInfo);
  return 1;
}

//----------------------------------------------------------------------------
int msvVTKFileSeriesReader::RequestUpdateExtent(
                                 vtkInformation* vtkNotUsed(request),
                                 vtkInformationVector** vtkNotUsed(inputVector),
                                 vtkInformationVector* outputVector)
{
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkstd::set<int> inputs = this->Internal->TimeRanges->ChooseInputs(outInfo);
  if (inputs.size() > 1)
    {
    vtkErrorMacro("vtkTemporalDataSet not fully supported.");
    // To support readers that give vtkTemporalDataSet, we would have to iterate
    // over all of the readers in RequestData and then combine the outputs into
    // some saved data set.
    return 0;
    }
  if (inputs.size() == 0)
    {
    vtkErrorMacro("Inputs are not set.");
    return 0;
    }

  int index = *(inputs.begin());
  if (index >= static_cast<int>(this->GetNumberOfFileNames()))
    {
    // this happens when there are no files set. That's an acceptable condition
    // when the file-series is not an essential filename eg. the Q file for
    // Plot3D reader.
    index = -1;
    }

  // Make sure that the reader file name is set correctly and that
  // RequestInformation has been called.
  this->RequestInformationForInput(index);

  // I commented out the following block because it is probably not important
  // and it is causing a crash in some circumstances (bug #7253).
#if 0
  // I'm not sure if the executive will wipe out this information before
  // RequestData is called, and I don't think I care.  I'm just setting this
  // here for completeness.
  if(outInfo->Has(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS()))
    {
    vtkstd::vector<double> times
      = this->Internal->TimeRanges->GetTimesForInput(index, outInfo);
    outInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS(),
                 &times[0], times.size());
    }
#endif
  return 1;
}

//-----------------------------------------------------------------------------
int msvVTKFileSeriesReader::RequestData(vtkInformation *request,
                                     vtkInformationVector **inputVector,
                                     vtkInformationVector *outputVector)
{
  // We have modified the TIME_STEPS information in the output vector.  Some
  // readers (e.g. the Exodus reader) reuse this array to get time indices.
  // Just in case, restore the vector.
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  this->Internal->TimeRanges->GetInputTimeInfo(
                                     this->LastRequestInformationIndex,outInfo);

  int retVal = -1;
  if (this->GetNumberOfFileNames() > 0)
    {
    retVal = this->Reader->ProcessRequest(request, inputVector, outputVector);
    // Now restore the information.
    this->Internal->TimeRanges->GetAggregateTimeInfo(outInfo);
    }

  return retVal;
}

//-----------------------------------------------------------------------------
int msvVTKFileSeriesReader::RequestInformationForInput(
                                             int index,
                                             vtkInformation *request,
                                             vtkInformationVector *outputVector)
{
  if (index == -1 || (index != this->LastRequestInformationIndex) || (outputVector != NULL))
    {
    if (index >= 0)
      {
      this->SetReaderFileName(this->GetFileName(index));
      }
    else
      {
      this->SetReaderFileName(0);
      }

    this->LastRequestInformationIndex = index;
    // Need to call RequestInformation on reader to refresh any metadata for the
    // new filename.
    vtkSmartPointer<vtkInformation> tempRequest;
    if (request)
      {
      tempRequest = request;
      }
    else
      {
      tempRequest = vtkSmartPointer<vtkInformation>::New();
      tempRequest->Set(vtkDemandDrivenPipeline::REQUEST_INFORMATION());
      }
    vtkSmartPointer<vtkInformationVector> tempOutputVector;
    if (outputVector)
      {
      tempOutputVector = outputVector;
      }
    else
      {
      tempOutputVector = vtkSmartPointer<vtkInformationVector>::New();
      VTK_CREATE(vtkInformation, tempOutputInfo);
      tempOutputVector->Append(tempOutputInfo);
      }
    return this->Reader->ProcessRequest(tempRequest,
                                        static_cast<vtkInformationVector**>(NULL),
                                        tempOutputVector);
    }
  return 1;
}

//-----------------------------------------------------------------------------
void msvVTKFileSeriesReader::SetCurrentFileName(const char *fname)
{
  // Basically operates the same as the code created by the vtkSetStringMacro
  // except that it does NOT call Modified.  This method is only called
  // internally and just manages the state of the actuall reader, usually
  // while in ProcessRequest.
  if (this->CurrentFileName == fname) return;
  if (this->CurrentFileName)
    {
    delete[] this->CurrentFileName;
    }
  if (fname)
    {
    this->CurrentFileName = new char[strlen(fname) + 1];
    strcpy(this->CurrentFileName, fname);
    }
  else
    {
    this->CurrentFileName = NULL;
    }
}

//-----------------------------------------------------------------------------
int msvVTKFileSeriesReader::FillOutputPortInformation(int port,
                                                   vtkInformation* info)
{
  if (this->Reader)
    {
    vtkInformation* rinfo = this->Reader->GetOutputPortInformation(port);
    info->CopyEntry(rinfo, vtkDataObject::DATA_TYPE_NAME());
    return 1;
    }
  vtkErrorMacro("No reader is defined. Cannot provide output information.");
  return 0;
}

//-----------------------------------------------------------------------------
int msvVTKFileSeriesReader::ReadMetaDataFile(const char *metafilename,
                                          vtkStringArray *filesToRead,
                                          int maxFilesToRead /*= VTK_LARGE_INTEGER*/)
{
  // Open the metafile.
  ifstream metafile(metafilename);
  if (metafile.bad())
    {
    return 0;
    }
  // Get the path of the metafile for relative paths within.
  vtkstd::string filePath = metafilename;
  vtkstd::string::size_type pos = filePath.find_last_of("/\\");
  if(pos != filePath.npos)
    {
    filePath = filePath.substr(0, pos+1);
    }
  else
    {
    filePath = "";
    }

  // Iterate over all files pointed to by the metafile.
  filesToRead->SetNumberOfTuples(0);
  filesToRead->SetNumberOfComponents(1);
  while (   metafile.good() && !metafile.eof()
         && (filesToRead->GetNumberOfTuples() < maxFilesToRead) )
    {
    vtkStdString fname;
    metafile >> fname;
    if (fname.empty()) continue;
    if ((fname.at(0) != '/') && ((fname.size() < 2) || (fname.at(1) != ':')))
      {
      fname = filePath + fname;
      }
    filesToRead->InsertNextValue(fname);
    }
  return 1;
}

//-----------------------------------------------------------------------------
void msvVTKFileSeriesReader::UpdateMetaData()
{
  if (this->UseMetaFile && (this->MetaFileReadTime < this->MTime))
    {
    VTK_CREATE(vtkStringArray, dataFiles);
    if (!this->ReadMetaDataFile(this->MetaFileName, dataFiles))
      {
      vtkErrorMacro(<< "Could not open metafile " << this->MetaFileName);
      return;
      }

    this->RemoveAllFileNames();
    for (int i = 0; i < dataFiles->GetNumberOfValues(); i++)
      {
      this->AddFileName(dataFiles->GetValue(i).c_str());
      }

    this->MetaFileReadTime.Modified();
    }
}

//------------------------------------------------------------------------------
void msvVTKFileSeriesReader::SetOutputTimeRange(double tMin, double tMax)
{
  double timeRange[2];
  timeRange[0] = tMin;
  timeRange[1] = tMax;
  this->SetOutputTimeRange(timeRange);
}

//------------------------------------------------------------------------------
void msvVTKFileSeriesReader::SetOutputTimeRange(double timeRange[2])
{
  this->Internal->TimeRanges->SetOutputTimeRange(timeRange);
}

//------------------------------------------------------------------------------
void msvVTKFileSeriesReader::GetOutputTimeRange(double timeRange[2])
{
  this->Internal->TimeRanges->GetOutputTimeRange(timeRange);
}

//------------------------------------------------------------------------------
void msvVTKFileSeriesReader::UpdateOutputTimeRange()
{
  double outputTimeRange[2];
  this->Internal->TimeRanges->GetOutputTimeRange(outputTimeRange);

  int numFiles = static_cast<int>(this->GetNumberOfFileNames());
  if (numFiles < 1 || outputTimeRange[1] < outputTimeRange[0])
    {
    return;
    }

  this->Internal->TimeRanges->ResetRangeMap();
  double subTimeRange = (outputTimeRange[1]-outputTimeRange[0]) /
                        static_cast<double>(numFiles);

  for (int i = 0; i < numFiles; ++i)
    {
    double timeRange[2];
    timeRange[0] = i*subTimeRange + outputTimeRange[0];
    timeRange[1] = (i+1)*subTimeRange + outputTimeRange[0] - 1e-15;
    this->Internal->TimeRanges->SetTimeRange(i, timeRange);
    }
}

//-----------------------------------------------------------------------------
void msvVTKFileSeriesReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "MetaFileName: "
     << (this->MetaFileName?this->MetaFileName:"(none)") << endl;
  os << indent << "UseMetaFile: " << this->UseMetaFile << endl;
  os << indent << "IgnoreReaderTime: " << this->IgnoreReaderTime << endl;
}
