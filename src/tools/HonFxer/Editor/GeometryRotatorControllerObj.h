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

#ifndef GeometryRotatorControllerObject_h__
#define GeometryRotatorControllerObject_h__

#include "K2Prerequisites.h"
#include "FxerPrerequisites.h"
#include "BaseEditorObject.h"
#include "ControllerBaseObj.h"
#include "PropertyTypes.h"

namespace Demi
{
    class DiGeometryRotatorControllerObj final : public DiControllerBaseObj
    {
    public:
        
        DiGeometryRotatorControllerObj();
        
        ~DiGeometryRotatorControllerObj();
        
    public:
        
        DiString    GetType() override { return "GeometryRotatorController"; }
    
        DiString    GetControllerType() { return "GeometryRotator"; }
    
        DiString    GetUICaption() override;
    
        DiString    GetUINodeType() { return "File"; }
    
    public:
    
        DiGeometryRotatorController*  GetController();
    
    protected:
    
        void        InitPropertyTable() override;
    };
}

#endif
