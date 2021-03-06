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

#include "GfxPch.h"
#include "GfxDriver.h"
#include "RenderTarget.h"
#include "DepthBuffer.h"
#include "RenderWindow.h"
#include "RenderPipeline.h"
#include "ShaderManager.h"
#include "Window.h"
#include "Command.h"

namespace Demi
{
    DiGfxDriver::DiGfxDriver()
        :
        mFullScreen(false),
        mDeviceLost(false),
        mMainWindow(nullptr),
        mCommandMgr(nullptr),
        mDeltaTime(0),
        mElapsedTime(0),
        mFrameNum(0),
        mWndManager(nullptr),
        mNeedTextureFlipping(false),
        mInvertVertexWinding(false),
        mCaps(nullptr)
    {
        Driver = this;
        mCommandMgr = DI_NEW DiCommandManager();
        mWndManager = DI_NEW DiWindowManager();

        InitCommandVars();
    }

    DiGfxDriver::~DiGfxDriver()
    {
        Driver = nullptr;
        mMainHwnd = nullptr;
        SAFE_DELETE(mCaps);
        SAFE_DELETE(mWndManager);
    }

    void DiGfxDriver::SwapBuffer(DiRenderTarget* renderwnd)
    {
        if (mDeviceLost || IsDeviceLost())
        {
            // Test if we can reset the device
            if (ReadyToReset())
            {
                NotifyDeviceLost();
            }
            else
            {
                // not ready to reset yet
                return;
            }
        }

        if (!renderwnd->SwapBuffer())
            NotifyDeviceLost();
    }

    void DiGfxDriver::NotifyDeviceLost()
    {
        mDeviceLost = true;

        uint32 w = 0;
        uint32 h = 0;
        GetWindowSize(mMainHwnd, w, h);

        if (mMainWindow)
            mMainWindow->OnMoveOrResize();

        // minimizing window, don't try to reset
        if (w == 0 || h == 0)
            return;

        OnDeviceLost();

        if (ResetDevice(w, h))
            mDeviceLost = false;

        OnDeviceReset();
    }

    void DiGfxDriver::CleanUpDepthBuffers(bool bCleanManualBuffers)
    {
        auto itMap = mDepthBufferPool.begin();
        auto enMap = mDepthBufferPool.end();

        while (itMap != enMap)
        {
            auto itor = itMap->second.begin();
            auto end  = itMap->second.end();

            while (itor != end)
            {
                if (bCleanManualBuffers || !(*itor)->IsManual())
                {
                    DI_DELETE *itor;
                }
                ++itor;
            }

            itMap->second.clear();
            ++itMap;
        }

        mDepthBufferPool.clear();
    }

    void DiGfxDriver::SetDepthBufferFor(DiRenderTarget *renderTarget)
    {
        uint16 poolId = renderTarget->GetDepthBufferPool();
        if (poolId == DiDepthBuffer::POOL_NO_DEPTH)
            return;

        auto itor = mDepthBufferPool[poolId].begin();
        auto end  = mDepthBufferPool[poolId].end();

        bool bAttached = false;
        while (itor != end && !bAttached)
        {
            bAttached = renderTarget->AttachDepthBuffer(*itor++);
        }

        if (!bAttached)
        {
            DiDepthBuffer *newDepthBuffer = renderTarget->CreateDepthBuffer();
            if (newDepthBuffer)
            {
                newDepthBuffer->SetPoolId(poolId);
                mDepthBufferPool[poolId].push_back(newDepthBuffer);

                bAttached = renderTarget->AttachDepthBuffer(newDepthBuffer);

                DI_ASSERT(bAttached);
            }
            else
            {
                DI_WARNING("Null depth buffer!");
            }
        }
    }

    void DiGfxDriver::AddDeviceLostListener(DiDeviceLostListener* res)
    {
#ifndef NDEBUG
        if (mGpuResources.contains(res))
        {
            DI_WARNING("The gpu resource : %x has already existed", res);
            return;
        }
#endif
        mGpuResources.push_back(res);
    }

    void DiGfxDriver::RemoveDeviceLostListener(DiDeviceLostListener* res)
    {
        auto it = mGpuResources.find(res);
        if (it != mGpuResources.end())
        {
            mGpuResources.erase(it);
            return;
        }
        DI_WARNING("Cannot find the gpu resource");
    }

    void DiGfxDriver::OnDeviceLost()
    {
        DI_DEBUG("*******Losing device*******");

        for (auto it = mGpuResources.begin(); it != mGpuResources.end(); ++it)
            (*it)->OnDeviceLost();

        for (auto it = mRenderWindows.begin(); it != mRenderWindows.end(); ++it)
            it->second->OnDeviceLost();
    }

    void DiGfxDriver::OnDeviceReset()
    {
        DI_DEBUG("*******Resetting device*******");

        for (auto it = mGpuResources.begin(); it != mGpuResources.end(); ++it)
            (*it)->OnDeviceReset();

        for (auto it = mRenderWindows.begin(); it != mRenderWindows.end(); ++it)
            it->second->OnDeviceReset();
    }

