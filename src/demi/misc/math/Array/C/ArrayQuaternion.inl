/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2014 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

namespace Demi
{
    // Arithmetic operations
#define DEFINE_OPERATION( leftClass, rightClass, op )\
    inline ArrayQuaternion operator op ( const leftClass &lhs, const rightClass &rhs )\
    {\
        return ArrayQuaternion(\
                lhs.w op rhs.w,\
                lhs.x op rhs.x,\
                lhs.y op rhs.y,\
                lhs.z op rhs.z );\
    }
#define DEFINE_L_OPERATION( leftType, rightClass, op )\
    inline ArrayQuaternion operator op ( const leftType lhs, const rightClass &rhs )\
    {\
        return ArrayQuaternion(\
                lhs op rhs.w,\
                lhs op rhs.x,\
                lhs op rhs.y,\
                lhs op rhs.z );\
    }
#define DEFINE_R_OPERATION( leftClass, rightType, op )\
    inline ArrayQuaternion operator op ( const leftClass &lhs, const rightType rhs )\
    {\
        return ArrayQuaternion(\
                lhs.w op rhs,\
                lhs.x op rhs,\
                lhs.y op rhs,\
                lhs.z op rhs );\
    }

    // Update operations
#define DEFINE_UPDATE_OPERATION( leftClass, op )\
    inline void ArrayQuaternion::operator op ( const leftClass &a )\
    {\
        w = w op a.w;\
        x = x op a.x;\
        y = y op a.y;\
        z = z op a.z;\
    }
#define DEFINE_UPDATE_R_OPERATION( rightType, op )\
    inline void ArrayQuaternion::operator op ( const rightType a )\
    {\
        w = w op a;\
        x = x op a;\
        y = y op a;\
        z = z op a;\
    }

    // + Addition
    DEFINE_OPERATION( ArrayQuaternion, ArrayQuaternion, + );

    // - Subtraction
    DEFINE_OPERATION( ArrayQuaternion, ArrayQuaternion, - );

    // * Multiplication (scalar only)
    DEFINE_L_OPERATION( ArrayFloat, ArrayQuaternion, * );
    DEFINE_R_OPERATION( ArrayQuaternion, ArrayFloat, * );

    // Update operations
    // +=
    DEFINE_UPDATE_OPERATION(            ArrayQuaternion,        += );

    // -=
    DEFINE_UPDATE_OPERATION(            ArrayQuaternion,        -= );

    // *=
    DEFINE_UPDATE_R_OPERATION(          ArrayFloat,          *= );

    // Notes: This operator doesn't get inlined. The generated instruction count is actually high so
    // the compiler seems to be clever in not inlining. There is no gain in doing a "mul()" equivalent
    // like we did with mul( const ArrayQuaternion&, ArrayVector3& ) because we would still need
    // a temporary variable to hold all the operations (we can't overwrite to any heap value
    // since all values are used until the last operation)
    inline ArrayQuaternion operator * ( const ArrayQuaternion &lhs, const ArrayQuaternion &rhs )
    {
        return ArrayQuaternion(
            /* w = (w * rkQ.w - x * rkQ.x) - (y * rkQ.y + z * rkQ.z) */
            ( lhs.w * rhs.w - lhs.x * rhs.x ) -
            ( lhs.y * rhs.y + lhs.z * rhs.z ),
            /* x = (w * rkQ.x + x * rkQ.w) + (y * rkQ.z - z * rkQ.y) */
            ( lhs.w * rhs.x + lhs.x * rhs.w ) +
            ( lhs.y * rhs.z - lhs.z * rhs.y ),
            /* y = (w * rkQ.y + y * rkQ.w) + (z * rkQ.x - x * rkQ.z) */
            ( lhs.w * rhs.y + lhs.y * rhs.w ) +
            ( lhs.z * rhs.x - lhs.x * rhs.z ),
            /* z = (w * rkQ.z + z * rkQ.w) + (x * rkQ.y - y * rkQ.x) */
            ( lhs.w * rhs.z + lhs.z * rhs.w ) +
            ( lhs.x * rhs.y - lhs.y * rhs.x ) );
    }

