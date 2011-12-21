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

#include "vtkThreadSafeRenderWindow.h"

#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkMutexLock.h>
#include <vtkMultiThreader.h>
#include "vtkThreadSafeRenderer.h"
#include "vtkThreadSafeRenderWindowInteractor.h"


#include <vector>

using namespace std;

////////////////////////////
// Private Implementor class
////////////////////////////

class vtkThreadSafeRenderWindowImpl: public vtkObject
{
    typedef vtkObject Superclass;
    virtual const char* GetClassNameInternal() const { return "vtkThreadSafeRenderWindowImpl"; }
public:
    static int IsTypeOf(const char *type)
    {
        if( !strcmp("vtkThreadSafeRenderWindowImpl",type) )
        {
            return 1;
        }
        return vtkRenderWindow::IsTypeOf(type);
    }
    virtual int IsA(const char *type)
    {
        if( this->vtkThreadSafeRenderWindowImpl::IsTypeOf(type) )
        {
            return 1;
        }
        return this->Impl->IsA( type );
    }
    static vtkThreadSafeRenderWindowImpl* SafeDownCast(vtkObjectBase *o)
    {
        if( o && o->IsA( "vtkThreadSafeRenderWindowImpl" ) )
        {
            return static_cast<vtkThreadSafeRenderWindowImpl *>(o);
        }
        return NULL;
    }
protected:
    virtual vtkObjectBase *NewInstanceInternal() const
    {
        return vtkThreadSafeRenderWindowImpl::New();
    }
public:
    vtkThreadSafeRenderWindowImpl *NewInstance() const
    {
        return vtkThreadSafeRenderWindowImpl::SafeDownCast(this->NewInstanceInternal());
    }

public:
    // Description:
    // Constructor and destructor
    vtkThreadSafeRenderWindowImpl( vtkThreadSafeRenderWindow* publicInterface );
    ~vtkThreadSafeRenderWindowImpl();

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
    vtkRendererCollection *GetRenderers()
    {
        vtkRendererCollection* aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->GetRenderers();
        this->Mutex->Unlock();
        return aux;
    };

    // Description:
    // Ask each renderer owned by this RenderWindow to render its image and
    // synchronize this process.
    virtual void Render();

    // Description:
    // Initialize the rendering process.
    virtual void Start()
    {
        this->Mutex->Lock();
        this->Impl->Start();
        this->Mutex->Unlock();
    }

    // Description:
    // Finalize the rendering process.
    virtual void Finalize()
    {
        this->Mutex->Lock();
        this->Impl->Finalize();
        this->Mutex->Unlock();
    }

    // Description:
    // A termination method performed at the end of the rendering process
    // to do things like swapping buffers (if necessary) or similar actions.
    virtual void Frame()
    {
        this->Mutex->Lock();
        this->Impl->Frame();
        this->Mutex->Unlock();
    }

    // Description:
    // Block the thread until the actual rendering is finished().
    // Useful for measurement only.
    virtual void WaitForCompletion()
    {
        this->Mutex->Lock();
        this->Impl->WaitForCompletion();
        this->Mutex->Unlock();
    }

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
    virtual void HideCursor()
    {
        this->Mutex->Lock();
        this->Impl->HideCursor();
        this->Mutex->Unlock();
    }
    virtual void ShowCursor()
    {
        this->Mutex->Lock();
        this->Impl->ShowCursor();
        this->Mutex->Unlock();
    }
    virtual void SetCursorPosition(int x, int y)
    {
        this->Mutex->Lock();
        this->Impl->SetCursorPosition( x, y );
        this->Mutex->Unlock();
    }

    // Description:
    // Change the shape of the cursor.
    vtkSafeSetMacro(CurrentCursor,int);
    vtkSafeGetMacro(CurrentCursor,int);

    // Description:
    // Turn on/off rendering full screen window size.
    virtual void SetFullScreen(int fullScreen )
    {
        this->Mutex->Lock();
        this->Impl->SetFullScreen( fullScreen );
        this->Mutex->Unlock();
    }
    vtkSafeGetMacro(FullScreen,int);
    vtkSafeBooleanMacro(FullScreen,int);

    // Description:
    // Turn on/off window manager borders. Typically, you shouldn't turn the
    // borders off, because that bypasses the window manager and can cause
    // undesirable behavior.
    vtkSafeSetMacro(Borders,int);
    vtkSafeGetMacro(Borders,int);
    vtkSafeBooleanMacro(Borders,int);

    // Description:
    // Prescribe that the window be created in a stereo-capable mode. This
    // method must be called before the window is realized. Default is off.
    vtkSafeGetMacro(StereoCapableWindow,int);
    vtkSafeBooleanMacro(StereoCapableWindow,int);
    virtual void SetStereoCapableWindow(int capable);

    // Description:
    // Turn on/off stereo rendering.
    vtkSafeGetMacro(StereoRender,int);
    void SetStereoRender(int stereo);
    vtkSafeBooleanMacro(StereoRender,int);

    // Description:
    // Turn on/off the use of alpha bitplanes.
    vtkSafeSetMacro(AlphaBitPlanes, int);
    vtkSafeGetMacro(AlphaBitPlanes, int);
    vtkSafeBooleanMacro(AlphaBitPlanes, int);

    // Description:
    // Turn on/off point smoothing. Default is off.
    // This must be applied before the first Render.
    vtkSafeSetMacro(PointSmoothing,int);
    vtkSafeGetMacro(PointSmoothing,int);
    vtkSafeBooleanMacro(PointSmoothing,int);

    // Description:
    // Turn on/off line smoothing. Default is off.
    // This must be applied before the first Render.
    vtkSafeSetMacro(LineSmoothing,int);
    vtkSafeGetMacro(LineSmoothing,int);
    vtkSafeBooleanMacro(LineSmoothing,int);

    // Description:
    // Turn on/off polygon smoothing. Default is off.
    // This must be applied before the first Render.
    vtkSafeSetMacro(PolygonSmoothing,int);
    vtkSafeGetMacro(PolygonSmoothing,int);
    vtkSafeBooleanMacro(PolygonSmoothing,int);

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
    vtkSafeGetMacro(StereoType,int);
    vtkSafeSetMacro(StereoType,int);
    void SetStereoTypeToCrystalEyes()
    {this->SetStereoType(VTK_STEREO_CRYSTAL_EYES);}
    void SetStereoTypeToRedBlue()
    {this->SetStereoType(VTK_STEREO_RED_BLUE);}
    void SetStereoTypeToInterlaced()
    {this->SetStereoType(VTK_STEREO_INTERLACED);}
    void SetStereoTypeToLeft()
    {this->SetStereoType(VTK_STEREO_LEFT);}
    void SetStereoTypeToRight()
    {this->SetStereoType(VTK_STEREO_RIGHT);}
    void SetStereoTypeToDresden()
    {this->SetStereoType(VTK_STEREO_DRESDEN);}
    void SetStereoTypeToAnaglyph()
    {this->SetStereoType(VTK_STEREO_ANAGLYPH);}
    void SetStereoTypeToCheckerboard()
    {this->SetStereoType(VTK_STEREO_CHECKERBOARD);}

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
    vtkSafeSetClampMacro(AnaglyphColorSaturation,float, 0.0f, 1.0f);
    vtkSafeGetMacro(AnaglyphColorSaturation,float);

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
    vtkSafeSetVector2Macro(AnaglyphColorMask,int);
    vtkSafeGetVectorMacro(AnaglyphColorMask,int,2);

