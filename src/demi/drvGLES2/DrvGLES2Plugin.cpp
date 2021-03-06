/**********************************************************************
This source file is a part of Demi3D
   __  ___  __  __  __
  |  \|_ |\/||   _)|  \ 
  |__/|__|  ||  __)|__/ 

Copyright (c) 2013-2014 Demi team
https://github.com/wangyanxing/Demi3D

Released under the MIT License
https://github.com/wangyanxing/Demi3D/blob/master/License.txt
***********************************************************************/

#include "DrvGLES2Pch.h"
#include "DrvGLES2Plugin.h"
#include "GLES2Driver.h"

namespace Demi
{
    const DiString& DiDrvGLES2Plugin::GetName() const
    {
        static DiString plugin = "DrvGLES2";
        return plugin;
    }

    void DiDrvGLES2Plugin::Install()
    {
        DI_ASSERT(!mDriver);
        DI_ASSERT(!DiBase::Driver);
        mDriver = DI_NEW DiGLES2Driver();
    }

    void DiDrvGLES2Plugin::Uninstall()
    {
        DI_ASSERT(mDriver);
        DI_DELETE mDriver;
        mDriver = nullptr;
    }

    DiDrvGLES2Plugin::DiDrvGLES2Plugin()
    {
#ifndef DEMI_STATIC_API
        mDriver = nullptr;
#endif
    }

#ifndef DEMI_STATIC_API
    DiDrvGLES2Plugin* plugin = nullptr;

    extern "C" void DI_GLES2_API PluginBegin() throw()
    {
        plugin = DI_NEW DiDrvGLES2Plugin();
        plugin->Install();
    }

    extern "C" void DI_GLES2_API PluginEnd()
    {
        plugin->Uninstall();
        DI_DELETE plugin;
    }
#else
    DiGLES2Driver* DiDrvGLES2Plugin::mDriver = nullptr;
#endif
}