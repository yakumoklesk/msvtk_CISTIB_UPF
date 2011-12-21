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

#include "vtkThreadSafeRenderer.h"

#include <vtkObjectFactory.h>
#include <vtkRenderer.h>
#include <vtkMutexLock.h>
#include <vtkMultiThreader.h>
#include "vtkThreadSafeRenderWindow.h"

#include <vector>

using namespace std;

////////////////////////////
// Private Implementor class
////////////////////////////

class vtkThreadSafeRendererImpl: public vtkObject
{
public:
    // Description:
    // Constructor and destructor
    vtkThreadSafeRendererImpl( vtkThreadSafeRenderer* publicInterface );
    ~vtkThreadSafeRendererImpl();

    void LockRenderer();
    void UnlockRenderer();
    vtkRenderer* SafeGetRenderer();

    virtual void PrintSelf( ostream& os, vtkIndent indent );

    // Description:
    // Add/Remove different types of props to the renderer.
    // These methods are all synonyms to AddViewProp and RemoveViewProp.
    // They are here for convenience and backwards compatibility.
    void AddActor(vtkProp *p);
    void AddVolume(vtkProp *p);
    void RemoveActor(vtkProp *p);
    void RemoveVolume(vtkProp *p);

    // Description:
    // Add a light to the list of lights.
    void AddLight(vtkLight *);

    // Description:
    // Remove a light from the list of lights.
    void RemoveLight(vtkLight *);

    // Description:
    // Remove all lights from the list of lights.
    void RemoveAllLights();

    // Description:
    // Return the collection of lights.
    vtkLightCollection *GetLights()
    {
        vtkLightCollection* aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->GetLights();
        this->Mutex->Unlock();
        return aux;
    }

    // Description:
    // Set the collection of lights.
    // We cannot name it SetLights because of TestSetGet
    // \pre lights_exist: lights!=0
    // \post lights_set: lights==this->GetLights()
    void SetLightCollection(vtkLightCollection *lights);

    // Description:
    // Create and add a light to renderer.
    void CreateLight(void);

    // Description:
    // Create a new Light sutible for use with this type of Renderer.
    // For example, a vtkMesaRenderer should create a vtkMesaLight
    // in this function.   The default is to just call vtkLight::New.
    virtual vtkLight *MakeLight();

    // Description:
    // Turn on/off two-sided lighting of surfaces. If two-sided lighting is
    // off, then only the side of the surface facing the light(s) will be lit,
    // and the other side dark. If two-sided lighting on, both sides of the
    // surface will be lit.
    vtkSafeGetMacro(TwoSidedLighting,int);
    vtkSafeSetMacro(TwoSidedLighting,int);
    vtkSafeBooleanMacro(TwoSidedLighting,int);

    // Description:
    // Turn on/off the automatic repositioning of lights as the camera moves.
    // If LightFollowCamera is on, lights that are designated as Headlights
    // or CameraLights will be adjusted to move with this renderer's camera.
    // If LightFollowCamera is off, the lights will not be adjusted.
    //
    // (Note: In previous versions of vtk, this light-tracking
    // functionality was part of the interactors, not the renderer. For
    // backwards compatibility, the older, more limited interactor
    // behavior is enabled by default. To disable this mode, turn the
    // interactor's LightFollowCamera flag OFF, and leave the renderer's
    // LightFollowCamera flag ON.)
    vtkSafeSetMacro(LightFollowCamera,int);
    vtkSafeGetMacro(LightFollowCamera,int);
    vtkSafeBooleanMacro(LightFollowCamera,int);

    // Description:
    // Turn on/off a flag which disables the automatic light creation capability.
    // Normally in VTK if no lights are associated with the renderer, then a light
    // is automatically created. However, in special circumstances this feature is
    // undesirable, so the following boolean is provided to disable automatic
    // light creation. (Turn AutomaticLightCreation off if you do not want lights
    // to be created.)
    vtkSafeGetMacro(AutomaticLightCreation,int);
    vtkSafeSetMacro(AutomaticLightCreation,int);
    vtkSafeBooleanMacro(AutomaticLightCreation,int);

    // Description:
    // Ask the lights in the scene that are not in world space
    // (for instance, Headlights or CameraLights that are attached to the
    // camera) to update their geometry to match the active camera.
    virtual int UpdateLightsGeometryToFollowCamera(void);

    // Description:
    // Return the collection of volumes.
    vtkVolumeCollection *GetVolumes();

    // Description:
    // Return any actors in this renderer.
    vtkActorCollection *GetActors();

    // Description:
    // Specify the camera to use for this renderer.
    void SetActiveCamera(vtkCamera *);

    // Description:
    // Get the current camera. If there is not camera assigned to the
    // renderer already, a new one is created automatically.
    // This does *not* reset the camera.
    vtkCamera *GetActiveCamera();

    // Description:
    // Create a new Camera sutible for use with this type of Renderer.
    // For example, a vtkMesaRenderer should create a vtkMesaCamera
    // in this function.   The default is to just call vtkCamera::New.
    virtual vtkCamera *MakeCamera();

    // Description:
    // When this flag is off, the renderer will not erase the background
    // or the Zbuffer.  It is used to have overlapping renderers.
    // Both the RenderWindow Erase and Render Erase must be on
    // for the camera to clear the renderer.  By default, Erase is on.
    vtkSafeSetMacro(Erase, int);
    vtkSafeGetMacro(Erase, int);
    vtkSafeBooleanMacro(Erase, int);

    // Description:
    // When this flag is off, render commands are ignored.  It is used to either
    // multiplex a vtkRenderWindow or render only part of a vtkRenderWindow.
    // By default, Draw is on.
    vtkSafeSetMacro(Draw, int);
    vtkSafeGetMacro(Draw, int);
    vtkSafeBooleanMacro(Draw, int);

    // Description:
    // Add an culler to the list of cullers.
    void AddCuller(vtkCuller *);

    // Description:
    // Remove an actor from the list of cullers.
    void RemoveCuller(vtkCuller *);

    // Description:
    // Return the collection of cullers.
    vtkCullerCollection *GetCullers();

    // Description:
    // Set the intensity of ambient lighting.
    vtkSafeSetVector3Macro(Ambient,double);
    vtkSafeGetVectorMacro(Ambient,double,3);