    DiRenderWindow* DiGfxDriver::CreateRenderWindow(const DiString& name, DiWndHandle hwnd)
    {
        if (mRenderWindows.contains(hwnd))
        {
            DI_WARNING("The render window [%d] has already existed", (long)hwnd);
            return mRenderWindows[hwnd];
        }

        DiRenderWindow* wnd = DI_NEW DiRenderWindow(name);
        mRenderWindows[hwnd] = wnd;
        wnd->Create(hwnd);
        return wnd;
    }

    void DiGfxDriver::DestroyRenderWindow(DiWndHandle hwnd)
    {
        auto it = mRenderWindows.find(hwnd);
        if (it != mRenderWindows.end())
        {
            if (hwnd == mMainHwnd)
            {
                mMainHwnd = nullptr;
            }

            DI_DELETE it->second;
            mRenderWindows.erase(it);
        }

        if (!mMainHwnd)
        {
            DI_ASSERT(mRenderWindows.empty());
            Shutdown();
        }
    }

    DiRenderWindow* DiGfxDriver::FindRenderWindow(DiWndHandle hwnd)
    {
        auto it = mRenderWindows.find(hwnd);
        if (it != mRenderWindows.end())
            return it->second;

        return nullptr;
    }

    DiRenderWindow* DiGfxDriver::GetMainRenderWindow()
    {
        if (mRenderWindows.empty() || !mMainHwnd)
            return nullptr;
        
        return mRenderWindows[mMainHwnd];
    }

    void DiGfxDriver::DestroyRenderWindows()
    {
        // close other windows
        for (auto it = mRenderWindows.begin(); it != mRenderWindows.end(); ++it)
        {
            if (it->first != mMainHwnd)
                DI_DELETE it->second;
        }
        
        // close main window
        if (mMainHwnd)
            DestroyRenderWindow(mMainHwnd);

        mRenderWindows.clear();
    }

    bool DiGfxDriver::Init(DiWndHandle wnd)
    {
        if (!wnd)
        {
            Shutdown();
            return false;
        }

        mMainHwnd = wnd;
        InitGfx(mMainHwnd);

        mCaps = InitGfxCaps();

        mPipeline = DI_NEW DiRenderPipeline();
        mShaderManager = DI_NEW DiShaderManager();
        DiRenderWindow* rw = CreateRenderWindow("main_wnd", wnd);

        if (mMainWindow)
            mMainWindow->SetParentRenderWnd(rw);

        NotifyDeviceLost();

        PostInit();

        return true;
    }

    bool DiGfxDriver::Init(uint32 w, uint32 h, const DiString& title, bool fullscreen)
    {
        mMainWindow = Driver->CreateWnd();
        mMainWindow->Create(w, h, title, fullscreen);
        mWndManager->RegisterWindow(mMainWindow);
        return Init(mMainWindow->GetWndHandle());
    }

    void DiGfxDriver::Shutdown(bool destroyWindows)
    {
        if (destroyWindows && !mRenderWindows.empty())
            DestroyRenderWindows();

        SAFE_DELETE(mShaderManager);
        SAFE_DELETE(mPipeline);

        ReleaseGfx();

        SAFE_DELETE(mCommandMgr);
    }

    void DiGfxDriver::Render(DiRenderWindow* render)
    {
        DI_PROFILE(Graphics_Update);

        mDeltaTime = mTimer.GetElapse();
        mElapsedTime += mDeltaTime;
        mTimer.Start();
        mFrameNum++;

        if (!render)
            render = GetMainRenderWindow();

        if (!render)
            return;

        if (mMainWindow)
            mMainWindow->Update();

        DiRenderWindow::ActiveWindow = render;
        render->Render();
        DiRenderWindow::ActiveWindow = nullptr;
    }

    DiSceneManager* DiGfxDriver::GetSceneManager()
    {
        if (mRenderWindows.empty())
            return nullptr;
        
        return mRenderWindows[mMainHwnd]->GetSceneManager();
    }

    DiShaderEnvironment* DiGfxDriver::GetShaderEnvironment() const
    {
        return mPipeline->GetShaderEnvironment();
    }

    void DiGfxDriver::GetWindowSize(DiWndHandle wnd, uint32& w, uint32& h)
    {
        DiWindow* window = mWndManager->GetWindow(wnd);
        if (window)
        {
            window->GetWindowSize(w, h);
        }
        else
        {
            w = h = 0;
            DI_WARNING("Cannot find the window, failed to get the window size");
        }
    }

    void DiGfxDriver::InitCommandVars()
    {
        CommandMgr->RegisterInt("force_softskin", 0, 0, "Force to use software skinning");
        CommandMgr->RegisterInt("def_filter", FILTER_BILINEAR, 0, "Default texture filter");
        CommandMgr->RegisterFloat("tex_anisotropy", 4.0f, 0, "Default texture anisotropy level");
        CommandMgr->RegisterFloat("clear_color", 0.0f, 0, "Clear color of the main render target");
        CommandMgr->RegisterFloat("clear_alpha", 1.0f, 0, "Clear color of the main render target");
    }
}