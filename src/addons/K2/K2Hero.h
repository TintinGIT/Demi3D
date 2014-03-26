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

#ifndef DiK2HeroEntity_h__
#define DiK2HeroEntity_h__

#include "K2Prerequisites.h"
#include "K2Model.h"

namespace Demi
{
    class DEMI_K2_API DiK2HeroEntity : public DiBase
    {
    public:

        DiK2HeroEntity(DiK2Game* game);

        ~DiK2HeroEntity();

    public:

        void            Release();

        DiK2ModelPtr    LoadModel(const DiString& mdf);

        DiCullNode*     GetNode() { return mNode; }

    private:

        DiK2ModelPtr    mModel;

        DiCullNode*     mNode;

        DiK2Game*       mGame;
    };
}

#endif