    // Description:
    // Set/Get the amount of time this renderer is allowed to spend
    // rendering its scene. This is used by vtkLODActor's.
    vtkSafeSetMacro(AllocatedRenderTime,double);
    virtual double GetAllocatedRenderTime();

    // Description:
    // Get the ratio between allocated time and actual render time.
    // TimeFactor has been taken out of the render process.
    // It is still computed in case someone finds it useful.
    // It may be taken away in the future.
    virtual double GetTimeFactor();

    // Description:
    // CALLED BY vtkRenderWindow ONLY. End-user pass your way and call
    // vtkRenderWindow::Render().
    // Create an image. This is a superclass method which will in turn
    // call the DeviceRender method of Subclasses of vtkRenderer.
    virtual void Render();

    // Description:
    // Create an image. Subclasses of vtkRenderer must implement this method.
    virtual void DeviceRender()
    {
        this->Mutex->Lock();
        this->Impl->DeviceRender();
        this->Mutex->Unlock();
    };

    // Description:
    // Render translucent polygonal geometry. Default implementation just call
    // UpdateTranslucentPolygonalGeometry().
    // Subclasses of vtkRenderer that can deal with depth peeling must
    // override this method.
    // It updates boolean ivar LastRenderingUsedDepthPeeling.
    virtual void DeviceRenderTranslucentPolygonalGeometry();

    // Description:
    // Clear the image to the background color.
    virtual void Clear() {};

    // Description:
    // Returns the number of visible actors.
    int VisibleActorCount();

    // Description:
    // Returns the number of visible volumes.
    int VisibleVolumeCount();

    // Description:
    // Compute the bounding box of all the visible props
    // Used in ResetCamera() and ResetCameraClippingRange()
    void ComputeVisiblePropBounds( double bounds[6] );

    // Description:
    // Wrapper-friendly version of ComputeVisiblePropBounds
    double *ComputeVisiblePropBounds();

    // Description:
    // Reset the camera clipping range based on the bounds of the
    // visible actors. This ensures that no props are cut off
    void ResetCameraClippingRange();

    // Description:
    // Reset the camera clipping range based on a bounding box.
    // This method is called from ResetCameraClippingRange()
    void ResetCameraClippingRange( double bounds[6] );
    void ResetCameraClippingRange( double xmin, double xmax,
        double ymin, double ymax,
        double zmin, double zmax);

    // Description:
    // Specify tolerance for near clipping plane distance to the camera as a
    // percentage of the far clipping plane distance. By default this will be
    // set to 0.01 for 16 bit zbuffers and 0.001 for higher depth z buffers
    vtkSafeSetClampMacro(NearClippingPlaneTolerance,double,0,0.99);
    vtkSafeGetMacro(NearClippingPlaneTolerance,double);

    // Description:
    // Automatically set up the camera based on the visible actors.
    // The camera will reposition itself to view the center point of the actors,
    // and move along its initial view plane normal (i.e., vector defined from
    // camera position to focal point) so that all of the actors can be seen.
    void ResetCamera();

    // Description:
    // Automatically set up the camera based on a specified bounding box
    // (xmin,xmax, ymin,ymax, zmin,zmax). Camera will reposition itself so
    // that its focal point is the center of the bounding box, and adjust its
    // distance and position to preserve its initial view plane normal
    // (i.e., vector defined from camera position to focal point). Note: is
    // the view plane is parallel to the view up axis, the view up axis will
    // be reset to one of the three coordinate axes.
    void ResetCamera(double bounds[6]);

    // Description:
    // Alternative version of ResetCamera(bounds[6]);
    void ResetCamera(double xmin, double xmax, double ymin, double ymax,
        double zmin, double zmax);

    // Description:
    // Specify the rendering window in which to draw. This is automatically set
    // when the renderer is created by MakeRenderer.  The user probably
    // shouldn't ever need to call this method.
    void SetRenderWindow(vtkRenderWindow *);
    vtkRenderWindow *GetRenderWindow()
    {
      vtkRenderWindow* aux( 0 );
      this->Mutex->Lock();
      aux = this->Impl->GetRenderWindow();
      this->Mutex->Unlock();
      return aux;
    }

    virtual vtkWindow *GetVTKWindow();

    // Description:
    // Turn on/off using backing store. This may cause the re-rendering
    // time to be slightly slower when the view changes. But it is
    // much faster when the image has not changed, such as during an
    // expose event.
    vtkSafeSetMacro(BackingStore,int);
    vtkSafeGetMacro(BackingStore,int);
    vtkSafeBooleanMacro(BackingStore,int);

    // Description:
    // Turn on/off interactive status.  An interactive renderer is one that
    // can receive events from an interactor.  Should only be set if
    // there are multiple renderers in the same section of the viewport.
    vtkSafeSetMacro(Interactive,int);
    vtkSafeGetMacro(Interactive,int);
    vtkSafeBooleanMacro(Interactive,int);

    // Description:
    // Set/Get the layer that this renderer belongs to.  This is only used if
    // there are layered renderers.
    vtkSafeSetMacro(Layer, int);
    vtkSafeGetMacro(Layer, int);

    // Description:
    // Normally a renderer is treated as transparent if Layer > 0. To treat a
    // renderer at Layer 0 as transparent, set this flag to true.
    vtkSafeSetMacro(PreserveDepthBuffer, int);
    vtkSafeGetMacro(PreserveDepthBuffer, int);
    vtkSafeBooleanMacro(PreserveDepthBuffer, int);

    // Description:
    // Returns a boolean indicating if this renderer is transparent.  It is
    // transparent if it is not in the deepest layer of its render window.
    int  Transparent();

    // Description:
    // Convert world point coordinates to view coordinates.
    void WorldToView();

    // Description:
    // Convert view point coordinates to world coordinates.
    void ViewToWorld();
    virtual void ViewToWorld(double &wx, double &wy, double &wz);

    // Description:
    // Convert world point coordinates to view coordinates.
    virtual void WorldToView(double &wx, double &wy, double &wz);

    // Description:
    // Given a pixel location, return the Z value. The z value is
    // normalized (0,1) between the front and back clipping planes.
    double GetZ (int x, int y);

    // Description:
    // Return the MTime of the renderer also considering its ivars.
    unsigned long GetMTime();

