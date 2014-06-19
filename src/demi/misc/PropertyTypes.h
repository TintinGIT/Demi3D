
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

#ifndef PropertyTypes_h__
#define PropertyTypes_h__

#include "Property.h"

namespace Demi
{
    typedef DiProperty<DiString           , PROPERTY_STRING>   DiStringProperty;
    typedef DiProperty<float              , PROPERTY_FLOAT>    DiFloatProperty;
    typedef DiProperty<int                , PROPERTY_INT>      DiIntProperty;
    typedef DiProperty<bool               , PROPERTY_BOOL>     DiBoolProperty;
    typedef DiProperty<DiVec2             , PROPERTY_VEC2>     DiVec2Property;
    typedef DiProperty<DiVec3             , PROPERTY_VEC3>     DiVec3Property;
    typedef DiProperty<DiVec4             , PROPERTY_VEC4>     DiVec4Property;
    typedef DiProperty<DiQuat             , PROPERTY_QUAT>     DiQuatProperty;
    typedef DiProperty<DiColor            , PROPERTY_COLOR>    DiColorProperty;
    typedef DiProperty<DiDynamicAttribute*, PROPERTY_DYN_ATTR> DiDynProperty;
}

#endif // PropertyTypes_h__