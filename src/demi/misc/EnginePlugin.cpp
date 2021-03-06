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
#include "MiscPch.h"
#include "EnginePlugin.h"
#include "DynamicLib.h"

#ifndef DEMI_STATIC_API

namespace Demi
{
    typedef void(*DLL_START_PLUGIN)(void);
    typedef void(*DLL_STOP_PLUGIN)(void);

    void DiPlugin::LoadPlugin(const DiString& pgName)
    {
        DiString plugin = pgName;
#if DEMI_PLATFORM == DEMI_PLATFORM_WIN32
#   if DEMI_DEBUG
        plugin += "_d";
#   endif
#endif
        DiDynLib* lib = DiDynLib::LoadLib(plugin);
        if (lib)
        {
            DLL_START_PLUGIN entryFunc = (DLL_START_PLUGIN)lib->GetSymbol("PluginBegin");

            if (!entryFunc)
            {
                DI_WARNING("Cannot find the beginning function of the plugin : %s", plugin.c_str());
            }

            entryFunc();
        }
    }

    void DiPlugin::UnloadPlugin(const DiString& pgName)
    {
        DiString plugin = pgName;
#if DEMI_PLATFORM == DEMI_PLATFORM_WIN32
#   if DEMI_DEBUG
        plugin += "_d";
#   endif
#endif
        if (!DiDynLib::HasLib(plugin))
        {
            DI_WARNING("The plugin : %s has not loaded yet.", plugin.c_str());
            return;
        }

        DiDynLib* lib = DiDynLib::LoadLib(plugin);
        DI_ASSERT(lib);
        DLL_START_PLUGIN endFunc = (DLL_START_PLUGIN)lib->GetSymbol("PluginEnd");

        if (!endFunc)
        {
            DI_WARNING("Cannot find the ending function of the plugin : %s", plugin.c_str());
        }

        endFunc();
    }
}

#endif