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

#include "vtkThreadSafeRenderWindowInteractor.h"

#include <vtkObjectFactory.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkMutexLock.h>
#include <vtkMultiThreader.h>
#include "vtkThreadSafeRenderWindow.h"


#include <vector>

using namespace std;

////////////////////////////
// Private Implementor class
////////////////////////////

class vtkThreadSafeRenderWindowInteractorImpl: public vtkObject
{
public:
    // Description:
    // Constructor and destructor
    vtkThreadSafeRenderWindowInteractorImpl( vtkThreadSafeRenderWindowInteractor* publicInterface );
    ~vtkThreadSafeRenderWindowInteractorImpl();

    virtual void PrintSelf( ostream& os, vtkIndent indent );

    // Description:
    // Prepare for handling events. This must be called before the
    // interactor will work.
    virtual void Initialize()
    {
        this->Mutex->Lock();
        this->Impl->Initialize();
        this->Mutex->Unlock();
    }
    void ReInitialize()
    {
        this->Mutex->Lock();
        this->Impl->ReInitialize();
        this->Mutex->Unlock();
    }

    // Description:
    // This Method detects loops of RenderWindow-Interactor,
    // so objects are freed properly.
    virtual void UnRegister(vtkObjectBase *o)
    {
        this->Mutex->Lock();
        this->Impl->UnRegister( o );
        this->Mutex->Unlock();
    }

    // Description:
    // Start the event loop. This is provided so that you do not have to
    // implement your own event loop. You still can use your own
    // event loop if you want. Initialize should be called before Start.
    virtual void Start()
    {
        this->Mutex->Lock();
        this->Impl->Start();
        this->Mutex->Unlock();
    }

    // Description:
    // Enable/Disable interactions.  By default interactors are enabled when
    // initialized.  Initialize() must be called prior to enabling/disabling
    // interaction. These methods are used when a window/widget is being
    // shared by multiple renderers and interactors.  This allows a "modal"
    // display where one interactor is active when its data is to be displayed
    // and all other interactors associated with the widget are disabled
    // when their data is not displayed.
    virtual void Enable()
    {
        this->Mutex->Lock();
        this->Impl->Enable();
        this->Mutex->Unlock();
    }
    virtual void Disable()
    {
        this->Mutex->Lock();
        this->Impl->Disable();
        this->Mutex->Unlock();
    }
    vtkSafeGetMacro(Enabled, int);

    // Description:
    // Enable/Disable whether vtkRenderWindowInteractor::Render() calls
    // this->RenderWindow->Render().
    vtkSafeBooleanMacro(EnableRender, bool);
    vtkSafeSetMacro(EnableRender, bool);
    vtkSafeGetMacro(EnableRender, bool);

    // Description:
    // Set/Get the rendering window being controlled by this object.
    void SetRenderWindow(vtkRenderWindow *aren)
    {
        // TODO
        this->Mutex->Lock();
        vtkThreadSafeRenderWindow* tsaren = dynamic_cast<vtkThreadSafeRenderWindow*>( aren );
        if( tsaren != 0 )
        {
            this->Impl->SetRenderWindow( tsaren );
            if( this->ThreadSafeRenderWindow != tsaren )
            {
                vtkThreadSafeRenderWindow* temp = this->ThreadSafeRenderWindow;
                this->ThreadSafeRenderWindow = tsaren;
                if( temp != 0 )
                {
                    //temp->UnRegister( this->PublicInterface );
                }
                if( this->ThreadSafeRenderWindow !=  0 )
                {
                    //this->this->ThreadSafeRenderWindow->Register( this->PublicInterface );
                    if( this->ThreadSafeRenderWindow->GetInteractor() != this->PublicInterface )
                    {
                        this->ThreadSafeRenderWindow->SetInteractor( this->PublicInterface );
                    }
                }
            }
        }
        else
        {
            // Should assert?
            this->Impl->SetRenderWindow( aren );
        }
        this->Mutex->Unlock();
    }
    vtkSafeGetObjectMacro(RenderWindow,vtkRenderWindow);

    // Description:
    // Event loop notification member for window size change.
    // Window size is measured in pixels.
    virtual void UpdateSize(int x,int y)
    {
        this->Mutex->Lock();
        this->Impl->UpdateSize( x, y );
        this->Mutex->Unlock();
    }