    // Description:
    // Remap the rendering window. This probably only works on UNIX right now.
    // It is useful for changing properties that can't normally be changed
    // once the window is up.
    virtual void WindowRemap()
    {
        this->Mutex->Lock();
        this->Impl->WindowRemap();
        this->Mutex->Unlock();
    }


    // Description:
    // Turn on/off buffer swapping between images.
    vtkSafeSetMacro(SwapBuffers,int);
    vtkSafeGetMacro(SwapBuffers,int);
    vtkSafeBooleanMacro(SwapBuffers,int);

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
        int front)
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->SetPixelData( x, y, x2, y2, data, front );
        this->Mutex->Unlock();
        return aux;
    }
    virtual int SetPixelData(int x, int y, int x2, int y2,
        vtkUnsignedCharArray *data, int front)
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->SetPixelData( x, y, x2, y2, data, front );
        this->Mutex->Unlock();
        return aux;
    }

    // Description:
    // Same as Get/SetPixelData except that the image also contains an alpha
    // component. The image is transmitted as RGBARGBARGBA... each of which is a
    // float value. The "blend" parameter controls whether the SetRGBAPixelData
    // method blends the data with the previous contents of the frame buffer
    // or completely replaces the frame buffer data.
    virtual float *GetRGBAPixelData(int x, int y, int x2, int y2, int front)
    {
        float* aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->GetRGBAPixelData( x, y, x2, y2, front );
        this->Mutex->Unlock();
        return aux;
    }
    virtual int GetRGBAPixelData(int x, int y, int x2, int y2, int front,
        vtkFloatArray *data)
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->GetRGBAPixelData( x, y, x2, y2, front, data );
        this->Mutex->Unlock();
        return aux;
    }
    virtual int SetRGBAPixelData(int x, int y, int x2, int y2, float *data,
        int front, int blend=0)
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->SetRGBAPixelData( x, y, x2, y2, data, front, blend );
        this->Mutex->Unlock();
        return aux;
    }
    virtual int SetRGBAPixelData(int x, int y, int x2, int y2, vtkFloatArray* data ,
        int front, int blend=0)
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->SetRGBAPixelData( x, y, x2, y2, data, front, blend );
        this->Mutex->Unlock();
        return aux;
    }
    virtual void ReleaseRGBAPixelData(float *data)
    {
        this->Mutex->Lock();
        this->Impl->ReleaseRGBAPixelData( data );
        this->Mutex->Unlock();
    }
    virtual unsigned char *GetRGBACharPixelData(int x, int y, int x2, int y2,
        int front)
    {
        unsigned char* aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->GetRGBACharPixelData( x, y, x2, y2, front );
        this->Mutex->Unlock();
        return aux;
    }
    virtual int GetRGBACharPixelData(int x, int y, int x2, int y2, int front,
        vtkUnsignedCharArray *data)
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->GetRGBACharPixelData( x, y, x2, y2, front, data );
        this->Mutex->Unlock();
        return aux;
    }
    virtual int SetRGBACharPixelData(int x,int y, int x2, int y2,
        unsigned char *data, int front,
        int blend=0)
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->SetRGBACharPixelData( x, y, x2, y2, data, front, blend );
        this->Mutex->Unlock();
        return aux;
    }
    virtual int SetRGBACharPixelData(int x, int y, int x2, int y2,
        vtkUnsignedCharArray *data, int front,
        int blend=0)
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->SetRGBACharPixelData( x, y, x2, y2, data, front, blend );
        this->Mutex->Unlock();
        return aux;
    }

    // Description:
    // Set/Get the zbuffer data from the frame buffer.
    // (x,y) is any corner of the rectangle. (x2,y2) is its opposite corner on
    // the diagonal.
    virtual float *GetZbufferData(int x, int y, int x2, int y2)
    {
        float* aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->GetZbufferData( x, y, x2, y2 );
        this->Mutex->Unlock();
        return aux;
    }
    virtual int GetZbufferData(int x, int y, int x2, int y2, float *z)
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->GetZbufferData( x, y, x2, y2, z );
        this->Mutex->Unlock();
        return aux;
    }
    virtual int GetZbufferData(int x, int y, int x2, int y2,
        vtkFloatArray *z)
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->GetZbufferData( x, y, x2, y2, z );
        this->Mutex->Unlock();
        return aux;
    }
    virtual int SetZbufferData(int x, int y, int x2, int y2, float *z)
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->SetZbufferData( x, y, x2, y2, z );
        this->Mutex->Unlock();
        return aux;
    }
    virtual int SetZbufferData(int x, int y, int x2, int y2,
        vtkFloatArray *z)
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->SetZbufferData( x, y, x2, y2, z );
        this->Mutex->Unlock();
        return aux;
    }
    float GetZbufferDataAtPoint(int x, int y)
    {
        float aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->GetZbufferDataAtPoint( x, y );
        this->Mutex->Unlock();
        return aux;
    }

    // Description:
    // Set the number of frames for doing antialiasing. The default is
    // zero. Typically five or six will yield reasonable results without
    // taking too long.
    vtkSafeGetMacro(AAFrames,int);
    vtkSafeSetMacro(AAFrames,int);

    // Description:
    // Set the number of frames for doing focal depth. The default is zero.
    // Depending on how your scene is organized you can get away with as
    // few as four frames for focal depth or you might need thirty.
    // One thing to note is that if you are using focal depth frames,
    // then you will not need many (if any) frames for antialiasing.
    vtkSafeGetMacro(FDFrames,int);
    vtkSafeSetMacro(FDFrames,int);

    // Description:
    // Set the number of sub frames for doing motion blur. The default is zero.
    // Once this is set greater than one, you will no longer see a new frame
    // for every Render().  If you set this to five, you will need to do
    // five Render() invocations before seeing the result. This isn't
    // very impressive unless something is changing between the Renders.
    // Changing this value may reset the current subframe count.
    vtkSafeGetMacro(SubFrames,int);
    virtual void SetSubFrames(int subFrames);

    // Description:
    // This flag is set if the window hasn't rendered since it was created
    vtkSafeGetMacro(NeverRendered,int);

    // Description:
    // This is a flag that can be set to interrupt a rendering that is in
    // progress.
    vtkSafeGetMacro(AbortRender,int);
    vtkSafeSetMacro(AbortRender,int);
    vtkSafeGetMacro(InAbortCheck,int);
    vtkSafeSetMacro(InAbortCheck,int);
    virtual int CheckAbortStatus();

    vtkSafeGetMacro(IsPicking,int);
    vtkSafeSetMacro(IsPicking,int);
    vtkSafeBooleanMacro(IsPicking,int);

    // Description:
    // Check to see if a mouse button has been pressed.  All other events
    // are ignored by this method.  Ideally, you want to abort the render
    // on any event which causes the DesiredUpdateRate to switch from
    // a high-quality rate to a more interactive rate.
    virtual int GetEventPending()
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->GetEventPending();
        this->Mutex->Unlock();
        return aux;
    }


    // Description:
    // Are we rendering at the moment
    virtual int  CheckInRenderStatus()
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->CheckInRenderStatus();
        this->Mutex->Unlock();
        return aux;
    }

    // Description:
    // Clear status (after an exception was thrown for example)
    virtual void ClearInRenderStatus()
    {
        this->Mutex->Lock();
        this->Impl->ClearInRenderStatus();
        this->Mutex->Unlock();
    }

    // Description:
    // Set/Get the desired update rate. This is used with
    // the vtkLODActor class. When using level of detail actors you
    // need to specify what update rate you require. The LODActors then
    // will pick the correct resolution to meet your desired update rate
    // in frames per second. A value of zero indicates that they can use
    // all the time they want to.
    virtual void SetDesiredUpdateRate(double);
    vtkSafeGetMacro(DesiredUpdateRate,double);

    // Description:
    // Get the number of layers for renderers.  Each renderer should have
    // its layer set individually.  Some algorithms iterate through all layers,
    // so it is not wise to set the number of layers to be exorbitantly large
    // (say bigger than 100).
    vtkSafeGetMacro(NumberOfLayers, int);
    vtkSafeSetClampMacro(NumberOfLayers, int, 1, VTK_LARGE_INTEGER);

    // Description:
    // Get the interactor associated with this render window
    vtkSafeGetObjectMacro(Interactor,vtkRenderWindowInteractor);

    // Description:
    // Set the interactor to the render window
    void SetInteractor(vtkRenderWindowInteractor *);

    // Description:
    // This Method detects loops of RenderWindow<->Interactor,
    // so objects are freed properly.
    virtual void UnRegister(vtkObjectBase *o);

    // Description:
    // Dummy stubs for vtkWindow API.
    virtual void SetDisplayId(void *displayId)
    {
        this->Mutex->Lock();
        this->Impl->SetDisplayId( displayId );
        this->Mutex->Unlock();
    }
    virtual void SetWindowId(void *windowId)
    {
        this->Mutex->Lock();
        this->Impl->SetWindowId( windowId );
        this->Mutex->Unlock();
    }
    virtual void SetNextWindowId(void *windowId)
    {
        this->Mutex->Lock();
        this->Impl->SetNextWindowId( windowId );
        this->Mutex->Unlock();
    }
    virtual void SetParentId(void *parentId)
    {
        this->Mutex->Lock();
        this->Impl->SetParentId( parentId );
        this->Mutex->Unlock();
    }
    virtual void *GetGenericDisplayId()
    {
        void* aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->GetGenericDisplayId();
        this->Mutex->Unlock();
        return aux;
    }
    virtual void *GetGenericWindowId()
    {
        void* aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->GetGenericWindowId();
        this->Mutex->Unlock();
        return aux;
    }
    virtual void *GetGenericParentId()
    {
        void* aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->GetGenericParentId();
        this->Mutex->Unlock();
        return aux;
    }
    virtual void *GetGenericContext()
    {
        void* aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->GetGenericContext();
        this->Mutex->Unlock();
        return aux;
    }
    virtual void *GetGenericDrawable()
    {
        void* aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->GetGenericDrawable();
        this->Mutex->Unlock();
        return aux;
    }
    virtual void SetWindowInfo(char *info)
    {
        this->Mutex->Lock();
        this->Impl->SetWindowInfo( info );
        this->Mutex->Unlock();
    }
    virtual void SetNextWindowInfo(char *info)
    {
        this->Mutex->Lock();
        this->Impl->SetNextWindowInfo( info );
        this->Mutex->Unlock();
    }
    virtual void SetParentInfo(char *info )
    {
        this->Mutex->Lock();
        this->Impl->SetParentInfo( info );
        this->Mutex->Unlock();
    }
    // Description:
    // Get the current size of the screen in pixels.
    virtual int  *GetScreenSize()
    {
        int* aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->GetScreenSize();
        this->Mutex->Unlock();
        return aux;
    }
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
        int front)
    {
        unsigned char* aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->GetPixelData( x, y, x2, y2, front );
        this->Mutex->Unlock();
        return aux;
    }
    virtual int GetPixelData(int x, int y, int x2, int y2, int front,
        vtkUnsignedCharArray *data)
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->GetPixelData( x, y, x2, y2, front, data );
        this->Mutex->Unlock();
        return aux;
    }

    // Description:
    // Attempt to make this window the current graphics context for the calling
    // thread.
    virtual void MakeCurrent()
    {
        this->Mutex->Lock();
        this->Impl->MakeCurrent();
        this->Mutex->Unlock();
    }

    // Description:
    // Tells if this window is the current graphics context for the calling
    // thread.
    virtual bool IsCurrent()
    {
        bool aux( 0 );
        this->Mutex->Lock();
        this->Impl->IsCurrent();
        this->Mutex->Unlock();
        return aux;
    }

    // Description:
    // If called, allow MakeCurrent() to skip cache-check when called.
    // MakeCurrent() reverts to original behavior of cache-checking
    // on the next render.
    virtual void SetForceMakeCurrent()
    {
        this->Mutex->Lock();
        this->Impl->SetForceMakeCurrent();
        this->Mutex->Unlock();
    }

    // Description:
    // Get report of capabilities for the render window
    virtual const char *ReportCapabilities() { return "Not Implemented";};

    // Description:
    // Does this render window support OpenGL? 0-false, 1-true
    virtual int SupportsOpenGL()
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->SupportsOpenGL();
        this->Mutex->Unlock();
        return aux;
    }

    // Description:
    // Is this render window using hardware acceleration? 0-false, 1-true
    virtual int IsDirect()
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->IsDirect();
        this->Mutex->Unlock();
        return aux;
    }

    // Description:
    // This method should be defined by the subclass. How many bits of
    // precision are there in the zbuffer?
    virtual int GetDepthBufferSize()
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->GetDepthBufferSize();
        this->Mutex->Unlock();
        return aux;
    }


    // Description:
    // Get the size of the color buffer.
    // Returns 0 if not able to determine otherwise sets R G B and A into buffer.
    virtual int GetColorBufferSizes(int *rgba)
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->GetColorBufferSizes( rgba );
        this->Mutex->Unlock();
        return aux;
    }

    // Description:
    // Get the vtkPainterDeviceAdapter which can be used to paint on
    // this render window.
    vtkSafeGetObjectMacro(PainterDeviceAdapter, vtkPainterDeviceAdapter);

    // Description:
    // Set / Get the number of multisamples to use for hardware antialiasing.
    vtkSafeSetMacro(MultiSamples,int);
    vtkSafeGetMacro(MultiSamples,int);

    // Description:
    // Set / Get the availability of the stencil buffer.
    vtkSafeSetMacro(StencilCapable, int);
    vtkSafeGetMacro(StencilCapable, int);
    vtkSafeBooleanMacro(StencilCapable, int);

    // Description:
    // Turn on/off report of graphic errors. Initial value is false (off).
    // This flag is used by vtkGraphicErrorMacro.
    vtkSafeSetMacro(ReportGraphicErrors,int);
    vtkSafeGetMacro(ReportGraphicErrors,int);
    vtkSafeBooleanMacro(ReportGraphicErrors,int);

    // Description:
    // Update graphic error status, regardless of ReportGraphicErrors flag.
    // It means this method can be used in any context and is not restricted to
    // debug mode.
    virtual void CheckGraphicError()
    {
        this->Mutex->Lock();
        this->Impl->CheckGraphicError();
        this->Mutex->Unlock();
    }

    // Description:
    // Return the last graphic error status. Initial value is false.
    virtual int HasGraphicError()
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->HasGraphicError();
        this->Mutex->Unlock();
        return aux;
    }

    // Description:
    // Return a string matching the last graphic error status.
    virtual const char *GetLastGraphicErrorString()
    {
        const char* aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->GetLastGraphicErrorString();
        this->Mutex->Unlock();
        return aux;
    }

