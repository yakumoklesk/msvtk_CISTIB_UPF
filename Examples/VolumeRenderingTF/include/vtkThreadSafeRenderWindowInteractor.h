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


#ifndef __VTKTHREADSAFERENDERWINDOWINTERACTOR_H__
#define __VTKTHREADSAFERENDERWINDOWINTERACTOR_H__

#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>

#include "msvThreadSafeGetSet.h"

class vtkThreadSafeRenderWindowInteractorImpl;

class vtkThreadSafeRenderWindowInteractor : public vtkRenderWindowInteractor
{
public:
    vtkTypeMacro( vtkThreadSafeRenderWindowInteractor, vtkRenderWindowInteractor );
    // Description:
    // Construct object
    static vtkThreadSafeRenderWindowInteractor* New();
    virtual void PrintSelf( ostream& os, vtkIndent indent );

    // Description:
    // Prepare for handling events. This must be called before the
    // interactor will work.
    virtual void Initialize();
    void ReInitialize();

    // Description:
    // This Method detects loops of RenderWindow-Interactor,
    // so objects are freed properly.
    virtual void UnRegister(vtkObjectBase *o);

    // Description:
    // Start the event loop. This is provided so that you do not have to
    // implement your own event loop. You still can use your own
    // event loop if you want. Initialize should be called before Start.
    virtual void Start();

    // Description:
    // Enable/Disable interactions.  By default interactors are enabled when
    // initialized.  Initialize() must be called prior to enabling/disabling
    // interaction. These methods are used when a window/widget is being
    // shared by multiple renderers and interactors.  This allows a "modal"
    // display where one interactor is active when its data is to be displayed
    // and all other interactors associated with the widget are disabled
    // when their data is not displayed.
    virtual void Enable();
    virtual void Disable();
    vtkImplGetMacroDecl(Enabled, int);

    // Description:
    // Enable/Disable whether vtkRenderWindowInteractor::Render() calls
    // this->RenderWindow->Render().
    vtkImplBooleanMacroDecl(EnableRender, bool);
    vtkImplSetMacroDecl(EnableRender, bool);
    vtkImplGetMacroDecl(EnableRender, bool);

    // Description:
    // Set/Get the rendering window being controlled by this object.
    void SetRenderWindow(vtkRenderWindow *aren);
    vtkImplGetObjectMacroDecl(RenderWindow,vtkRenderWindow);

    // Description:
    // Event loop notification member for window size change.
    // Window size is measured in pixels.
    virtual void UpdateSize(int x,int y);

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
    virtual int CreateTimer(int timerType); //first group, for backward compatibility
    virtual int DestroyTimer(); //first group, for backward compatibility
    int CreateRepeatingTimer(unsigned long duration);
    int CreateOneShotTimer(unsigned long duration);
    int IsOneShotTimer(int timerId);
    unsigned long GetTimerDuration(int timerId);
    int ResetTimer(int timerId);
    int DestroyTimer(int timerId);
    virtual int GetVTKTimerId(int platformTimerId);

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
    vtkImplSetClampMacroDecl(TimerDuration,unsigned long,1,100000);
    vtkImplGetMacroDecl(TimerDuration,unsigned long);

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
    vtkImplSetMacroDecl(TimerEventId, int);
    vtkImplGetMacroDecl(TimerEventId, int);
    vtkImplSetMacroDecl(TimerEventType, int);
    vtkImplGetMacroDecl(TimerEventType, int);
    vtkImplSetMacroDecl(TimerEventDuration, int);
    vtkImplGetMacroDecl(TimerEventDuration, int);
    vtkImplSetMacroDecl(TimerEventPlatformId, int);
    vtkImplGetMacroDecl(TimerEventPlatformId, int);

    // Description:
    // This function is called on 'q','e' keypress if exitmethod is not
    // specified and should be overridden by platform dependent subclasses
    // to provide a termination procedure if one is required.
    virtual void TerminateApp(void);

    // Description:
    // External switching between joystick/trackball/new? modes. Initial value
    // is a vtkInteractorStyleSwitch object.
    virtual void SetInteractorStyle(vtkInteractorObserver *);
    vtkImplGetObjectMacroDecl(InteractorStyle,vtkInteractorObserver);

