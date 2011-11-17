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

#ifndef __MSVOBJECTFACTORY_H__
#define __MSVOBJECTFACTORY_H__

#include <vtkSmartPointer.h>
#include <vtkObjectFactory.h>

class msvObjectFactory : public vtkObjectFactory
{
public:
	// This allows wrong Leaks message dissapear
	vtkTypeMacro( msvObjectFactory, vtkObjectFactory );
	static msvObjectFactory* New();

	// Must override, get the VTK_SOURCE_VERSION value
	virtual const char* GetVTKSourceVersion();
	// Must override, gets the description of this factoru
	virtual const char* GetDescription();

	static vtkObject* msvObjectFactoryCreatemsvRTRenderWindowInteractor();

protected:
	msvObjectFactory();
	virtual ~msvObjectFactory();

	virtual vtkObject* CreateObject( const char* className );

};

typedef vtkSmartPointer<msvObjectFactory>  msvObjectFactorySP;

#endif	// #ifndef __MSVOBJECTFACTORY_H__