private:

    vtkRenderWindow*                        Impl;
    vtkMutexLock*                           Mutex;
    vtkMultiThreaderIDType                  LockOwnerThreadID;
    vtkRendererCollection*                  ThreadSafeRenderers;
    vtkThreadSafeRenderWindow*              PublicInterface;
    vtkThreadSafeRenderWindowInteractor*    ThreadSafeRenderWindowInteractor;
};

vtkThreadSafeRenderWindowImpl::vtkThreadSafeRenderWindowImpl( vtkThreadSafeRenderWindow* publicInterface )
: Impl( 0 )
, Mutex( 0 )
, LockOwnerThreadID( -1 )
, PublicInterface( publicInterface )
, ThreadSafeRenderWindowInteractor( 0 )
{
    this->Mutex = vtkMutexLock::New();
    this->Impl = vtkRenderWindow::New();
}

vtkThreadSafeRenderWindowImpl::~vtkThreadSafeRenderWindowImpl()
{
    this->Mutex->Unlock();
    this->Mutex->Delete();
    this->Impl->Delete();
}

void vtkThreadSafeRenderWindowImpl::PrintSelf( ostream& os, vtkIndent indent )
{
    this->Superclass::PrintSelf( os, indent );
    this->Mutex->Lock();
    this->Impl->PrintSelf( os, indent );
    this->Mutex->Unlock();
}