    // Description:
    // Get the time required, in seconds, for the last Render call.
    vtkSafeGetMacro( LastRenderTimeInSeconds, double );

    // Description:
    // Should be used internally only during a render
    // Get the number of props that were rendered using a
    // RenderOpaqueGeometry or RenderTranslucentPolygonalGeometry call.
    // This is used to know if something is in the frame buffer.
    vtkSafeGetMacro( NumberOfPropsRendered, int );

    // Description:
    // Return the prop (via a vtkAssemblyPath) that has the highest z value
    // at the given x, y position in the viewport.  Basically, the top most
    // prop that renders the pixel at selectionX, selectionY will be returned.
    // If nothing was picked then NULL is returned.  This method selects from
    // the renderers Prop list.
    vtkAssemblyPath* PickProp(double selectionX, double selectionY)
    {
        return this->PickProp(selectionX, selectionY, selectionX, selectionY);
    }
    vtkAssemblyPath* PickProp(double selectionX1, double selectionY1,
        double selectionX2, double selectionY2);

    // Description:
    // Do anything necessary between rendering the left and right viewpoints
    // in a stereo render. Doesn't do anything except in the derived
    // vtkIceTRenderer in ParaView.
    virtual void StereoMidpoint() { return; };

    // Description:
    // Compute the aspect ratio of this renderer for the current tile. When
    // tiled displays are used the aspect ratio of the renderer for a given
    // tile may be diferent that the aspect ratio of the renderer when rendered
    // in it entirity
    double GetTiledAspectRatio();

    // Description:
    // This method returns 1 if the ActiveCamera has already been set or
    // automatically created by the renderer. It returns 0 if the
    // ActiveCamera does not yet exist.
    int IsActiveCameraCreated()
    {
      int aux( 0 );
      this->Mutex->Lock();
      aux = this->Impl->IsActiveCameraCreated();
      this->Mutex->Unlock();
      return aux;
    }


    // Description:
    // Turn on/off rendering of translucent material with depth peeling
    // technique. The render window must have alpha bits (ie call
    // SetAlphaBitPlanes(1)) and no multisample buffer (ie call
    // SetMultiSamples(0) ) to support depth peeling.
    // If UseDepthPeeling is on and the GPU supports it, depth peeling is used
    // for rendering translucent materials.
    // If UseDepthPeeling is off, alpha blending is used.
    // Initial value is off.
    vtkSafeSetMacro(UseDepthPeeling,int);
    vtkSafeGetMacro(UseDepthPeeling,int);
    vtkSafeBooleanMacro(UseDepthPeeling,int);

    // Description:
    // In case of use of depth peeling technique for rendering translucent
    // material, define the threshold under which the algorithm stops to
    // iterate over peel layers. This is the ratio of the number of pixels
    // that have been touched by the last layer over the total number of pixels
    // of the viewport area.
    // Initial value is 0.0, meaning rendering have to be exact. Greater values
    // may speed-up the rendering with small impact on the quality.
    vtkSafeSetClampMacro(OcclusionRatio,double,0.0,0.5);
    vtkSafeGetMacro(OcclusionRatio,double);

    // Description:
    // In case of depth peeling, define the maximum number of peeling layers.
    // Initial value is 4. A special value of 0 means no maximum limit.
    // It has to be a positive value.
    vtkSafeSetMacro(MaximumNumberOfPeels,int);
    vtkSafeGetMacro(MaximumNumberOfPeels,int);

    // Description:
    // Tells if the last call to DeviceRenderTranslucentPolygonalGeometry()
    // actually used depth peeling.
    // Initial value is false.
    vtkSafeGetMacro(LastRenderingUsedDepthPeeling,int);

    // Description:
    // Set/Get a custom Render call. Allows to hook a Render call from an
    // external project.It will be used in place of vtkRenderer::Render() if it
    // is not NULL and its Used ivar is set to true.
    // Initial value is NULL.
    void SetDelegate(vtkRendererDelegate *d);
    vtkSafeGetObjectMacro(Delegate,vtkRendererDelegate);

    // Description:
    // Set/Get a custom render pass.
    // Initial value is NULL.
    void SetPass(vtkRenderPass *p);
    vtkSafeGetObjectMacro(Pass,vtkRenderPass);

    // Description:
    // Get the current hardware selector. If the Selector is set, it implies the
    // current render pass is for selection. Mappers/Properties may choose to
    // behave differently when rendering for hardware selection.
    vtkSafeGetObjectMacro(Selector, vtkHardwareSelector);

    // Description:
    // Set/Get the texture to be used for the background. If set
    // and enabled this gets the priority over the gradient background.
    void SetBackgroundTexture(vtkTexture*);
    vtkSafeGetObjectMacro(BackgroundTexture, vtkTexture);

    // Description:
    // Set/Get whether this viewport should have a textured background.
    // Default is off.
    vtkSafeSetMacro(TexturedBackground,bool);
    vtkSafeGetMacro(TexturedBackground,bool);
    vtkSafeBooleanMacro(TexturedBackground,bool);

    // Description:
    // Return the Z value for the last picked Prop.
    virtual double GetPickedZ();

private:

    vtkRenderer*                        Impl;
    vtkMutexLock*                       Mutex;
    vtkMultiThreaderIDType              LockOwnerThreadID;
    vtkThreadSafeRenderWindow*          ThreadSafeRenderWindow;
    vtkThreadSafeRenderer*              PublicInterface;
};



vtkThreadSafeRendererImpl::vtkThreadSafeRendererImpl( vtkThreadSafeRenderer* publicInterface )
: Impl( 0 )
, Mutex( 0 )
, LockOwnerThreadID( -1 )
, PublicInterface( publicInterface )
{
    this->Mutex = vtkMutexLock::New();
    this->Impl = vtkRenderer::New();
}

vtkThreadSafeRendererImpl::~vtkThreadSafeRendererImpl()
{
    this->Mutex->Unlock();
    this->Mutex->Delete();
    this->Impl->Delete();
}

void vtkThreadSafeRendererImpl::PrintSelf( ostream& os, vtkIndent indent )
{
    this->Superclass::PrintSelf( os, indent );
    this->Mutex->Lock();
    this->Impl->PrintSelf( os, indent );
    this->Mutex->Unlock();
}