    // Description:
    // Turn on/off the automatic repositioning of lights as the camera moves.
    // Default is On.
    vtkImplSetMacroDecl(LightFollowCamera,int);
    vtkImplGetMacroDecl(LightFollowCamera,int);
    vtkImplBooleanMacroDecl(LightFollowCamera,int);

    // Description:
    // Set/Get the desired update rate. This is used by vtkLODActor's to tell
    // them how quickly they need to render.  This update is in effect only
    // when the camera is being rotated, or zoomed.  When the interactor is
    // still, the StillUpdateRate is used instead. 
    // The default is 15.
    vtkImplSetClampMacroDecl(DesiredUpdateRate,double,0.0001,VTK_LARGE_FLOAT);
    vtkImplGetMacroDecl(DesiredUpdateRate,double);

    // Description:
    // Set/Get the desired update rate when movement has stopped.
    // For the non-still update rate, see the SetDesiredUpdateRate method.
    // The default is 0.0001
    vtkImplSetClampMacroDecl(StillUpdateRate,double,0.0001,VTK_LARGE_FLOAT);
    vtkImplGetMacroDecl(StillUpdateRate,double);

    // Description:
    // See whether interactor has been initialized yet.
    // Default is 0.
    vtkImplGetMacroDecl(Initialized,int);

    // Description:
    // Set/Get the object used to perform pick operations. In order to
    // pick instances of vtkProp, the picker must be a subclass of 
    // vtkAbstractPropPicker, meaning that it can identify a particular 
    // instance of vtkProp.
    virtual void SetPicker(vtkAbstractPicker*);
    vtkImplGetObjectMacroDecl(Picker,vtkAbstractPicker);

    // Description:
    // Create default picker. Used to create one when none is specified.
    // Default is an instance of vtkPropPicker.
    virtual vtkAbstractPropPicker *CreateDefaultPicker();

    // Description:
    // These methods correspond to the the Exit, User and Pick
    // callbacks. They allow for the Style to invoke them.
    virtual void ExitCallback();
    virtual void UserCallback();
    virtual void StartPickCallback();
    virtual void EndPickCallback();

    // Description:
    // Get the current position of the mouse.
    virtual void GetMousePosition(int *x, int *y);

    // Description:
    // Hide or show the mouse cursor, it is nice to be able to hide the
    // default cursor if you want VTK to display a 3D cursor instead.
    void HideCursor();
    void ShowCursor();

    // Description:
    // Render the scene. Just pass the render call on to the 
    // associated vtkRenderWindow.
    virtual void Render();

    // Description:
    // Given a position x, move the current camera's focal point to x.
    // The movement is animated over the number of frames specified in
    // NumberOfFlyFrames. The LOD desired frame rate is used.
    void FlyTo(vtkRenderer *ren, double x, double y, double z);
    void FlyTo(vtkRenderer *ren, double *x);
    void FlyToImage(vtkRenderer *ren, double x, double y);
    void FlyToImage(vtkRenderer *ren, double *x);

    // Description:
    // Set the number of frames to fly to when FlyTo is invoked.
    vtkImplSetClampMacroDecl(NumberOfFlyFrames,int,1,VTK_LARGE_INTEGER);
    vtkImplGetMacroDecl(NumberOfFlyFrames,int);

    // Description:
    // Set the total Dolly value to use when flying to (FlyTo()) a
    // specified point. Negative values fly away from the point.
    vtkImplSetMacroDecl(Dolly,double);
    vtkImplGetMacroDecl(Dolly,double);

