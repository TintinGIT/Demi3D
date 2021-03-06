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
#include "EditorManager.h"
#include "HonFxerApp.h"
#include "MainPaneControl.h"
#include "RenderWindowControl.h"
#include "ParticleSystemObj.h"
#include "ParticleElementObj.h"
#include "PointEmitterObj.h"
#include "CircleEmitterObj.h"
#include "LineEmitterObj.h"
#include "BoxEmitterObj.h"
#include "EffectManager.h"
#include "Grid.h"
#include "ColorControllerObj.h"
#include "VortexControllerObj.h"
#include "GravityControllerObj.h"
#include "TextureRotatorControllerObj.h"
#include "GeometryRotatorControllerObj.h"
#include "RandomiserControllerObj.h"
#include "JetControllerObj.h"
#include "ColliderControllerObj.h"
#include "ForceControllerObj.h"
#include "ScaleControllerObj.h"
#include "K2Configs.h"
#include "ParticleSystem.h"
#include "ParticleElement.h"
#include "ParticleEmitter.h"
#include "ParticleController.h"
#include "CommandManager.h"
#include "TokensParser.h"
#include "RenderWindow.h"
#include "Window.h"
#include "PathLib.h"
#include "MessageBox.h"
#include "AssetManager.h"
#include "RefModelObj.h"
#include "XMLFile.h"
#include "XMLElement.h"
#include "PropertyItem.h"

namespace Demi
{
    DiEditorManager* DiEditorManager::sEditorMgr = nullptr;
    
    DiEditorManager::DiEditorManager()
    {
        sEditorMgr = this;
        
        InitFactories();
        InitCommands();

        mRootObject = CreateEditorObject("Base");
        mRootObject->OnCreate();
        mRootObject->OnCreateUI();

        DiMaterialPtr dbgHelperMat = DiMaterial::QuickCreate("basic_v", "basic_p", SHADER_FLAG_USE_COLOR);
        dbgHelperMat->SetDepthCheck(false);
        mGridPlane = make_shared<DiGridPlane>(30, 10, DiColor(0.1f, 0.1f, 0.1f), DiColor(0.5f, 0.5f, 0.5f));
        mGridPlane->SetMaterial(dbgHelperMat);
        Driver->GetSceneManager()->GetRootNode()->AttachObject(mGridPlane);
        
        CommandManager::getInstance().registerCommand("Command_ToolPlay", MyGUI::newDelegate(this, &DiEditorManager::Command_ToolPlay));
        CommandManager::getInstance().registerCommand("Command_ToolPause", MyGUI::newDelegate(this, &DiEditorManager::Command_ToolPause));
        CommandManager::getInstance().registerCommand("Command_ToolStop", MyGUI::newDelegate(this, &DiEditorManager::Command_ToolStop));
        CommandManager::getInstance().registerCommand("Command_ToolNewFile", MyGUI::newDelegate(this, &DiEditorManager::Command_ToolNew));
        CommandManager::getInstance().registerCommand("Command_ToolSaveFile", MyGUI::newDelegate(this, &DiEditorManager::Command_ToolSave));
        CommandManager::getInstance().registerCommand("Command_ToolOpenFile", MyGUI::newDelegate(this, &DiEditorManager::Command_ToolOpen));
        CommandManager::getInstance().registerCommand("Command_ToolReset", MyGUI::newDelegate(this, &DiEditorManager::Command_ToolReset));
        CommandManager::getInstance().registerCommand("Command_ToolSelect", MyGUI::newDelegate(this, &DiEditorManager::Command_ToolSelect));
        CommandManager::getInstance().registerCommand("Command_ToolMove", MyGUI::newDelegate(this, &DiEditorManager::Command_ToolMove));
        CommandManager::getInstance().registerCommand("Command_ToolRotate", MyGUI::newDelegate(this, &DiEditorManager::Command_ToolRotate));
        CommandManager::getInstance().registerCommand("Command_ToolScale", MyGUI::newDelegate(this, &DiEditorManager::Command_ToolScale));
    }

    DiEditorManager::~DiEditorManager()
    {
        SAFE_DELETE(mRootObject);
        
        sEditorMgr = nullptr;
    }
    
    void DiEditorManager::Command_ToolPlay(const MyGUI::UString& _commandName, bool& _result)
    {
        if (mSelectingFx)
        {
            if(mSelectingFx->GetState() == DiParticleSystem::PSS_PAUSED)
            {
                mSelectingFx->Resume();
            }
            else if(mSelectingFx->GetState() == DiParticleSystem::PSS_STOPPED)
            {
                mSelectingFx->Start();
            }
            else
            {
                mSelectingFx->Stop();
                mSelectingFx->Start();
            }
        }
        _result = true;
    }
    
