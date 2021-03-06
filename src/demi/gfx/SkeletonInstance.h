
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

#ifndef _SkeletonInstance2_H__
#define _SkeletonInstance2_H__

#include "SkeletonAnimation.h"
#include "NewBone.h"

namespace Demi
{
    class SkeletonDef;
    typedef DiVector<SkeletonAnimation> SkeletonAnimationVec;
    typedef DiVector<SkeletonAnimation*> ActiveAnimationsVec;

    /** Instance of a Skeleton, main external interface for retrieving bone positions and applying
        animations.
    @remarks
        The new SkeletonInstance uses SIMD to animate up to 4 bones at the same time, though
        this depends on the number of bones on each parent level depth in the hierachy.
    @par
        I.e. if there is 1 root bone with 6 child bones; the root node will be animated solo,
        the first 4 child bones will be animated at the same time, and the 2 last bones
        will be animated together in the next loop iteration.
    @par
        Note however, when updating bones in the hierarchy to obtain the derived transforms
        (rather than animating), the root bone will be updated together using SIMD with the
        root bones from 3 other SkeletonInstances that share the same SkeletonDef.
        Only animating them has this restriction.
        The animation system won't be able to "share" though, if the SkeletonDef had 3 root
        nodes instead of 1; because we need to put them in a SIMD block in a repeating
        pattern And repeating 3 bones at least twice gives 6 bones, which doesn't fit in
        SSE2 (though it should in AVX, where ARRAY_PACKED_REALS = 8)

        To those interested in the original repository of OgreAnimation to obtain full
        history, go to:
            https://bitbucket.org/dark_sylinc/ogreanimation
    */
    class DI_GFX_API SkeletonInstance : public DiBase
    {
    public:
        typedef DiVector<DiNewBone> BoneVec;

    protected:
        BoneVec             mBones;
        TransformArray      mBoneStartTransforms; /// The start of Transform at each depth level

        RawSimdUniquePtr<ArrayFloat, MEMCATEGORY_ANIMATION> mManualBones;

        FastArray<size_t>       mSlotStarts;

        SkeletonAnimationVec    mAnimations;
        ActiveAnimationsVec     mActiveAnimations;

        SkeletonDef const       *mDefinition;

        /** Unused slots for each parent depth level that had more bones
            than >= ARRAY_PACKED_REALS /2 but less than < ARRAY_PACKED_REALS (or a multiple of it)
        */
        BoneVec                 mUnusedNodes;

        /// Node this SkeletonInstance is attached to (so we can work in world space)
        DiNode                  *mParentNode;

    public:
        SkeletonInstance( const SkeletonDef *skeletonDef, BoneMemoryManager *boneMemoryManager );
        ~SkeletonInstance();

        const SkeletonDef* getDefinition(void) const { return mDefinition; }

        void update(void);

        /// Resets the transform of all bones to the binding pose. Manual bones are not reset
        void resetToPose(void);

        /** Sets the given node to manual. Manual bones won't be reset to binding pose
            (@see resetToPose) and thus are suitable for manual control. However if the
            bone is animated, you're responsible for resetting the position/rotation/scale
            before each call to @see update
        @remarks
            You can prevent the bone from receiving animation by setting the bone weight
            to zero. @See SkeletonAnimation::setBoneWeight
        @param bone
            Bone to set/unset to manual. Must belong to this SkeletonInstance (an assert
            is triggered on non-release builds). Behavior is undefined if node doesn't
            belong to this instance.
        @param isManual
            True to set to manual, false to restore it.
        */
        void setManualBone( DiNewBone *bone, bool isManual );

        /** Returns true if the bone is manually controlled. @See setManualBone
        @param bone
            Bone to query if manual. Must belong to this SkeletonInstance.
        */
        bool isManualBone(DiNewBone *bone);

        /// Gets full transform of a bone by its index.
        FORCEINLINE const SimpleMatrixAf4x3& _getBoneFullTransform( size_t index ) const
        {
            return mBones[index]._getFullTransform();
        }

        /// Gets the bone with given name. Throws if not found.
        DiNewBone* getBone( IdString boneName );

        bool hasAnimation( IdString name ) const;
        /// Returns the requested animations. Throws if not found. O(N) Linear search
        SkeletonAnimation* getAnimation( IdString name );

        /// Internal use. Enables given animation. Input should belong to us and not already animated.
        void _enableAnimation( SkeletonAnimation *animation );

        /// Internal use. Disables given animation. Input should belong to us and already being animated.
        void _disableAnimation( SkeletonAnimation *animation );

        /** Sets our parent node so that our bones are in World space.
            Iterates through all our bones and sets the root bones
        */
        void setParentNode(DiNode *parentNode);

        /// Returns our parent node. May be null.
        DiNode* getParentNode(void) const { return mParentNode; }

        void getTransforms( SimpleMatrixAf4x3 * RESTRICT_ALIAS outTransform,
                            const FastArray<unsigned short> &usedBones ) const;

        /** Updates the contents of @mBoneStartTransforms. Needed when our
            memory manager performs a cleanup or similar memory change.
        */
        void _updateBoneStartTransforms(void);

        const TransformArray& _getTransformArray() const        { return mBoneStartTransforms; }

        const void* _getMemoryBlock(void) const;
        const void* _getMemoryUniqueOffset(void) const;
    };

    inline bool OrderSkeletonInstanceByMemory( const SkeletonInstance *_left,
                                                const SkeletonInstance *_right )
    {
        return _left->_getMemoryUniqueOffset() < _right->_getMemoryUniqueOffset();
    }

}

#endif