// Concrete render method.
void vtkThreadSafeRendererImpl::Render(void)
{
    this->Mutex->Lock();
    this->Impl->Render();
    this->Mutex->Unlock();
}

// ----------------------------------------------------------------------------
// Description:
// Render translucent polygonal geometry. Default implementation just call
// UpdateTranslucentPolygonalGeometry().
// Subclasses of vtkRenderer that can deal with depth peeling must
// override this method.
void vtkThreadSafeRendererImpl::DeviceRenderTranslucentPolygonalGeometry()
{
    this->Mutex->Lock();
    this->Impl->DeviceRenderTranslucentPolygonalGeometry();
    this->Mutex->Unlock();
}

// ----------------------------------------------------------------------------
double vtkThreadSafeRendererImpl::GetAllocatedRenderTime()
{
    double aux( 0 );
    this->Mutex->Lock();
    aux = this->Impl->GetAllocatedRenderTime();
    this->Mutex->Unlock();
    return aux;
}

double vtkThreadSafeRendererImpl::GetTimeFactor()
{
    double aux( 0 );
    this->Mutex->Lock();
    aux = this->Impl->GetTimeFactor();
    this->Mutex->Unlock();
    return aux;
}

// Ask active camera to load its view matrix.
/*
//protected:
int vtkThreadSafeRendererImpl::UpdateCamera()
{
    this->Mutex->Lock();
    this->Impl->UpdateCamera();
    this->Mutex->Unlock();

    return 1;
}
*/

int vtkThreadSafeRendererImpl::UpdateLightsGeometryToFollowCamera()
{
    int aux( 0 );
    this->Mutex->Lock();
    aux = this->Impl->UpdateLightsGeometryToFollowCamera();
    this->Mutex->Unlock();
    return aux;
}

/*
//protected:
int vtkThreadSafeRendererImpl::UpdateLightGeometry()
{
    this->Mutex->Lock();
    this->Impl->UpdateLightGeometry();
    this->Mutex->Unlock();
}
*/

// Do all outer culling to set allocated time for each prop.
// Possibly re-order the actor list.
/*
//protected:
void vtkThreadSafeRendererImpl::AllocateTime()
{
    this->Mutex->Lock();
    this->Impl->AllocateTime();
    this->Mutex->Unlock();
}
*/

// Ask actors to render themselves. As a side effect will cause
// visualization network to update.
/*
//protected:
int vtkThreadSafeRendererImpl::UpdateGeometry()
{
    int aux( 0 );
    this->Mutex->Lock();
    aux = this->Impl->UpdateGeometry();
    this->Mutex->Unlock();
    return aux;
}
*/

// ----------------------------------------------------------------------------
// Description:
// Ask all props to update and draw any translucent polygonal
// geometry. This includes both vtkActors and vtkVolumes
// Return the number of rendered props.
// It is called once with alpha blending technique. It is called multiple
// times with depth peeling technique.
/*
//protected:
int vtkThreadSafeRendererImpl::UpdateTranslucentPolygonalGeometry()
{
    int aux( 0 );
    this->Mutex->Lock();
    aux = this->Impl->UpdateTranslucentPolygonalGeometry();
    this->Mutex->Unlock();
    return aux;
}
*/

// ----------------------------------------------------------------------------
vtkWindow *vtkThreadSafeRendererImpl::GetVTKWindow()
{
    vtkWindow* aux( 0 );
    this->Mutex->Lock();
    aux = this->Impl->GetVTKWindow();
    this->Mutex->Unlock();
    return aux;
}

// Specify the camera to use for this renderer.
void vtkThreadSafeRendererImpl::SetActiveCamera(vtkCamera *cam)
{
    this->Mutex->Lock();
    this->Impl->SetActiveCamera( cam );
    this->Mutex->Unlock();
}

//----------------------------------------------------------------------------
vtkCamera* vtkThreadSafeRendererImpl::MakeCamera()
{
    vtkCamera* aux( 0 );
    this->Mutex->Lock();
    aux = this->Impl->MakeCamera();
    this->Mutex->Unlock();

    //this->InvokeEvent(vtkCommand::CreateCameraEvent, cam);
    return aux;
}

//----------------------------------------------------------------------------
vtkCamera *vtkThreadSafeRendererImpl::GetActiveCamera()
{
    vtkCamera* aux( 0 );
    this->Mutex->Lock();
    aux = this->Impl->GetActiveCamera();
    this->Mutex->Unlock();
    return aux;
}

//----------------------------------------------------------------------------
/*vtkCamera *vtkThreadSafeRendererImpl::GetActiveCameraAndResetIfCreated()
{
    vtkCamera* aux( 0 );
    this->Mutex->Lock();
    aux = this->Impl->GetActiveCameraAndResetIfCreated();
    this->Mutex->Unlock();
    return aux;
}
*/

//----------------------------------------------------------------------------
void vtkThreadSafeRendererImpl::AddActor(vtkProp* p)
{
    this->Mutex->Lock();
    this->Impl->AddActor( p );
    this->Mutex->Unlock();
}

//----------------------------------------------------------------------------
void vtkThreadSafeRendererImpl::AddVolume(vtkProp* p)
{
    this->Mutex->Lock();
    this->Impl->AddVolume( p );
    this->Mutex->Unlock();
}

//----------------------------------------------------------------------------
void vtkThreadSafeRendererImpl::RemoveActor(vtkProp* p)
{
    this->Mutex->Lock();
    this->Impl->RemoveActor( p );
    this->Mutex->Unlock();
}

//----------------------------------------------------------------------------
void vtkThreadSafeRendererImpl::RemoveVolume(vtkProp* p)
{
    this->Mutex->Lock();
    this->Impl->RemoveVolume( p );
    this->Mutex->Unlock();
}

// Add a light to the list of lights.
void vtkThreadSafeRendererImpl::AddLight(vtkLight *light)
{
    this->Mutex->Lock();
    this->Impl->AddLight( light );
    this->Mutex->Unlock();
}

// look through the props and get all the actors
vtkActorCollection *vtkThreadSafeRendererImpl::GetActors()
{
    vtkActorCollection* aux( 0 );
    this->Mutex->Lock();
    aux = this->Impl->GetActors();
    this->Mutex->Unlock();
    return aux;
}

