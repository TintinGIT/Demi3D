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

/// This file is adapted from Ogre 2.0 (working version)

#include "MiscPch.h"

#include "math/Array/ArrayVector3.h"
#include "math/Array/ArrayAabb.h"

namespace Demi
{
    const ArrayAabb ArrayAabb::BOX_INFINITE(ArrayVector3(0.0f, 0.0f, 0.0f),
                                ArrayVector3( std::numeric_limits<float>::infinity(),
                                                std::numeric_limits<float>::infinity(),
                                                std::numeric_limits<float>::infinity() ) );
    const ArrayAabb ArrayAabb::BOX_ZERO(ArrayVector3(0.0f, 0.0f, 0.0f), ArrayVector3(0.0f, 0.0f, 0.0f));
}