    void DiEditorManager::Command_ToolPause(const MyGUI::UString& _commandName, bool& _result)
    {
        if (mSelectingFx)
        {
            mSelectingFx->Pause();
        }
        _result = true;
    }
    
    void DiEditorManager::Command_ToolStop(const MyGUI::UString& _commandName, bool& _result)
    {
        if (mSelectingFx)
        {
            mSelectingFx->Stop();
        }
        _result = true;
    }
    
    void DiEditorManager::Command_ToolNew(const MyGUI::UString& _commandName, bool& _result)
    {
        DiString msg;
        
        if (!mFxFileName.empty())
            msg = "Do you want to keep the new effects?";
        else
            msg = "Do you want to save the effects?";
        
        auto msgBox = MyGUI::Message::createMessageBox("Message", "New",
                                                    msg.c_str(),
                                                    MyGUI::MessageBoxStyle::Yes|MyGUI::MessageBoxStyle::No|
                                                    MyGUI::MessageBoxStyle::Cancel|MyGUI::MessageBoxStyle::IconDefault);
        
        msgBox->eventMessageBoxResultLambda = [this](MyGUI::Message*, MyGUI::MessageBoxStyle style){
            if(style == MyGUI::MessageBoxStyle::Yes)
            {
                SaveAll();
                NewFx();
            }
            else if(style == MyGUI::MessageBoxStyle::No)
            {
                NewFx();
            }
        };
        _result = true;
    }
    
    void DiEditorManager::Command_ToolSave(const MyGUI::UString& _commandName, bool& _result)
    {
        SaveAll();
        _result = true;
    }
    
    void DiEditorManager::Command_ToolOpen(const MyGUI::UString& _commandName, bool& _result)
    {
        void* wndHandle = DiBase::Driver->GetMainRenderWindow()->GetWindow()->GetWndHandle();
        
        DiVector<DiString> out;
        DiString filter = "XML effect file(fx)|*.fx|All files (*.*)|*.*";
        DiPathLib::OpenFileDialog(wndHandle, "Effect file", DiAssetManager::GetInstance().GetBasePath(), "", filter, 0, out);
        if (out.size() >= 1)
        {
            OpenFx(out[0]);
        }
        _result = true;
    }
    
    void DiEditorManager::Command_ToolReset(const MyGUI::UString& _commandName, bool& _result)
    {
        auto msg = MyGUI::Message::createMessageBox("Message", "Reset",
                                                    "Do you really want to reset?",
                                                    MyGUI::MessageBoxStyle::Yes|MyGUI::MessageBoxStyle::No|
                                                    MyGUI::MessageBoxStyle::IconDefault);
        
        msg->eventMessageBoxResultLambda = [this](MyGUI::Message*, MyGUI::MessageBoxStyle style){
            if(style == MyGUI::MessageBoxStyle::Yes)
            {
                Reset();
            }
        };
        _result = true;
    }
    
    void DiEditorManager::Command_ToolSelect(const MyGUI::UString& _commandName, bool& _result)
    {
        if(!HonFxerApp::GetFxApp()->GetMainPane()->GetWorkspaceControl()->GetRenderWndControl()->canvasFocus())
            return;
        
        SetGizmoMode(DiTransGizmo::GIZMO_SELECT);
        _result = true;
    }
    
    void DiEditorManager::Command_ToolRotate(const MyGUI::UString& _commandName, bool& _result)
    {
        if(!HonFxerApp::GetFxApp()->GetMainPane()->GetWorkspaceControl()->GetRenderWndControl()->canvasFocus())
            return;
        
        SetGizmoMode(DiTransGizmo::GIZMO_ROTATE);
        _result = true;
    }
    
    void DiEditorManager::Command_ToolScale(const MyGUI::UString& _commandName, bool& _result)
    {
        if(!HonFxerApp::GetFxApp()->GetMainPane()->GetWorkspaceControl()->GetRenderWndControl()->canvasFocus())
            return;
        
        SetGizmoMode(DiTransGizmo::GIZMO_SCALE);
        _result = true;
    }
    