// look through the props and get all the volumes
vtkVolumeCollection *vtkThreadSafeRendererImpl::GetVolumes()
{
    vtkVolumeCollection* aux( 0 );
    this->Mutex->Lock();
    aux = this->Impl->GetVolumes();
    this->Mutex->Unlock();
    return aux;
}

// Remove a light from the list of lights.
void vtkThreadSafeRendererImpl::RemoveLight(vtkLight *light)
{
    this->Mutex->Lock();
    this->Impl->RemoveLight( light );
    this->Mutex->Unlock();
}

// Remove all lights from the list of lights.
void vtkThreadSafeRendererImpl::RemoveAllLights()
{
    this->Mutex->Lock();
    this->Impl->RemoveAllLights();
    this->Mutex->Unlock();
}

// Add an culler to the list of cullers.
void vtkThreadSafeRendererImpl::AddCuller(vtkCuller *culler)
{
    this->Mutex->Lock();
    this->Impl->AddCuller( culler );
    this->Mutex->Unlock();
}

// Remove an actor from the list of cullers.
void vtkThreadSafeRendererImpl::RemoveCuller(vtkCuller *culler)
{
    this->Mutex->Lock();
    this->Impl->RemoveCuller( culler );
    this->Mutex->Unlock();
}

// ----------------------------------------------------------------------------
void vtkThreadSafeRendererImpl::SetLightCollection(vtkLightCollection *lights)
{
    this->Mutex->Lock();
    this->Impl->SetLightCollection( lights );
    this->Mutex->Unlock();
}

// ----------------------------------------------------------------------------
vtkLight *vtkThreadSafeRendererImpl::MakeLight()
{
    vtkLight* aux( 0 );
    this->Mutex->Lock();
    aux = this->Impl->MakeLight();
    this->Mutex->Unlock();
    return aux;
}

void vtkThreadSafeRendererImpl::CreateLight(void)
{
    this->Mutex->Lock();
    this->Impl->CreateLight();
    this->Mutex->Unlock();
}

// Compute the bounds of the visible props
void vtkThreadSafeRendererImpl::ComputeVisiblePropBounds( double allBounds[6] )
{
    this->Mutex->Lock();
    this->Impl->ComputeVisiblePropBounds( allBounds );
    this->Mutex->Unlock();
}

double *vtkThreadSafeRendererImpl::ComputeVisiblePropBounds()
{
    double* aux( 0 );
    this->Mutex->Lock();
    aux = this->Impl->ComputeVisiblePropBounds();
    this->Mutex->Unlock();
    return aux;
}

// Automatically set up the camera based on the visible actors.
// The camera will reposition itself to view the center point of the actors,
// and move along its initial view plane normal (i.e., vector defined from
// camera position to focal point) so that all of the actors can be seen.
void vtkThreadSafeRendererImpl::ResetCamera()
{
    this->Mutex->Lock();
    this->Impl->ResetCamera();
    this->Mutex->Unlock();

    // Here to let parallel/distributed compositing intercept
    // and do the right thing.
    //this->InvokeEvent(vtkCommand::ResetCameraEvent,this);
}

// Automatically set the clipping range of the camera based on the
// visible actors
void vtkThreadSafeRendererImpl::ResetCameraClippingRange()
{
    this->Mutex->Lock();
    this->Impl->ResetCameraClippingRange();
    this->Mutex->Unlock();

    // Here to let parallel/distributed compositing intercept
    // and do the right thing.
    //this->InvokeEvent(vtkCommand::ResetCameraClippingRangeEvent,this);
}


// Automatically set up the camera based on a specified bounding box
// (xmin,xmax, ymin,ymax, zmin,zmax). Camera will reposition itself so
// that its focal point is the center of the bounding box, and adjust its
// distance and position to preserve its initial view plane normal
// (i.e., vector defined from camera position to focal point). Note: if
// the view plane is parallel to the view up axis, the view up axis will
// be reset to one of the three coordinate axes.
void vtkThreadSafeRendererImpl::ResetCamera(double bounds[6])
{
    this->Mutex->Lock();
    this->Impl->ResetCamera( bounds );
    this->Mutex->Unlock();
}

// Alternative version of ResetCamera(bounds[6]);
void vtkThreadSafeRendererImpl::ResetCamera(double xmin, double xmax,
                              double ymin, double ymax,
                              double zmin, double zmax)
{
    this->Mutex->Lock();
    this->Impl->ResetCamera( xmin, xmax, ymin, ymax, zmin, zmax );
    this->Mutex->Unlock();
}

// Reset the camera clipping range to include this entire bounding box
void vtkThreadSafeRendererImpl::ResetCameraClippingRange( double bounds[6] )
{
    this->Mutex->Lock();
    this->Impl->ResetCameraClippingRange( bounds );
    this->Mutex->Unlock();
}

// Alternative version of ResetCameraClippingRange(bounds[6]);
void vtkThreadSafeRendererImpl::ResetCameraClippingRange(double xmin, double xmax,
                                           double ymin, double ymax,
                                           double zmin, double zmax)
{
    this->Mutex->Lock();
    this->Impl->ResetCameraClippingRange( xmin, xmax, ymin, ymax, zmin, zmax );
    this->Mutex->Unlock();
}

// Specify the rendering window in which to draw. This is automatically set
// when the renderer is created by MakeRenderer.  The user probably
// shouldn't ever need to call this method.
// no reference counting!
void vtkThreadSafeRendererImpl::SetRenderWindow(vtkRenderWindow *renwin)
{
    this->Mutex->Lock();
    vtkThreadSafeRenderWindow* tsrenwin = dynamic_cast<vtkThreadSafeRenderWindow*>( renwin );
    if( tsrenwin != 0 )
    {
        this->ThreadSafeRenderWindow = tsrenwin;
        this->Impl->SetRenderWindow( tsrenwin );
    }
    else
    {
        // Should assert?
        this->Impl->SetRenderWindow( renwin );
    }
    //this->Impl->SetRenderWindow( renwin );
    this->Mutex->Unlock();
}

// Given a pixel location, return the Z value
double vtkThreadSafeRendererImpl::GetZ( int x, int y )
{
    double aux( 0 );
    this->Mutex->Lock();
    aux = this->Impl->GetZ( x, y );
    this->Mutex->Unlock();
    return aux;
}


