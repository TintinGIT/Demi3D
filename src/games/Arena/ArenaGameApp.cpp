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

#include "ArenaPch.h"
#include "ArenaGameApp.h"
#include "GfxDriver.h"
#include "AssetManager.h"
#include "SceneManager.h"
#include "RenderWindow.h"
#include "ArenaInput.h"
#include "K2Configs.h"
#include "EnginePlugin.h"
#include "GfxDriver.h"
#include "Command.h"
#include "LogManager.h"
#include "Window.h"
#include "ScriptPlugin.h"
#include "ScriptManager.h"
#include "ArenaBinding.h"
#include "PathLib.h"

#include "ArenaEntityManager.h"
#include "ArenaGameEntity.h"
#include "ArenaAIProperty.h"
#include "ArenaNPCEntity.h"
#include "K2RenderObjects.h"

#if DEMI_PLATFORM == DEMI_PLATFORM_OSX
#   include "ArenaGameAppOSX.h"
#elif DEMI_PLATFORM == DEMI_PLATFORM_IOS
#   include "ArenaGameAppIOS.h"
#endif

#define DRV_DX9     0
#define DRV_GL      1
#define DRV_GL_ES2  2

#if DEMI_PLATFORM == DEMI_PLATFORM_OSX
#   define USE_DRV     DRV_GL
#elif DEMI_PLATFORM == DEMI_PLATFORM_IOS
#   define USE_DRV     DRV_GL_ES2
#else
#   define USE_DRV     DRV_DX9
#endif

#ifdef DEMI_STATIC_API
#   if USE_DRV == DRV_DX9
#       include "DrvD3D9Plugin.h"
#   elif USE_DRV == DRV_GL
#       include "DrvGLPlugin.h"
#   elif USE_DRV == DRV_GL_ES2
#       include "DrvGLES2Plugin.h"
#   endif
#endif

namespace Demi
{
    static bool Quit(DiCmdArgs*)
    {
        ArGameApp::Get()->QuitApp();
        return true;
    }

    ArGameApp::ArGameApp()
        :mAssetManager(nullptr)
        , mInputMgr(nullptr)
        , mQuit(false)
        , mGame(nullptr)
    {
        DI_INIT_PROFILER;

        DI_ASSERT(!sApp);
        sApp = this;

        srand((unsigned int)time(NULL));

        DiLogManager* logmgr = DI_NEW DiLogManager();
        logmgr->Init("Arena.log");

#if USE_DRV == DRV_DX9
        DI_INSTALL_PLUGIN(DiDrvD3D9);
#elif USE_DRV == DRV_GL
        DI_INSTALL_PLUGIN(DiDrvGL);
#elif USE_DRV == DRV_GL_ES2
        DI_INSTALL_PLUGIN(DiDrvGLES2);
#endif
        DI_INSTALL_PLUGIN(DiScript);

        mAssetManager = new DiAssetManager;
        
#if DEMI_PLATFORM == DEMI_PLATFORM_IOS
        mAssetManager->SetBasePath("media");
#else
        mAssetManager->SetBasePath("../../media");
#endif

        CommandMgr->RegisterString("scene_type", "Octree", 0, "Scene manager type");
        CommandMgr->AddCommand("quit", Quit, "Close Application");
    }

    ArGameApp::~ArGameApp()
    {
        DI_CLOSE_PROFILER;
    }

    void ArGameApp::Update()
    {
        DI_PROFILE_BEGIN_FRAME

        mInputMgr->Update();

        if (mMainHwnd)
            Driver->Render();

        if (mQuit)
            CloseEngine();

        DI_PROFILE_END_FRAME
    }

    bool ArGameApp::IsOpen()
    {
        return mMainHwnd ? true : false;
    }

    void ArGameApp::Open(int argc, char *argv[])
    {
#if DEMI_PLATFORM == DEMI_PLATFORM_OSX
        NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
        
        mAppDelegate = [[AppDelegate alloc] init];
        [[NSApplication sharedApplication] setDelegate:mAppDelegate];
        
        [NSApp run];
        
        [pool drain];
        [mAppDelegate release];
        
#elif DEMI_PLATFORM == DEMI_PLATFORM_WIN32
        OpenImpl();
        Run();
#elif DEMI_PLATFORM == DEMI_PLATFORM_IOS
        NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
        UIApplicationMain(argc, argv, @"UIApplication", @"AppDelegate");
        [pool release];
#else
        DI_WARNING("Unsupported platform, quiting");
#endif
    }

    void ArGameApp::CloseEngine()
    {
        SAFE_DELETE(mInputMgr);

        SAFE_DELETE(mGame);

        DiK2Configs::Shutdown();

        Driver->Shutdown();

        DI_UNINSTALL_PLUGIN(DiScript);

#if USE_DRV == DRV_DX9
        DI_UNINSTALL_PLUGIN(DiDrvD3D9);
#elif USE_DRV == DRV_GL
        DI_UNINSTALL_PLUGIN(DiDrvGL);
#elif USE_DRV == DRV_GL_ES2
        DI_UNINSTALL_PLUGIN(DiDrvGLES2);
#endif

        SAFE_DELETE(mAssetManager);

        if (DiLogManager::GetInstancePtr())
            DI_DELETE DiLogManager::GetInstancePtr();
    }

    void ArGameApp::Run()
    {
        while (IsOpen())
            Update();
    }

    void ArGameApp::OpenImpl()
    {
        // iphone 5s's resolution
        bool ret = Driver->Init(1136,640, "Game", false);
        DI_ASSERT(ret);

        mInputMgr = new ArInput();

        DiWindow* wnd = Driver->GetMainRenderWindow()->GetWindow();
        mInputMgr->CreateInput(wnd->GetWndHandle(), wnd->GetWndViewHandle());

        using namespace std::placeholders;

        DiK2Configs::Init();

        mGame = DI_NEW ArGame();
        
        // load the scripts
        DI_LOG("Binding Arena APIs to lua...");
        tolua_arenaMain_open(DiScriptManager::Get()->GetLuaState());

#if DEMI_PLATFORM != DEMI_PLATFORM_IOS
        DiString baseFolder = DiPathLib::GetApplicationPath() + "../../media/arena/";
#else
        DiString baseFolder = DiPathLib::GetApplicationPath() + "media/arena/";
#endif
        baseFolder.SimplifyPath();
        DiScriptManager::Get()->SetBaseScriptPath(baseFolder);

        auto mainScript = DiAssetManager::GetInstance().OpenArchive("arena_main.lua");
        DiScriptManager::Get()->RunBuffer(mainScript);
        
        // test NPC
        auto npc = mGame->GetEntityManager()->CreateNPC(2,"npc_test.xml");
        npc->GetRenderObj()->SetPosition(DiK2Pos(202, 42));
        //npc->GetEntity<ArNPCEntity>()->GetAIProperty()->CommandMoveTo(DiK2Pos(213, 32));

        Driver->GetMainRenderWindow()->SetUpdateCallback([this](){
            mGame->Update();
        });
    }

    ArGameApp* ArGameApp::sApp = nullptr;
}