    // Description:
    // This class provides two groups of methods for manipulating timers.  The
    // first group (CreateTimer(timerType) and DestroyTimer()) implicitly use
    // an internal timer id (and are present for backward compatibility). The
    // second group (CreateRepeatingTimer(long),CreateOneShotTimer(long),
    // ResetTimer(int),DestroyTimer(int)) use timer ids so multiple timers can
    // be independently managed. In the first group, the CreateTimer() method
    // takes an argument indicating whether the timer is created the first time
    // (timerType==VTKI_TIMER_FIRST) or whether it is being reset
    // (timerType==VTKI_TIMER_UPDATE). (In initial implementations of VTK this
    // was how one shot and repeating timers were managed.) In the second
    // group, the create methods take a timer duration argument (in
    // milliseconds) and return a timer id. Thus the ResetTimer(timerId) and
    // DestroyTimer(timerId) methods take this timer id and operate on the
    // timer as appropriate. Methods are also available for determining
    virtual int CreateTimer(int timerType)
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->CreateTimer( timerType );
        this->Mutex->Unlock();
        return aux;
    }
    virtual int DestroyTimer()
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->DestroyTimer();
        this->Mutex->Unlock();
        return aux;
    }
    int CreateRepeatingTimer(unsigned long duration)
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->CreateRepeatingTimer( duration );
        this->Mutex->Unlock();
        return aux;
    }
    int CreateOneShotTimer(unsigned long duration)
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->CreateOneShotTimer( duration );
        this->Mutex->Unlock();
        return aux;
    }
    int IsOneShotTimer(int timerId)
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->IsOneShotTimer( timerId );
        this->Mutex->Unlock();
        return aux;
    }
    unsigned long GetTimerDuration(int timerId)
    {
        long aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->GetTimerDuration( timerId );
        this->Mutex->Unlock();
        return aux;
    }
    int ResetTimer(int timerId)
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->ResetTimer( timerId );
        this->Mutex->Unlock();
        return aux;
    }
    int DestroyTimer(int timerId)
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->DestroyTimer( timerId );
        this->Mutex->Unlock();
        return aux;
    }
    virtual int GetVTKTimerId(int platformTimerId)
    {
        int aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->GetVTKTimerId( platformTimerId );
        this->Mutex->Unlock();
        return aux;
    }

    //BTX
    // Moved into the public section of the class so that classless timer procs
    // can access these enum members without being "friends"...
    enum {OneShotTimer=1,RepeatingTimer};
    //ETX

    // Description:
    // Specify the default timer interval (in milliseconds). (This is used in
    // conjunction with the timer methods described previously, e.g.,
    // CreateTimer() uses this value; and CreateRepeatingTimer(duration) and
    // CreateOneShotTimer(duration) use the default value if the parameter
    // "duration" is less than or equal to zero.) Care must be taken when
    // adjusting the timer interval from the default value of 10
    // milliseconds--it may adversely affect the interactors.
    vtkSafeSetClampMacro(TimerDuration,unsigned long,1,100000);
    vtkSafeGetMacro(TimerDuration,unsigned long);

    // Description:
    // These methods are used to communicate information about the currently
    // firing CreateTimerEvent or DestroyTimerEvent. The caller of
    // CreateTimerEvent sets up TimerEventId, TimerEventType and
    // TimerEventDuration. The observer of CreateTimerEvent should set up an
    // appropriate platform specific timer based on those values and set the
    // TimerEventPlatformId before returning. The caller of DestroyTimerEvent
    // sets up TimerEventPlatformId. The observer of DestroyTimerEvent should
    // simply destroy the platform specific timer created by CreateTimerEvent.
    // See vtkGenericRenderWindowInteractor's InternalCreateTimer and
    // InternalDestroyTimer for an example.
    vtkSafeSetMacro(TimerEventId, int);
    vtkSafeGetMacro(TimerEventId, int);
    vtkSafeSetMacro(TimerEventType, int);
    vtkSafeGetMacro(TimerEventType, int);
    vtkSafeSetMacro(TimerEventDuration, int);
    vtkSafeGetMacro(TimerEventDuration, int);
    vtkSafeSetMacro(TimerEventPlatformId, int);
    vtkSafeGetMacro(TimerEventPlatformId, int);

    // Description:
    // This function is called on 'q','e' keypress if exitmethod is not
    // specified and should be overridden by platform dependent subclasses
    // to provide a termination procedure if one is required.
    virtual void TerminateApp(void)
    {
        this->Mutex->Lock();
        this->Impl->TerminateApp();
        this->Mutex->Unlock();
    }

    // Description:
    // External switching between joystick/trackball/new? modes. Initial value
    // is a vtkInteractorStyleSwitch object.
    virtual void SetInteractorStyle(vtkInteractorObserver *observer )
    {
        this->Mutex->Lock();
        this->Impl->SetInteractorStyle( observer );
        this->Mutex->Unlock();
    }
    vtkSafeGetObjectMacro(InteractorStyle,vtkInteractorObserver);

    // Description:
    // Turn on/off the automatic repositioning of lights as the camera moves.
    // Default is On.
    vtkSafeSetMacro(LightFollowCamera,int);
    vtkSafeGetMacro(LightFollowCamera,int);
    vtkSafeBooleanMacro(LightFollowCamera,int);

    // Description:
    // Set/Get the desired update rate. This is used by vtkLODActor's to tell
    // them how quickly they need to render.  This update is in effect only
    // when the camera is being rotated, or zoomed.  When the interactor is
    // still, the StillUpdateRate is used instead. 
    // The default is 15.
    vtkSafeSetClampMacro(DesiredUpdateRate,double,0.0001,VTK_LARGE_FLOAT);
    vtkSafeGetMacro(DesiredUpdateRate,double);

    // Description:
    // Set/Get the desired update rate when movement has stopped.
    // For the non-still update rate, see the SetDesiredUpdateRate method.
    // The default is 0.0001
    vtkSafeSetClampMacro(StillUpdateRate,double,0.0001,VTK_LARGE_FLOAT);
    vtkSafeGetMacro(StillUpdateRate,double);

    // Description:
    // See whether interactor has been initialized yet.
    // Default is 0.
    vtkSafeGetMacro(Initialized,int);

    // Description:
    // Set/Get the object used to perform pick operations. In order to
    // pick instances of vtkProp, the picker must be a subclass of 
    // vtkAbstractPropPicker, meaning that it can identify a particular 
    // instance of vtkProp.
    virtual void SetPicker(vtkAbstractPicker* picker )
    {
        this->Mutex->Lock();
        this->Impl->SetPicker( picker );
        this->Mutex->Unlock();
    }
    vtkSafeGetObjectMacro(Picker,vtkAbstractPicker);

    // Description:
    // Create default picker. Used to create one when none is specified.
    // Default is an instance of vtkPropPicker.
    virtual vtkAbstractPropPicker *CreateDefaultPicker()
    {
        vtkAbstractPropPicker* aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->CreateDefaultPicker();
        this->Mutex->Unlock();
        return aux;
    }

    // Description:
    // These methods correspond to the the Exit, User and Pick
    // callbacks. They allow for the Style to invoke them.
    virtual void ExitCallback()
    {
        this->Mutex->Lock();
        this->Impl->ExitCallback();
        this->Mutex->Unlock();
    }
    virtual void UserCallback()
    {
        this->Mutex->Lock();
        this->Impl->UserCallback();
        this->Mutex->Unlock();
    }
    virtual void StartPickCallback()
    {
        this->Mutex->Lock();
        this->Impl->StartPickCallback();
        this->Mutex->Unlock();
    }
    virtual void EndPickCallback()
    {
        this->Mutex->Lock();
        this->Impl->EndPickCallback();
        this->Mutex->Unlock();
    }

    // Description:
    // Get the current position of the mouse.
    virtual void GetMousePosition(int *x, int *y)
    {
        this->Mutex->Lock();
        this->Impl->GetMousePosition( x, y );
        this->Mutex->Unlock();
    }

    // Description:
    // Hide or show the mouse cursor, it is nice to be able to hide the
    // default cursor if you want VTK to display a 3D cursor instead.
    void HideCursor()
    {
        this->Mutex->Lock();
        this->Impl->HideCursor();
        this->Mutex->Unlock();
    }
    void ShowCursor()
    {
        this->Mutex->Lock();
        this->Impl->ShowCursor();
        this->Mutex->Unlock();
    }

    // Description:
    // Render the scene. Just pass the render call on to the 
    // associated vtkRenderWindow.
    virtual void Render()
    {
        this->Mutex->Lock();
        this->Impl->Render();
        this->Mutex->Unlock();
    }

    // Description:
    // Given a position x, move the current camera's focal point to x.
    // The movement is animated over the number of frames specified in
    // NumberOfFlyFrames. The LOD desired frame rate is used.
    void FlyTo(vtkRenderer *ren, double x, double y, double z)
    {
        this->Mutex->Lock();
        this->Impl->FlyTo( ren, x, y, z );
        this->Mutex->Unlock();
    }
    void FlyTo(vtkRenderer *ren, double *x)
    {
        this->Mutex->Lock();
        this->Impl->FlyTo( ren, x );
        this->Mutex->Unlock();
    }
    void FlyToImage(vtkRenderer *ren, double x, double y)
    {
        this->Mutex->Lock();
        this->Impl->FlyToImage( ren, x, y );
        this->Mutex->Unlock();
    }
    void FlyToImage(vtkRenderer *ren, double *x)
    {
        this->Mutex->Lock();
        this->Impl->FlyToImage( ren, x );
        this->Mutex->Unlock();
    }

    // Description:
    // Set the number of frames to fly to when FlyTo is invoked.
    vtkSafeSetClampMacro(NumberOfFlyFrames,int,1,VTK_LARGE_INTEGER);
    vtkSafeGetMacro(NumberOfFlyFrames,int);

    // Description:
    // Set the total Dolly value to use when flying to (FlyTo()) a
    // specified point. Negative values fly away from the point.
    vtkSafeSetMacro(Dolly,double);
    vtkSafeGetMacro(Dolly,double);

    // Description:
    // Set/Get information about the current event. 
    // The current x,y position is in the EventPosition, and the previous
    // event position is in LastEventPosition, updated automatically each
    // time EventPosition is set using its Set() method. Mouse positions
    // are measured in pixels.
    // The other information is about key board input.
    vtkSafeGetVector2Macro(EventPosition,int);
    vtkSafeGetVector2Macro(LastEventPosition,int);
    vtkSafeSetVector2Macro(LastEventPosition,int);
    virtual void SetEventPosition(int x, int y)
    {
        this->Mutex->Lock();
        this->Impl->SetEventPosition( x, y );
        this->Mutex->Unlock();
    }
    virtual void SetEventPosition(int pos[2])
    {
        this->Mutex->Lock();
        this->Impl->SetEventPosition( pos );
        this->Mutex->Unlock();
    }
    virtual void SetEventPositionFlipY(int x, int y)
    {
        this->Mutex->Lock();
        this->Impl->SetEventPositionFlipY( x, y );
        this->Mutex->Unlock();
    }
    virtual void SetEventPositionFlipY(int pos[2])
    {
        this->Mutex->Lock();
        this->Impl->SetEventPosition( pos );
        this->Mutex->Unlock();
    }
    vtkSafeSetMacro(AltKey, int);
    vtkSafeGetMacro(AltKey, int);
    vtkSafeSetMacro(ControlKey, int);
    vtkSafeGetMacro(ControlKey, int);
    vtkSafeSetMacro(ShiftKey, int);
    vtkSafeGetMacro(ShiftKey, int);
    vtkSafeSetMacro(KeyCode, char);
    vtkSafeGetMacro(KeyCode, char);
    vtkSafeSetMacro(RepeatCount, int);
    vtkSafeGetMacro(RepeatCount, int);
    vtkSafeSetStringMacro(KeySym);
    vtkSafeGetStringMacro(KeySym);

    // Description:
    // Set all the event information in one call.
    void SetEventInformation(int x, 
        int y, 
        int ctrl=0, 
        int shift=0, 
        char keycode=0, 
        int repeatcount=0,
        const char* keysym=0)
    {
        this->Mutex->Lock();
        this->Impl->SetEventInformation( x, y, ctrl, shift, keycode, repeatcount, keysym );
        this->Mutex->Unlock();
    }

    // Description:
    // Calls SetEventInformation, but flips the Y based on the current Size[1] 
    // value (i.e. y = this->Size[1] - y - 1).
    void SetEventInformationFlipY(int x, 
        int y, 
        int ctrl=0, 
        int shift=0, 
        char keycode=0, 
        int repeatcount=0,
        const char* keysym=0)
    {
        this->Mutex->Lock();
        this->Impl->SetEventInformationFlipY( x, y, ctrl, shift, keycode, repeatcount, keysym );
        this->Mutex->Unlock();
    }

    // Description:
    // Set all the keyboard-related event information in one call.
    void SetKeyEventInformation(int ctrl=0, 
        int shift=0, 
        char keycode=0, 
        int repeatcount=0,
        const char* keysym=0)
    {
        this->Mutex->Lock();
        this->Impl->SetKeyEventInformation( ctrl, shift, keycode, repeatcount, keysym );
        this->Mutex->Unlock();
    }

    // Description:
    // This methods sets the Size ivar of the interactor without
    // actually changing the size of the window. Normally
    // application programmers would use UpdateSize if anything.
    // This is useful for letting someone else change the size of
    // the rendering window and just letting the interactor
    // know about the change.
    // The current event width/height (if any) is in EventSize 
    // (Expose event, for example).
    // Window size is measured in pixels.
    vtkSafeSetVector2Macro(Size,int);
    vtkSafeGetVector2Macro(Size,int);
    vtkSafeSetVector2Macro(EventSize,int);
    vtkSafeGetVector2Macro(EventSize,int);

    // Description:
    // When an event occurs, we must determine which Renderer the event
    // occurred within, since one RenderWindow may contain multiple
    // renderers.
    virtual vtkRenderer *FindPokedRenderer(int x,int y)
    {
        vtkRenderer* aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->FindPokedRenderer( x, y );
        this->Mutex->Unlock();
        return aux;
    }

    // Description:
    // Return the object used to mediate between vtkInteractorObservers
    // contending for resources. Multiple interactor observers will often
    // request different resources (e.g., cursor shape); the mediator uses a
    // strategy to provide the resource based on priority of the observer plus
    // the particular request (default versus non-default cursor shape).
    vtkObserverMediator *GetObserverMediator()
    {
        vtkObserverMediator* aux( 0 );
        this->Mutex->Lock();
        aux = this->Impl->GetObserverMediator();
        this->Mutex->Unlock();
        return aux;
    }

    // Description:
    // Use a 3DConnexion device. Initial value is false.
    // If VTK is not build with the TDx option, this is no-op.
    // If VTK is build with the TDx option, and a device is not connected,
    // a warning is emitted.
    // It is must be called before the first Render to be effective, otherwise
    // it is ignored.
    vtkSafeSetMacro(UseTDx,bool);
    vtkSafeGetMacro(UseTDx,bool);

    // Description:
    // Fire various events. SetEventInformation should be called just prior
    // to calling any of these methods. These methods will Invoke the
    // corresponding vtk event.
    virtual void MouseMoveEvent()
    {
        this->Mutex->Lock();
        //this->Impl->MouseMoveEvent();
        this->Mutex->Unlock();
    }
    virtual void RightButtonPressEvent()
    {
        this->Mutex->Lock();
        //this->Impl->RightButtonPressEvent();
        this->Mutex->Unlock();
    }
    virtual void RightButtonReleaseEvent()
    {
        this->Mutex->Lock();
        //this->Impl->RightButtonReleaseEvent();
        this->Mutex->Unlock();
    }
    virtual void LeftButtonPressEvent()
    {
        this->Mutex->Lock();
        //this->Impl->LeftButtonPressEvent();
        this->Mutex->Unlock();
    }
    virtual void LeftButtonReleaseEvent()
    {
        this->Mutex->Lock();
        //this->Impl->LeftButtonReleaseEvent();
        this->Mutex->Unlock();
    }
    virtual void MiddleButtonPressEvent()
    {
        this->Mutex->Lock();
        //this->Impl->MiddleButtonPressEvent();
        this->Mutex->Unlock();
    }
    virtual void MiddleButtonReleaseEvent()
    {
        this->Mutex->Lock();
        //this->Impl->MiddleButtonReleaseEvent();
        this->Mutex->Unlock();
    }
    virtual void MouseWheelForwardEvent()
    {
        this->Mutex->Lock();
        //this->Impl->MouseWheelForwardEvent();
        this->Mutex->Unlock();
    }
    virtual void MouseWheelBackwardEvent()
    {
        this->Mutex->Lock();
        //this->Impl->MouseWheelBackwardEvent();
        this->Mutex->Unlock();
    }
    virtual void ExposeEvent()
    {
        this->Mutex->Lock();
        //this->Impl->ExposeEvent();
        this->Mutex->Unlock();
    }
    virtual void ConfigureEvent()
    {
        this->Mutex->Lock();
        //this->Impl->ConfigureEvent();
        this->Mutex->Unlock();
    }
    virtual void EnterEvent()
    {
        this->Mutex->Lock();
        //this->Impl->EnterEvent();
        this->Mutex->Unlock();
    }
    virtual void LeaveEvent()
    {
        this->Mutex->Lock();
        //this->Impl->LeaveEvent();
        this->Mutex->Unlock();
    }
    virtual void KeyPressEvent()
    {
        this->Mutex->Lock();
        //this->Impl->KeyPressEvent();
        this->Mutex->Unlock();
    }
    virtual void KeyReleaseEvent()
    {
        this->Mutex->Lock();
        //this->Impl->KeyReleaseEvent();
        this->Mutex->Unlock();
    }
    virtual void CharEvent()
    {
        this->Mutex->Lock();
        //this->Impl->CharEvent();
        this->Mutex->Unlock();
    }
    virtual void ExitEvent()
    {
        this->Mutex->Lock();
        //this->Impl->ExitEvent();
        this->Mutex->Unlock();
    }


