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


#ifndef __VTKTHREADSAFERENDERWINDOW_H__
#define __VTKTHREADSAFERENDERWINDOW_H__

#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>

#include "msvThreadSafeGetSet.h"

class vtkThreadSafeRenderWindowImpl;

class vtkThreadSafeRenderWindow : public vtkRenderWindow
{
public:
    //vtkImplTypeMacro( vtkThreadSafeRenderWindow, vtkRenderWindow, vtkRenderWindow );
    typedef vtkRenderWindow Superclass;
private:
    virtual const char* GetClassNameInternal() const { return "vtkThreadSafeRenderWindow"; }
public:
    static int IsTypeOf(const char *type);
    virtual int IsA(const char *type);
    static vtkThreadSafeRenderWindow* SafeDownCast(vtkObjectBase *o);
protected:
    virtual vtkObjectBase *NewInstanceInternal() const;
public:
    vtkThreadSafeRenderWindow *NewInstance() const;
public:
    // Description:
    // Construct object
    static vtkThreadSafeRenderWindow* New();
    virtual void PrintSelf( ostream& os, vtkIndent indent );

    // Description:
    // Add a renderer to the list of renderers.
    virtual void AddRenderer(vtkRenderer *);

    // Description:
    // Remove a renderer from the list of renderers.
    void RemoveRenderer(vtkRenderer *);

    // Description:
    // Query if a renderer is in the list of renderers.
    int HasRenderer(vtkRenderer *);

    // Description:
    // What rendering library has the user requested
    static const char *GetRenderLibrary();

    // Description:
    // Return the collection of renderers in the render window.
    vtkRendererCollection *GetRenderers();

    // Description:
    // Ask each renderer owned by this RenderWindow to render its image and
    // synchronize this process.
    virtual void Render();

    // Description:
    // Initialize the rendering process.
    virtual void Start();

    // Description:
    // Finalize the rendering process.
    virtual void Finalize();

    // Description:
    // A termination method performed at the end of the rendering process
    // to do things like swapping buffers (if necessary) or similar actions.
    virtual void Frame();

    // Description:
    // Block the thread until the actual rendering is finished().
    // Useful for measurement only.
    virtual void WaitForCompletion();

    // Description:
    // Performed at the end of the rendering process to generate image.
    // This is typically done right before swapping buffers.
    virtual void CopyResultFrame();

    // Description:
    // Create an interactor to control renderers in this window. We need
    // to know what type of interactor to create, because we might be in
    // X Windows or MS Windows.
    virtual vtkRenderWindowInteractor *MakeRenderWindowInteractor();

    // Description:
    // Hide or Show the mouse cursor, it is nice to be able to hide the
    // default cursor if you want VTK to display a 3D cursor instead.
    // Set cursor position in window (note that (0,0) is the lower left
    // corner).
    virtual void HideCursor();
    virtual void ShowCursor();
    virtual void SetCursorPosition(int , int );

    // Description:
    // Change the shape of the cursor.
    vtkImplSetMacroDecl(CurrentCursor,int);
    vtkImplGetMacroDecl(CurrentCursor,int);

    // Description:
    // Turn on/off rendering full screen window size.
    virtual void SetFullScreen(int);
    vtkImplGetMacroDecl(FullScreen,int);
    vtkImplBooleanMacroDecl(FullScreen,int);

    // Description:
    // Turn on/off window manager borders. Typically, you shouldn't turn the
    // borders off, because that bypasses the window manager and can cause
    // undesirable behavior.
    vtkImplSetMacroDecl(Borders,int);
    vtkImplGetMacroDecl(Borders,int);
    vtkImplBooleanMacroDecl(Borders,int);

    // Description:
    // Prescribe that the window be created in a stereo-capable mode. This
    // method must be called before the window is realized. Default is off.
    vtkImplGetMacroDecl(StereoCapableWindow,int);
    vtkImplBooleanMacroDecl(StereoCapableWindow,int);
    virtual void SetStereoCapableWindow(int capable);