//----------------------------------------------------------------------------
// Create an interactor that will work with this renderer.
vtkRenderWindowInteractor *vtkThreadSafeRenderWindowImpl::MakeRenderWindowInteractor()
{
    vtkRenderWindowInteractor* aux( 0 );
    this->Mutex->Lock();
    aux = this->Impl->MakeRenderWindowInteractor();
    this->Mutex->Unlock();
    return aux;
}

//----------------------------------------------------------------------------
// Set the interactor that will work with this renderer.
void vtkThreadSafeRenderWindowImpl::SetInteractor(vtkRenderWindowInteractor *rwi)
{
    this->Mutex->Lock();
    vtkThreadSafeRenderWindowInteractor* tsrwi = dynamic_cast<vtkThreadSafeRenderWindowInteractor*>( rwi );
    if( tsrwi != 0 )
    {
        this->Impl->SetInteractor( tsrwi );
        if( this->ThreadSafeRenderWindowInteractor != tsrwi )
        {
            vtkThreadSafeRenderWindowInteractor* temp = this->ThreadSafeRenderWindowInteractor;
            this->ThreadSafeRenderWindowInteractor = tsrwi;
            if( temp != 0 )
            {
                //temp->UnRegister( this->PublicInterface );
            }
            if( this->ThreadSafeRenderWindowInteractor !=  0 )
            {
                //this->this->ThreadSafeRenderWindow->Register( this->PublicInterface );
                /*
                int isize[2];
                this->ThreadSafeRenderWindowInteractor->GetSize(isize);
                if (0 == isize[0] && 0 == isize[1])
                {
                    this->ThreadSafeRenderWindowInteractor->SetSize(this->GetSize());
                }
                */

                if( this->ThreadSafeRenderWindowInteractor->GetRenderWindow() != this->PublicInterface )
                {
                    this->ThreadSafeRenderWindowInteractor->SetRenderWindow( this->PublicInterface );
                }
            }
        }
    }
    else
    {
        // Should assert
        this->Impl->SetInteractor( rwi );
    }
    this->Mutex->Unlock();
}

//----------------------------------------------------------------------------
void vtkThreadSafeRenderWindowImpl::SetSubFrames(int subFrames)
{
    this->Mutex->Lock();
    this->Impl->SetSubFrames( subFrames );
    this->Mutex->Unlock();
}

//----------------------------------------------------------------------------
void vtkThreadSafeRenderWindowImpl::SetDesiredUpdateRate(double rate)
{
    this->Mutex->Lock();
    this->Impl->SetDesiredUpdateRate( rate );
    this->Mutex->Unlock();
}


//----------------------------------------------------------------------------
//
// Set the variable that indicates that we want a stereo capable window
// be created. This method can only be called before a window is realized.
//
void vtkThreadSafeRenderWindowImpl::SetStereoCapableWindow(int capable)
{
    this->Mutex->Lock();
    this->Impl->SetStereoCapableWindow( capable );
    this->Mutex->Unlock();
}

//----------------------------------------------------------------------------
// Turn on stereo rendering
void vtkThreadSafeRenderWindowImpl::SetStereoRender(int stereo)
{
    this->Mutex->Lock();
    this->Impl->SetStereoRender( stereo );
    this->Mutex->Unlock();
}

//----------------------------------------------------------------------------
// Ask each renderer owned by this RenderWindow to render its image and
// synchronize this process.
void vtkThreadSafeRenderWindowImpl::Render()
{
    this->Mutex->Lock();
    this->Impl->Render();
    this->Mutex->Unlock();

    //this->InvokeEvent(vtkCommand::EndEvent,NULL);
}
/*
protected:
//----------------------------------------------------------------------------
// Handle rendering any antialiased frames.
void vtkThreadSafeRenderWindowImpl::DoAARender()
{
    this->Mutex->Lock();
    this->Impl->DoAARender();
    this->Mutex->Unlock();
}
*/

/*
protected:
//----------------------------------------------------------------------------
// Handle rendering any focal depth frames.
void vtkThreadSafeRenderWindowImpl::DoFDRender()
{
    this->Mutex->Lock();
    this->Impl->DoFDRender();
    this->Mutex->Unlock();
}
*/

/*
protected:
//----------------------------------------------------------------------------
// Handle rendering the two different views for stereo rendering.
void vtkThreadSafeRenderWindowImpl::DoStereoRender()
{
    this->Mutex->Lock();
    this->Impl->DoStereoRender();
    this->Mutex->Unlock();
}
*/

//----------------------------------------------------------------------------
// Add a renderer to the list of renderers.
void vtkThreadSafeRenderWindowImpl::AddRenderer(vtkRenderer *ren)
{
    this->Mutex->Lock();
    vtkThreadSafeRenderer* tsren = dynamic_cast<vtkThreadSafeRenderer*>( ren );
    if( tsren != 0 )
    {
        tsren->SetRenderWindow( this->PublicInterface );
        this->Impl->AddRenderer( tsren );
    }
    else
    {
        // Should assert?
        this->Impl->AddRenderer( ren );
    }
    this->Mutex->Unlock();
}

//----------------------------------------------------------------------------
// Remove a renderer from the list of renderers.
void vtkThreadSafeRenderWindowImpl::RemoveRenderer(vtkRenderer *ren)
{
    this->Mutex->Lock();
    this->Impl->RemoveRenderer( ren );
    this->Mutex->Unlock();
}

int vtkThreadSafeRenderWindowImpl::HasRenderer(vtkRenderer *ren)
{
    int aux( 0 );
    this->Mutex->Lock();
    aux = this->Impl->HasRenderer( ren );
    this->Mutex->Unlock();
    return aux;
}