private:

    vtkRenderWindowInteractor*              Impl;
    vtkMutexLock*                           Mutex;
    vtkMultiThreaderIDType                  LockOwnerThreadID;
    vtkThreadSafeRenderWindowInteractor*    PublicInterface;
    vtkThreadSafeRenderWindow*              ThreadSafeRenderWindow;
};



vtkThreadSafeRenderWindowInteractorImpl::vtkThreadSafeRenderWindowInteractorImpl( vtkThreadSafeRenderWindowInteractor* publicInterface )
: Impl( 0 )
, Mutex( 0 )
, LockOwnerThreadID( -1 )
, PublicInterface( publicInterface )
, ThreadSafeRenderWindow( 0 )
{
    this->Mutex = vtkMutexLock::New();
    this->Impl = vtkRenderWindowInteractor::New();
}

vtkThreadSafeRenderWindowInteractorImpl::~vtkThreadSafeRenderWindowInteractorImpl()
{
    this->Mutex->Unlock();
    this->Mutex->Delete();
    this->Impl->Delete();
}

void vtkThreadSafeRenderWindowInteractorImpl::PrintSelf( ostream& os, vtkIndent indent )
{
    this->Superclass::PrintSelf( os, indent );
    this->Mutex->Lock();
    this->Impl->PrintSelf( os, indent );
    this->Mutex->Unlock();
}



