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


#ifndef __VTKTHREADSAFERENDERER_H__
#define __VTKTHREADSAFERENDERER_H__

#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include "msvThreadSafeGetSet.h"

class vtkThreadSafeRendererImpl;

class vtkThreadSafeRenderer : public vtkRenderer
{
public:
    vtkTypeMacro( vtkThreadSafeRenderer, vtkRenderer );
    // Description:
    // Construct object
    static vtkThreadSafeRenderer* New();
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
    vtkLightCollection *GetLights();

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
    vtkImplGetMacroDecl(TwoSidedLighting,int);
    vtkImplSetMacroDecl(TwoSidedLighting,int);
    vtkImplBooleanMacroDecl(TwoSidedLighting,int);

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
    vtkImplSetMacroDecl(LightFollowCamera,int);
    vtkImplGetMacroDecl(LightFollowCamera,int);
    vtkImplBooleanMacroDecl(LightFollowCamera,int);

    // Description:
    // Turn on/off a flag which disables the automatic light creation capability.
    // Normally in VTK if no lights are associated with the renderer, then a light
    // is automatically created. However, in special circumstances this feature is
    // undesirable, so the following boolean is provided to disable automatic
    // light creation. (Turn AutomaticLightCreation off if you do not want lights
    // to be created.)
    vtkImplGetMacroDecl(AutomaticLightCreation,int);
    vtkImplSetMacroDecl(AutomaticLightCreation,int);
    vtkImplBooleanMacroDecl(AutomaticLightCreation,int);

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
    vtkImplSetMacroDecl(Erase, int);
    vtkImplGetMacroDecl(Erase, int);
    vtkImplBooleanMacroDecl(Erase, int);

    // Description:
    // When this flag is off, render commands are ignored.  It is used to either
    // multiplex a vtkRenderWindow or render only part of a vtkRenderWindow.
    // By default, Draw is on.
    vtkImplSetMacroDecl(Draw, int);
    vtkImplGetMacroDecl(Draw, int);
    vtkImplBooleanMacroDecl(Draw, int);

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
    vtkImplSetVector3MacroDecl(Ambient,double);
    vtkImplGetVectorMacroDecl(Ambient,double,3);

    // Description:
    // Set/Get the amount of time this renderer is allowed to spend
    // rendering its scene. This is used by vtkLODActor's.
    vtkImplSetMacroDecl(AllocatedRenderTime,double);
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
    virtual void DeviceRender();

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
    vtkImplSetClampMacroDecl(NearClippingPlaneTolerance,double,0,0.99);
    vtkImplBooleanMacroDecl(NearClippingPlaneTolerance,double);

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
    vtkRenderWindow *GetRenderWindow();
    virtual vtkWindow *GetVTKWindow();

    // Description:
    // Turn on/off using backing store. This may cause the re-rendering
    // time to be slightly slower when the view changes. But it is
    // much faster when the image has not changed, such as during an
    // expose event.
    vtkImplSetMacroDecl(BackingStore,int);
    vtkImplGetMacroDecl(BackingStore,int);
    vtkImplBooleanMacroDecl(BackingStore,int);

    // Description:
    // Turn on/off interactive status.  An interactive renderer is one that
    // can receive events from an interactor.  Should only be set if
    // there are multiple renderers in the same section of the viewport.
    vtkImplSetMacroDecl(Interactive,int);
    vtkImplGetMacroDecl(Interactive,int);
    vtkImplBooleanMacroDecl(Interactive,int);

    // Description:
    // Set/Get the layer that this renderer belongs to.  This is only used if
    // there are layered renderers.
    vtkImplSetMacroDecl(Layer, int);
    vtkImplGetMacroDecl(Layer, int);

    // Description:
    // Normally a renderer is treated as transparent if Layer > 0. To treat a
    // renderer at Layer 0 as transparent, set this flag to true.
    vtkImplSetMacroDecl(PreserveDepthBuffer, int);
    vtkImplGetMacroDecl(PreserveDepthBuffer, int);
    vtkImplBooleanMacroDecl(PreserveDepthBuffer, int);

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
    vtkImplGetMacroDecl( LastRenderTimeInSeconds, double );

    // Description:
    // Should be used internally only during a render
    // Get the number of props that were rendered using a
    // RenderOpaqueGeometry or RenderTranslucentPolygonalGeometry call.
    // This is used to know if something is in the frame buffer.
    vtkImplGetMacroDecl( NumberOfPropsRendered, int );

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
    { return (this->ActiveCamera != NULL); }