    // Description:
    // Set/Get information about the current event. 
    // The current x,y position is in the EventPosition, and the previous
    // event position is in LastEventPosition, updated automatically each
    // time EventPosition is set using its Set() method. Mouse positions
    // are measured in pixels.
    // The other information is about key board input.
    vtkImplGetVector2MacroDecl(EventPosition,int);
    vtkImplGetVector2MacroDecl(LastEventPosition,int);
    vtkImplSetVector2MacroDecl(LastEventPosition,int);
    virtual void SetEventPosition(int x, int y);
    virtual void SetEventPosition(int pos[2]);
    virtual void SetEventPositionFlipY(int x, int y);
    virtual void SetEventPositionFlipY(int pos[2]);
    vtkImplSetMacroDecl(AltKey, int);
    vtkImplGetMacroDecl(AltKey, int);
    vtkImplSetMacroDecl(ControlKey, int);
    vtkImplGetMacroDecl(ControlKey, int);
    vtkImplSetMacroDecl(ShiftKey, int);
    vtkImplGetMacroDecl(ShiftKey, int);
    vtkImplSetMacroDecl(KeyCode, char);
    vtkImplGetMacroDecl(KeyCode, char);
    vtkImplSetMacroDecl(RepeatCount, int);
    vtkImplGetMacroDecl(RepeatCount, int);
    vtkImplSetStringMacroDecl(KeySym);
    vtkImplGetStringMacroDecl(KeySym);

    // Description:
    // Set all the event information in one call.
    void SetEventInformation(int x, 
        int y, 
        int ctrl=0, 
        int shift=0, 
        char keycode=0, 
        int repeatcount=0,
        const char* keysym=0);

    // Description:
    // Calls SetEventInformation, but flips the Y based on the current Size[1] 
    // value (i.e. y = this->Size[1] - y - 1).
    void SetEventInformationFlipY(int x, 
        int y, 
        int ctrl=0, 
        int shift=0, 
        char keycode=0, 
        int repeatcount=0,
        const char* keysym=0);

    // Description:
    // Set all the keyboard-related event information in one call.
    void SetKeyEventInformation(int ctrl=0, 
        int shift=0, 
        char keycode=0, 
        int repeatcount=0,
        const char* keysym=0);

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
    vtkImplSetVector2MacroDecl(Size,int);
    vtkImplGetVector2MacroDecl(Size,int);
    vtkImplSetVector2MacroDecl(EventSize,int);
    vtkImplGetVector2MacroDecl(EventSize,int);

    // Description:
    // When an event occurs, we must determine which Renderer the event
    // occurred within, since one RenderWindow may contain multiple
    // renderers.
    virtual vtkRenderer *FindPokedRenderer(int,int);

    // Description:
    // Return the object used to mediate between vtkInteractorObservers
    // contending for resources. Multiple interactor observers will often
    // request different resources (e.g., cursor shape); the mediator uses a
    // strategy to provide the resource based on priority of the observer plus
    // the particular request (default versus non-default cursor shape).
    vtkObserverMediator *GetObserverMediator();

    // Description:
    // Use a 3DConnexion device. Initial value is false.
    // If VTK is not build with the TDx option, this is no-op.
    // If VTK is build with the TDx option, and a device is not connected,
    // a warning is emitted.
    // It is must be called before the first Render to be effective, otherwise
    // it is ignored.
    vtkImplSetMacroDecl(UseTDx,bool);
    vtkImplGetMacroDecl(UseTDx,bool);

    // Description:
    // Fire various events. SetEventInformation should be called just prior
    // to calling any of these methods. These methods will Invoke the
    // corresponding vtk event.
    virtual void MouseMoveEvent();
    virtual void RightButtonPressEvent();
    virtual void RightButtonReleaseEvent();
    virtual void LeftButtonPressEvent();
    virtual void LeftButtonReleaseEvent();
    virtual void MiddleButtonPressEvent();
    virtual void MiddleButtonReleaseEvent();
    virtual void MouseWheelForwardEvent();
    virtual void MouseWheelBackwardEvent();
    virtual void ExposeEvent();
    virtual void ConfigureEvent();
    virtual void EnterEvent();
    virtual void LeaveEvent();
    virtual void KeyPressEvent();
    virtual void KeyReleaseEvent();
    virtual void CharEvent();
    virtual void ExitEvent();

protected:
    vtkThreadSafeRenderWindowInteractor();
    ~vtkThreadSafeRenderWindowInteractor();

private:

    vtkThreadSafeRenderWindowInteractorImpl*          Impl;
};

typedef vtkSmartPointer<vtkThreadSafeRenderWindowInteractor>  vtkThreadSafeRenderWindowInteractorSP;

#endif	// #ifndef __VTKTHREADSAFERENDERWINDOWINTERACTOR_H__