/////////////////////////
// Public interface class
/////////////////////////

vtkStandardNewMacro( vtkThreadSafeRenderWindowInteractor );

vtkThreadSafeRenderWindowInteractor::vtkThreadSafeRenderWindowInteractor()
{
    this->Impl = new vtkThreadSafeRenderWindowInteractorImpl( this );
}

vtkThreadSafeRenderWindowInteractor::~vtkThreadSafeRenderWindowInteractor()
{
    delete this->Impl;
}

void vtkThreadSafeRenderWindowInteractor::PrintSelf( ostream& os, vtkIndent indent )
{
    this->Superclass::PrintSelf( os, indent );
    this->Impl->PrintSelf( os, indent );
}


// Description:
// Prepare for handling events. This must be called before the
// interactor will work.
void vtkThreadSafeRenderWindowInteractor::Initialize()
{
    this->Impl->Initialize();
}

void vtkThreadSafeRenderWindowInteractor::ReInitialize()
{
    this->Impl->ReInitialize();
}

// Description:
// This Method detects loops of RenderWindow-Interactor,
// so objects are freed properly.
void vtkThreadSafeRenderWindowInteractor::UnRegister(vtkObjectBase *o)
{
    this->Impl->UnRegister( o );
}

// Description:
// Start the event loop. This is provided so that you do not have to
// implement your own event loop. You still can use your own
// event loop if you want. Initialize should be called before Start.
void vtkThreadSafeRenderWindowInteractor::Start()
{
    this->Impl->Start();
}