    void DiEditorManager::Command_ToolMove(const MyGUI::UString& _commandName, bool& _result)
    {
        if(!HonFxerApp::GetFxApp()->GetMainPane()->GetWorkspaceControl()->GetRenderWndControl()->canvasFocus())
            return;
        
        SetGizmoMode(DiTransGizmo::GIZMO_MOVE);
        _result = true;
    }
    
    void DiEditorManager::SetWorldSpaceGizmo(bool val)
    {
        mWorldSpaceGizmoOrientation = val;
    }
    
    void DiEditorManager::SetGizmoMode(DiTransGizmo::GizmoMode mode)
    {
        mGlobalGizmoMode = mode;
        if(mCurrentSel && mCurrentSel->GetGizmo())
        {
            mCurrentSel->GetGizmo()->SetGizmoMode(mode);
        }
    }
    
    void DiEditorManager::NewFx()
    {
        Reset();
        
        auto psObj = mRootObject->_CreateChild("ParticleSystem");
        psObj->_CreateChild("ParticleElement");
    }
    
    void DiEditorManager::OpenFx(const DiString& fxFileName)
    {
        DiString base = fxFileName.ExtractFileName();
        if(!DiAssetManager::GetInstance().HasArchive(base))
        {
            DiString message;
            message.Format("Cannot find the effect file(%s) in our media folders!", base.c_str());
            MyGUI::Message::createMessageBox("Message", "Error", message.c_str(),
                                             MyGUI::MessageBoxStyle::Ok|MyGUI::MessageBoxStyle::IconError);
            
            return;
        }
        
        DiFxTokensParser parser;
        
        auto stream = DiAssetManager::GetInstance().OpenArchive(base);
        shared_ptr<DiXMLFile> xmlfile(DI_NEW DiXMLFile());
        xmlfile->Load(stream->GetAsString());
        DiXMLElement root = xmlfile->GetRoot();
        
        if (!root.CheckName("Effects"))
        {
            DI_WARNING("Bad effect file: %s", base.c_str());
            return;
        }
        
        auto child = root.GetChild();
        while (child)
        {
            if (child.CheckName("ParticleSystem"))
            {
                auto ps = parser.ReadSystem(child);
                LoadParticleSystem(ps);
            }
            else if(child.CheckName("ReferenceModel"))
            {
                LoadRefModel(child);
            }
            
            child = child.GetNext();
        }
        
        SetCurrentFileName(fxFileName);
    }
    
    void DiEditorManager::Reset()
    {
        mRootObject->ClearChildren();
        SetCurrentFileName("");
    }
    
    void DiEditorManager::SetCurrentFileName(const DiString& name)
    {
        mFxFileName = name;
        
        DiString title = "Hon Fxer";
        if(!name.empty())
            title.Format("Hon Fxer - %s",name.ExtractFileName().c_str());
        
        DiBase::Driver->GetMainRenderWindow()->GetWindow()->SetTitle(title.c_str());
    }
    
    void DiEditorManager::SaveAll(const DiString& fxFileName)
    {
        DiFxTokensParser parser;
        shared_ptr<DiXMLFile> xmlfile(new DiXMLFile());
        DiXMLElement root = xmlfile->CreateRoot("Effects");
        
        mRootObject->TraversalChildren([&](size_t id, DiBaseEditorObj* obj){
            if(obj->GetType() == "ParticleSystem")
            {
                auto psObj = dynamic_cast<DiParticleSystemObj*>(obj);
                DiParticleSystemPtr ps = psObj->GetParticleSystem();

                auto nd = root.CreateChild("ParticleSystem");
                parser.WriteSystem(ps, nd);
            }
            else if(obj->GetType() == "ReferenceModel")
            {
                auto refmdObj = dynamic_cast<DiRefModelObj*>(obj);
                refmdObj->Save(root);
            }
        });
        
        xmlfile->Save(fxFileName);
        SetCurrentFileName(fxFileName);
    }
    
    void DiEditorManager::SaveAll()
    {
        if (!mFxFileName.empty())
        {
            SaveAll(mFxFileName);
        }
        else
        {
            void* wndHandle = DiBase::Driver->GetMainRenderWindow()->GetWindow()->GetWndHandle();
            
            DiVector<DiString> out;
            DiString filter = "XML effect file(fx)|*.fx|All files (*.*)|*.*";
            DiPathLib::SaveFileDialog(wndHandle, "Effect file", DiPathLib::GetApplicationPath(), "", filter, 0, out);
            if (out.size() >= 1)
            {
                SaveAll(out[0]);
            }
        }
    }
    
