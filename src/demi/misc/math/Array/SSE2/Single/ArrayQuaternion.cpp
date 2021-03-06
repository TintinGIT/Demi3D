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

/// This file is adapted from Ogre 2.0 (unstable version)

#include "MiscPch.h"

#include "Math/Array/ArrayQuaternion.h"

#if __DEMI_HAVE_SSE

namespace Demi
{
    const ArrayQuaternion ArrayQuaternion::ZERO( _mm_setzero_ps(), _mm_setzero_ps(), _mm_setzero_ps(), _mm_setzero_ps() );
    const ArrayQuaternion ArrayQuaternion::IDENTITY( _mm_set_ps1( 1.0f ), _mm_setzero_ps(), _mm_setzero_ps(), _mm_setzero_ps() );
}

#endif
