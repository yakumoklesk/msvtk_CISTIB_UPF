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


#ifndef __MSVENTITYMGR_H__
#define __MSVENTITYMGR_H__

#include <vtkProp3D.h>
#include <vtkSmartPointer.h>

#include <vector>

class vtkStringArray;
class vtkStdString;
class vtkRenderer;

class msvEntity;

class msvEntityMgr : public vtkObject
{
public:
	vtkTypeMacro( msvEntityMgr, vtkObject );
	// Description:
	// Construct object
	static msvEntityMgr* New();

	msvEntityMgr();
	~msvEntityMgr();

	msvEntity* CreateEntityFromDirectory( const std::string& DirectoryName, const std::string& FileWildcard = "*" );

	void SetRenderer( vtkRenderer* assignedRenderer );

	virtual void PrintSelf( ostream& os, vtkIndent indent );

	const std::vector<msvEntity*>& GetEntitiesVector() const;

private:
	msvEntity* CreateEntityFromSeriesOfStructuredPoints( vtkStringArray* series );
	msvEntity* CreateEntityFromSeries( vtkStringArray* series );

private:

	vtkRenderer*						m_AssignedRenderer;
	std::vector<msvEntity*>             m_Entities;
};

typedef vtkSmartPointer<msvEntityMgr>  msvEntityMgrSP;

#endif	// #ifndef __MSVENTITYMGR_H__