// Description:
// Enable/Disable interactions.  By default interactors are enabled when
// initialized.  Initialize() must be called prior to enabling/disabling
// interaction. These methods are used when a window/widget is being
// shared by multiple renderers and interactors.  This allows a "modal"
// display where one interactor is active when its data is to be displayed
// and all other interactors associated with the widget are disabled
// when their data is not displayed.
void vtkThreadSafeRenderWindowInteractor::Enable()
{
    this->Impl->Enable();
}
void vtkThreadSafeRenderWindowInteractor::Disable()
{
    this->Impl->Disable();
}
vtkImplGetMacroDef(vtkThreadSafeRenderWindowInteractor,Enabled, int);

// Description:
// Enable/Disable whether vtkRenderWindowInteractor::Render() calls
// this->RenderWindow->Render().
vtkImplBooleanMacroDef(vtkThreadSafeRenderWindowInteractor,EnableRender, bool);
vtkImplSetMacroDef(vtkThreadSafeRenderWindowInteractor,EnableRender, bool);
vtkImplGetMacroDef(vtkThreadSafeRenderWindowInteractor,EnableRender, bool);

// Description:
// Set/Get the rendering window being controlled by this object.
void vtkThreadSafeRenderWindowInteractor::SetRenderWindow(vtkRenderWindow *aren)
{
    this->Impl->SetRenderWindow( aren );
}
vtkImplGetObjectMacroDef(vtkThreadSafeRenderWindowInteractor,RenderWindow,vtkRenderWindow);

// Description:
// Event loop notification member for window size change.
// Window size is measured in pixels.
void vtkThreadSafeRenderWindowInteractor::UpdateSize(int x,int y)
{
    this->Impl->UpdateSize( x, y );
}

// Description:
// This class provides two groups of methods for manipulating timers.  The
// first group (CreateTimer(timerType) and DestroyTimer()) implicitly use
// an internal timer id (and are present for backward compatibility). The
// second group (CreateRepeatingTimer(long),CreateOneShotTimer(long),
// ResetTimer(int),DestroyTimer(int)) use timer ids so multiple timers can
// be independently managed. In the first group, the CreateTimer() method
// takes an argument indicating whether the timer is created the first time
// (timerType==VTKI_TIMER_FIRST) or whether it is being reset
// (timerType==VTKI_TIMER_UPDATE). (In initial implementations of VTK this
// was how one shot and repeating timers were managed.) In the second
// group, the create methods take a timer duration argument (in
// milliseconds) and return a timer id. Thus the ResetTimer(timerId) and
// DestroyTimer(timerId) methods take this timer id and operate on the
// timer as appropriate. Methods are also available for determining
int vtkThreadSafeRenderWindowInteractor::CreateTimer(int timerType)
{
    return this->Impl->CreateTimer( timerType );
}
int vtkThreadSafeRenderWindowInteractor::DestroyTimer()
{
    return this->Impl->DestroyTimer();
}
int vtkThreadSafeRenderWindowInteractor::CreateRepeatingTimer(unsigned long duration)
{
    return this->Impl->CreateRepeatingTimer( duration );
}
int vtkThreadSafeRenderWindowInteractor::CreateOneShotTimer(unsigned long duration)
{
    return this->Impl->CreateOneShotTimer( duration );
}
int vtkThreadSafeRenderWindowInteractor::IsOneShotTimer(int timerId)
{
    return this->Impl->IsOneShotTimer( timerId );
}
unsigned long vtkThreadSafeRenderWindowInteractor::GetTimerDuration(int timerId)
{
    return this->Impl->GetTimerDuration( timerId );
}
int vtkThreadSafeRenderWindowInteractor::ResetTimer(int timerId)
{
    return this->Impl->ResetTimer( timerId );
}
int vtkThreadSafeRenderWindowInteractor::DestroyTimer(int timerId)
{
    return this->Impl->DestroyTimer( timerId );
}
int vtkThreadSafeRenderWindowInteractor::GetVTKTimerId(int platformTimerId)
{
    return this->Impl->GetVTKTimerId( platformTimerId );
}

