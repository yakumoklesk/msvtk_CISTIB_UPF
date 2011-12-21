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

#ifndef __MSVTHREADSAFEGETSET_H__
#define __MSVTHREADSAFEGETSET_H__





// Function declaration

#define vtkImplSetMacroDecl(name,type) \
    virtual void Set##name(type _arg);

#define vtkImplGetMacroDecl(name,type) \
    virtual type Get##name();

#define vtkImplBooleanMacroDecl(name,type) \
    virtual void name##On (); \
    virtual void name##Off ();

#define vtkImplGetObjectMacroDecl(name,type) \
    virtual type *Get##name();

#define vtkImplSetClampMacroDecl(name,type,min,max) \
    virtual void Set##name(type _arg); \
    virtual type Get##name##MinValue(); \
    virtual type Get##name##MaxValue();

#define vtkImplSetVector2MacroDecl(name,type) \
    virtual void Set##name (type _arg1, type _arg2); \
    void Set##name( type _arg[2] );

#define vtkImplGetVector2MacroDecl(name,type) \
    virtual type *Get##name(); \
    virtual void Get##name(type &_arg1, type &_arg2); \
    virtual void Get##name (type _arg[2]);

#define vtkImplSetVector3MacroDecl(name,type) \
    virtual void Set##name( type _arg1, type _arg2, type _arg3); \
    virtual void Set##name( type _arg[3]);

#define vtkImplGetVector3MacroDecl(name,type) \
    virtual type *Get##name(); \
    virtual void Get##name( type &_arg1, type &_arg2, type &_arg3 ); \
    virtual void Get##name( type _arg[3] );

#define vtkImplGetVectorMacroDecl(name,type,count) \
    virtual type *Get##name(); \
    virtual void Get##name( type data[count] );

#define vtkImplSetStringMacroDecl(name) \
    virtual void Set##name (const char* _arg);

#define vtkImplGetStringMacroDecl(name) \
    virtual char* Get##name ();





// Function definition

#define vtkImplSetMacroDef(classname,name,type) \
    void classname::Set##name (type _arg) \
    { \
        this->Impl->Set##name(_arg); \
    }

#define vtkImplGetMacroDef(classname,name,type) \
    type classname::Get##name () \
    { \
        return this->Impl->Get##name(); \
    }

#define vtkImplBooleanMacroDef(classname,name,type) \
    void classname::name##On () { this->Impl->Set##name(static_cast<type>(1));}   \
    void classname::name##Off () { this->Impl->Set##name(static_cast<type>(0));}

#define vtkImplGetObjectMacroDef(classname,name,type) \
    type *classname::Get##name () \
    { \
        return this->Impl->Get##name(); \
    }

#define vtkImplSetClampMacroDef(classname,name,type,min,max) \
    void classname::Set##name (type _arg) \
    { \
        this->Impl->Set##name( _arg ); \
    } \
    type classname::Get##name##MinValue() \
    { \
        return this->Impl->Get##name##MinValue(); \
    } \
    type classname::Get##name##MaxValue() \
    { \
        return this->Impl->Get##name##MaxValue(); \
    }

#define vtkImplSetVector2MacroDef(classname,name,type) \
    void classname::Set##name (type _arg1, type _arg2) \
    { \
        this->Impl->Set##name( _arg1, _arg2 ); \
    } \
    void classname::Set##name( type _arg[2] ) \
    { \
        this->Impl->Set##name( _arg[0], _arg[1] ); \
    }

#define vtkImplGetVector2MacroDef(classname,name,type) \
    type *classname::Get##name() \
    { \
        return this->Impl->Get##name(); \
    } \
    void classname::Get##name(type &_arg1, type &_arg2) \
    { \
        this->Impl->Get##name( _arg1, _arg2 ); \
    } \
    void classname::Get##name(type _arg[2]) \
    { \
        this->Impl->Get##name( _arg[0], _arg[1] );\
    }

#define vtkImplSetVector3MacroDef(classname,name,type) \
    void classname::Set##name( type _arg1, type _arg2, type _arg3) \
    { \
        this->Impl->Set##name( _arg1, _arg2, _arg3); \
    } \
    void classname::Set##name( type _arg[3] ) \
    { \
        this->Impl->Set##name( _arg[0], _arg[1], _arg[2] ); \
    }

#define vtkImplGetVector3MacroDef(classname,name,type) \
    type *classname::Get##name() \
    { \
        return this->Impl->Get##name(); \
    } \
    void classname::Get##name( type &_arg1, type &_arg2, type &_arg3 ) \
    { \
        this->Impl->Get##name( _arg1, _arg2, _arg3 ); \
    } \
    void classname::Get##name( type _arg[3] ) \
    { \
        this->Impl->Get##name( _arg[0], _arg[1], _arg[2] ); \
    }

#define vtkImplGetVectorMacroDef(classname,name,type,count) \
    type *classname::Get##name() \
    { \
        return this->Impl->Get##name(); \
    } \
    void classname::Get##name(type data[count]) \
    { \
        this->Impl->Get##name( data ); \
    }