    inline ArrayQuaternion ArrayQuaternion::Slerp( ArrayFloat fT, const ArrayQuaternion &rkP,
                                                        const ArrayQuaternion &rkQ /*, bool shortestPath*/ )
    {
        ArrayFloat fCos = rkP.Dot( rkQ );
        /* Clamp fCos to [-1; 1] range */
        fCos = Ogre::min( 1.0f, Ogre::max( -1.0f, fCos ) );
        
        /* Invert the rotation for shortest path? */
        /* m = fCos < 0.0f ? -1.0f : 1.0f; */
        ArrayFloat m = MathlibC::Cmov4( -1.0f, 1.0f, fCos < 0 /*&& shortestPath*/ );
        ArrayQuaternion rkT( rkQ.w * m, rkQ.x * m, rkQ.y * m, rkQ.z * m );
        
        ArrayFloat fSin = sqrt( 1.0f - ( fCos * fCos ) );
        
        /* ATan2 in original Quaternion is slightly absurd, because fSin was derived from
           fCos (hence never negative) which makes ACos a better replacement. ACos is much
           faster than ATan2, as long as the input is whithin range [-1; 1], otherwise the generated
           NaNs make it slower (whether clamping the input outweights the benefit is
           arguable). We use ACos4 to avoid implementing ATan2_4.
        */
        ArrayFloat fAngle = MathlibC::ACos4( fCos );

        // mask = Abs( fCos ) < 1-epsilon
        ArrayMaskR mask   = Math::Abs( fCos ) < MathlibC::OneMinusEpsilon;
        ArrayFloat fInvSin = MathlibC::InvNonZero4( fSin );
        ArrayFloat oneSubT = 1.0f - fT;
        // fCoeff1 = Sin( fT * fAngle ) * fInvSin
        ArrayFloat fCoeff0 = MathlibC::Sin4( oneSubT * fAngle ) * fInvSin;
        ArrayFloat fCoeff1 = MathlibC::Sin4( fT * fAngle ) * fInvSin;
        // fCoeff1 = mask ? fCoeff1 : fT; (switch to lerp when rkP & rkQ are too close->fSin=0, or 180°)
        fCoeff0 = MathlibC::CmovRobust( fCoeff0, oneSubT, mask );
        fCoeff1 = MathlibC::CmovRobust( fCoeff1, fT, mask );

        // retVal = fCoeff0 * rkP + fCoeff1 * rkT;
        rkT.w = ( rkP.w * fCoeff0 ) + ( rkT.w * fCoeff1 );
        rkT.x = ( rkP.x * fCoeff0 ) + ( rkT.x * fCoeff1 );
        rkT.y = ( rkP.y * fCoeff0 ) + ( rkT.y * fCoeff1 );
        rkT.z = ( rkP.z * fCoeff0 ) + ( rkT.z * fCoeff1 );

        rkT.normalise();

        return rkT;
    }

    inline ArrayQuaternion ArrayQuaternion::nlerp( ArrayFloat fT, const ArrayQuaternion &rkP,
                                                        const ArrayQuaternion &rkQ )
    {
        ArrayQuaternion retVal(
                ogre_madd( fT, rkQ.w - rkP.w, rkP.w ),
                ogre_madd( fT, rkQ.x - rkP.x, rkP.x ),
                ogre_madd( fT, rkQ.y - rkP.y, rkP.y ),
                ogre_madd( fT, rkQ.z - rkP.z, rkP.z ) );
        retVal.normalise();

        return retVal;
    }
    
    inline ArrayQuaternion ArrayQuaternion::Cmov4( const ArrayQuaternion &arg1,
                                                    const ArrayQuaternion &arg2, ArrayMaskR mask )
    {
        return ArrayQuaternion(
                MathlibC::Cmov4( arg1.w, arg2.w, mask ),
                MathlibC::Cmov4( arg1.x, arg2.x, mask ),
                MathlibC::Cmov4( arg1.y, arg2.y, mask ),
                MathlibC::Cmov4( arg1.z, arg2.z, mask ) );
    }
    