    // Description:
    // Turn on/off stereo rendering.
    vtkImplGetMacroDecl(StereoRender,int);
    void SetStereoRender(int stereo);
    vtkImplBooleanMacroDecl(StereoRender,int);

    // Description:
    // Turn on/off the use of alpha bitplanes.
    vtkImplSetMacroDecl(AlphaBitPlanes, int);
    vtkImplGetMacroDecl(AlphaBitPlanes, int);
    vtkImplBooleanMacroDecl(AlphaBitPlanes, int);

    // Description:
    // Turn on/off point smoothing. Default is off.
    // This must be applied before the first Render.
    vtkImplSetMacroDecl(PointSmoothing,int);
    vtkImplGetMacroDecl(PointSmoothing,int);
    vtkImplBooleanMacroDecl(PointSmoothing,int);

    // Description:
    // Turn on/off line smoothing. Default is off.
    // This must be applied before the first Render.
    vtkImplSetMacroDecl(LineSmoothing,int);
    vtkImplGetMacroDecl(LineSmoothing,int);
    vtkImplBooleanMacroDecl(LineSmoothing,int);

    // Description:
    // Turn on/off polygon smoothing. Default is off.
    // This must be applied before the first Render.
    vtkImplSetMacroDecl(PolygonSmoothing,int);
    vtkImplGetMacroDecl(PolygonSmoothing,int);
    vtkImplBooleanMacroDecl(PolygonSmoothing,int);

    // Description:
    // Set/Get what type of stereo rendering to use.  CrystalEyes
    // mode uses frame-sequential capabilities available in OpenGL
    // to drive LCD shutter glasses and stereo projectors.  RedBlue
    // mode is a simple type of stereo for use with red-blue glasses.
    // Anaglyph mode is a superset of RedBlue mode, but the color
    // output channels can be configured using the AnaglyphColorMask
    // and the color of the original image can be (somewhat) maintained
    // using AnaglyphColorSaturation;  the default colors for Anaglyph
    // mode is red-cyan.  Interlaced stereo mode produces a composite
    // image where horizontal lines alternate between left and right
    // views.  StereoLeft and StereoRight modes choose one or the other
    // stereo view.  Dresden mode is yet another stereoscopic
    // interleaving.
    vtkImplGetMacroDecl(StereoType,int);
    vtkImplSetMacroDecl(StereoType,int);
    void SetStereoTypeToCrystalEyes();
    void SetStereoTypeToRedBlue();
    void SetStereoTypeToInterlaced();
    void SetStereoTypeToLeft();
    void SetStereoTypeToRight();
    void SetStereoTypeToDresden();
    void SetStereoTypeToAnaglyph();
    void SetStereoTypeToCheckerboard();

    const char *GetStereoTypeAsString();

    // Description:
    // Update the system, if needed, due to stereo rendering. For some stereo
    // methods, subclasses might need to switch some hardware settings here.
    virtual void StereoUpdate();

    // Description:
    // Intermediate method performs operations required between the rendering
    // of the left and right eye.
    virtual void StereoMidpoint();

    // Description:
    // Handles work required once both views have been rendered when using
    // stereo rendering.
    virtual void StereoRenderComplete();

    //Description:
    // Set/get the anaglyph color saturation factor.  This number ranges from
    // 0.0 to 1.0:  0.0 means that no color from the original object is
    // maintained, 1.0 means all of the color is maintained.  The default
    // value is 0.65.  Too much saturation can produce uncomfortable 3D
    // viewing because anaglyphs also use color to encode 3D.
    vtkImplSetClampMacroDecl(AnaglyphColorSaturation,float, 0.0f, 1.0f);
    vtkImplGetMacroDecl(AnaglyphColorSaturation,float);