//----------------------------------------------------------------------------
int vtkThreadSafeRenderWindowImpl::CheckAbortStatus()
{
    int aux( 0 );
    this->Mutex->Lock();
    aux = this->Impl->CheckAbortStatus();
    this->Mutex->Unlock();
    return aux;
}

//----------------------------------------------------------------------------
// Update the system, if needed, due to stereo rendering. For some stereo
// methods, subclasses might need to switch some hardware settings here.
void vtkThreadSafeRenderWindowImpl::StereoUpdate(void)
{
    this->Mutex->Lock();
    this->Impl->StereoUpdate();
    this->Mutex->Unlock();
}

//----------------------------------------------------------------------------
// Intermediate method performs operations required between the rendering
// of the left and right eye.
void vtkThreadSafeRenderWindowImpl::StereoMidpoint(void)
{
    this->Mutex->Lock();
    this->Impl->StereoMidpoint();
    this->Mutex->Unlock();
}

//----------------------------------------------------------------------------
// Handles work required once both views have been rendered when using
// stereo rendering.
void vtkThreadSafeRenderWindowImpl::StereoRenderComplete(void)
{
    this->Mutex->Lock();
    this->Impl->StereoRenderComplete();
    this->Mutex->Unlock();
}

//----------------------------------------------------------------------------
void vtkThreadSafeRenderWindowImpl::CopyResultFrame(void)
{
    this->Mutex->Lock();
    this->Impl->CopyResultFrame();
    this->Mutex->Unlock();
}


//----------------------------------------------------------------------------
// treat renderWindow and interactor as one object.
// it might be easier if the GetReference count method were redefined.
void vtkThreadSafeRenderWindowImpl::UnRegister(vtkObjectBase *o)
{
    this->Mutex->Lock();
    this->Impl->UnRegister( o );
    this->Mutex->Unlock();
}

//----------------------------------------------------------------------------
const char *vtkThreadSafeRenderWindowImpl::GetRenderLibrary()
{
    return vtkRenderWindow::GetRenderLibrary();
}

// Description: Return the stereo type as a character string.
// when this method was inlined, static linking on BlueGene failed
// (symbol referenced which is defined in discarded section)
const char *vtkThreadSafeRenderWindowImpl::GetStereoTypeAsString()
{
    const char* aux( 0 );
    this->Mutex->Lock();
    aux = this->Impl->GetStereoTypeAsString();
    this->Mutex->Unlock();
    return aux;
}


/////////////////////////
// Public interface class
/////////////////////////

vtkStandardNewMacro( vtkThreadSafeRenderWindow );

int vtkThreadSafeRenderWindow::IsTypeOf(const char *type)
{
    if( !strcmp( "vtkThreadSafeRenderWindow",type ) )
    {
        return 1;
    }
    return vtkThreadSafeRenderWindowImpl::IsTypeOf(type);
}
int vtkThreadSafeRenderWindow::IsA(const char *type)
{
    if( this->vtkThreadSafeRenderWindow::IsTypeOf(type) )
    {
        return 1;
    }
    return this->Impl->IsA( type );
}
vtkThreadSafeRenderWindow* vtkThreadSafeRenderWindow::SafeDownCast(vtkObjectBase *o)
{
    if( o && o->IsA( "vtkThreadSafeRenderWindow" ) )
    {
        vtkThreadSafeRenderWindow* casted = static_cast<vtkThreadSafeRenderWindow *>(o);
        vtkThreadSafeRenderWindowImpl* rwi = vtkThreadSafeRenderWindowImpl::SafeDownCast( casted->Impl );
        if( rwi )
        {
            return casted;
        }
    }
    return NULL;
}
vtkObjectBase *vtkThreadSafeRenderWindow::NewInstanceInternal() const
{
    return vtkThreadSafeRenderWindow::New();
}
vtkThreadSafeRenderWindow *vtkThreadSafeRenderWindow::NewInstance() const
{
    return vtkThreadSafeRenderWindow::SafeDownCast(this->NewInstanceInternal());
}

vtkThreadSafeRenderWindow::vtkThreadSafeRenderWindow()
{
    this->Impl = new vtkThreadSafeRenderWindowImpl( this );
}

vtkThreadSafeRenderWindow::~vtkThreadSafeRenderWindow()
{
    delete this->Impl;
}

// Description:
// Add a renderer to the list of renderers.
void vtkThreadSafeRenderWindow::AddRenderer(vtkRenderer *ren)
{
    this->Impl->AddRenderer( ren );
}

// Description:
// Remove a renderer from the list of renderers.
void vtkThreadSafeRenderWindow::RemoveRenderer(vtkRenderer *ren)
{
    this->Impl->RemoveRenderer( ren );
}

// Description:
// Query if a renderer is in the list of renderers.
int vtkThreadSafeRenderWindow::HasRenderer(vtkRenderer *ren)
{
    return this->Impl->HasRenderer( ren );
}

// Description:
// What rendering library has the user requested
const char *vtkThreadSafeRenderWindow::GetRenderLibrary()
{
    return vtkThreadSafeRenderWindowImpl::GetRenderLibrary();
}

// Description:
// Return the collection of renderers in the render window.
vtkRendererCollection *vtkThreadSafeRenderWindow::GetRenderers()
{
    return this->Impl->GetRenderers();
}

// Description:
// Ask each renderer owned by this RenderWindow to render its image and
// synchronize this process.
void vtkThreadSafeRenderWindow::Render()
{
    this->Impl->Render();
}

// Description:
// Initialize the rendering process.
void vtkThreadSafeRenderWindow::Start()
{
    this->Impl->Start();
}

// Description:
// Finalize the rendering process.
void vtkThreadSafeRenderWindow::Finalize()
{
    this->Impl->Finalize();
}

// Description:
// A termination method performed at the end of the rendering process
// to do things like swapping buffers (if necessary) or similar actions.
void vtkThreadSafeRenderWindow::Frame()
{
    this->Impl->Frame();
}

// Description:
// Block the thread until the actual rendering is finished().
// Useful for measurement only.
void vtkThreadSafeRenderWindow::WaitForCompletion()
{
    this->Impl->WaitForCompletion();
}

// Description:
// Performed at the end of the rendering process to generate image.
// This is typically done right before swapping buffers.
void vtkThreadSafeRenderWindow::CopyResultFrame()
{
    this->Impl->CopyResultFrame();
}

// Description:
// Create an interactor to control renderers in this window. We need
// to know what type of interactor to create, because we might be in
// X Windows or MS Windows.
vtkRenderWindowInteractor *vtkThreadSafeRenderWindow::MakeRenderWindowInteractor()
{
    return this->Impl->MakeRenderWindowInteractor();
}

// Description:
// Hide or Show the mouse cursor, it is nice to be able to hide the
// default cursor if you want VTK to display a 3D cursor instead.
// Set cursor position in window (note that (0,0) is the lower left
// corner).
void vtkThreadSafeRenderWindow::HideCursor()
{
    this->Impl->HideCursor();
}

void vtkThreadSafeRenderWindow::ShowCursor()
{
    this->Impl->ShowCursor();
}

void vtkThreadSafeRenderWindow::SetCursorPosition( int x, int y )
{
    this->Impl->SetCursorPosition( x, y );
}

void vtkThreadSafeRenderWindow::SetFullScreen(int val)
{
    this->Impl->SetFullScreen( val );
}

