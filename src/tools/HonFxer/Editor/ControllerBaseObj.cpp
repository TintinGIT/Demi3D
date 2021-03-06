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

#include "FxerPch.h"
#include "ControllerBaseObj.h"
#include "HonFxerApp.h"
#include "EditorManager.h"
#include "EffectManager.h"
#include "ParticleElementObj.h"
#include "ParticleElement.h"
#include "ParticleController.h"

namespace Demi
{
    DiControllerBaseObj::DiControllerBaseObj()
    {
    }

    DiControllerBaseObj::~DiControllerBaseObj()
    {
    }

    void DiControllerBaseObj::OnMenuPopup(MyGUI::PopupMenu* menu, bool multiSelection)
    {
        DiEditorManager::Get()->SetCurrentSelection(this);

        menu->removeAllItems();
        menu->addItem("Delete", MyGUI::MenuItemType::Normal, "removeObj");
    }

    void DiControllerBaseObj::OnDestroy()
    {
        mController->SetParentElement(nullptr);
        auto parent = dynamic_cast<DiParticleElementObj*>(GetParent());
        parent->GetParticleElement()->DestroyController(mController);
    }

    void DiControllerBaseObj::OnSelect()
    {
        DiBaseEditorObj::OnSelect();
    }

    DiString DiControllerBaseObj::GetUICaption()
    {
        return mController->GetName();
    }

    void DiControllerBaseObj::OnCreate()
    {
        DiBaseEditorObj::OnCreate();
        
        auto parent = dynamic_cast<DiParticleElementObj*>(GetParent());
        mController = parent->GetParticleElement()->CreateController(GetControllerType());
        mController->SetName(DiEditorManager::Get()->GenerateControllerName(GetControllerType()));
    }
    
    void DiControllerBaseObj::OnCreate(const DiAny& param)
    {
        DiBaseEditorObj::OnCreate();
        
        mController = any_cast<DiParticleController*>(param);
        
        DI_ASSERT(mController);
        if(mController->GetName().empty())
        {
            mController->SetName(DiEditorManager::Get()->GenerateControllerName(GetControllerType()));
        }
    }

    void DiControllerBaseObj::InitPropertyTable()
    {
        DiPropertyGroup* g = DI_NEW DiPropertyGroup("Controller");
     
        g->AddProperty("Name"            , DI_NEW DiStringProperty([&]{ return mController->GetName(); },
                                                                   [&](DiString& val){ mController->SetName(val); }));
        
        g->AddProperty("Enable"          , DI_NEW DiBoolProperty(  [&]{ return mController->IsEnabled(); },
                                                                   [&](bool& val){ mController->SetEnabled(val); }));
        
        auto prop = g->AddProperty("Position", DI_NEW DiVec3Property([&]{ return mController->position; },
                                                                     [&](DiVec3& val){
                                                                         mController->position = val;
                                                                         NotifyTransfromUpdate();
                                                                     }));
        mPositionProp = static_cast<DiVec3Property*>(prop->mProperty);
        
        g->AddProperty("Mass"            , DI_NEW DiFloatProperty( [&]{ return mController->mass; },
                                                                   [&](float& val){ mController->mass = val; }));

        g->CreateUI();

        mPropGroups.push_back(g);
    }
}