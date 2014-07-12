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
#include "BaseEditorObject.h"
#include "HonFxerApp.h"
#include "EditorManager.h"
#include "ResTreeControl.h"
#include "MainWorkspaceControl.h"
#include "MainPaneControl.h"
#include "PanelView.h"
#include "TransGizmo.h"

namespace Demi
{
    DiBaseEditorObj::DiBaseEditorObj()
    {
    }

    DiBaseEditorObj::~DiBaseEditorObj()
    {
        SAFE_DELETE(mGizmo);
    }
    
    void DiBaseEditorObj::Release()
    {
        if(mParent)
            mParent->RemoveChild(this);
        
        for (auto p : mPropGroups)
            DI_DELETE p;
        mPropGroups.clear();
        
        for (auto ch : mChildren)
            DI_DELETE ch;
        mChildren.clear();
        
        OnDestroyUI();
        OnDestroy();
    }

    void DiBaseEditorObj::OnMenuPopup(MyGUI::PopupMenu* menu, bool multiSelection)
    {
        DiEditorManager::Get()->SetCurrentSelection(this);

        menu->removeAllItems();

        menu->addItem("Create ParticleSystem", MyGUI::MenuItemType::Normal, "createChild ParticleSystem");
        //menu->addItem("Create Reference Model", MyGUI::MenuItemType::Normal, "createChild Model");
    }
    
    DiBaseEditorObj* DiBaseEditorObj::LookForParent(const DiString& type)
    {
        if(GetType() == type)
        {
            return this;
        }
        
        auto parent = GetParent();
        while(parent)
        {
            if(parent->GetType() == type)
                return parent;
            parent = parent->GetParent();
        }
        return nullptr;
    }
    
    void DiBaseEditorObj::TraversalChildren(const std::function<void(size_t, DiBaseEditorObj*)> func)
    {
        for(size_t i = 0; i < mChildren.size(); ++i)
        {
            func(i, mChildren[i]);
        }
    }

    DiBaseEditorObj* DiBaseEditorObj::_CreateChild(const DiString& type)
    {
        DI_LOG("Creating child object [type = %s]", type.c_str());
        auto ret = DiEditorManager::Get()->CreateEditorObject(type);
        
        ret->mParent = this;
        ret->OnCreate();
        ret->OnCreateUI();
        ret->NotifyTransfromUpdate();

        mChildren.push_back(ret);
        return ret;
    }
    
    DiBaseEditorObj* DiBaseEditorObj::_CreateChildFrom(const DiString& type, const DiAny& param)
    {
        DI_LOG("Creating child object with parameter [type = %s]", type.c_str());
        auto ret = DiEditorManager::Get()->CreateEditorObject(type);
        
        ret->mParent = this;
        ret->OnCreate(param);
        ret->OnCreateUI();
        ret->NotifyTransfromUpdate();
        
        mChildren.push_back(ret);
        return ret;
    }

    void DiBaseEditorObj::RemoveChild(const DiBaseEditorObj* child)
    {
        auto it = std::find_if(mChildren.begin(), mChildren.end(),
            [child](const DiBaseEditorObj* o) { return o == child; });
        if (it != mChildren.end())
        {
            mChildren.erase(it);
        }
        else
        {
            DI_WARNING("Cannot remove the editor object");
        }
    }
    
    void DiBaseEditorObj::ClearChildren()
    {
        for(auto& c: mChildren)
        {
            DiEditorManager::Get()->DeleteEditorObject(c);
        }
        mChildren.clear();
    }

    void DiBaseEditorObj::OnCreateUI()
    {
        // tree control node
        auto resTree = HonFxerApp::GetFxApp()->GetMainPane()->GetWorkspaceControl()->GetResourceTree();
        auto treeCtrl = resTree->GetTreeCtrl();

        auto root = !mParent ? treeCtrl->getRoot() : mParent->GetUINode();
        mUINode = new MyGUI::TreeControl::Node(GetUICaption().c_str(), GetUINodeType().c_str());
        mUINode->setData((DiBaseEditorObj*)this);
        root->add(mUINode);
        
        // property table
        InitPropertyTable();
    }
    
    void DiBaseEditorObj::OnSelectLost()
    {
        mGizmo->Show(false);
    }
    
    void DiBaseEditorObj::OnSelect()
    {
        auto lastSelection = DiEditorManager::Get()->GetCurrentSelection();
        
        if( lastSelection == this )
            return;
        
        if(lastSelection)
            lastSelection->SetPropertyTableVisible(false);
        
        DiEditorManager::Get()->SetCurrentSelection(this);
        
        SetPropertyTableVisible(true);
        
        mGizmo->Show(true);
    }

    void DiBaseEditorObj::OnDestroyUI()
    {
        mUINode->getParent()->remove(mUINode, true);
        
        DestroyPropertyTable();
    }
    
    void DiBaseEditorObj::SetPropertyTableVisible(bool visible)
    {
        for (auto& p : mPropGroups)
        {
            if(p->mUIGroup)
            {
                p->mUIGroup->setVisible(visible);
            }
        }
    }

    void DiBaseEditorObj::Update(float dt)
    {
        mGizmo->Update();
        
        for (auto ch : mChildren)
        {
            ch->Update(dt);
        }
    }
    
    void DiBaseEditorObj::DestroyPropertyTable()
    {
        for (auto g : mPropGroups)
        {
            DI_DELETE g;
        }
        mPropGroups.clear();
    }
    
    void DiBaseEditorObj::OnCreate()
    {
        DiSceneManager* sm = DiBase::Driver->GetSceneManager();
        mSceneNode = sm->GetRootNode()->CreateChild();
        
        mGizmo = DI_NEW DiTransGizmo(this);
        mGizmo->Show(false);
    }
    
    void DiBaseEditorObj::NotifyTransfromUpdate()
    {
        mSceneNode->SetPosition(GetPosition());
    }
    
    void DiBaseEditorObj::NotifyMousePressed(int _left, int _top, MyGUI::MouseButton _id)
    {
        if(mGizmo->IsActive())
            mGizmo->OnMouseDown(_left, _top, _id);
    }
    
    void DiBaseEditorObj::NotifyMouseReleased(int _left, int _top, MyGUI::MouseButton _id)
    {
        if(mGizmo->IsActive())
            mGizmo->OnMouseUp(_left, _top, _id);
    }
    
    void DiBaseEditorObj::NotifyMouseMove(int _left, int _top)
    {
        if(mGizmo->IsActive())
            mGizmo->OnMouseMove(_left, _top);
    }
    
    void DiBaseEditorObj::NotifyMouseDrag(int _left, int _top, MyGUI::MouseButton _id)
    {
        if(mGizmo->IsActive())
            mGizmo->OnMouseDrag(_left, _top, _id);
    }
}