    DiBaseEditorObj* DiEditorManager::LoadRefModel(const DiXMLElement& node)
    {
        auto psObj = mRootObject->_CreateChildFrom("ReferenceModel", DiAny(node));
        return psObj;
    }
    
    DiBaseEditorObj* DiEditorManager::LoadParticleSystem(DiParticleSystemPtr ps)
    {
        DI_ASSERT(ps);
        
        auto psObj = mRootObject->_CreateChildFrom("ParticleSystem", DiAny(ps));
        
        size_t numElements = ps->GetNumElements();
        for(size_t i = 0; i < numElements; ++i)
        {
            DiParticleElement* element = ps->GetElement(i);
            auto elementObj = psObj->_CreateChildFrom("ParticleElement", DiAny(element));
            
            // emitters
            size_t numEmits = element->GetNumEmitters();
            for(size_t e = 0; e < numEmits; ++e)
            {
                DiParticleEmitter* emitter = element->GetEmitter(e);
                auto type = emitter->GetEmitterType() + "Emitter";
                elementObj->_CreateChildFrom(type, DiAny(emitter));
            }
            
            // controllers
            size_t numCtrls = element->GetNumControllers();
            for(size_t c = 0; c < numCtrls; ++c)
            {
                DiParticleController* ctrl = element->GetController(c);
                auto type = ctrl->GetControllerType() + "Controller";
                elementObj->_CreateChildFrom(type, DiAny(ctrl));
            }
        }
        
        return psObj;
    }

    DiBaseEditorObj* DiEditorManager::CreateEditorObject(const DiString& type)
    {
        DiBaseEditorObj* ret = nullptr;
        auto it = mObjFactories.find(type);
        if (it != mObjFactories.end())
        {
            ret = it->second();
        }
        else
        {
            DI_WARNING("Cannot create the object [type = %s]", type.c_str());
        }
        
        mLastCreatedObject = ret;
        return ret;
    }

    void DiEditorManager::DeleteEditorObject(DiBaseEditorObj* obj)
    {
        if(mLastCreatedObject == obj)
            mLastCreatedObject = nullptr;
        
        if(obj->GetType() == "ParticleSystem")
        {
            if(dynamic_cast<DiParticleSystemObj*>(obj)->GetParticleSystem() == mSelectingFx)
            {
                if(mSelectingFx)
                {
                    DI_DEBUG("Change selecting ps from [name = %s] to nullptr", mSelectingFx->GetName().c_str());
                }
                
                mSelectingFx = nullptr;
            }
        }
        
        bool triggerChangemodel = obj->GetType() == "ReferenceModel";
            
        DI_ASSERT(obj);
        obj->Release();
        SAFE_DELETE(obj);
        
        mCurrentSel = nullptr;
        
        if(triggerChangemodel)
        {
            DiEditorManager::Get()->TriggerEvent("RefModel");
        }
    }
    
    void DiEditorManager::SetCurrentSelection(DiBaseEditorObj* sel)
    {
        if (mCurrentSel != sel)
        {
            mCurrentSel = sel;

            SetGizmoMode(mGlobalGizmoMode);
            
            auto psParent = mCurrentSel->LookForParent("ParticleSystem");
            if(!psParent)
                return;
            
            auto psObj = dynamic_cast<DiParticleSystemObj*>(psParent);
            
            mSelectingFx = psObj->GetParticleSystem();
            DI_DEBUG("Change selecting ps to [name = %s]", mSelectingFx->GetName().c_str());
        }
    }

