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
    /// Concatenates two 4x3 array matrices.
    /// @remarks
    ///     99.99% of the cases the matrix isn't concatenated with itself, therefore it's
    ///     safe to assume &lhs != &rhs. RESTRICT_ALIAS modifier is used (a non-standard
    ///     C++ extension) is used when available to dramatically improve performance,
    ///     particularly of the update operations ( a *= b )
    ///     This function will assert if DEMI_RESTRICT_ALIASING is enabled and any of the
    ///     given pointers point to the same location
    FORCEINLINE void concatArrayMatAf4x3( ArrayFloat * RESTRICT_ALIAS outChunkBase,
                                        const ArrayFloat * RESTRICT_ALIAS lhsChunkBase,
                                        const ArrayFloat * RESTRICT_ALIAS rhsChunkBase )
    {
#if DEMI_RESTRICT_ALIASING != 0
        assert( outChunkBase != lhsChunkBase && outChunkBase != rhsChunkBase &&
                lhsChunkBase != rhsChunkBase &&
                "Re-strict aliasing rule broken. Compile without DEMI_RESTRICT_ALIASING" );
#endif
        outChunkBase[0] =   _mm_madd_ps(    lhsChunkBase[0], rhsChunkBase[0],
                            _mm_madd_ps(    lhsChunkBase[1], rhsChunkBase[4],
                            _mm_mul_ps(     lhsChunkBase[2], rhsChunkBase[8] ) ) );
        outChunkBase[1] =   _mm_madd_ps(    lhsChunkBase[0], rhsChunkBase[1],
                            _mm_madd_ps(    lhsChunkBase[1], rhsChunkBase[5],
                            _mm_mul_ps(     lhsChunkBase[2], rhsChunkBase[9] ) ) );
        outChunkBase[2] =   _mm_madd_ps(    lhsChunkBase[0], rhsChunkBase[2],
                            _mm_madd_ps(    lhsChunkBase[1], rhsChunkBase[6],
                            _mm_mul_ps(     lhsChunkBase[2], rhsChunkBase[10] ) ) );
        outChunkBase[3] =   _mm_madd_ps(    lhsChunkBase[0], rhsChunkBase[3],
                            _mm_madd_ps(    lhsChunkBase[1], rhsChunkBase[7],
                            _mm_madd_ps(    lhsChunkBase[2], rhsChunkBase[11],
                                            lhsChunkBase[3] ) ) );

        /* Next row (1) */
        outChunkBase[4] =   _mm_madd_ps(    lhsChunkBase[4], rhsChunkBase[0],
                            _mm_madd_ps(    lhsChunkBase[5], rhsChunkBase[4],
                            _mm_mul_ps(     lhsChunkBase[6], rhsChunkBase[8] ) ) );
        outChunkBase[5] =   _mm_madd_ps(    lhsChunkBase[4], rhsChunkBase[1],
                            _mm_madd_ps(    lhsChunkBase[5], rhsChunkBase[5],
                            _mm_mul_ps(     lhsChunkBase[6], rhsChunkBase[9] ) ) );
        outChunkBase[6] =   _mm_madd_ps(    lhsChunkBase[4], rhsChunkBase[2],
                            _mm_madd_ps(    lhsChunkBase[5], rhsChunkBase[6],
                            _mm_mul_ps(     lhsChunkBase[6], rhsChunkBase[10] ) ) );
        outChunkBase[7] =   _mm_madd_ps(    lhsChunkBase[4], rhsChunkBase[3],
                            _mm_madd_ps(    lhsChunkBase[5], rhsChunkBase[7],
                            _mm_madd_ps(    lhsChunkBase[6], rhsChunkBase[11],
                                            lhsChunkBase[7] ) ) );

        /* Next row (2) */
        outChunkBase[8] =   _mm_madd_ps(    lhsChunkBase[8], rhsChunkBase[0],
                            _mm_madd_ps(    lhsChunkBase[9], rhsChunkBase[4],
                            _mm_mul_ps(     lhsChunkBase[10],rhsChunkBase[8] ) ) );
        outChunkBase[9] =   _mm_madd_ps(    lhsChunkBase[8], rhsChunkBase[1],
                            _mm_madd_ps(    lhsChunkBase[9], rhsChunkBase[5],
                            _mm_mul_ps(     lhsChunkBase[10],rhsChunkBase[9] ) ) );
        outChunkBase[10] =  _mm_madd_ps(    lhsChunkBase[8], rhsChunkBase[2],
                            _mm_madd_ps(    lhsChunkBase[9], rhsChunkBase[6],
                            _mm_mul_ps(     lhsChunkBase[10],rhsChunkBase[10] ) ) );
        outChunkBase[11] =  _mm_madd_ps(    lhsChunkBase[8], rhsChunkBase[3],
                            _mm_madd_ps(    lhsChunkBase[9], rhsChunkBase[7],
                            _mm_madd_ps(    lhsChunkBase[10],rhsChunkBase[11],
                                            lhsChunkBase[11] ) ) );
    }

    /// Update version
    FORCEINLINE void concatArrayMatAf4x3( ArrayFloat * RESTRICT_ALIAS lhsChunkBase,
                                        const ArrayFloat * RESTRICT_ALIAS rhsChunkBase )
    {
#if DEMI_RESTRICT_ALIASING != 0
        assert( lhsChunkBase != rhsChunkBase &&
                "Re-strict aliasing rule broken. Compile without DEMI_RESTRICT_ALIASING" );
#endif
        ArrayFloat lhs0 = lhsChunkBase[0];
        lhsChunkBase[0] =   _mm_madd_ps(    lhsChunkBase[0], rhsChunkBase[0],
                            _mm_madd_ps(    lhsChunkBase[1], rhsChunkBase[4],
                            _mm_mul_ps(     lhsChunkBase[2], rhsChunkBase[8] ) ) );
        ArrayFloat lhs1 = lhsChunkBase[1];
        lhsChunkBase[1] =   _mm_madd_ps(    lhs0, rhsChunkBase[1],
                            _mm_madd_ps(    lhsChunkBase[1], rhsChunkBase[5],
                            _mm_mul_ps(     lhsChunkBase[2], rhsChunkBase[9] ) ) );
        ArrayFloat lhs2 = lhsChunkBase[2];
        lhsChunkBase[2] =   _mm_madd_ps(    lhs0, rhsChunkBase[2],
                            _mm_madd_ps(    lhs1, rhsChunkBase[6],
                            _mm_mul_ps(     lhsChunkBase[2], rhsChunkBase[10] ) ) );

        lhsChunkBase[3] =   _mm_madd_ps(    lhs0, rhsChunkBase[3],
                            _mm_madd_ps(    lhs1, rhsChunkBase[7],
                            _mm_madd_ps(    lhs2, rhsChunkBase[11],
                                            lhsChunkBase[3] ) ) );

        /* Next row (1) */
        lhs0 = lhsChunkBase[4];
        lhsChunkBase[4] =   _mm_madd_ps(    lhsChunkBase[4], rhsChunkBase[0],
                            _mm_madd_ps(    lhsChunkBase[5], rhsChunkBase[4],
                            _mm_mul_ps(     lhsChunkBase[6], rhsChunkBase[8] ) ) );
        lhs1 = lhsChunkBase[5];
        lhsChunkBase[5] =   _mm_madd_ps(    lhs0, rhsChunkBase[1],
                            _mm_madd_ps(    lhsChunkBase[5], rhsChunkBase[5],
                            _mm_mul_ps(     lhsChunkBase[6], rhsChunkBase[9] ) ) );
        lhs2 = lhsChunkBase[6];
        lhsChunkBase[6] =   _mm_madd_ps(    lhs0, rhsChunkBase[2],
                            _mm_madd_ps(    lhs1, rhsChunkBase[6],
                            _mm_mul_ps(     lhsChunkBase[6], rhsChunkBase[10] ) ) );

        lhsChunkBase[7] =   _mm_madd_ps(    lhs0, rhsChunkBase[3],
                            _mm_madd_ps(    lhs1, rhsChunkBase[7],
                            _mm_madd_ps(    lhs2, rhsChunkBase[11],
                                            lhsChunkBase[7] ) ) );

        /* Next row (2) */
        lhs0 = lhsChunkBase[8];
        lhsChunkBase[8] =   _mm_madd_ps(    lhsChunkBase[8], rhsChunkBase[0],
                            _mm_madd_ps(    lhsChunkBase[9], rhsChunkBase[4],
                            _mm_mul_ps(     lhsChunkBase[10],rhsChunkBase[8] ) ) );
        lhs1 = lhsChunkBase[9];
        lhsChunkBase[9] =   _mm_madd_ps(    lhs0, rhsChunkBase[1],
                            _mm_madd_ps(    lhsChunkBase[9], rhsChunkBase[5],
                            _mm_mul_ps(     lhsChunkBase[10],rhsChunkBase[9] ) ) );
        lhs2 = lhsChunkBase[10];
        lhsChunkBase[10] =  _mm_madd_ps(    lhs0, rhsChunkBase[2],
                            _mm_madd_ps(    lhs1, rhsChunkBase[6],
                            _mm_mul_ps(     lhsChunkBase[10],rhsChunkBase[10] ) ) );

        lhsChunkBase[11] =  _mm_madd_ps(    lhs0, rhsChunkBase[3],
                            _mm_madd_ps(    lhs1, rhsChunkBase[7],
                            _mm_madd_ps(    lhs2,rhsChunkBase[11],
                                            lhsChunkBase[11] ) ) );
    }

    FORCEINLINE ArrayMatrixAf4x3 operator * ( const ArrayMatrixAf4x3 &lhs, const ArrayMatrixAf4x3 &rhs )
    {
        ArrayMatrixAf4x3 retVal;
        concatArrayMatAf4x3( retVal.mChunkBase, lhs.mChunkBase, rhs.mChunkBase );
        return retVal;
    }
    
    inline ArrayVector3 ArrayMatrixAf4x3::operator * ( const ArrayVector3 &rhs ) const
    {
        return ArrayVector3(
            //X
            //  (m00 * v.x + m01 * v.y) + (m02 * v.z + m03)
            _mm_add_ps(
                _mm_madd_ps(
                            mChunkBase[0], rhs.mChunkBase[0],
                _mm_mul_ps( mChunkBase[1], rhs.mChunkBase[1] ) ),
                _mm_madd_ps(
                            mChunkBase[2], rhs.mChunkBase[2],
                            mChunkBase[3] ) ),
            //Y
            //  (m10 * v.x + m11 * v.y) + (m12 * v.z + m13)
            _mm_add_ps(
                _mm_madd_ps(
                            mChunkBase[4], rhs.mChunkBase[0],
                _mm_mul_ps( mChunkBase[5], rhs.mChunkBase[1] ) ),
                _mm_madd_ps(
                            mChunkBase[6], rhs.mChunkBase[2],
                            mChunkBase[7] ) ),
            //Z
            //  (m20 * v.x + m21 * v.y) + (m22 * v.z + m23)
            _mm_add_ps(
                _mm_madd_ps(
                            mChunkBase[8], rhs.mChunkBase[0],
                _mm_mul_ps( mChunkBase[9], rhs.mChunkBase[1] ) ),
                _mm_madd_ps(
                            mChunkBase[10], rhs.mChunkBase[2],
                            mChunkBase[11] ) ) );
    }
    
    FORCEINLINE void ArrayMatrixAf4x3::operator *= ( const ArrayMatrixAf4x3 &rhs )
    {
        concatArrayMatAf4x3( mChunkBase, rhs.mChunkBase );
    }
    
    inline void ArrayMatrixAf4x3::fromQuaternion( const ArrayQuaternion &q )
    {
        ArrayFloat * RESTRICT_ALIAS chunkBase = mChunkBase;
        const ArrayFloat * RESTRICT_ALIAS qChunkBase = q.mChunkBase;
        ArrayFloat fTx  = _mm_add_ps( qChunkBase[1], qChunkBase[1] );        // 2 * x
        ArrayFloat fTy  = _mm_add_ps( qChunkBase[2], qChunkBase[2] );        // 2 * y
        ArrayFloat fTz  = _mm_add_ps( qChunkBase[3], qChunkBase[3] );        // 2 * z
        ArrayFloat fTwx = _mm_mul_ps( fTx, qChunkBase[0] );                  // fTx*w;
        ArrayFloat fTwy = _mm_mul_ps( fTy, qChunkBase[0] );                  // fTy*w;
        ArrayFloat fTwz = _mm_mul_ps( fTz, qChunkBase[0] );                  // fTz*w;
        ArrayFloat fTxx = _mm_mul_ps( fTx, qChunkBase[1] );                  // fTx*x;
        ArrayFloat fTxy = _mm_mul_ps( fTy, qChunkBase[1] );                  // fTy*x;
        ArrayFloat fTxz = _mm_mul_ps( fTz, qChunkBase[1] );                  // fTz*x;
        ArrayFloat fTyy = _mm_mul_ps( fTy, qChunkBase[2] );                  // fTy*y;
        ArrayFloat fTyz = _mm_mul_ps( fTz, qChunkBase[2] );                  // fTz*y;
        ArrayFloat fTzz = _mm_mul_ps( fTz, qChunkBase[3] );                  // fTz*z;

        chunkBase[0] = _mm_sub_ps( MathlibSSE2::ONE, _mm_add_ps( fTyy, fTzz ) );
        chunkBase[1] = _mm_sub_ps( fTxy, fTwz );
        chunkBase[2] = _mm_add_ps( fTxz, fTwy );
        chunkBase[4] = _mm_add_ps( fTxy, fTwz );
        chunkBase[5] = _mm_sub_ps( MathlibSSE2::ONE, _mm_add_ps( fTxx, fTzz ) );
        chunkBase[6] = _mm_sub_ps( fTyz, fTwx );
        chunkBase[8] = _mm_sub_ps( fTxz, fTwy );
        chunkBase[9] = _mm_add_ps( fTyz, fTwx );
        chunkBase[10]= _mm_sub_ps( MathlibSSE2::ONE, _mm_add_ps( fTxx, fTyy ) );
    }
    
    inline void ArrayMatrixAf4x3::makeTransform( const ArrayVector3 &position, const ArrayVector3 &scale,
                                             const ArrayQuaternion &orientation )
    {
        ArrayFloat * RESTRICT_ALIAS chunkBase            = mChunkBase;
        const ArrayFloat * RESTRICT_ALIAS posChunkBase   = position.mChunkBase;
        const ArrayFloat * RESTRICT_ALIAS scaleChunkBase = scale.mChunkBase;
        this->fromQuaternion( orientation );
        chunkBase[0] = _mm_mul_ps( chunkBase[0], scaleChunkBase[0] );   //m00 * scale.x
        chunkBase[1] = _mm_mul_ps( chunkBase[1], scaleChunkBase[1] );   //m01 * scale.y
        chunkBase[2] = _mm_mul_ps( chunkBase[2], scaleChunkBase[2] );   //m02 * scale.z
        chunkBase[3] =  posChunkBase[0];                                //m03 * pos.x

        chunkBase[4] = _mm_mul_ps( chunkBase[4], scaleChunkBase[0] );   //m10 * scale.x
        chunkBase[5] = _mm_mul_ps( chunkBase[5], scaleChunkBase[1] );   //m11 * scale.y
        chunkBase[6] = _mm_mul_ps( chunkBase[6], scaleChunkBase[2] );   //m12 * scale.z
        chunkBase[7] =  posChunkBase[1];                                //m13 * pos.y

        chunkBase[8] = _mm_mul_ps( chunkBase[8], scaleChunkBase[0] );   //m20 * scale.x
        chunkBase[9] = _mm_mul_ps( chunkBase[9], scaleChunkBase[1] );   //m21 * scale.y
        chunkBase[10]= _mm_mul_ps( chunkBase[10],scaleChunkBase[2] );   //m22 * scale.z
        chunkBase[11]=  posChunkBase[2];                                //m23 * pos.z
    }
    
    inline void ArrayMatrixAf4x3::setToInverse(void)
    {
        ArrayFloat m10 = mChunkBase[4], m11 = mChunkBase[5], m12 = mChunkBase[6];
        ArrayFloat m20 = mChunkBase[8], m21 = mChunkBase[9], m22 = mChunkBase[10];

        ArrayFloat t00 = _mm_nmsub_ps( m21, m12, _mm_mul_ps( m22, m11 ) ); //m22 * m11 - m21 * m12;
        ArrayFloat t10 = _mm_nmsub_ps( m22, m10, _mm_mul_ps( m20, m12 ) ); //m20 * m22 - m22 * m10;
        ArrayFloat t20 = _mm_nmsub_ps( m20, m11, _mm_mul_ps( m21, m10 ) ); //m21 * m10 - m20 * m11;

        ArrayFloat m00 = mChunkBase[0], m01 = mChunkBase[1], m02 = mChunkBase[2];

        //det = m00 * t00 + m01 * t10 + m02 * t20
        ArrayFloat det   = _mm_madd_ps( m00, t00, _mm_madd_ps( m01, t10,  _mm_mul_ps( m02, t20 ) ) );
        ArrayFloat invDet= _mm_div_ps( MathlibSSE2::ONE, det ); //High precision division

        t00 = _mm_mul_ps( t00, invDet );
        t10 = _mm_mul_ps( t10, invDet );
        t20 = _mm_mul_ps( t20, invDet );

        m00 = _mm_mul_ps( m00, invDet );
        m01 = _mm_mul_ps( m01, invDet );
        m02 = _mm_mul_ps( m02, invDet );

        ArrayFloat r00 = t00;
        ArrayFloat r01 = _mm_nmsub_ps( m01, m22, _mm_mul_ps( m02, m21 ) ); //m02 * m21 - m01 * m22;
        ArrayFloat r02 = _mm_nmsub_ps( m02, m11, _mm_mul_ps( m01, m12 ) ); //m01 * m12 - m02 * m11;

        ArrayFloat r10 = t10;
        ArrayFloat r11 = _mm_nmsub_ps( m02, m20, _mm_mul_ps( m00, m22 ) ); //m00 * m22 - m02 * m20;
        ArrayFloat r12 = _mm_nmsub_ps( m00, m12, _mm_mul_ps( m02, m10 ) ); //m02 * m10 - m00 * m12;

        ArrayFloat r20 = t20;
        ArrayFloat r21 = _mm_nmsub_ps( m00, m21, _mm_mul_ps( m01, m20 ) ); //m01 * m20 - m00 * m21;
        ArrayFloat r22 = _mm_nmsub_ps( m01, m10, _mm_mul_ps( m00, m11 ) ); //m00 * m11 - m01 * m10;

        ArrayFloat m03 = mChunkBase[3], m13 = mChunkBase[7], m23 = mChunkBase[11];

        //r03 = (r00 * m03 + r01 * m13 + r02 * m23)
        //r13 = (r10 * m03 + r11 * m13 + r12 * m23)
        //r23 = (r20 * m03 + r21 * m13 + r22 * m23)
        ArrayFloat r03 = _mm_madd_ps( r00, m03, _mm_madd_ps( r01, m13, _mm_mul_ps( r02, m23 ) ) );
        ArrayFloat r13 = _mm_madd_ps( r10, m03, _mm_madd_ps( r11, m13, _mm_mul_ps( r12, m23 ) ) );
        ArrayFloat r23 = _mm_madd_ps( r20, m03, _mm_madd_ps( r21, m13, _mm_mul_ps( r22, m23 ) ) );

        r03 = _mm_mul_ps( r03, MathlibSSE2::NEG_ONE ); //r03 = -r03
        r13 = _mm_mul_ps( r13, MathlibSSE2::NEG_ONE ); //r13 = -r13
        r23 = _mm_mul_ps( r23, MathlibSSE2::NEG_ONE ); //r23 = -r23

        mChunkBase[0] = r00;
        mChunkBase[1] = r01;
        mChunkBase[2] = r02;
        mChunkBase[3] = r03;

        mChunkBase[4] = r10;
        mChunkBase[5] = r11;
        mChunkBase[6] = r12;
        mChunkBase[7] = r13;

        mChunkBase[8] = r20;
        mChunkBase[9] = r21;
        mChunkBase[10]= r22;
        mChunkBase[11]= r23;
    }
    
    inline void ArrayMatrixAf4x3::setToInverseDegeneratesAsIdentity(void)
    {
        ArrayFloat m10 = mChunkBase[4], m11 = mChunkBase[5], m12 = mChunkBase[6];
        ArrayFloat m20 = mChunkBase[8], m21 = mChunkBase[9], m22 = mChunkBase[10];

        ArrayFloat t00 = _mm_nmsub_ps( m21, m12, _mm_mul_ps( m22, m11 ) ); //m22 * m11 - m21 * m12;
        ArrayFloat t10 = _mm_nmsub_ps( m22, m10, _mm_mul_ps( m20, m12 ) ); //m20 * m22 - m22 * m10;
        ArrayFloat t20 = _mm_nmsub_ps( m20, m11, _mm_mul_ps( m21, m10 ) ); //m21 * m10 - m20 * m11;

        ArrayFloat m00 = mChunkBase[0], m01 = mChunkBase[1], m02 = mChunkBase[2];

        //det = m00 * t00 + m01 * t10 + m02 * t20
        ArrayFloat det   = _mm_madd_ps( m00, t00, _mm_madd_ps( m01, t10,  _mm_mul_ps( m02, t20 ) ) );
        ArrayFloat invDet= _mm_div_ps( MathlibSSE2::ONE, det ); //High precision division

        //degenerateMask = Abs( det ) < fEpsilon;
        ArrayMaskR degenerateMask = _mm_cmplt_ps( MathlibSSE2::Abs4( det ), MathlibSSE2::fEpsilon );

        t00 = _mm_mul_ps( t00, invDet );
        t10 = _mm_mul_ps( t10, invDet );
        t20 = _mm_mul_ps( t20, invDet );

        m00 = _mm_mul_ps( m00, invDet );
        m01 = _mm_mul_ps( m01, invDet );
        m02 = _mm_mul_ps( m02, invDet );

        ArrayFloat r00 = t00;
        ArrayFloat r01 = _mm_nmsub_ps( m01, m22, _mm_mul_ps( m02, m21 ) ); //m02 * m21 - m01 * m22;
        ArrayFloat r02 = _mm_nmsub_ps( m02, m11, _mm_mul_ps( m01, m12 ) ); //m01 * m12 - m02 * m11;

        ArrayFloat r10 = t10;
        ArrayFloat r11 = _mm_nmsub_ps( m02, m20, _mm_mul_ps( m00, m22 ) ); //m00 * m22 - m02 * m20;
        ArrayFloat r12 = _mm_nmsub_ps( m00, m12, _mm_mul_ps( m02, m10 ) ); //m02 * m10 - m00 * m12;

        ArrayFloat r20 = t20;
        ArrayFloat r21 = _mm_nmsub_ps( m00, m21, _mm_mul_ps( m01, m20 ) ); //m01 * m20 - m00 * m21;
        ArrayFloat r22 = _mm_nmsub_ps( m01, m10, _mm_mul_ps( m00, m11 ) ); //m00 * m11 - m01 * m10;

        ArrayFloat m03 = mChunkBase[3], m13 = mChunkBase[7], m23 = mChunkBase[11];

        //r03 = (r00 * m03 + r01 * m13 + r02 * m23)
        //r13 = (r10 * m03 + r11 * m13 + r12 * m23)
        //r13 = (r20 * m03 + r21 * m13 + r22 * m23)
        ArrayFloat r03 = _mm_madd_ps( r00, m03, _mm_madd_ps( r01, m13, _mm_mul_ps( r02, m23 ) ) );
        ArrayFloat r13 = _mm_madd_ps( r10, m03, _mm_madd_ps( r11, m13, _mm_mul_ps( r12, m23 ) ) );
        ArrayFloat r23 = _mm_madd_ps( r20, m03, _mm_madd_ps( r21, m13, _mm_mul_ps( r22, m23 ) ) );

        r03 = _mm_mul_ps( r03, MathlibSSE2::NEG_ONE ); //r03 = -r03
        r13 = _mm_mul_ps( r13, MathlibSSE2::NEG_ONE ); //r13 = -r13
        r23 = _mm_mul_ps( r23, MathlibSSE2::NEG_ONE ); //r23 = -r23

        mChunkBase[0] = MathlibSSE2::CmovRobust( MathlibSSE2::ONE, r00, degenerateMask );
        mChunkBase[1] = MathlibSSE2::CmovRobust( _mm_setzero_ps(), r01, degenerateMask );
        mChunkBase[2] = MathlibSSE2::CmovRobust( _mm_setzero_ps(), r02, degenerateMask );
        mChunkBase[3] = MathlibSSE2::CmovRobust( _mm_setzero_ps(), r03, degenerateMask );

        mChunkBase[4] = MathlibSSE2::CmovRobust( _mm_setzero_ps(), r10, degenerateMask );
        mChunkBase[5] = MathlibSSE2::CmovRobust( MathlibSSE2::ONE, r11, degenerateMask );
        mChunkBase[6] = MathlibSSE2::CmovRobust( _mm_setzero_ps(), r12, degenerateMask );
        mChunkBase[7] = MathlibSSE2::CmovRobust( _mm_setzero_ps(), r13, degenerateMask );

        mChunkBase[8] = MathlibSSE2::CmovRobust( _mm_setzero_ps(), r20, degenerateMask );
        mChunkBase[9] = MathlibSSE2::CmovRobust( _mm_setzero_ps(), r21, degenerateMask );
        mChunkBase[10]= MathlibSSE2::CmovRobust( MathlibSSE2::ONE, r22, degenerateMask );
        mChunkBase[11]= MathlibSSE2::CmovRobust( _mm_setzero_ps(), r23, degenerateMask );
    }
    
    inline void ArrayMatrixAf4x3::retain( ArrayMaskR orientation, ArrayMaskR scale )
    {
        ArrayVector3 row0( mChunkBase[0], mChunkBase[1], mChunkBase[2] );
        ArrayVector3 row1( mChunkBase[4], mChunkBase[5], mChunkBase[6] );
        ArrayVector3 row2( mChunkBase[8], mChunkBase[9], mChunkBase[10] );

        ArrayVector3 vScale( row0.length(), row1.length(), row2.length() );
        ArrayVector3 vInvScale( vScale );
        vInvScale.inverseLeaveZeroes();

        row0 *= vInvScale.mChunkBase[0];
        row1 *= vInvScale.mChunkBase[1];
        row2 *= vInvScale.mChunkBase[2];

        vScale.Cmov4( scale, ArrayVector3::UNIT_SCALE );

        row0.Cmov4( orientation, ArrayVector3::UNIT_X );
        row1.Cmov4( orientation, ArrayVector3::UNIT_Y );
        row2.Cmov4( orientation, ArrayVector3::UNIT_Z );

        row0 *= vScale.mChunkBase[0];
        row1 *= vScale.mChunkBase[1];
        row2 *= vScale.mChunkBase[2];

        mChunkBase[0] = row0.mChunkBase[0];
        mChunkBase[1] = row0.mChunkBase[1];
        mChunkBase[2] = row0.mChunkBase[2];

        mChunkBase[4] = row1.mChunkBase[0];
        mChunkBase[5] = row1.mChunkBase[1];
        mChunkBase[6] = row1.mChunkBase[2];

        mChunkBase[8] = row2.mChunkBase[0];
        mChunkBase[9] = row2.mChunkBase[1];
        mChunkBase[10]= row2.mChunkBase[2];
    }
    
    inline void ArrayMatrixAf4x3::streamToAoS( DiMat4 * RESTRICT_ALIAS dst ) const
    {
        //Do not use the unpack version, use the shuffle. Shuffle is faster in k10 processors
        //("The conceptual shuffle" http://developer.amd.com/community/blog/the-conceptual-shuffle/)
        //and the unpack version uses 64-bit moves, which can cause store forwarding issues when
        //then loading them with 128-bit movaps
#define _MM_TRANSPOSE4_SRC_DST_PS(row0, row1, row2, row3, dst0, dst1, dst2, dst3) { \
            __m128 tmp3, tmp2, tmp1, tmp0;                          \
                                                                    \
            tmp0   = _mm_shuffle_ps((row0), (row1), 0x44);          \
            tmp2   = _mm_shuffle_ps((row0), (row1), 0xEE);          \
            tmp1   = _mm_shuffle_ps((row2), (row3), 0x44);          \
            tmp3   = _mm_shuffle_ps((row2), (row3), 0xEE);          \
                                                                    \
            (dst0) = _mm_shuffle_ps(tmp0, tmp1, 0x88);              \
            (dst1) = _mm_shuffle_ps(tmp0, tmp1, 0xDD);              \
            (dst2) = _mm_shuffle_ps(tmp2, tmp3, 0x88);              \
            (dst3) = _mm_shuffle_ps(tmp2, tmp3, 0xDD);              \
        }
        register ArrayFloat m0, m1, m2, m3;

        _MM_TRANSPOSE4_SRC_DST_PS(
                            this->mChunkBase[0], this->mChunkBase[1],
                            this->mChunkBase[2], this->mChunkBase[3],
                            m0, m1, m2, m3 );
        _mm_stream_ps( dst[0]._m, m0 );
        _mm_stream_ps( dst[1]._m, m1 );
        _mm_stream_ps( dst[2]._m, m2 );
        _mm_stream_ps( dst[3]._m, m3 );
        _MM_TRANSPOSE4_SRC_DST_PS(
                            this->mChunkBase[4], this->mChunkBase[5],
                            this->mChunkBase[6], this->mChunkBase[7],
                            m0, m1, m2, m3 );
        _mm_stream_ps( dst[0]._m+4, m0 );
        _mm_stream_ps( dst[1]._m+4, m1 );
        _mm_stream_ps( dst[2]._m+4, m2 );
        _mm_stream_ps( dst[3]._m+4, m3 );
        _MM_TRANSPOSE4_SRC_DST_PS(
                            this->mChunkBase[8], this->mChunkBase[9],
                            this->mChunkBase[10], this->mChunkBase[11],
                            m0, m1, m2, m3 );
        _mm_stream_ps( dst[0]._m+8, m0 );
        _mm_stream_ps( dst[1]._m+8, m1 );
        _mm_stream_ps( dst[2]._m+8, m2 );
        _mm_stream_ps( dst[3]._m+8, m3 );

        _mm_stream_ps( dst[0]._m+12, MathlibSSE2::LAST_AFFINE_COLUMN );
        _mm_stream_ps( dst[1]._m+12, MathlibSSE2::LAST_AFFINE_COLUMN );
        _mm_stream_ps( dst[2]._m+12, MathlibSSE2::LAST_AFFINE_COLUMN );
        _mm_stream_ps( dst[3]._m+12, MathlibSSE2::LAST_AFFINE_COLUMN );
    }
    
    inline void ArrayMatrixAf4x3::storeToAoS( SimpleMatrixAf4x3 * RESTRICT_ALIAS dst ) const
    {
        _MM_TRANSPOSE4_SRC_DST_PS(
                            this->mChunkBase[0], this->mChunkBase[1],
                            this->mChunkBase[2], this->mChunkBase[3],
                            dst[0].mChunkBase[0], dst[1].mChunkBase[0],
                            dst[2].mChunkBase[0], dst[3].mChunkBase[0] );
        _MM_TRANSPOSE4_SRC_DST_PS(
                            this->mChunkBase[4], this->mChunkBase[5],
                            this->mChunkBase[6], this->mChunkBase[7],
                            dst[0].mChunkBase[1], dst[1].mChunkBase[1],
                            dst[2].mChunkBase[1], dst[3].mChunkBase[1] );
        _MM_TRANSPOSE4_SRC_DST_PS(
                            this->mChunkBase[8], this->mChunkBase[9],
                            this->mChunkBase[10], this->mChunkBase[11],
                            dst[0].mChunkBase[2], dst[1].mChunkBase[2],
                            dst[2].mChunkBase[2], dst[3].mChunkBase[2] );
    }
    
    inline void ArrayMatrixAf4x3::streamToAoS( SimpleMatrixAf4x3 * RESTRICT_ALIAS _dst ) const
    {
        register __m128 dst0, dst1, dst2, dst3;
        float *dst = reinterpret_cast<float*>( _dst );

        _MM_TRANSPOSE4_SRC_DST_PS(
                            this->mChunkBase[0], this->mChunkBase[1],
                            this->mChunkBase[2], this->mChunkBase[3],
                            dst0, dst1, dst2, dst3 );

        _mm_stream_ps( &dst[0],  dst0 );
        _mm_stream_ps( &dst[12], dst1 );
        _mm_stream_ps( &dst[24], dst2 );
        _mm_stream_ps( &dst[36], dst3 );

        _MM_TRANSPOSE4_SRC_DST_PS(
                            this->mChunkBase[4], this->mChunkBase[5],
                            this->mChunkBase[6], this->mChunkBase[7],
                            dst0, dst1, dst2, dst3 );

        _mm_stream_ps( &dst[4],  dst0 );
        _mm_stream_ps( &dst[16], dst1 );
        _mm_stream_ps( &dst[28], dst2 );
        _mm_stream_ps( &dst[40], dst3 );

        _MM_TRANSPOSE4_SRC_DST_PS(
                            this->mChunkBase[8], this->mChunkBase[9],
                            this->mChunkBase[10], this->mChunkBase[11],
                            dst0, dst1, dst2, dst3 );

        _mm_stream_ps( &dst[8],  dst0 );
        _mm_stream_ps( &dst[20], dst1 );
        _mm_stream_ps( &dst[32], dst2 );
        _mm_stream_ps( &dst[44], dst3 );
    }
    
    inline void ArrayMatrixAf4x3::loadFromAoS( const DiMat4 * RESTRICT_ALIAS src )
    {
        _MM_TRANSPOSE4_SRC_DST_PS(
                            _mm_load_ps( src[0]._m ), _mm_load_ps( src[1]._m ), 
                            _mm_load_ps( src[2]._m ), _mm_load_ps( src[3]._m ),
                            this->mChunkBase[0], this->mChunkBase[1],
                            this->mChunkBase[2], this->mChunkBase[3] );
        _MM_TRANSPOSE4_SRC_DST_PS(
                            _mm_load_ps( src[0]._m+4 ), _mm_load_ps( src[1]._m+4 ), 
                            _mm_load_ps( src[2]._m+4 ), _mm_load_ps( src[3]._m+4 ),
                            this->mChunkBase[4], this->mChunkBase[5],
                            this->mChunkBase[6], this->mChunkBase[7] );
        _MM_TRANSPOSE4_SRC_DST_PS(
                            _mm_load_ps( src[0]._m+8 ), _mm_load_ps( src[1]._m+8 ), 
                            _mm_load_ps( src[2]._m+8 ), _mm_load_ps( src[3]._m+8 ),
                            this->mChunkBase[8], this->mChunkBase[9],
                            this->mChunkBase[10], this->mChunkBase[11] );
    }
    
    inline void ArrayMatrixAf4x3::loadFromAoS( const SimpleMatrixAf4x3 * RESTRICT_ALIAS src )
    {
        _MM_TRANSPOSE4_SRC_DST_PS(
                            src[0].mChunkBase[0], src[1].mChunkBase[0],
                            src[2].mChunkBase[0], src[3].mChunkBase[0],
                            this->mChunkBase[0], this->mChunkBase[1],
                            this->mChunkBase[2], this->mChunkBase[3] );
        _MM_TRANSPOSE4_SRC_DST_PS(
                            src[0].mChunkBase[1], src[1].mChunkBase[1],
                            src[2].mChunkBase[1], src[3].mChunkBase[1],
                            this->mChunkBase[4], this->mChunkBase[5],
                            this->mChunkBase[6], this->mChunkBase[7] );
        _MM_TRANSPOSE4_SRC_DST_PS(
                            src[0].mChunkBase[2], src[1].mChunkBase[2],
                            src[2].mChunkBase[2], src[3].mChunkBase[2],
                            this->mChunkBase[8], this->mChunkBase[9],
                            this->mChunkBase[10], this->mChunkBase[11] );
    }
    
    inline void ArrayMatrixAf4x3::loadFromAoS( const SimpleMatrixAf4x3 * * RESTRICT_ALIAS src )
    {
        _MM_TRANSPOSE4_SRC_DST_PS(
                            src[0]->mChunkBase[0], src[1]->mChunkBase[0],
                            src[2]->mChunkBase[0], src[3]->mChunkBase[0],
                            this->mChunkBase[0], this->mChunkBase[1],
                            this->mChunkBase[2], this->mChunkBase[3] );
        _MM_TRANSPOSE4_SRC_DST_PS(
                            src[0]->mChunkBase[1], src[1]->mChunkBase[1],
                            src[2]->mChunkBase[1], src[3]->mChunkBase[1],
                            this->mChunkBase[4], this->mChunkBase[5],
                            this->mChunkBase[6], this->mChunkBase[7] );
        _MM_TRANSPOSE4_SRC_DST_PS(
                            src[0]->mChunkBase[2], src[1]->mChunkBase[2],
                            src[2]->mChunkBase[2], src[3]->mChunkBase[2],
                            this->mChunkBase[8], this->mChunkBase[9],
                            this->mChunkBase[10], this->mChunkBase[11] );
    }
    
}
