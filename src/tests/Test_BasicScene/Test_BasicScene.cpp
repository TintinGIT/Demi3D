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

//DiAnimModelPtr am;
//DiDebugHelperPtr debugger;

void CreateModels()
{
    DiSceneManager* sm = DiBase::Driver->GetSceneManager();
    const int size = 5;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            for (int k = 0; k < size; k++)
            {
                DiMaterialPtr mat = DiMaterial::QuickCreate("lambert_v", "lambert_p", SHADER_FLAG_SKINNED);
                mat->SetDiffuse(DiColor(1, 1, 1));

                DiString name;
                name.Format("md_%d_%d", i, j);
                DiAnimModelPtr model = make_shared<DiAnimModel>(name, "robot.model", "robot.motion");
                //DiModelPtr model = make_shared<DiModel>(name, "robot.model");
                model->SetMaterial(mat);

                model->SetAutoUpdateAnims(true);
                model->GetClipSet()->GetClipController("Walk")->SetEnabled(true);

                DiCullNode* cullnode = sm->GetRootNode()->CreateChild();
                cullnode->AttachObject(model);
                cullnode->SetPosition(i * 120.0f, j * 120.0f, k * 120.0f);

                //debugger = make_shared<DiDebugHelper>();
                //debugger->AddSkeleton(model->GetSkeleton(), DiColor::Red);
                //DiMaterialPtr m = DiMaterial::QuickCreate("basic_v", "basic_p", SHADER_FLAG_USE_COLOR);
                //debugger->SetMaterial(m);
                //sm->GetRootNode()->AttachObject(debugger);
                //am = model;
            }
        }
    }

    sm->GetCamera()->SetPosition(DiVec3(999.205261f, 966.364136f,1007.95703f));
    sm->GetCamera()->SetOrientation(DiQuat(0.883716464f, -0.272288710f, 0.363790601f, 0.112090491f));
}

void InitScene()
{
    DiSceneManager* sm = DiBase::Driver->GetSceneManager();
    sm->SetAmbientColor(DiColor(0.1f, 0.1f, 0.1f, 0.1f));

    DiDirLightPtr dirlight = make_shared<DiDirLight>();
    sm->AttachObject(dirlight);
    dirlight->SetColor(DiColor());
    dirlight->SetDirection(-DiVec3(1, 1, 2).normalisedCopy());

    CreateModels();
}

void UpdateScene()
{
    //debugger->Clear();
    //debugger->AddSkeleton(am->GetSkeleton(), DiColor::Red);

//     DiSceneManager* sm = DiBase::Driver->GetSceneManager();
//     DiCamera* c = sm->GetCamera();
//     DiVec3 p = c->GetPosition();
//     DiQuat q = c->GetOrientation();
// 
//     int x = 0;
//     x++;
}

int main(int argc, char *argv[])
{
	DemiDemo app(DemoConfig("Demi test: basic scene"));
	app.SetInitCallback(InitScene);
	app.SetUpdateCallback(UpdateScene);
    app.Open(argc, argv);

	return 0;
}