    void DiEditorManager::InitFactories()
    {
        mObjFactories["Base"]                      = [](){return DI_NEW DiBaseEditorObj(); };
        mObjFactories["ParticleSystem"]            = [](){return DI_NEW DiParticleSystemObj(); };
        mObjFactories["ParticleElement"]           = [](){return DI_NEW DiParticleElementObj(); };
        mObjFactories["PointEmitter"]              = [](){return DI_NEW DiPointEmitterObj(); };
        mObjFactories["BoxEmitter"]                = [](){return DI_NEW DiBoxEmitterObj(); };
        mObjFactories["CircleEmitter"]             = [](){return DI_NEW DiCircleEmitterObj(); };
        mObjFactories["LineEmitter"]               = [](){return DI_NEW DiLineEmitterObj(); };

        mObjFactories["ColorController"]           = [](){return DI_NEW DiColorControllerObj(); };
        mObjFactories["ScaleController"]           = [](){return DI_NEW DiScaleControllerObj(); };
        mObjFactories["VortexController"]          = [](){return DI_NEW DiVortexControllerObj(); };
        mObjFactories["JetController"]             = [](){return DI_NEW DiJetControllerObj(); };
        mObjFactories["GravityController"]         = [](){return DI_NEW DiGravityControllerObj(); };
        mObjFactories["RandomiserController"]      = [](){return DI_NEW DiRandomiserControllerObj(); };
        mObjFactories["GeometryRotatorController"] = [](){return DI_NEW DiGeometryRotatorControllerObj(); };
        mObjFactories["TextureRotatorController"]  = [](){return DI_NEW DiTextureRotatorControllerObj(); };
        mObjFactories["LinearForceController"]     = [](){return DI_NEW DiLinearForceControllerObj(); };
        mObjFactories["SineForceController"]       = [](){return DI_NEW DiSineForceControllerObj(); };
        mObjFactories["BoxColliderController"]     = [](){return DI_NEW DiBoxColliderControllerObj(); };
        mObjFactories["SphereColliderController"]  = [](){return DI_NEW DiSphereColliderControllerObj(); };
        mObjFactories["PlaneColliderController"]   = [](){return DI_NEW DiPlaneColliderControllerObj(); };

        mObjFactories["ReferenceModel"]            = [](){return DI_NEW DiRefModelObj(); };
    }

    DiString DiEditorManager::GenerateSystemName()
    {
        static int id = 0;
        DiString ret;
        ret.Format("ParticleSystem_%d", id++);
        return ret;
    }
    
    DiString DiEditorManager::GenerateRefModelName()
    {
        static int id = 0;
        DiString ret;
        ret.Format("Model_%d", id++);
        return ret;
    }

    DiString DiEditorManager::GenerateElementName()
    {
        static int id = 0;
        DiString ret;
        ret.Format("Element_%d", id++);
        return ret;
    }

    DiString DiEditorManager::GenerateEmitterName(const DiString& type)
    {
        static int id = 0;
        DiString ret;
        ret.Format("%s_%d", type.c_str(), id++);
        return ret;
    }

    DiString DiEditorManager::GenerateControllerName(const DiString& type)
    {
        static int id = 0;
        DiString ret;
        ret.Format("%s_%d", type.c_str(), id++);
        return ret;
    }

    void DiEditorManager::InitCommands()
    {
        CommandMgr->AddCommand("removeObj", [&](Demi::DiCmdArgs* args){
            if (mCurrentSel)
            {
                DeleteEditorObject(mCurrentSel);
                return true;
            }
            else
            {
                DI_WARNING("No object selected, cannot remove!");
                return false;
            }
        });

        CommandMgr->AddCommand("createChild", [&](Demi::DiCmdArgs* args){
            if (mCurrentSel)
            {
                DI_ASSERT(args->GetArgCount() == 2);
                mCurrentSel->_CreateChild(args->GetArg(1));
                return true;
            }
            else
            {
                DI_WARNING("No object selected, cannot create child!");
                return false;
            }
        });
        
        CommandMgr->AddCommand("selectLast", [&](Demi::DiCmdArgs* args){
            mCurrentSel = mLastCreatedObject;
            return true;
        });
    }

    void DiEditorManager::Update()
    {
        DiEffectManager::GetInstance().Update();

        mRootObject->Update(Driver->GetDeltaSecond());
    }
    
    void DiEditorManager::SetK2ResourcePack(const DiString& resPack, const DiString& texturePack)
    {
        if (texturePack.empty())
            DiK2Configs::SetK2ResourcePack(resPack);
        else
            DiK2Configs::SetK2ResourcePack(resPack, texturePack);
    }
    
    void DiEditorManager::RegisterDynEnumItem(DiDynamicEnumPropertyItem* item, const DiString& eventName)
    {
        mDynEnumItems[eventName].insert( item );
    }
    
    void DiEditorManager::UnregisterDynEnumItem(DiDynamicEnumPropertyItem* item, const DiString& eventName)
    {
        auto it = mDynEnumItems.find(eventName);
        if(it != mDynEnumItems.end())
        {
            auto& sets = it->second;
            auto itemIt = sets.find(item);
            if(itemIt != sets.end())
            {
                sets.erase(itemIt);
            }
        }
    }
    
    void DiEditorManager::TriggerEvent(const DiString& eventName)
    {
        auto it = mDynEnumItems.find(eventName);
        if(it != mDynEnumItems.end())
        {
            for(auto& s : it->second)
            {
                s->NotifyDynEnumChanged();
            }
        }
    }
}