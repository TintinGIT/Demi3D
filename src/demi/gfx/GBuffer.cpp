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
#include "GBuffer.h"
#include "RenderTarget.h"
#include "AssetManager.h"
#include "GfxDriver.h"
#include "RenderWindow.h"
#include "DepthBuffer.h"

namespace Demi 
{
    DiGBuffer::DiGBuffer(DiRenderWindow* renderWnd)
        :mParentWnd(renderWnd)
    {
        Create();
    }

    DiGBuffer::~DiGBuffer(void)
    {
    }

    void DiGBuffer::Create()
    {
        // create RTs
        DiString gbName;
        gbName = "_gb_0";

        DiTexturePtr tex;
        tex = DiAssetManager::GetInstance().CreateOrReplaceAsset<DiTexture>(gbName);
        tex->SetDimensions(0,0);
        tex->SetAdaptedRT(mParentWnd->GetSceneCanvas());
        tex->SetViewportScale(DiVec2(1,1));
        tex->SetFormat(PF_A32B32G32R32F);
        tex->SetResourceUsage(RU_NONE);
        tex->SetUsage(TU_RENDER_TARGET);
        tex->CreateTexture();
        tex->SetAddressing(AM_BORDER);

        DiRenderTarget* rt = tex->GetRenderTarget();
        if (rt)
            rt->SetFlippingUV(true);
        //rt->SetDepthBufferPool(DiDepthBuffer::POOL_DEFAULT);

        //rt->AttachDepthBuffer(mParentWnd->GetRenderBuffer()->GetDepthBuffer());
    
        mTexture.push_back(tex);
        mRenderTargets.push_back(rt);

        // create materials
        mMaterial = DiMaterial::QuickCreate("gbuffer_v","gbuffer_p");
        mMaterial->SetDepthWrite(true);
        mMaterial->SetDepthCheck(true);
    }

    void DiGBuffer::Bind()
    {
        for(size_t i = 0; i < mRenderTargets.size(); i++)
        {
            DiRenderTarget* rt = mRenderTargets[i];
            if (rt) 
                rt->Bind((uint8)i);
        }
    }

}