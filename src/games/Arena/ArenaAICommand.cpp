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

#include "ArenaPch.h"
#include "ArenaAICommand.h"
#include "ArenaNPCEntity.h"

namespace Demi
{
    ArAICommand::ArAICommand(ArEntity* entity)
        : mValid(true)
        , mType(ENUM_AI_TYPE_EMPTY)
    {
        mEntity = dynamic_cast<ArNPCEntity*>(entity);
    }

}