void vtkThreadSafeRenderWindow::WindowRemap()
{
    this->Impl->WindowRemap();
}

// Description:
// Change the shape of the cursor.
vtkImplSetMacroDef(vtkThreadSafeRenderWindow,CurrentCursor,int);
vtkImplGetMacroDef(vtkThreadSafeRenderWindow,CurrentCursor,int);

// Description:
// Turn on/off rendering full screen window size.
vtkImplGetMacroDef(vtkThreadSafeRenderWindow,FullScreen,int);
vtkImplBooleanMacroDef(vtkThreadSafeRenderWindow,FullScreen,int);

// Description:
// Turn on/off window manager borders. Typically, you shouldn't turn the
// borders off, because that bypasses the window manager and can cause
// undesirable behavior.
vtkImplSetMacroDef(vtkThreadSafeRenderWindow,Borders,int);
vtkImplGetMacroDef(vtkThreadSafeRenderWindow,Borders,int);
vtkImplBooleanMacroDef(vtkThreadSafeRenderWindow,Borders,int);

// Description:
// Prescribe that the window be created in a stereo-capable mode. This
// method must be called before the window is realized. Default is off.
vtkImplGetMacroDef(vtkThreadSafeRenderWindow,StereoCapableWindow,int);
vtkImplBooleanMacroDef(vtkThreadSafeRenderWindow,StereoCapableWindow,int);
void vtkThreadSafeRenderWindow::SetStereoCapableWindow(int capable)
{
    this->Impl->SetStereoCapableWindow( capable );
}

// Description:
// Turn on/off stereo rendering.
vtkImplGetMacroDef(vtkThreadSafeRenderWindow,StereoRender,int);
void vtkThreadSafeRenderWindow::SetStereoRender(int stereo)
{
    this->Impl->SetStereoRender( stereo );
}
vtkImplBooleanMacroDef(vtkThreadSafeRenderWindow,StereoRender,int);

// Description:
// Turn on/off the use of alpha bitplanes.
vtkImplSetMacroDef(vtkThreadSafeRenderWindow,AlphaBitPlanes, int);
vtkImplGetMacroDef(vtkThreadSafeRenderWindow,AlphaBitPlanes, int);
vtkImplBooleanMacroDef(vtkThreadSafeRenderWindow,AlphaBitPlanes, int);

// Description:
// Turn on/off point smoothing. Default is off.
// This must be applied before the first Render.
vtkImplSetMacroDef(vtkThreadSafeRenderWindow,PointSmoothing,int);
vtkImplGetMacroDef(vtkThreadSafeRenderWindow,PointSmoothing,int);
vtkImplBooleanMacroDef(vtkThreadSafeRenderWindow,PointSmoothing,int);

// Description:
// Turn on/off line smoothing. Default is off.
// This must be applied before the first Render.
vtkImplSetMacroDef(vtkThreadSafeRenderWindow,LineSmoothing,int);
vtkImplGetMacroDef(vtkThreadSafeRenderWindow,LineSmoothing,int);
vtkImplBooleanMacroDef(vtkThreadSafeRenderWindow,LineSmoothing,int);

// Description:
// Turn on/off polygon smoothing. Default is off.
// This must be applied before the first Render.
vtkImplSetMacroDef(vtkThreadSafeRenderWindow,PolygonSmoothing,int);
vtkImplGetMacroDef(vtkThreadSafeRenderWindow,PolygonSmoothing,int);
vtkImplBooleanMacroDef(vtkThreadSafeRenderWindow,PolygonSmoothing,int);

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
vtkImplGetMacroDef(vtkThreadSafeRenderWindow,StereoType,int);
vtkImplSetMacroDef(vtkThreadSafeRenderWindow,StereoType,int);
void vtkThreadSafeRenderWindow::SetStereoTypeToCrystalEyes()
{
    this->Impl->SetStereoType(VTK_STEREO_CRYSTAL_EYES);
}
void vtkThreadSafeRenderWindow::SetStereoTypeToRedBlue()
{
    this->Impl->SetStereoType(VTK_STEREO_RED_BLUE);
}
void vtkThreadSafeRenderWindow::SetStereoTypeToInterlaced()
{
    this->Impl->SetStereoType(VTK_STEREO_INTERLACED);
}
void vtkThreadSafeRenderWindow::SetStereoTypeToLeft()
{
    this->Impl->SetStereoType(VTK_STEREO_LEFT);
}
void vtkThreadSafeRenderWindow::SetStereoTypeToRight()
{
    this->Impl->SetStereoType(VTK_STEREO_RIGHT);
}
void vtkThreadSafeRenderWindow::SetStereoTypeToDresden()
{
    this->Impl->SetStereoType(VTK_STEREO_DRESDEN);
}
void vtkThreadSafeRenderWindow::SetStereoTypeToAnaglyph()
{
    this->Impl->SetStereoType(VTK_STEREO_ANAGLYPH);
}
void vtkThreadSafeRenderWindow::SetStereoTypeToCheckerboard()
{
    this->Impl->SetStereoType(VTK_STEREO_CHECKERBOARD);
}

const char *vtkThreadSafeRenderWindow::GetStereoTypeAsString()
{
   return  this->Impl->GetStereoTypeAsString();
}

// Description:
// Update the system, if needed, due to stereo rendering. For some stereo
// methods, subclasses might need to switch some hardware settings here.
void vtkThreadSafeRenderWindow::StereoUpdate()
{
    this->Impl->StereoUpdate();
}

// Description:
// Intermediate method performs operations required between the rendering
// of the left and right eye.
void vtkThreadSafeRenderWindow::StereoMidpoint()
{
    this->Impl->StereoMidpoint();
}

// Description:
// Handles work required once both views have been rendered when using
// stereo rendering.
void vtkThreadSafeRenderWindow::StereoRenderComplete()
{
    this->Impl->StereoRenderComplete();
}

//Description:
// Set/get the anaglyph color saturation factor.  This number ranges from
// 0.0 to 1.0:  0.0 means that no color from the original object is
// maintained, 1.0 means all of the color is maintained.  The default
// value is 0.65.  Too much saturation can produce uncomfortable 3D
// viewing because anaglyphs also use color to encode 3D.
vtkImplSetClampMacroDef(vtkThreadSafeRenderWindow,AnaglyphColorSaturation,float, 0.0f, 1.0f);
vtkImplGetMacroDef(vtkThreadSafeRenderWindow,AnaglyphColorSaturation,float);

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
vtkImplSetVector2MacroDef(vtkThreadSafeRenderWindow,AnaglyphColorMask,int);
vtkImplGetVectorMacroDef(vtkThreadSafeRenderWindow,AnaglyphColorMask,int,2);

// Description:
// Turn on/off buffer swapping between images.
vtkImplSetMacroDef(vtkThreadSafeRenderWindow,SwapBuffers,int);
vtkImplGetMacroDef(vtkThreadSafeRenderWindow,SwapBuffers,int);
vtkImplBooleanMacroDef(vtkThreadSafeRenderWindow,SwapBuffers,int);

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
int vtkThreadSafeRenderWindow::SetPixelData(int x, int y, int x2, int y2, unsigned char *data,
                         int front)
{
    return this->Impl->SetPixelData( x, y, x2, y2, data, front );
}