// Convert view point coordinates to world coordinates.
void vtkThreadSafeRendererImpl::ViewToWorld()
{
    this->Mutex->Lock();
    this->Impl->ViewToWorld();
    this->Mutex->Unlock();
}

void vtkThreadSafeRendererImpl::ViewToWorld(double &x, double &y, double &z)
{
    this->Mutex->Lock();
    this->Impl->ViewToWorld( x, y, z );
    this->Mutex->Unlock();
}

// Convert world point coordinates to view coordinates.
void vtkThreadSafeRendererImpl::WorldToView()
{
    this->Mutex->Lock();
    this->Impl->WorldToView();
    this->Mutex->Unlock();
}

// Convert world point coordinates to view coordinates.
void vtkThreadSafeRendererImpl::WorldToView(double &x, double &y, double &z)
{
    this->Mutex->Lock();
    this->Impl->WorldToView( x, y, z );
    this->Mutex->Unlock();
}

/*
void vtkThreadSafeRendererImpl::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Mutex->Lock();
    this->Impl->PrintSelf( os, indent );
    this->Mutex->Unlock();
}
*/

int vtkThreadSafeRendererImpl::VisibleActorCount()
{
    int aux( 0 );
    this->Mutex->Lock();
    aux = this->Impl->VisibleActorCount();
    this->Mutex->Unlock();
    return aux;
}

int vtkThreadSafeRendererImpl::VisibleVolumeCount()
{
    int aux( 0 );
    this->Mutex->Lock();
    aux = this->Impl->VisibleVolumeCount();
    this->Mutex->Unlock();
    return aux;
}

unsigned long int vtkThreadSafeRendererImpl::GetMTime()
{
    unsigned long int aux( 0 );
    this->Mutex->Lock();
    aux = this->Impl->GetMTime();
    this->Mutex->Unlock();
    return aux;
}


vtkAssemblyPath* vtkThreadSafeRendererImpl::PickProp(double selectionX1, double selectionY1,
                                       double selectionX2, double selectionY2)
{
    vtkAssemblyPath* aux( 0 );
    this->Mutex->Lock();
    aux = this->Impl->PickProp( selectionX1, selectionY1, selectionX2, selectionY2 );
    this->Mutex->Unlock();
    return aux;
}

// Do a render in pick or select mode.  This is normally done with
// rendering turned off. Before each Prop is rendered the pick id is
// incremented
/*
//protected:
void vtkThreadSafeRendererImpl::PickRender(vtkPropCollection *props)
{
    this->Mutex->Lock();
    this->Impl->PickRender( props );
    this->Mutex->Unlock();
}
*/
/*
//protected:
void vtkThreadSafeRendererImpl::PickGeometry()
{

    this->Mutex->Lock();
    this->Impl->PickGeometry( props );
    this->Mutex->Unlock();
}
*/

int  vtkThreadSafeRendererImpl::Transparent()
{
    int aux( 0 );
    this->Mutex->Lock();
    aux = this->Impl->Transparent();
    this->Mutex->Unlock();
    return aux;
}

double vtkThreadSafeRendererImpl::GetTiledAspectRatio()
{
    double aux( 0 );
    this->Mutex->Lock();
    aux = this->Impl->GetTiledAspectRatio();
    this->Mutex->Unlock();
    return aux;
}

#if !defined(VTK_LEGACY_REMOVE)
//----------------------------------------------------------------------------
/*
int vtkThreadSafeRendererImpl::UpdateGeometryForSelection()
{
    int aux( 0 );
    this->Mutex->Lock();
    aux = this->Impl->UpdateGeometryForSelection();
    this->Mutex->Unlock();
    return aux;
}

//----------------------------------------------------------------------------
vtkPainter* vtkThreadSafeRendererImpl::SwapInSelectablePainter(
    vtkProp *prop,
    int &orig_visibility)
{
    vtkPainter* aux( 0 );
    this->Mutex->Lock();
    aux = this->Impl->SwapInSelectablePainter( prop, orig_visibility );
    this->Mutex->Unlock();
    return aux;
}

//----------------------------------------------------------------------------
void vtkThreadSafeRendererImpl::SwapOutSelectablePainter(
    vtkProp *prop,
    vtkPainter* orig_painter,
    int orig_visibility)
{
    this->Mutex->Lock();
    this->Impl->SwapOutSelectablePainter( prop, orig_painter, orig_visibility );
    this->Mutex->Unlock();
}
*/
#endif  // #if !defined(VTK_LEGACY_REMOVE)

double vtkThreadSafeRendererImpl::GetPickedZ()
{
    double aux( 0 );
    this->Mutex->Lock();
    aux = this->Impl->GetPickedZ();
    this->Mutex->Unlock();
    return aux;
}

/////////////////////////
// Public interface class
/////////////////////////

vtkStandardNewMacro( vtkThreadSafeRenderer );

vtkThreadSafeRenderer::vtkThreadSafeRenderer()
{
    this->Impl = new vtkThreadSafeRendererImpl( this );
}

vtkThreadSafeRenderer::~vtkThreadSafeRenderer()
{
    delete this->Impl;
}

// Description:
// Add/Remove different types of props to the renderer.
// These methods are all synonyms to AddViewProp and RemoveViewProp.
// They are here for convenience and backwards compatibility.
void vtkThreadSafeRenderer::AddActor(vtkProp *p)
{
    this->Impl->AddActor( p );
}

void vtkThreadSafeRenderer::AddVolume(vtkProp *p)
{
    this->Impl->AddVolume( p );
}

void vtkThreadSafeRenderer::RemoveActor(vtkProp *p)
{
    this->Impl->RemoveActor( p );
}

void vtkThreadSafeRenderer::RemoveVolume(vtkProp *p)
{
    this->Impl->RemoveVolume( p );
}

// Description:
// Add a light to the list of lights.
void vtkThreadSafeRenderer::AddLight(vtkLight *l)
{
    this->Impl->AddLight( l );
}

// Description:
// Remove a light from the list of lights.
void vtkThreadSafeRenderer::RemoveLight(vtkLight *l)
{
    this->Impl->RemoveLight( l );
}