    //Description:
    // Set/get the anaglyph color mask values.  These two numbers are bits
    // mask that control which color channels of the original stereo
    // images are used to produce the final anaglyph image.  The first
    // value is the color mask for the left view, the second the mask
    // for the right view.  If a bit in the mask is on for a particular
    // color for a view, that color is passed on to the final view; if
    // it is not set, that channel for that view is ignored.
    // The bits are arranged as r, g, and b, so r = 4, g = 2, and b = 1.
    // By default, the first value (the left view) is set to 4, and the
    // second value is set to 3.  That means that the red output channel
    // comes from the left view, and the green and blue values come from
    // the right view.
    vtkImplSetVector2MacroDecl(AnaglyphColorMask,int);
    vtkImplGetVectorMacroDecl(AnaglyphColorMask,int,2);

    // Description:
    // Remap the rendering window. This probably only works on UNIX right now.
    // It is useful for changing properties that can't normally be changed
    // once the window is up.
    virtual void WindowRemap();

    // Description:
    // Turn on/off buffer swapping between images.
    vtkImplSetMacroDecl(SwapBuffers,int);
    vtkImplGetMacroDecl(SwapBuffers,int);
    vtkImplBooleanMacroDecl(SwapBuffers,int);

    // Description:
    // Set/Get the pixel data of an image, transmitted as RGBRGBRGB. The
    // front argument indicates if the front buffer should be used or the back
    // buffer. It is the caller's responsibility to delete the resulting
    // array. It is very important to realize that the memory in this array
    // is organized from the bottom of the window to the top. The origin
    // of the screen is in the lower left corner. The y axis increases as
    // you go up the screen. So the storage of pixels is from left to right
    // and from bottom to top.
    // (x,y) is any corner of the rectangle. (x2,y2) is its opposite corner on
    // the diagonal.
    virtual int SetPixelData(int x, int y, int x2, int y2, unsigned char *data,
        int front);
    virtual int SetPixelData(int x, int y, int x2, int y2,
        vtkUnsignedCharArray *data, int front);

    // Description:
    // Same as Get/SetPixelData except that the image also contains an alpha
    // component. The image is transmitted as RGBARGBARGBA... each of which is a
    // float value. The "blend" parameter controls whether the SetRGBAPixelData
    // method blends the data with the previous contents of the frame buffer
    // or completely replaces the frame buffer data.
    virtual float *GetRGBAPixelData(int x, int y, int x2, int y2, int front);
    virtual int GetRGBAPixelData(int x, int y, int x2, int y2, int front,
        vtkFloatArray *data);
    virtual int SetRGBAPixelData(int x, int y, int x2, int y2, float *,
        int front, int blend=0);
    virtual int SetRGBAPixelData(int, int, int, int, vtkFloatArray*,
        int, int blend=0);
    virtual void ReleaseRGBAPixelData(float *data);
    virtual unsigned char *GetRGBACharPixelData(int x, int y, int x2, int y2,
        int front);
    virtual int GetRGBACharPixelData(int x, int y, int x2, int y2, int front,
        vtkUnsignedCharArray *data);
    virtual int SetRGBACharPixelData(int x,int y, int x2, int y2,
        unsigned char *data, int front,
        int blend=0);
    virtual int SetRGBACharPixelData(int x, int y, int x2, int y2,
        vtkUnsignedCharArray *data, int front,
        int blend=0);

    // Description:
    // Set/Get the zbuffer data from the frame buffer.
    // (x,y) is any corner of the rectangle. (x2,y2) is its opposite corner on
    // the diagonal.
    virtual float *GetZbufferData(int x, int y, int x2, int y2);
    virtual int GetZbufferData(int x, int y, int x2, int y2, float *z);
    virtual int GetZbufferData(int x, int y, int x2, int y2,
        vtkFloatArray *z);
    virtual int SetZbufferData(int x, int y, int x2, int y2, float *z);
    virtual int SetZbufferData(int x, int y, int x2, int y2,
        vtkFloatArray *z);
    float GetZbufferDataAtPoint(int x, int y);