int vtkThreadSafeRenderWindow::SetPixelData(int x, int y, int x2, int y2,
                         vtkUnsignedCharArray *data, int front)
{
    return this->Impl->SetPixelData( x, y, x2, y2, data, front );
}

// Description:
// Same as Get/SetPixelData except that the image also contains an alpha
// component. The image is transmitted as RGBARGBARGBA... each of which is a
// float value. The "blend" parameter controls whether the SetRGBAPixelData
// method blends the data with the previous contents of the frame buffer
// or completely replaces the frame buffer data.
float *vtkThreadSafeRenderWindow::GetRGBAPixelData(int x, int y, int x2, int y2, int front)
{
    return this->Impl->GetRGBAPixelData( x, y, x2, y2, front );
}

int vtkThreadSafeRenderWindow::GetRGBAPixelData(int x, int y, int x2, int y2, int front,
                             vtkFloatArray *data)
{
    return this->Impl->GetRGBAPixelData( x, y, x2, y2, front, data);
}

int vtkThreadSafeRenderWindow::SetRGBAPixelData(int x, int y, int x2, int y2, float *data,
                             int front, int blend)
{
    return this->Impl->SetRGBAPixelData( x, y, x2, y2, data, front, blend );
}

int vtkThreadSafeRenderWindow::SetRGBAPixelData(int x, int y, int x2, int y2, vtkFloatArray* data,
                             int front, int blend)
{
    return this->Impl->SetRGBAPixelData( x, y, x2, y2, data, front, blend );
}

void vtkThreadSafeRenderWindow::ReleaseRGBAPixelData(float *data)
{
    this->Impl->ReleaseRGBAPixelData( data );
}

unsigned char *vtkThreadSafeRenderWindow::GetRGBACharPixelData(int x, int y, int x2, int y2,
                                            int front)
{
    return this->Impl->GetRGBACharPixelData( x, y, x2, y2, front );
}

int vtkThreadSafeRenderWindow::GetRGBACharPixelData(int x, int y, int x2, int y2, int front,
                                 vtkUnsignedCharArray *data)
{
    return this->Impl->GetRGBACharPixelData( x, y, x2, y2, front, data );
}

int vtkThreadSafeRenderWindow::SetRGBACharPixelData(int x,int y, int x2, int y2,
                                 unsigned char *data, int front,
                                 int blend)
{
    return this->Impl->SetRGBACharPixelData( x,y, x2, y2, data, front, blend );
}

int vtkThreadSafeRenderWindow::SetRGBACharPixelData(int x, int y, int x2, int y2,
                                 vtkUnsignedCharArray *data, int front,
                                 int blend)
{
    return this->Impl->SetRGBACharPixelData( x, y, x2, y2, data, front, blend );
}

// Description:
// Set/Get the zbuffer data from the frame buffer.
// (x,y) is any corner of the rectangle. (x2,y2) is its opposite corner on
// the diagonal.
float *vtkThreadSafeRenderWindow::GetZbufferData(int x, int y, int x2, int y2)
{
    return this->Impl->GetZbufferData( x, y, x2, y2 );
}

int vtkThreadSafeRenderWindow::GetZbufferData(int x, int y, int x2, int y2, float *z)
{
    return this->Impl->GetZbufferData( x, y, x2, y2, z );
}

int vtkThreadSafeRenderWindow::GetZbufferData(int x, int y, int x2, int y2,
                           vtkFloatArray *z)
{
    return this->Impl->GetZbufferData( x, y, x2, y2, z );
}

int vtkThreadSafeRenderWindow::SetZbufferData(int x, int y, int x2, int y2, float *z)
{
    return this->Impl->SetZbufferData( x, y, x2, y2, z );
}

int vtkThreadSafeRenderWindow::SetZbufferData(int x, int y, int x2, int y2,
                           vtkFloatArray *z)
{
    return this->Impl->SetZbufferData( x, y, x2, y2, z );
}

float vtkThreadSafeRenderWindow::GetZbufferDataAtPoint(int x, int y)
{
    return this->Impl->GetZbufferDataAtPoint( x, y );
}

// Description:
// Set the number of frames for doing antialiasing. The default is
// zero. Typically five or six will yield reasonable results without
// taking too long.
vtkImplGetMacroDef(vtkThreadSafeRenderWindow,AAFrames,int);
vtkImplSetMacroDef(vtkThreadSafeRenderWindow,AAFrames,int);

// Description:
// Set the number of frames for doing focal depth. The default is zero.
// Depending on how your scene is organized you can get away with as
// few as four frames for focal depth or you might need thirty.
// One thing to note is that if you are using focal depth frames,
// then you will not need many (if any) frames for antialiasing.
vtkImplGetMacroDef(vtkThreadSafeRenderWindow,FDFrames,int);
vtkImplSetMacroDef(vtkThreadSafeRenderWindow,FDFrames,int);

// Description:
// Set the number of sub frames for doing motion blur. The default is zero.
// Once this is set greater than one, you will no longer see a new frame
// for every Render().  If you set this to five, you will need to do
// five Render() invocations before seeing the result. This isn't
// very impressive unless something is changing between the Renders.
// Changing this value may reset the current subframe count.
vtkImplGetMacroDef(vtkThreadSafeRenderWindow,SubFrames,int);
void vtkThreadSafeRenderWindow::SetSubFrames(int subFrames)
{
    return this->Impl->SetSubFrames( subFrames );
}

// Description:
// This flag is set if the window hasn't rendered since it was created
vtkImplGetMacroDef(vtkThreadSafeRenderWindow,NeverRendered,int);

// Description:
// This is a flag that can be set to interrupt a rendering that is in
// progress.
vtkImplGetMacroDef(vtkThreadSafeRenderWindow,AbortRender,int);
vtkImplSetMacroDef(vtkThreadSafeRenderWindow,AbortRender,int);
vtkImplGetMacroDef(vtkThreadSafeRenderWindow,InAbortCheck,int);
vtkImplSetMacroDef(vtkThreadSafeRenderWindow,InAbortCheck,int);
int vtkThreadSafeRenderWindow::CheckAbortStatus()
{
    return this->Impl->CheckAbortStatus();
}

vtkImplGetMacroDef(vtkThreadSafeRenderWindow,IsPicking,int);
vtkImplSetMacroDef(vtkThreadSafeRenderWindow,IsPicking,int);
vtkImplBooleanMacroDef(vtkThreadSafeRenderWindow,IsPicking,int);

// Description:
// Check to see if a mouse button has been pressed.  All other events
// are ignored by this method.  Ideally, you want to abort the render
// on any event which causes the DesiredUpdateRate to switch from
// a high-quality rate to a more interactive rate.
int vtkThreadSafeRenderWindow::GetEventPending()
{
    return this->Impl->GetEventPending();
}

// Description:
// Are we rendering at the moment
int  vtkThreadSafeRenderWindow::CheckInRenderStatus()
{
    return this->Impl->CheckInRenderStatus();
}

// Description:
// Clear status (after an exception was thrown for example)
void vtkThreadSafeRenderWindow::ClearInRenderStatus()
{
    return this->Impl->ClearInRenderStatus();
}