    inline void ArrayQuaternion::mul( const ArrayQuaternion &inQ, ArrayVector3 &inOutVec )
    {
        // nVidia SDK implementation
        ArrayVector3 qVec( inQ.x, inQ.y, inQ.z );

        ArrayVector3 uv     = qVec.crossProduct( inOutVec );
        ArrayVector3 uuv    = qVec.crossProduct( uv );

        // uv = uv * (2.0f * w)
        ArrayFloat w2 = inQ.w + inQ.w;
        uv.mChunkBase[0] = uv.mChunkBase[0] * w2;
        uv.mChunkBase[1] = uv.mChunkBase[1] * w2;
        uv.mChunkBase[2] = uv.mChunkBase[2] * w2;

        // uuv = uuv * 2.0f
        uuv.mChunkBase[0] = uuv.mChunkBase[0] + uuv.mChunkBase[0];
        uuv.mChunkBase[1] = uuv.mChunkBase[1] + uuv.mChunkBase[1];
        uuv.mChunkBase[2] = uuv.mChunkBase[2] + uuv.mChunkBase[2];

        //inOutVec = v + uv + uuv
        inOutVec.mChunkBase[0] = inOutVec.mChunkBase[0] + uv.mChunkBase[0] + uuv.mChunkBase[0];
        inOutVec.mChunkBase[1] = inOutVec.mChunkBase[1] + uv.mChunkBase[1] + uuv.mChunkBase[1];
        inOutVec.mChunkBase[2] = inOutVec.mChunkBase[2] + uv.mChunkBase[2] + uuv.mChunkBase[2];
    }
    
    inline void ArrayQuaternion::FromAngleAxis( const ArrayRadian& rfAngle, const ArrayVector3& rkAxis )
    {
        // assert:  axis[] is unit length
        //
        // The quaternion representing the rotation is
        //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

        ArrayFloat fHalfAngle( rfAngle.valueRadians() * 0.5f );

        ArrayFloat fSin;
        MathlibC::SinCos4( fHalfAngle, fSin, w );

        ArrayFloat * RESTRICT_ALIAS chunkBase = &w;
        const ArrayFloat * RESTRICT_ALIAS rkAxisChunkBase = rkAxis.mChunkBase;

        chunkBase[1] = fSin * rkAxisChunkBase[0]; //x = fSin*rkAxis.x;
        chunkBase[2] = fSin * rkAxisChunkBase[1]; //y = fSin*rkAxis.y;
        chunkBase[3] = fSin * rkAxisChunkBase[2]; //z = fSin*rkAxis.z;
    }
    
    inline void ArrayQuaternion::ToAngleAxis( ArrayRadian &rfAngle, ArrayVector3 &rkAxis ) const
    {
        // The quaternion representing the rotation is
        //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)
        ArrayFloat sqLength = x * x + y * y + z * z;

        ArrayMaskR mask = sqLength > 0;

        //sqLength = sqLength > 0 ? sqLength : 1; so that invSqrt doesn't give NaNs or Infs
        //when 0 (to avoid using CmovRobust just to select the non-nan results)
        sqLength = MathlibC::Cmov4( sqLength, 1.0f, sqLength > MathlibC::FLOAT_MIN );
        ArrayFloat fInvLength = MathlibC::InvSqrtNonZero4( sqLength );

        const ArrayFloat acosW = MathlibC::ACos4( w );
        rfAngle = MathlibC::Cmov4( //sqLength > 0 ? (2 * ACos(w)) : 0
                    acosW + acosW,
                    0, mask );

        rkAxis.mChunkBase[0] = MathlibC::Cmov4(//sqLength > 0 ? (x * fInvLength) : 1
                                                x * fInvLength, 1.0f, mask );
        rkAxis.mChunkBase[1] = MathlibC::Cmov4(//sqLength > 0 ? (y * fInvLength) : 0
                                                y * fInvLength, 0, mask );
        rkAxis.mChunkBase[2] = MathlibC::Cmov4(//sqLength > 0 ? (y * fInvLength) : 0
                                                z * fInvLength, 0, mask );
    }
    
