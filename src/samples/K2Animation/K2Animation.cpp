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
#include "DebugHelper.h"

#include "K2Model.h"
#include "K2Clip.h"
#include "K2Configs.h"
#include "K2GameDefines.h"

#include "EnginePlugin.h"

DiK2ModelPtr k2md = nullptr;
DiK2ModelPtr k2md2 = nullptr;

void InitScene()
{
    DiPlugin::LoadPlugin("DiK2");
    
    DiSceneManager* sm = DiBase::Driver->GetSceneManager();
    
    DiDirLightPtr dirlight;
    
    sm->SetAmbientColor(DiColor(0.3f, 0.3f, 0.3f));
    
    dirlight = make_shared<DiDirLight>();
    DiCullNode* dirNode = sm->GetRootNode()->CreateChild();
    dirNode->AttachObject(dirlight);
    dirlight->SetColor(DiColor());
    dirlight->SetDirection(DiVec3(0, -0.3f, -0.4).normalisedCopy());
    
    DiBase::Driver->GetMainRenderWindow()->GetSceneCanvas()->SetClearColor(DiColor(0.5f,0.5f,0.5f));
    
    DiCullNode* nd1 = sm->GetRootNode()->CreateChild();
    k2md2 = make_shared<DiK2Model>("buildings/legion/well/model.mdf");
    //k2md2 = make_shared<DiK2Model>("heroes/javaras/model.mdf");
    nd1->AttachObject(k2md2);
    nd1->Translate(50, -50, 0);
    k2md2->GetAnimation()->Play("idle");

    DiCullNode* nd0 = sm->GetRootNode()->CreateChild();
    k2md = make_shared<DiK2Model>("heroes/aluna/model.mdf");
    // nd0->AttachObject(k2md);
    // nd0->Translate(-50, -50, 0);
    // k2md->GetAnimation()->Play("idle");

    DemiDemo::GetApp()->GetInputManager()->registerKeyReleaseEvent("changeClip",
          [&](const OIS::KeyEvent& e){
              switch (e.key)
              {
                  case OIS::KC_1:
                      k2md->GetAnimation()->Play(K2PrefabClip::ANIM_IDLE);
                      k2md2->GetAnimation()->Play(K2PrefabClip::ANIM_IDLE);
                      break;
                  case OIS::KC_2:
                      k2md->GetAnimation()->Play(K2PrefabClip::ANIM_WALK);
                      k2md2->GetAnimation()->Play(K2PrefabClip::ANIM_WALK);
                      break;
              }
          });
}

void UpdateScene()
{
}

int main(int argc, char *argv[])
{
    DemiDemo app( DemoConfig("Demi Sample: Hon Model Animation"));
	app.SetInitCallback(InitScene);
	app.SetUpdateCallback(UpdateScene);
    app.SetShutdownCallback([&](){
        k2md.reset();
        k2md2.reset();
        DiPlugin::UnloadPlugin("DiK2");
    });
	app.Open();
    
	return 0;
}