//BTX
// Moved into the public section of the class so that classless timer procs
// can access these enum members without being "friends"...
enum {OneShotTimer=1,RepeatingTimer};
//ETX

// Description:
// Specify the default timer interval (in milliseconds). (This is used in
// conjunction with the timer methods described previously, e.g.,
// CreateTimer() uses this value; and CreateRepeatingTimer(duration) and
// CreateOneShotTimer(duration) use the default value if the parameter
// "duration" is less than or equal to zero.) Care must be taken when
// adjusting the timer interval from the default value of 10
// milliseconds--it may adversely affect the interactors.
vtkImplSetClampMacroDef(vtkThreadSafeRenderWindowInteractor,TimerDuration,unsigned long,1,100000);
vtkImplGetMacroDef(vtkThreadSafeRenderWindowInteractor,TimerDuration,unsigned long);

// Description:
// These methods are used to communicate information about the currently
// firing CreateTimerEvent or DestroyTimerEvent. The caller of
// CreateTimerEvent sets up TimerEventId, TimerEventType and
// TimerEventDuration. The observer of CreateTimerEvent should set up an
// appropriate platform specific timer based on those values and set the
// TimerEventPlatformId before returning. The caller of DestroyTimerEvent
// sets up TimerEventPlatformId. The observer of DestroyTimerEvent should
// simply destroy the platform specific timer created by CreateTimerEvent.
// See vtkGenericRenderWindowInteractor's InternalCreateTimer and
// InternalDestroyTimer for an example.
vtkImplSetMacroDef(vtkThreadSafeRenderWindowInteractor,TimerEventId, int);
vtkImplGetMacroDef(vtkThreadSafeRenderWindowInteractor,TimerEventId, int);
vtkImplSetMacroDef(vtkThreadSafeRenderWindowInteractor,TimerEventType, int);
vtkImplGetMacroDef(vtkThreadSafeRenderWindowInteractor,TimerEventType, int);
vtkImplSetMacroDef(vtkThreadSafeRenderWindowInteractor,TimerEventDuration, int);
vtkImplGetMacroDef(vtkThreadSafeRenderWindowInteractor,TimerEventDuration, int);
vtkImplSetMacroDef(vtkThreadSafeRenderWindowInteractor,TimerEventPlatformId, int);
vtkImplGetMacroDef(vtkThreadSafeRenderWindowInteractor,TimerEventPlatformId, int);

// Description:
// This function is called on 'q','e' keypress if exitmethod is not
// specified and should be overridden by platform dependent subclasses
// to provide a termination procedure if one is required.
void vtkThreadSafeRenderWindowInteractor::TerminateApp(void)
{
    this->Impl->TerminateApp();
}

// Description:
// External switching between joystick/trackball/new? modes. Initial value
// is a vtkInteractorStyleSwitch object.
void vtkThreadSafeRenderWindowInteractor::SetInteractorStyle(vtkInteractorObserver *observer )
{
    this->Impl->SetInteractorStyle( observer );
}
vtkImplGetObjectMacroDef(vtkThreadSafeRenderWindowInteractor,InteractorStyle,vtkInteractorObserver);

// Description:
// Turn on/off the automatic repositioning of lights as the camera moves.
// Default is On.
vtkImplSetMacroDef(vtkThreadSafeRenderWindowInteractor,LightFollowCamera,int);
vtkImplGetMacroDef(vtkThreadSafeRenderWindowInteractor,LightFollowCamera,int);
vtkImplBooleanMacroDef(vtkThreadSafeRenderWindowInteractor,LightFollowCamera,int);

// Description:
// Set/Get the desired update rate. This is used by vtkLODActor's to tell
// them how quickly they need to render.  This update is in effect only
// when the camera is being rotated, or zoomed.  When the interactor is
// still, the StillUpdateRate is used instead. 
// The default is 15.
vtkImplSetClampMacroDef(vtkThreadSafeRenderWindowInteractor,DesiredUpdateRate,double,0.0001,VTK_LARGE_FLOAT);
vtkImplGetMacroDef(vtkThreadSafeRenderWindowInteractor,DesiredUpdateRate,double);

// Description:
// Set/Get the desired update rate when movement has stopped.
// For the non-still update rate, see the SetDesiredUpdateRate method.
// The default is 0.0001
vtkImplSetClampMacroDef(vtkThreadSafeRenderWindowInteractor,StillUpdateRate,double,0.0001,VTK_LARGE_FLOAT);
vtkImplGetMacroDef(vtkThreadSafeRenderWindowInteractor,StillUpdateRate,double);

// Description:
// See whether interactor has been initialized yet.
// Default is 0.
vtkImplGetMacroDef(vtkThreadSafeRenderWindowInteractor,Initialized,int);

// Description:
// Set/Get the object used to perform pick operations. In order to
// pick instances of vtkProp, the picker must be a subclass of 
// vtkAbstractPropPicker, meaning that it can identify a particular 
// instance of vtkProp.
void vtkThreadSafeRenderWindowInteractor::SetPicker(vtkAbstractPicker* picker )
{
    this->Impl->SetPicker( picker );
}
vtkImplGetObjectMacroDef(vtkThreadSafeRenderWindowInteractor,Picker,vtkAbstractPicker);