// Description:
// Remove all lights from the list of lights.
void vtkThreadSafeRenderer::RemoveAllLights()
{
    this->Impl->RemoveAllLights();
}

// Description:
// Return the collection of lights.
vtkLightCollection *vtkThreadSafeRenderer::GetLights()
{
    return this->Impl->GetLights();
}

// Description:
// Set the collection of lights.
// We cannot name it SetLights because of TestSetGet
// \pre lights_exist: lights!=0
// \post lights_set: lights==this->GetLights()
void vtkThreadSafeRenderer::SetLightCollection(vtkLightCollection *lights)
{
    this->Impl->SetLightCollection( lights );
}

// Description:
// Create and add a light to renderer.
void vtkThreadSafeRenderer::CreateLight(void)
{
    this->Impl->CreateLight();
}

// Description:
// Create a new Light sutible for use with this type of Renderer.
// For example, a vtkMesaRenderer should create a vtkMesaLight
// in this function.   The default is to just call vtkLight::New.
vtkLight *vtkThreadSafeRenderer::MakeLight()
{
    return this->Impl->MakeLight();
}


vtkImplGetMacroDef(vtkThreadSafeRenderer,TwoSidedLighting,int);
vtkImplSetMacroDef(vtkThreadSafeRenderer,TwoSidedLighting,int);
vtkImplBooleanMacroDef(vtkThreadSafeRenderer,TwoSidedLighting,int);


vtkImplSetMacroDef(vtkThreadSafeRenderer,LightFollowCamera,int);
vtkImplGetMacroDef(vtkThreadSafeRenderer,LightFollowCamera,int);
vtkImplBooleanMacroDef(vtkThreadSafeRenderer,LightFollowCamera,int);

// Description:
// Turn on/off a flag which disables the automatic light creation capability.
// Normally in VTK if no lights are associated with the renderer, then a light
// is automatically created. However, in special circumstances this feature is
// undesirable, so the following boolean is provided to disable automatic
// light creation. (Turn AutomaticLightCreation off if you do not want lights
// to be created.)
vtkImplGetMacroDef(vtkThreadSafeRenderer,AutomaticLightCreation,int);
vtkImplSetMacroDef(vtkThreadSafeRenderer,AutomaticLightCreation,int);
vtkImplBooleanMacroDef(vtkThreadSafeRenderer,AutomaticLightCreation,int);

// Description:
// When this flag is off, the renderer will not erase the background
// or the Zbuffer.  It is used to have overlapping renderers.
// Both the RenderWindow Erase and Render Erase must be on
// for the camera to clear the renderer.  By default, Erase is on.
vtkImplSetMacroDef(vtkThreadSafeRenderer,Erase, int);
vtkImplGetMacroDef(vtkThreadSafeRenderer,Erase, int);
vtkImplBooleanMacroDef(vtkThreadSafeRenderer,Erase, int);

// Description:
// When this flag is off, render commands are ignored.  It is used to either
// multiplex a vtkRenderWindow or render only part of a vtkRenderWindow.
// By default, Draw is on.
vtkImplSetMacroDef(vtkThreadSafeRenderer,Draw, int);
vtkImplGetMacroDef(vtkThreadSafeRenderer,Draw, int);
vtkImplBooleanMacroDef(vtkThreadSafeRenderer,Draw, int);


// Description:
// Set the intensity of ambient lighting.
vtkImplSetVector3MacroDef(vtkThreadSafeRenderer,Ambient,double);
vtkImplGetVectorMacroDef(vtkThreadSafeRenderer,Ambient,double,3);

// Description:
// Set/Get the amount of time this renderer is allowed to spend
// rendering its scene. This is used by vtkLODActor's.
vtkImplSetMacroDef(vtkThreadSafeRenderer,AllocatedRenderTime,double);


// Description:
// Specify tolerance for near clipping plane distance to the camera as a
// percentage of the far clipping plane distance. By default this will be
// set to 0.01 for 16 bit zbuffers and 0.001 for higher depth z buffers
vtkImplSetClampMacroDef(vtkThreadSafeRenderer,NearClippingPlaneTolerance,double,0,0.99);
vtkImplBooleanMacroDef(vtkThreadSafeRenderer,NearClippingPlaneTolerance,double);


// Description:
// Turn on/off using backing store. This may cause the re-rendering
// time to be slightly slower when the view changes. But it is
// much faster when the image has not changed, such as during an
// expose event.
vtkImplSetMacroDef(vtkThreadSafeRenderer,BackingStore,int);
vtkImplGetMacroDef(vtkThreadSafeRenderer,BackingStore,int);
vtkImplBooleanMacroDef(vtkThreadSafeRenderer,BackingStore,int);

// Description:
// Turn on/off interactive status.  An interactive renderer is one that
// can receive events from an interactor.  Should only be set if
// there are multiple renderers in the same section of the viewport.
vtkImplSetMacroDef(vtkThreadSafeRenderer,Interactive,int);
vtkImplGetMacroDef(vtkThreadSafeRenderer,Interactive,int);
vtkImplBooleanMacroDef(vtkThreadSafeRenderer,Interactive,int);

// Description:
// Set/Get the layer that this renderer belongs to.  This is only used if
// there are layered renderers.
vtkImplSetMacroDef(vtkThreadSafeRenderer,Layer, int);
vtkImplGetMacroDef(vtkThreadSafeRenderer,Layer, int);

// Description:
// Normally a renderer is treated as transparent if Layer > 0. To treat a
// renderer at Layer 0 as transparent, set this flag to true.
vtkImplSetMacroDef(vtkThreadSafeRenderer,PreserveDepthBuffer, int);
vtkImplGetMacroDef(vtkThreadSafeRenderer,PreserveDepthBuffer, int);
vtkImplBooleanMacroDef(vtkThreadSafeRenderer,PreserveDepthBuffer, int);

// Description:
// Get the time required, in seconds, for the last Render call.
vtkImplGetMacroDef(vtkThreadSafeRenderer, LastRenderTimeInSeconds, double );

// Description:
// Should be used internally only during a render
// Get the number of props that were rendered using a
// RenderOpaqueGeometry or RenderTranslucentPolygonalGeometry call.
// This is used to know if something is in the frame buffer.
vtkImplGetMacroDef(vtkThreadSafeRenderer, NumberOfPropsRendered, int );