    // Description:
    // Set the number of frames for doing antialiasing. The default is
    // zero. Typically five or six will yield reasonable results without
    // taking too long.
    vtkImplGetMacroDecl(AAFrames,int);
    vtkImplSetMacroDecl(AAFrames,int);

    // Description:
    // Set the number of frames for doing focal depth. The default is zero.
    // Depending on how your scene is organized you can get away with as
    // few as four frames for focal depth or you might need thirty.
    // One thing to note is that if you are using focal depth frames,
    // then you will not need many (if any) frames for antialiasing.
    vtkImplGetMacroDecl(FDFrames,int);
    vtkImplSetMacroDecl(FDFrames,int);

    // Description:
    // Set the number of sub frames for doing motion blur. The default is zero.
    // Once this is set greater than one, you will no longer see a new frame
    // for every Render().  If you set this to five, you will need to do
    // five Render() invocations before seeing the result. This isn't
    // very impressive unless something is changing between the Renders.
    // Changing this value may reset the current subframe count.
    vtkImplGetMacroDecl(SubFrames,int);
    virtual void SetSubFrames(int subFrames);

    // Description:
    // This flag is set if the window hasn't rendered since it was created
    vtkImplGetMacroDecl(NeverRendered,int);

    // Description:
    // This is a flag that can be set to interrupt a rendering that is in
    // progress.
    vtkImplGetMacroDecl(AbortRender,int);
    vtkImplSetMacroDecl(AbortRender,int);
    vtkImplGetMacroDecl(InAbortCheck,int);
    vtkImplSetMacroDecl(InAbortCheck,int);
    virtual int CheckAbortStatus();

    vtkImplGetMacroDecl(IsPicking,int);
    vtkImplSetMacroDecl(IsPicking,int);
    vtkImplBooleanMacroDecl(IsPicking,int);

    // Description:
    // Check to see if a mouse button has been pressed.  All other events
    // are ignored by this method.  Ideally, you want to abort the render
    // on any event which causes the DesiredUpdateRate to switch from
    // a high-quality rate to a more interactive rate.
    virtual int GetEventPending();

    // Description:
    // Are we rendering at the moment
    virtual int  CheckInRenderStatus();

    // Description:
    // Clear status (after an exception was thrown for example)
    virtual void ClearInRenderStatus();

    // Description:
    // Set/Get the desired update rate. This is used with
    // the vtkLODActor class. When using level of detail actors you
    // need to specify what update rate you require. The LODActors then
    // will pick the correct resolution to meet your desired update rate
    // in frames per second. A value of zero indicates that they can use
    // all the time they want to.
    virtual void SetDesiredUpdateRate(double);
    vtkImplGetMacroDecl(DesiredUpdateRate,double);

    // Description:
    // Get the number of layers for renderers.  Each renderer should have
    // its layer set individually.  Some algorithms iterate through all layers,
    // so it is not wise to set the number of layers to be exorbitantly large
    // (say bigger than 100).
    vtkImplGetMacroDecl(NumberOfLayers, int);
    vtkImplSetClampMacroDecl(NumberOfLayers, int, 1, VTK_LARGE_INTEGER);

    // Description:
    // Get the interactor associated with this render window
    vtkImplGetObjectMacroDecl(Interactor,vtkRenderWindowInteractor);

    // Description:
    // Set the interactor to the render window
    void SetInteractor(vtkRenderWindowInteractor *);

    // Description:
    // This Method detects loops of RenderWindow<->Interactor,
    // so objects are freed properly.
    virtual void UnRegister(vtkObjectBase *o);