// Description:
// Create default picker. Used to create one when none is specified.
// Default is an instance of vtkPropPicker.
vtkAbstractPropPicker *vtkThreadSafeRenderWindowInteractor::CreateDefaultPicker()
{
    return this->Impl->CreateDefaultPicker();
}

// Description:
// These methods correspond to the the Exit, User and Pick
// callbacks. They allow for the Style to invoke them.
void vtkThreadSafeRenderWindowInteractor::ExitCallback()
{
    this->Impl->ExitCallback();
}
void vtkThreadSafeRenderWindowInteractor::UserCallback()
{
    this->Impl->UserCallback();
}
void vtkThreadSafeRenderWindowInteractor::StartPickCallback()
{
    this->Impl->StartPickCallback();
}
void vtkThreadSafeRenderWindowInteractor::EndPickCallback()
{
    this->Impl->EndPickCallback();
}

// Description:
// Get the current position of the mouse.
void vtkThreadSafeRenderWindowInteractor::GetMousePosition(int *x, int *y)
{
    this->Impl->GetMousePosition( x, y );
}

// Description:
// Hide or show the mouse cursor, it is nice to be able to hide the
// default cursor if you want VTK to display a 3D cursor instead.
void vtkThreadSafeRenderWindowInteractor::HideCursor()
{
    this->Impl->HideCursor();
}
void vtkThreadSafeRenderWindowInteractor::ShowCursor()
{
    this->Impl->ShowCursor();
}

// Description:
// Render the scene. Just pass the render call on to the 
// associated vtkRenderWindow.
void vtkThreadSafeRenderWindowInteractor::Render()
{
    this->Impl->Render();
}

// Description:
// Given a position x, move the current camera's focal point to x.
// The movement is animated over the number of frames specified in
// NumberOfFlyFrames. The LOD desired frame rate is used.
void vtkThreadSafeRenderWindowInteractor::FlyTo(vtkRenderer *ren, double x, double y, double z)
{
    this->Impl->FlyTo( ren, x, y, z );
}
void vtkThreadSafeRenderWindowInteractor::FlyTo(vtkRenderer *ren, double *x)
{
    this->Impl->FlyTo( ren, x );
}
void vtkThreadSafeRenderWindowInteractor::FlyToImage(vtkRenderer *ren, double x, double y)
{
    this->Impl->FlyToImage( ren, x, y );
}
void vtkThreadSafeRenderWindowInteractor::FlyToImage(vtkRenderer *ren, double *x)
{
    this->Impl->FlyToImage( ren, x );
}

// Description:
// Set the number of frames to fly to when FlyTo is invoked.
vtkImplSetClampMacroDef(vtkThreadSafeRenderWindowInteractor,NumberOfFlyFrames,int,1,VTK_LARGE_INTEGER);
vtkImplGetMacroDef(vtkThreadSafeRenderWindowInteractor,NumberOfFlyFrames,int);

// Description:
// Set the total Dolly value to use when flying to (FlyTo()) a
// specified point. Negative values fly away from the point.
vtkImplSetMacroDef(vtkThreadSafeRenderWindowInteractor,Dolly,double);
vtkImplGetMacroDef(vtkThreadSafeRenderWindowInteractor,Dolly,double);

// Description:
// Set/Get information about the current event. 
// The current x,y position is in the EventPosition, and the previous
// event position is in LastEventPosition, updated automatically each
// time EventPosition is set using its Set() method. Mouse positions
// are measured in pixels.
// The other information is about key board input.
vtkImplGetVector2MacroDef(vtkThreadSafeRenderWindowInteractor,EventPosition,int);
vtkImplGetVector2MacroDef(vtkThreadSafeRenderWindowInteractor,LastEventPosition,int);
vtkImplSetVector2MacroDef(vtkThreadSafeRenderWindowInteractor,LastEventPosition,int);
void vtkThreadSafeRenderWindowInteractor::SetEventPosition(int x, int y)
{
    this->Impl->SetEventPosition( x, y );
}
void vtkThreadSafeRenderWindowInteractor::SetEventPosition(int pos[2])
{
    this->Impl->SetEventPosition( pos );
}
void vtkThreadSafeRenderWindowInteractor::SetEventPositionFlipY(int x, int y)
{
    this->Impl->SetEventPositionFlipY( x, y );
}
void vtkThreadSafeRenderWindowInteractor::SetEventPositionFlipY(int pos[2])
{
    this->Impl->SetEventPositionFlipY( pos );
}
vtkImplSetMacroDef(vtkThreadSafeRenderWindowInteractor,AltKey, int);
vtkImplGetMacroDef(vtkThreadSafeRenderWindowInteractor,AltKey, int);
vtkImplSetMacroDef(vtkThreadSafeRenderWindowInteractor,ControlKey, int);
vtkImplGetMacroDef(vtkThreadSafeRenderWindowInteractor,ControlKey, int);
vtkImplSetMacroDef(vtkThreadSafeRenderWindowInteractor,ShiftKey, int);
vtkImplGetMacroDef(vtkThreadSafeRenderWindowInteractor,ShiftKey, int);
vtkImplSetMacroDef(vtkThreadSafeRenderWindowInteractor,KeyCode, char);
vtkImplGetMacroDef(vtkThreadSafeRenderWindowInteractor,KeyCode, char);
vtkImplSetMacroDef(vtkThreadSafeRenderWindowInteractor,RepeatCount, int);
vtkImplGetMacroDef(vtkThreadSafeRenderWindowInteractor,RepeatCount, int);
vtkImplSetStringMacroDef(vtkThreadSafeRenderWindowInteractor,KeySym);
vtkImplGetStringMacroDef(vtkThreadSafeRenderWindowInteractor,KeySym);

