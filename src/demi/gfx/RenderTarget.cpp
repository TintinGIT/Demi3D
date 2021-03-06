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
#include "RenderTarget.h"
#include "GfxDriver.h"
#include "Texture.h"
#include "DepthBuffer.h"
#include "GfxDriver.h"
#include "RenderWindow.h"
#include "PostEffectManager.h"

#if DEMI_COMPILER == DEMI_COMPILER_MSVC
#   pragma warning(disable : 4706)
#endif

namespace Demi 
{
    DiRenderTarget::DiRenderTarget()
       :mWidth(0),
        mHeight(0),
        mViewport(this),
        mParentTex(nullptr),
        mDepthBuffer(nullptr),
        mDepthClearValue(1.0f),
        mClearPerFrame(true),
        mClearColor(DiColor::Black),
        mDepthBufferPoolId(DiDepthBuffer::POOL_DEFAULT),
        mClearFlag(CLEAR_COLOR|CLEAR_DEPTH|CLEAR_STENCIL),
        mIsActive(true),
        mFlippingUV(false)
    {
        Driver->AddDeviceLostListener(this);
    }

    DiRenderTarget::~DiRenderTarget()
    {
        if (Driver)
            Driver->RemoveDeviceLostListener(this);

        DetachDepthBuffer();
    }

    void DiRenderTarget::OnDeviceLost() 
    {
    }

    void DiRenderTarget::AttachTexture(DiTexturePtr surface)
    {
        mParentTex = surface;
        mWidth = mParentTex->GetWidth();
        mHeight = mParentTex->GetHeight();
        AttachSurface();
    }

    void DiRenderTarget::DetachTexture()
    {
        mParentTex = nullptr;
        DetachSurface();
    }

    void DiRenderTarget::SetDepthBufferPool( uint16 poolId )
    {
        if( mDepthBufferPoolId != poolId )
        {
            mDepthBufferPoolId = poolId;    
            DetachDepthBuffer();
        }
    }

    uint16 DiRenderTarget::GetDepthBufferPool() const
    {
        return mDepthBufferPoolId;
    }

    DiDepthBuffer* DiRenderTarget::GetDepthBuffer() const
    {
        return mDepthBuffer;
    }

    bool DiRenderTarget::AttachDepthBuffer( DiDepthBuffer *depthBuffer )
    {
        bool retVal = false;

        if ((retVal = IsCompatibleWith(depthBuffer)))
        {
            DetachDepthBuffer();
            mDepthBuffer = depthBuffer;
            mDepthBuffer->NotifyRenderTargetAttached( this );
        }

        return retVal;
    }

    void DiRenderTarget::DetachDepthBuffer()
    {
        if( mDepthBuffer )
        {
            mDepthBuffer->NotifyRenderTargetDetached( this );
            mDepthBuffer = nullptr;
        }
    }

    void DiRenderTarget::Bind(uint8 mrtID)
    {
        if(!ActiveRenderWindow->GetPostEffectManager()->HasEnabledPostPasses())
        {
            Driver->SetTextureFlipping(this == ActiveRenderWindow->GetRenderBuffer()?
                                       false : true);
        }
        else
        {
            Driver->SetTextureFlipping(mFlippingUV);
        }
        
        PreBind();

        if (!BindRenderTarget(mrtID))
        {
            DI_WARNING("Cannot bind render target with mrtID %d", mrtID);
            return;
        }

        if (!BindDepthStencil())
        {
            DI_WARNING("Cannot bind depth/stencil buffer");
        }

        if (mClearPerFrame)
            Driver->Clear(mClearFlag, mClearColor, mDepthClearValue);
    }

    void DiRenderTarget::ClearDepthBuffer()
    {
        mDepthBuffer = nullptr;
    }

    bool DiRenderTarget::IsCompatibleWith(DiDepthBuffer* db)
    {
        if (this->GetWidth() >= db->GetWidth() &&
            this->GetHeight() >= db->GetHeight())
        {
            return true;
        }
        return false;
    }

}