#define vtkImplSetStringMacroDef(classname,name) \
    void classname::Set##name(const char* _arg) \
    { \
        this->Impl->Set##name( _arg ); \
    }

#define vtkImplGetStringMacroDef(classname,name) \
    char* classname::Get##name() \
    { \
        return this->Impl->Get##name(); \
    }





// Safe get set

#define vtkSafeSetMacro(name,type) \
    virtual void Set##name (type _arg) \
    { \
        this->Mutex->Lock(); \
        Impl->Set##name(_arg); \
        this->Mutex->Unlock(); \
    }

#define vtkSafeGetMacro(name,type) \
    virtual type Get##name() \
    { \
        type aux( 0 ); \
        this->Mutex->Lock(); \
        aux = this->Impl->Get##name(); \
        this->Mutex->Unlock(); \
        return aux; \
    }

#define vtkSafeBooleanMacro(name,type) \
    virtual void name##On () { this->Set##name(static_cast<type>(1));} \
    virtual void name##Off () { this->Set##name(static_cast<type>(0));}

#define vtkSafeGetObjectMacro(name,type) \
    type *Get##name() \
    { \
        type* aux( 0 ); \
        this->Mutex->Lock(); \
        aux = this->Impl->Get##name(); \
        this->Mutex->Unlock(); \
        return aux; \
    }

#define vtkSafeSetClampMacro(name,type,min,max) \
    virtual void Set##name(type _arg) \
    { \
        this->Mutex->Lock(); \
        this->Impl->Set##name( _arg ); \
        this->Mutex->Unlock(); \
    } \
    virtual type Get##name##MinValue() \
    { \
        type aux( 0 ); \
        this->Mutex->Lock(); \
        aux = this->Impl->Get##name##MinValue(); \
        this->Mutex->Unlock(); \
        return aux; \
    } \
    virtual type Get##name##MaxValue() \
    { \
        type aux( 0 ); \
        this->Mutex->Lock(); \
        aux = this->Impl->Get##name##MaxValue(); \
        this->Mutex->Unlock(); \
        return aux; \
    }

#define vtkSafeSetVector2Macro(name,type) \
    void Set##name( type _arg1, type _arg2 ) \
    { \
        this->Mutex->Lock(); \
        this->Impl->Set##name( _arg1, _arg2 ); \
        this->Mutex->Unlock(); \
    } \
    void Set##name( type _arg[2] ) \
    { \
        this->Mutex->Lock(); \
        this->Impl->Set##name( _arg[0], _arg[1] ); \
        this->Mutex->Unlock(); \
    }

#define vtkSafeGetVector2Macro(name,type) \
    type *Get##name() \
    { \
        return this->Impl->Get##name(); \
    } \
    void Get##name( type &_arg1, type &_arg2 ) \
    { \
        this->Impl->Get##name( _arg1, _arg2 ); \
    } \
    void Get##name( type _arg[2] ) \
    { \
        this->Impl->Get##name( _arg[0], _arg[1] ); \
    }

#define vtkSafeSetVector3Macro(name,type) \
    void Set##name( type _arg1, type _arg2, type _arg3) \
    { \
        this->Mutex->Lock(); \
        this->Impl->Set##name( _arg1, _arg2, _arg3); \
        this->Mutex->Unlock(); \
    } \
    void Set##name( type _arg[3] ) \
    { \
        this->Mutex->Lock(); \
        this->Impl->Set##name( _arg[0], _arg[1], _arg[2] ); \
        this->Mutex->Unlock(); \
    }

#define vtkSafeGetVector3Macro(name,type) \
    type *Get##name() \
    { \
        return this->Impl->Get##name(); \
    } \
    void Get##name( type &_arg1, type &_arg2, type &_arg3 ) \
    { \
        this->Impl->Get##name( _arg1, _arg2, _arg3 ); \
    } \
    void Get##name( type _arg[3] ) \
    { \
        this->Impl->Get##name( _arg[0], _arg[1], _arg[2] ); \
    }

#define vtkSafeGetVectorMacro(name,type,count) \
    type *Get##name() \
    { \
        type* aux( 0 ); \
        this->Mutex->Lock(); \
        aux = this->Impl->Get##name(); \
        this->Mutex->Unlock(); \
        return aux; \
    } \
    void Get##name(type data[count]) \
    { \
        this->Mutex->Lock(); \
        this->Impl->Get##name( data ); \
        this->Mutex->Unlock(); \
    }

#define vtkSafeSetStringMacro(name) \
    virtual void Set##name( const char* _arg) \
    { \
        this->Mutex->Lock(); \
        Impl->Set##name(_arg); \
        this->Mutex->Unlock(); \
    }

#define vtkSafeGetStringMacro(name) \
    virtual char* Get##name() \
    { \
        char* aux( 0 ); \
        this->Mutex->Lock(); \
        aux = this->Impl->Get##name(); \
        this->Mutex->Unlock(); \
        return aux; \
    }

#endif  // #ifndef __MSVTHREADSAFEGETSET_H__