// Description:
// Set/Get the desired update rate. This is used with
// the vtkLODActor class. When using level of detail actors you
// need to specify what update rate you require. The LODActors then
// will pick the correct resolution to meet your desired update rate
// in frames per second. A value of zero indicates that they can use
// all the time they want to.
void vtkThreadSafeRenderWindow::SetDesiredUpdateRate(double updateRate )
{
    this->Impl->SetDesiredUpdateRate( updateRate );
}
vtkImplGetMacroDef(vtkThreadSafeRenderWindow,DesiredUpdateRate,double);

// Description:
// Get the number of layers for renderers.  Each renderer should have
// its layer set individually.  Some algorithms iterate through all layers,
// so it is not wise to set the number of layers to be exorbitantly large
// (say bigger than 100).
vtkImplGetMacroDef(vtkThreadSafeRenderWindow,NumberOfLayers, int);
vtkImplSetClampMacroDef(vtkThreadSafeRenderWindow,NumberOfLayers, int, 1, VTK_LARGE_INTEGER);

// Description:
// Get the interactor associated with this render window
vtkImplGetObjectMacroDef(vtkThreadSafeRenderWindow,Interactor,vtkRenderWindowInteractor);

// Description:
// Set the interactor to the render window
void vtkThreadSafeRenderWindow::SetInteractor(vtkRenderWindowInteractor *rwi)
{
    this->Impl->SetInteractor( rwi );
}

// Description:
// This Method detects loops of RenderWindow<->Interactor,
// so objects are freed properly.
void vtkThreadSafeRenderWindow::UnRegister(vtkObjectBase *o)
{
    this->Impl->UnRegister( o );
}

// Description:
// Dummy stubs for vtkWindow API.
void vtkThreadSafeRenderWindow::SetDisplayId(void *displayId)
{
    this->Impl->SetDisplayId( displayId );
}

void vtkThreadSafeRenderWindow::SetWindowId(void *windowId)
{
    this->Impl->SetWindowId( windowId );
}

void vtkThreadSafeRenderWindow::SetNextWindowId(void *windowId)
{
    this->Impl->SetNextWindowId( windowId );
}

void vtkThreadSafeRenderWindow::SetParentId(void *parentId)
{
    this->Impl->SetParentId( parentId );
}

void *vtkThreadSafeRenderWindow::GetGenericDisplayId()
{
    return this->Impl->GetGenericDisplayId();
}

void *vtkThreadSafeRenderWindow::GetGenericWindowId()
{
    return this->Impl->GetGenericWindowId();
}

void *vtkThreadSafeRenderWindow::GetGenericParentId()
{
    return this->Impl->GetGenericParentId();
}

void *vtkThreadSafeRenderWindow::GetGenericContext()
{
    return this->Impl->GetGenericContext();
}

void *vtkThreadSafeRenderWindow::GetGenericDrawable()
{
    return this->Impl->GetGenericDrawable();
}

void vtkThreadSafeRenderWindow::SetWindowInfo(char *info)
{
    this->Impl->SetWindowInfo( info );
}

void vtkThreadSafeRenderWindow::SetNextWindowInfo(char *info)
{
    this->Impl->SetNextWindowInfo( info );
}

void vtkThreadSafeRenderWindow::SetParentInfo(char *info)
{
    this->Impl->SetParentInfo( info );
}

int  *vtkThreadSafeRenderWindow::GetScreenSize()
{
    return this->Impl->GetScreenSize();
}
unsigned char *vtkThreadSafeRenderWindow::GetPixelData(int x, int y, int x2, int y2,
                                    int front)
{
    return this->Impl->GetPixelData( x, y, x2, y2, front );
}

int vtkThreadSafeRenderWindow::GetPixelData(int x, int y, int x2, int y2, int front,
                         vtkUnsignedCharArray *data)
{
    return this->Impl->GetPixelData( x, y, x2, y2, front, data );
}


// Description:
// Attempt to make this window the current graphics context for the calling
// thread.
void vtkThreadSafeRenderWindow::MakeCurrent()
{
    this->Impl->MakeCurrent();
}


// Description:
// Tells if this window is the current graphics context for the calling
// thread.
bool vtkThreadSafeRenderWindow::IsCurrent()
{
    return this->Impl->IsCurrent();
}

// Description:
// If called, allow MakeCurrent() to skip cache-check when called.
// MakeCurrent() reverts to original behavior of cache-checking
// on the next render.
void vtkThreadSafeRenderWindow::SetForceMakeCurrent()
{
    this->Impl->SetForceMakeCurrent();
}

// Description:
// Does this render window support OpenGL? 0-false, 1-true
int vtkThreadSafeRenderWindow::SupportsOpenGL()
{
    return this->Impl->SupportsOpenGL();
}

// Description:
// Is this render window using hardware acceleration? 0-false, 1-true
int vtkThreadSafeRenderWindow::IsDirect()
{
    return this->Impl->IsDirect();
}

// Description:
// This method should be defined by the subclass. How many bits of
// precision are there in the zbuffer?
int vtkThreadSafeRenderWindow::GetDepthBufferSize()
{
    return this->Impl->GetDepthBufferSize();
}

// Description:
// Get the size of the color buffer.
// Returns 0 if not able to determine otherwise sets R G B and A into buffer.
int vtkThreadSafeRenderWindow::GetColorBufferSizes(int *rgba)
{
    return this->Impl->GetColorBufferSizes( rgba );
}

// Description:
// Get the vtkPainterDeviceAdapter which can be used to paint on
// this render window.
vtkImplGetObjectMacroDef(vtkThreadSafeRenderWindow,PainterDeviceAdapter, vtkPainterDeviceAdapter);

// Description:
// Set / Get the number of multisamples to use for hardware antialiasing.
vtkImplSetMacroDef(vtkThreadSafeRenderWindow,MultiSamples,int);
vtkImplGetMacroDef(vtkThreadSafeRenderWindow,MultiSamples,int);

// Description:
// Set / Get the availability of the stencil buffer.
vtkImplSetMacroDef(vtkThreadSafeRenderWindow,StencilCapable, int);
vtkImplGetMacroDef(vtkThreadSafeRenderWindow,StencilCapable, int);
vtkImplBooleanMacroDef(vtkThreadSafeRenderWindow,StencilCapable, int);

// Description:
// Turn on/off report of graphic errors. Initial value is false (off).
// This flag is used by vtkGraphicErrorMacro.
vtkImplSetMacroDef(vtkThreadSafeRenderWindow,ReportGraphicErrors,int);
vtkImplGetMacroDef(vtkThreadSafeRenderWindow,ReportGraphicErrors,int);
vtkImplBooleanMacroDef(vtkThreadSafeRenderWindow,ReportGraphicErrors,int);

// Description:
// Update graphic error status, regardless of ReportGraphicErrors flag.
// It means this method can be used in any context and is not restricted to
// debug mode.
void vtkThreadSafeRenderWindow::CheckGraphicError()
{
    this->Impl->CheckGraphicError();
}

// Description:
// Return the last graphic error status. Initial value is false.
int vtkThreadSafeRenderWindow::HasGraphicError()
{
    return this->Impl->HasGraphicError();
}

// Description:
// Return a string matching the last graphic error status.
const char *vtkThreadSafeRenderWindow::GetLastGraphicErrorString()
{
    return this->Impl->GetLastGraphicErrorString();
}


void vtkThreadSafeRenderWindow::PrintSelf( ostream& os, vtkIndent indent )
{
    this->Superclass::PrintSelf( os, indent );
    this->Impl->PrintSelf( os, indent );
}

