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

#ifndef LineEmitterObject_h__
#define LineEmitterObject_h__

#include "FxerPrerequisites.h"
#include "EmitterBaseObj.h"

namespace Demi 
{
    class DiLineEmitterObj : public DiEmitterBaseObj
    {
    public:

        DiLineEmitterObj();

        virtual              ~DiLineEmitterObj();

    public:

        virtual DiString     GetType() override { return "LineEmitter"; }

        virtual DiString     GetEmitterType() override { return "Line"; }

        DiLineEmitter*       GetLineEmitter();
        
        void                 InitPropertyTable() override;
    };
}

#endif