// Description:
// Turn on/off rendering of translucent material with depth peeling
// technique. The render window must have alpha bits (ie call
// SetAlphaBitPlanes(1)) and no multisample buffer (ie call
// SetMultiSamples(0) ) to support depth peeling.
// If UseDepthPeeling is on and the GPU supports it, depth peeling is used
// for rendering translucent materials.
// If UseDepthPeeling is off, alpha blending is used.
// Initial value is off.
vtkImplSetMacroDef(vtkThreadSafeRenderer,UseDepthPeeling,int);
vtkImplGetMacroDef(vtkThreadSafeRenderer,UseDepthPeeling,int);
vtkImplBooleanMacroDef(vtkThreadSafeRenderer,UseDepthPeeling,int);

// Description:
// In case of use of depth peeling technique for rendering translucent
// material, define the threshold under which the algorithm stops to
// iterate over peel layers. This is the ratio of the number of pixels
// that have been touched by the last layer over the total number of pixels
// of the viewport area.
// Initial value is 0.0, meaning rendering have to be exact. Greater values
// may speed-up the rendering with small impact on the quality.
vtkImplSetClampMacroDef(vtkThreadSafeRenderer,OcclusionRatio,double,0.0,0.5);
vtkImplGetMacroDef(vtkThreadSafeRenderer,OcclusionRatio,double);

// Description:
// In case of depth peeling, define the maximum number of peeling layers.
// Initial value is 4. A special value of 0 means no maximum limit.
// It has to be a positive value.
vtkImplSetMacroDef(vtkThreadSafeRenderer,MaximumNumberOfPeels,int);
vtkImplGetMacroDef(vtkThreadSafeRenderer,MaximumNumberOfPeels,int);

// Description:
// Tells if the last call to DeviceRenderTranslucentPolygonalGeometry()
// actually used depth peeling.
// Initial value is false.
vtkImplGetMacroDef(vtkThreadSafeRenderer,LastRenderingUsedDepthPeeling,int);

// Description:
// Set/Get a custom Render call. Allows to hook a Render call from an
// external project.It will be used in place of vtkRenderer::Render() if it
// is not NULL and its Used ivar is set to true.
// Initial value is NULL.
vtkImplGetObjectMacroDef(vtkThreadSafeRenderer,Delegate,vtkRendererDelegate);

// Description:
// Set/Get a custom render pass.
// Initial value is NULL.
vtkImplGetObjectMacroDef(vtkThreadSafeRenderer,Pass,vtkRenderPass);

// Description:
// Get the current hardware selector. If the Selector is set, it implies the
// current render pass is for selection. Mappers/Properties may choose to
// behave differently when rendering for hardware selection.
vtkImplGetObjectMacroDef(vtkThreadSafeRenderer,Selector, vtkHardwareSelector);

// Description:
// Set/Get the texture to be used for the background. If set
// and enabled this gets the priority over the gradient background.
vtkImplGetObjectMacroDef(vtkThreadSafeRenderer,BackgroundTexture, vtkTexture);

// Description:
// Set/Get whether this viewport should have a textured background.
// Default is off.
vtkImplSetMacroDef(vtkThreadSafeRenderer,TexturedBackground,bool);
vtkImplGetMacroDef(vtkThreadSafeRenderer,TexturedBackground,bool);
vtkImplBooleanMacroDef(vtkThreadSafeRenderer,TexturedBackground,bool);

unsigned long vtkThreadSafeRenderer::GetMTime()
{
    return this->Impl->GetMTime();
}

void vtkThreadSafeRenderer::ResetCamera()
{
    this->Impl->ResetCamera();
}

void vtkThreadSafeRenderer::ResetCamera(double bounds[6])
{
    this->Impl->ResetCamera( bounds );
}

void vtkThreadSafeRenderer::SetRenderWindow(vtkRenderWindow *renderWindow )
{
    this->Impl->SetRenderWindow( renderWindow );
}

vtkRenderWindow *vtkThreadSafeRenderer::GetRenderWindow()
{
    return this->Impl->GetRenderWindow();
}

vtkWindow *vtkThreadSafeRenderer::GetVTKWindow()
{
    return this->Impl->GetVTKWindow();
}

int vtkThreadSafeRenderer::UpdateLightsGeometryToFollowCamera(void)
{
    return this->Impl->UpdateLightsGeometryToFollowCamera();
}

vtkCamera *vtkThreadSafeRenderer::MakeCamera()
{
    return this->Impl->MakeCamera();
}

double vtkThreadSafeRenderer::GetAllocatedRenderTime()
{
    return this->Impl->GetAllocatedRenderTime();
}

double vtkThreadSafeRenderer::GetTimeFactor()
{
    return this->Impl->GetTimeFactor();
}

void vtkThreadSafeRenderer::Render()
{
    this->Impl->Render();
}

void vtkThreadSafeRenderer::DeviceRender()
{
    this->Impl->DeviceRender();
}

void vtkThreadSafeRenderer::DeviceRenderTranslucentPolygonalGeometry()
{
    this->Impl->DeviceRenderTranslucentPolygonalGeometry();
}

void vtkThreadSafeRenderer::ViewToWorld()
{
    return this->Impl->ViewToWorld();
}

void vtkThreadSafeRenderer::ViewToWorld(double &wx, double &wy, double &wz)
{
    this->Impl->ViewToWorld( wx, wy, wz );
}

void vtkThreadSafeRenderer::WorldToView()
{
    this->Impl->WorldToView();
}

void vtkThreadSafeRenderer::WorldToView(double &x, double &y, double &z)
{
    this->Impl->WorldToView( x, y, z );
}

vtkAssemblyPath* vtkThreadSafeRenderer::PickProp(double selectionX1, double selectionY1,
                          double selectionX2, double selectionY2)
{
    return this->Impl->PickProp( selectionX1, selectionY1, selectionX2, selectionY2 );
}


double vtkThreadSafeRenderer::GetPickedZ()
{
    return this->Impl->GetPickedZ();
}

void vtkThreadSafeRenderer::PrintSelf( ostream& os, vtkIndent indent )
{
    this->Superclass::PrintSelf( os, indent );
    this->Impl->PrintSelf( os, indent );
}

