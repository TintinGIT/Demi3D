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
#include "Demi.h"
#include "DemoFrame.h"


void InitScene()
{
    DiPostEffectManager* peMgr = DiBase::Driver->GetMainRenderWindow()->GetPostEffectManager();

    DiPostEffect* effectFilm = peMgr->CreatePostEffect("Heart");
    effectFilm->SetEnable(true);

    DiPostEffectPass* passFilm = effectFilm->CreatePass("output");
    passFilm->SetQuadScale(1);
    passFilm->BuildMaterial("heart_v", "heart_p");
}

int main(int argc, char *argv[])
{
    DemiDemo app(DemoConfig("Demi3D Sample - Heart"));
    app.SetInitCallback(functor(InitScene));
    app.Open();

    return 0;
}