// Description:
// Set all the event information in one call.
void vtkThreadSafeRenderWindowInteractor::SetEventInformation(int x, 
                         int y, 
                         int ctrl, 
                         int shift, 
                         char keycode, 
                         int repeatcount,
                         const char* keysym)
{
    this->Impl->SetEventInformation( x, y, ctrl, shift, keycode, repeatcount, keysym );
}

// Description:
// Calls SetEventInformation, but flips the Y based on the current Size[1] 
// value (i.e. y = this->Size[1] - y - 1).
void vtkThreadSafeRenderWindowInteractor::SetEventInformationFlipY(int x, 
                              int y, 
                              int ctrl, 
                              int shift, 
                              char keycode, 
                              int repeatcount,
                              const char* keysym)
{
    this->Impl->SetEventInformationFlipY( x, y, ctrl, shift, keycode, repeatcount, keysym );
}

// Description:
// Set all the keyboard-related event information in one call.
void vtkThreadSafeRenderWindowInteractor::SetKeyEventInformation(int ctrl, 
                            int shift, 
                            char keycode, 
                            int repeatcount,
                            const char* keysym)
{
    this->Impl->SetKeyEventInformation( ctrl, shift, keycode, repeatcount, keysym );
}

// Description:
// This methods sets the Size ivar of the interactor without
// actually changing the size of the window. Normally
// application programmers would use UpdateSize if anything.
// This is useful for letting someone else change the size of
// the rendering window and just letting the interactor
// know about the change.
// The current event width/height (if any) is in EventSize 
// (Expose event, for example).
// Window size is measured in pixels.
vtkImplSetVector2MacroDef(vtkThreadSafeRenderWindowInteractor,Size,int);
vtkImplGetVector2MacroDef(vtkThreadSafeRenderWindowInteractor,Size,int);
vtkImplSetVector2MacroDef(vtkThreadSafeRenderWindowInteractor,EventSize,int);
vtkImplGetVector2MacroDef(vtkThreadSafeRenderWindowInteractor,EventSize,int);

// Description:
// When an event occurs, we must determine which Renderer the event
// occurred within, since one RenderWindow may contain multiple
// renderers.
vtkRenderer *vtkThreadSafeRenderWindowInteractor::FindPokedRenderer(int x,int y)
{
    return this->Impl->FindPokedRenderer( x, y );
}

// Description:
// Return the object used to mediate between vtkInteractorObservers
// contending for resources. Multiple interactor observers will often
// request different resources (e.g., cursor shape); the mediator uses a
// strategy to provide the resource based on priority of the observer plus
// the particular request (default versus non-default cursor shape).
vtkObserverMediator *vtkThreadSafeRenderWindowInteractor::GetObserverMediator()
{
    return this->Impl->GetObserverMediator();
}

// Description:
// Use a 3DConnexion device. Initial value is false.
// If VTK is not build with the TDx option, this is no-op.
// If VTK is build with the TDx option, and a device is not connected,
// a warning is emitted.
// It is must be called before the first Render to be effective, otherwise
// it is ignored.
vtkImplSetMacroDef(vtkThreadSafeRenderWindowInteractor,UseTDx,bool);
vtkImplGetMacroDef(vtkThreadSafeRenderWindowInteractor,UseTDx,bool);

// Description:
// Fire various events. SetEventInformation should be called just prior
// to calling any of these methods. These methods will Invoke the
// corresponding vtk event.
void vtkThreadSafeRenderWindowInteractor::MouseMoveEvent()
{
    this->Impl->MouseMoveEvent();
}
void vtkThreadSafeRenderWindowInteractor::RightButtonPressEvent()
{
    this->Impl->RightButtonPressEvent();
}
void vtkThreadSafeRenderWindowInteractor::RightButtonReleaseEvent()
{
    this->Impl->RightButtonReleaseEvent();
}
void vtkThreadSafeRenderWindowInteractor::LeftButtonPressEvent()
{
    this->Impl->LeftButtonPressEvent();
}
void vtkThreadSafeRenderWindowInteractor::LeftButtonReleaseEvent()
{
    this->Impl->LeftButtonReleaseEvent();
}
void vtkThreadSafeRenderWindowInteractor::MiddleButtonPressEvent()
{
    this->Impl->MiddleButtonPressEvent();
}
void vtkThreadSafeRenderWindowInteractor::MiddleButtonReleaseEvent()
{
    this->Impl->MiddleButtonReleaseEvent();
}
void vtkThreadSafeRenderWindowInteractor::MouseWheelForwardEvent()
{
    this->Impl->MouseWheelForwardEvent();
}
void vtkThreadSafeRenderWindowInteractor::MouseWheelBackwardEvent()
{
    this->Impl->MouseWheelBackwardEvent();
}
void vtkThreadSafeRenderWindowInteractor::ExposeEvent()
{
    this->Impl->ExposeEvent();
}
void vtkThreadSafeRenderWindowInteractor::ConfigureEvent()
{
    this->Impl->ConfigureEvent();
}
void vtkThreadSafeRenderWindowInteractor::EnterEvent()
{
    this->Impl->EnterEvent();
}
void vtkThreadSafeRenderWindowInteractor::LeaveEvent()
{
    this->Impl->LeaveEvent();
}
void vtkThreadSafeRenderWindowInteractor::KeyPressEvent()
{
    this->Impl->KeyPressEvent();
}
void vtkThreadSafeRenderWindowInteractor::KeyReleaseEvent()
{
    this->Impl->KeyReleaseEvent();
}
void vtkThreadSafeRenderWindowInteractor::CharEvent()
{
    this->Impl->CharEvent();
}
void vtkThreadSafeRenderWindowInteractor::ExitEvent()
{
    this->Impl->ExitEvent();
}


