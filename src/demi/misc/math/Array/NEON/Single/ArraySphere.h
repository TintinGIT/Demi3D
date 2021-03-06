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

#ifndef __NEON_ArraySphere_H__
#define __NEON_ArraySphere_H__

#ifndef __ArraySphere_H__
    #error "Don't include this file directly. include Math/Array/ArraySphere.h"
#endif

#include "DiSphere.h"

#include "Math/Array/Mathlib.h"
#include "Math/Array/ArrayVector3.h"

namespace Demi
{

    /** Cache-friendly array of DiSphere represented as a SoA array.
        @remarks
            ArraySphere is a SIMD & cache-friendly version of DiSphere.
            @See ArrayVector3 for more information.
        @par
            Extracting one sphere needs 64 bytes, which is within
            the 64 byte size of common cache lines.
            Architectures where the cache line == 32 bytes may want to
            set ARRAY_PACKED_REALS = 2 depending on their needs
    */
    class DI_MISC_API ArraySphere
    {
    public:
        ArrayFloat           mRadius;
        ArrayVector3        mCenter;

        ArraySphere()
        {
        }

        ArraySphere( const ArrayFloat &radius, const ArrayVector3 &center ) :
                    mRadius( radius ),
                    mCenter( center )
        {
        }

        void getAsSphere( DiSphere &out, size_t index ) const
        {
            //Be careful of not writing to these regions or else strict aliasing rule gets broken!!!
            const float *aliasedRadius = reinterpret_cast<const float*>( &mRadius );

            DiVec3 center;
            mCenter.getAsVector3( center, index );
            out.setCenter( center );
            out.setRadius( *aliasedRadius );
        }

        /// Prefer using @see getAsSphere() because this function may have more
        /// overhead (the other one is faster)
        DiSphere getAsSphere( size_t index ) const
        {
            DiSphere retVal;
            getAsSphere( retVal, index );
            return retVal;
        }

        void setFromSphere( const DiSphere &sphere, size_t index )
        {
            float *aliasedRadius = reinterpret_cast<float*>( &mRadius );
            aliasedRadius[index] = sphere.getRadius();
            mCenter.setFromVector3( sphere.getCenter(), index );
        }

        /// Sets all packed spheres to the same value as the scalar input sphere
        void setAll( const DiSphere &sphere )
        {
            const float fRadius  = sphere.getRadius();
            const DiVec3 &center = sphere.getCenter();
            mRadius = vdupq_n_f32( fRadius );
            mCenter.mChunkBase[0] = vdupq_n_f32( center.x );
            mCenter.mChunkBase[1] = vdupq_n_f32( center.y );
            mCenter.mChunkBase[2] = vdupq_n_f32( center.z );
        }

        /// @copydoc DiSphere::intersects()
        inline ArrayFloat intersects( const ArraySphere &s ) const;

        /// @copydoc DiSphere::intersects()
        inline ArrayFloat intersects( const ArrayAabb &aabb ) const;

        /// @copydoc DiSphere::intersects()
        inline ArrayFloat intersects( const ArrayVector3 &v ) const;
    };
    /** @} */
    /** @} */

}

#include "ArraySphere.inl"

#endif