    inline ArrayVector3 ArrayQuaternion::xAxis( void ) const
    {
        ArrayFloat fTy  = y + y;     // 2 * y
        ArrayFloat fTz  = z + z;     // 2 * z
        ArrayFloat fTwy = fTy * w;                   // fTy*w;
        ArrayFloat fTwz = fTz * w;                   // fTz*w;
        ArrayFloat fTxy = fTy * x;                   // fTy*x;
        ArrayFloat fTxz = fTz * x;                   // fTz*x;
        ArrayFloat fTyy = fTy * y;                   // fTy*y;
        ArrayFloat fTzz = fTz * z;                   // fTz*z;

        return ArrayVector3( 1.0f - (fTyy + fTzz), fTxy + fTwz, fTxz - fTwy );
    }
    
    inline ArrayVector3 ArrayQuaternion::yAxis( void ) const
    {
        ArrayFloat fTx  = x + x;                     // 2 * x
        ArrayFloat fTy  = y + y;                     // 2 * y
        ArrayFloat fTz  = z + z;                     // 2 * z
        ArrayFloat fTwx = fTx * w;                   // fTx*w;
        ArrayFloat fTwz = fTz * w;                   // fTz*w;
        ArrayFloat fTxx = fTx * x;                   // fTx*x;
        ArrayFloat fTxy = fTy * x;                   // fTy*x;
        ArrayFloat fTyz = fTz * y;                   // fTz*y;
        ArrayFloat fTzz = fTz * z;                   // fTz*z;

        return ArrayVector3( fTxy - fTwz, 1.0f - (fTxx + fTzz), fTyz + fTwx );
    }
    
    inline ArrayVector3 ArrayQuaternion::zAxis( void ) const
    {
        ArrayFloat fTx  = x + x;                     // 2 * x
        ArrayFloat fTy  = y + y;                     // 2 * y
        ArrayFloat fTz  = z + z;                     // 2 * z
        ArrayFloat fTwx = fTx * w;                   // fTx*w;
        ArrayFloat fTwy = fTy * w;                   // fTy*w;
        ArrayFloat fTxx = fTx * x;                   // fTx*x;
        ArrayFloat fTxz = fTz * x;                   // fTz*x;
        ArrayFloat fTyy = fTy * y;                   // fTy*y;
        ArrayFloat fTyz = fTz * y;                   // fTz*y;

        return ArrayVector3( fTxz + fTwy, fTyz - fTwx, 1.0f - (fTxx + fTyy) );
    }
    
    inline ArrayFloat ArrayQuaternion::Dot( const ArrayQuaternion& rkQ ) const
    {
        return w * rkQ.w + x * rkQ.x + y * rkQ.y + z * rkQ.z;
    }
    
    inline ArrayFloat ArrayQuaternion::Norm( void ) const
    {
        return w * w + x * x + y * y + z * z;
    }
    
    inline void ArrayQuaternion::normalise( void )
    {
        ArrayFloat sqLength = w * w + x * x + y * y + z * z;

        //Convert sqLength's 0s into 1, so that zero vectors remain as zero
        //Denormals are treated as 0 during the check.
        //Note: We could create a mask now and nuke nans after InvSqrt, however
        //generating the nans could impact performance in some architectures
        sqLength = MathlibC::Cmov4( sqLength, 1.0f, sqLength > MathlibC::FLOAT_MIN );
        ArrayFloat invLength = MathlibC::InvSqrtNonZero4( sqLength );
        w = w * invLength;
        x = x * invLength;
        y = y * invLength;
        z = z * invLength;
    }
    
    inline ArrayQuaternion ArrayQuaternion::Inverse( void ) const
    {
        ArrayFloat fNorm = w * w + x * x + y * y + z * z;

        //Will return a zero Quaternion if original is zero length (Quaternion's behavior)
        fNorm = MathlibC::Cmov4( fNorm, 1.0f, fNorm > MathlibC::fEpsilon );
        ArrayFloat invNorm    = MathlibC::Inv4( fNorm );
        ArrayFloat negInvNorm = -invNorm;

        return ArrayQuaternion( w * invNorm, x * negInvNorm, y * negInvNorm, z * negInvNorm );
    }
    
