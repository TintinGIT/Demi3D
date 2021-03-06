/*
 *********************************************************************
 This source file is a part of Demi3D
 __  ___  __  __  __
 |  \|_ |\/||   _)|  \
 |__/|__|  ||  __)|__/
 
 Copyright (c) 2013-2014 Demi team
 https://github.com/wangyanxing/Demi3D
 
 Released under the MIT License
 https://github.com/wangyanxing/Demi3D/blob/master/License.txt
 ***********************************************************************/

/*
Where EXPLICITLY stated, the following license will apply (it's from the
Sony's SIMD Math library):

Copyright (C) 2006, 2007 Sony Computer Entertainment Inc.
All rights reserved.

Redistribution and use in source and binary forms,
with or without modification, are permitted provided that the
following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Sony Computer Entertainment Inc nor the names
      of its contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

namespace Demi
{
    /*inline ArrayFloat ArrayRadian::valueDegrees() const
    {
        return vmulq_f32( mRad, MathlibNEON::fRad2Deg );
    }*/
    
    inline const ArrayRadian& ArrayRadian::operator + () const
    {
        return *this;
    }
    
    inline ArrayRadian ArrayRadian::operator + ( const ArrayRadian& r ) const
    {
        return ArrayRadian( vaddq_f32( mRad, r.mRad ) );
    }
    
    /*inline ArrayRadian ArrayRadian::operator + ( const ArrayDegree& d ) const
    {
        return ArrayRadian( vaddq_f32( mRad, r.valueRadians() ) );
    }*/
    
    inline ArrayRadian& ArrayRadian::operator += ( const ArrayRadian& r )
    {
        mRad = vaddq_f32( mRad, r.mRad );
        return *this;
    }
    
    /*inline ArrayRadian& ArrayRadian::operator += ( const ArrayDegree& r )
    {
        mRad = vaddq_f32( mRad, r.valueRadians() );
        return *this;
    }*/
    
    inline ArrayRadian ArrayRadian::operator - () const
    {
        return ArrayRadian( veorq_s32( mRad, MathlibNEON::SIGN_MASK ) );
    }
    
    inline ArrayRadian ArrayRadian::operator - ( const ArrayRadian& r ) const
    {
        return ArrayRadian( vsubq_f32( mRad, r.mRad ) );
    }
    
    /*inline ArrayRadian ArrayRadian::operator - ( const ArrayDegree& d ) const
    {
        return ArrayRadian( vsubq_f32( mRad, r.valueRadians() ) );
    }*/
    
    inline ArrayRadian& ArrayRadian::operator -= ( const ArrayRadian& r )
    {
        mRad = vsubq_f32( mRad, r.mRad );
        return *this;
    }
    
    /*inline ArrayRadian& ArrayRadian::operator -= ( const ArrayDegree& r )
    {
        mRad = vsubq_f32( mRad, r.valueRadians() );
        return *this;
    }*/
    
    inline ArrayRadian ArrayRadian::operator * ( const ArrayRadian& r ) const
    {
        return ArrayRadian( vmulq_f32( mRad, r.mRad ) );
    }
    
    inline ArrayRadian ArrayRadian::operator / ( ArrayFloat r ) const
    {
        return ArrayRadian( vdivq_f32( mRad, r ) );
    }
    
    inline ArrayRadian& ArrayRadian::operator /= ( ArrayFloat r )
    {
        mRad = vdivq_f32( mRad, r );
        return *this;
    }

    inline ArrayFloat ArrayRadian::operator <  ( const ArrayRadian& r ) const { return vcltq_f32( mRad, r.mRad ); }
    inline ArrayFloat ArrayRadian::operator <= ( const ArrayRadian& r ) const { return vcleq_f32( mRad, r.mRad ); }
    inline ArrayFloat ArrayRadian::operator == ( const ArrayRadian& r ) const { return vceqq_f32( mRad, r.mRad ); }
    inline ArrayFloat ArrayRadian::operator != ( const ArrayRadian& r ) const { return vcneqq_f32( mRad, r.mRad );}
    inline ArrayFloat ArrayRadian::operator >= ( const ArrayRadian& r ) const { return vcgeq_f32( mRad, r.mRad ); }
    inline ArrayFloat ArrayRadian::operator >  ( const ArrayRadian& r ) const { return vcgtq_f32( mRad, r.mRad ); }

    
    inline ArrayFloat MathlibNEON::Modf4( ArrayFloat x, ArrayFloat &outIntegral )
    {
        outIntegral = (ArrayFloat)( (ArrayInt)( x ) );   //outIntegral = floor( x )
        return vsubq_f32( x, outIntegral );
    }
    
    inline ArrayFloat MathlibNEON::ACos4( ArrayFloat x)
    {
        // This function, ACos4, is under Copyright (C) 2006, 2007
        // Sony Computer Entertainment Inc. (BSD style license) See
        // header for details. Adapted/ported to Ogre intrinsics.
        ArrayFloat xabs = Abs4( x );
        ArrayFloat select = vcltq_f32( x, vdupq_n_f32(0.0f) );
        ArrayFloat t1 = vrsqrteq_f32( vsubq_f32( ONE, xabs ) );
        
        /* Instruction counts can be reduced if the polynomial was
         * computed entirely from nested (dependent) fma's. However, 
         * to reduce the number of pipeline stalls, the polygon is evaluated 
         * in two halves (hi amd lo). 
         */
        ArrayFloat xabs2 = vmulq_f32( xabs,  xabs );
        ArrayFloat xabs4 = vmulq_f32( xabs2, xabs2 );
        ArrayFloat hi = _mm_madd_ps(_mm_madd_ps(_mm_madd_ps(vdupq_n_f32(-0.0012624911f),
            xabs, vdupq_n_f32(0.0066700901f)),
                xabs, vdupq_n_f32(-0.0170881256f)),
                    xabs, vdupq_n_f32( 0.0308918810f));
        ArrayFloat lo = _mm_madd_ps(_mm_madd_ps(_mm_madd_ps(vdupq_n_f32(-0.0501743046f),
            xabs, vdupq_n_f32(0.0889789874f)),
                xabs, vdupq_n_f32(-0.2145988016f)),
                    xabs, vdupq_n_f32( 1.5707963050f));
        
        ArrayFloat result = _mm_madd_ps( hi, xabs4, lo );
        
        // Adjust the result if x is negactive.
        return Cmov4(   _mm_nmsub_ps( t1, result, PI ), // Negative
                        vmulq_f32( t1, result ),        // Positive
                        select );
    }

}