    // Description:
    // Turn on/off rendering of translucent material with depth peeling
    // technique. The render window must have alpha bits (ie call
    // SetAlphaBitPlanes(1)) and no multisample buffer (ie call
    // SetMultiSamples(0) ) to support depth peeling.
    // If UseDepthPeeling is on and the GPU supports it, depth peeling is used
    // for rendering translucent materials.
    // If UseDepthPeeling is off, alpha blending is used.
    // Initial value is off.
    vtkImplSetMacroDecl(UseDepthPeeling,int);
    vtkImplGetMacroDecl(UseDepthPeeling,int);
    vtkImplBooleanMacroDecl(UseDepthPeeling,int);

    // Description:
    // In case of use of depth peeling technique for rendering translucent
    // material, define the threshold under which the algorithm stops to
    // iterate over peel layers. This is the ratio of the number of pixels
    // that have been touched by the last layer over the total number of pixels
    // of the viewport area.
    // Initial value is 0.0, meaning rendering have to be exact. Greater values
    // may speed-up the rendering with small impact on the quality.
    vtkImplSetClampMacroDecl(OcclusionRatio,double,0.0,0.5);
    vtkImplGetMacroDecl(OcclusionRatio,double);

    // Description:
    // In case of depth peeling, define the maximum number of peeling layers.
    // Initial value is 4. A special value of 0 means no maximum limit.
    // It has to be a positive value.
    vtkImplSetMacroDecl(MaximumNumberOfPeels,int);
    vtkImplGetMacroDecl(MaximumNumberOfPeels,int);

    // Description:
    // Tells if the last call to DeviceRenderTranslucentPolygonalGeometry()
    // actually used depth peeling.
    // Initial value is false.
    vtkImplGetMacroDecl(LastRenderingUsedDepthPeeling,int);

    // Description:
    // Set/Get a custom Render call. Allows to hook a Render call from an
    // external project.It will be used in place of vtkRenderer::Render() if it
    // is not NULL and its Used ivar is set to true.
    // Initial value is NULL.
    void SetDelegate(vtkRendererDelegate *d);
    vtkImplGetObjectMacroDecl(Delegate,vtkRendererDelegate);

    // Description:
    // Set/Get a custom render pass.
    // Initial value is NULL.
    void SetPass(vtkRenderPass *p);
    vtkImplGetObjectMacroDecl(Pass,vtkRenderPass);

    // Description:
    // Get the current hardware selector. If the Selector is set, it implies the
    // current render pass is for selection. Mappers/Properties may choose to
    // behave differently when rendering for hardware selection.
    vtkImplGetObjectMacroDecl(Selector, vtkHardwareSelector);

    // Description:
    // Set/Get the texture to be used for the background. If set
    // and enabled this gets the priority over the gradient background.
    void SetBackgroundTexture(vtkTexture*);
    vtkImplGetObjectMacroDecl(BackgroundTexture, vtkTexture);

    // Description:
    // Set/Get whether this viewport should have a textured background.
    // Default is off.
    vtkImplSetMacroDecl(TexturedBackground,bool);
    vtkImplGetMacroDecl(TexturedBackground,bool);
    vtkImplBooleanMacroDecl(TexturedBackground,bool);

    // Description:
    // Return the Z value for the last picked Prop.
    virtual double GetPickedZ();

protected:
    vtkThreadSafeRenderer();
    ~vtkThreadSafeRenderer();

    // Default implementation in order to avoid
    // abstract class error. These will not be called by client code
    virtual void DevicePickRender() {};
    // Enter a pick mode
    virtual void StartPick(unsigned int pickFromSize) {};
    // Set the pick id to the next id before drawing an object
    virtual void UpdatePickId() {};
    // Exit Pick mode
    virtual void DonePick() {};
    // Return the id of the picked object, only valid after a call to DonePick
    virtual unsigned int GetPickedId() { return 0; };
    // Return the number of objects picked, only valid after a call to DonePick
    virtual unsigned int GetNumPickedIds() { return 0; };
    // Put no more than atMost picked object ids into the callerBuffer and
    // return the number of picked objects returned.
    virtual int GetPickedIds(unsigned int atMost, unsigned int *callerBuffer) { return 0; };


private:

    vtkThreadSafeRendererImpl*          Impl;
};

typedef vtkSmartPointer<vtkThreadSafeRenderer>  vtkThreadSafeRendererSP;

#endif	// #ifndef __VTKTHREADSAFERENDERER_H__