    inline ArrayQuaternion ArrayQuaternion::UnitInverse( void ) const
    {
        return ArrayQuaternion( w, -x, -y, -z );
    }
    
    inline ArrayQuaternion ArrayQuaternion::Exp( void ) const
    {
        // If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
        // exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
        // use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

        ArrayFloat fAngle = sqrt( x * x + y * y + z * z );

        ArrayFloat w, fSin;
        MathlibC::SinCos4( fAngle, fSin, w );

        //coeff = Abs(fSin) >= msEpsilon ? (fSin / fAngle) : 1.0f;
        ArrayFloat coeff = MathlibC::CmovRobust( fSin / fAngle, 1.0f,
                                                Math::Abs( fSin ) >= MathlibC::fEpsilon );
        return ArrayQuaternion( w, x * coeff, y * coeff, z * coeff );
    }
    
    inline ArrayQuaternion ArrayQuaternion::Log( void ) const
    {
        // If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
        // log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
        // sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

        ArrayFloat fAngle    = MathlibC::ACos4( w );
        ArrayFloat fSin      = MathlibC::Sin4( fAngle );

        //mask = Math::Abs(w) < 1.0 && Math::Abs(fSin) >= msEpsilon
        ArrayMaskR mask = Math::Abs( w ) < MathlibC::ONE && Math::Abs( fSin ) >= MathlibC::fEpsilon;

        //coeff = mask ? (fAngle / fSin) : 1.0
        //Unlike Exp(), we can use InvNonZero4 (which is faster) instead of div because we know for
        //sure CMov will copy the 1 instead of the NaN when fSin is close to zero, guarantee we might
        //not have in Exp()
        ArrayFloat coeff = MathlibC::CmovRobust( fAngle * MathlibC::InvNonZero4( fSin ), 1.0f, mask );

        return ArrayQuaternion( 0, x * coeff, y * coeff, z * coeff );
    }
    
    inline ArrayVector3 ArrayQuaternion::operator * ( const ArrayVector3 &v ) const
    {
        // nVidia SDK implementation
        ArrayVector3 qVec( x, y, z );

        ArrayVector3 uv = qVec.crossProduct( v );
        ArrayVector3 uuv= qVec.crossProduct( uv );

        // uv = uv * (2.0f * w)
        ArrayFloat w2 = w + w;
        uv.mChunkBase[0] = uv.mChunkBase[0] * w2;
        uv.mChunkBase[1] = uv.mChunkBase[1] * w2;
        uv.mChunkBase[2] = uv.mChunkBase[2] * w2;

        // uuv = uuv * 2.0f
        uuv.mChunkBase[0] = uuv.mChunkBase[0] + uuv.mChunkBase[0];
        uuv.mChunkBase[1] = uuv.mChunkBase[1] + uuv.mChunkBase[1];
        uuv.mChunkBase[2] = uuv.mChunkBase[2] + uuv.mChunkBase[2];

        //uv = v + uv + uuv
        uv.mChunkBase[0] = v.mChunkBase[0] + uv.mChunkBase[0] + uuv.mChunkBase[0];
        uv.mChunkBase[1] = v.mChunkBase[1] + uv.mChunkBase[1] + uuv.mChunkBase[1];
        uv.mChunkBase[2] = v.mChunkBase[2] + uv.mChunkBase[2] + uuv.mChunkBase[2];

        return uv;
    }
    
    inline void ArrayQuaternion::Cmov4( ArrayMaskR mask, const ArrayQuaternion &replacement )
    {
        ArrayFloat * RESTRICT_ALIAS aChunkBase = &w;
        const ArrayFloat * RESTRICT_ALIAS bChunkBase = &w;
        aChunkBase[0] = MathlibC::Cmov4( aChunkBase[0], bChunkBase[0], mask );
        aChunkBase[1] = MathlibC::Cmov4( aChunkBase[1], bChunkBase[1], mask );
        aChunkBase[2] = MathlibC::Cmov4( aChunkBase[2], bChunkBase[2], mask );
        aChunkBase[3] = MathlibC::Cmov4( aChunkBase[3], bChunkBase[3], mask );
    }
}