    // Description:
    // Dummy stubs for vtkWindow API.
    virtual void SetDisplayId(void *);
    virtual void SetWindowId(void *);
    virtual void SetNextWindowId(void *);
    virtual void SetParentId(void *);
    virtual void *GetGenericDisplayId();
    virtual void *GetGenericWindowId();
    virtual void *GetGenericParentId();
    virtual void *GetGenericContext();
    virtual void *GetGenericDrawable();
    virtual void SetWindowInfo(char *);
    virtual void SetNextWindowInfo(char *);
    virtual void SetParentInfo(char *);
    // Description:
    // Get the current size of the screen in pixels.
    virtual int  *GetScreenSize();
    // Description:
    // Get the pixel data of an image, transmitted as RGBRGBRGB. The
    // front argument indicates if the front buffer should be used or the back 
    // buffer. It is the caller's responsibility to delete the resulting 
    // array. It is very important to realize that the memory in this array
    // is organized from the bottom of the window to the top. The origin
    // of the screen is in the lower left corner. The y axis increases as
    // you go up the screen. So the storage of pixels is from left to right
    // and from bottom to top.
    // (x,y) is any corner of the rectangle. (x2,y2) is its opposite corner on
    // the diagonal.
    virtual unsigned char *GetPixelData(int x, int y, int x2, int y2,
        int front);
    virtual int GetPixelData(int x, int y, int x2, int y2, int front,
        vtkUnsignedCharArray *data);

    // Description:
    // Attempt to make this window the current graphics context for the calling
    // thread.
    virtual void MakeCurrent();

    // Description:
    // Tells if this window is the current graphics context for the calling
    // thread.
    virtual bool IsCurrent();

    // Description:
    // If called, allow MakeCurrent() to skip cache-check when called.
    // MakeCurrent() reverts to original behavior of cache-checking
    // on the next render.
    virtual void SetForceMakeCurrent();

    // Description:
    // Get report of capabilities for the render window
    virtual const char *ReportCapabilities() { return "Not Implemented";};

    // Description:
    // Does this render window support OpenGL? 0-false, 1-true
    virtual int SupportsOpenGL();

    // Description:
    // Is this render window using hardware acceleration? 0-false, 1-true
    virtual int IsDirect();

    // Description:
    // This method should be defined by the subclass. How many bits of
    // precision are there in the zbuffer?
    virtual int GetDepthBufferSize();

    // Description:
    // Get the size of the color buffer.
    // Returns 0 if not able to determine otherwise sets R G B and A into buffer.
    virtual int GetColorBufferSizes(int *rgba);

    // Description:
    // Get the vtkPainterDeviceAdapter which can be used to paint on
    // this render window.
    vtkImplGetObjectMacroDecl(PainterDeviceAdapter, vtkPainterDeviceAdapter);

    // Description:
    // Set / Get the number of multisamples to use for hardware antialiasing.
    vtkImplSetMacroDecl(MultiSamples,int);
    vtkImplGetMacroDecl(MultiSamples,int);

    // Description:
    // Set / Get the availability of the stencil buffer.
    vtkImplSetMacroDecl(StencilCapable, int);
    vtkImplGetMacroDecl(StencilCapable, int);
    vtkImplBooleanMacroDecl(StencilCapable, int);

    // Description:
    // Turn on/off report of graphic errors. Initial value is false (off).
    // This flag is used by vtkGraphicErrorMacro.
    vtkImplSetMacroDecl(ReportGraphicErrors,int);
    vtkImplGetMacroDecl(ReportGraphicErrors,int);
    vtkImplBooleanMacroDecl(ReportGraphicErrors,int);

    // Description:
    // Update graphic error status, regardless of ReportGraphicErrors flag.
    // It means this method can be used in any context and is not restricted to
    // debug mode.
    virtual void CheckGraphicError();

    // Description:
    // Return the last graphic error status. Initial value is false.
    virtual int HasGraphicError();

    // Description:
    // Return a string matching the last graphic error status.
    virtual const char *GetLastGraphicErrorString();

protected:
    vtkThreadSafeRenderWindow();
    ~vtkThreadSafeRenderWindow();

private:

    vtkThreadSafeRenderWindowImpl*          Impl;
};

typedef vtkSmartPointer<vtkThreadSafeRenderWindow>  vtkThreadSafeRenderWindowSP;

#endif	// #ifndef __VTKTHREADSAFERENDERWINDOW_H__

