#ifndef KyObject_h__
#define KyObject_h__

#include "KyDefines.h"
#include "KyHandle.h"
#include "KyAttribute.h"

class KyObject : public KyMainContext , public DiBase
{
	KyDeclareRootRTTI(KyObject);

    friend class KyRefHandle;
    friend class KyObjectMgr;
public:
    KyObject(){}
    virtual ~KyObject(){}

    const KyHandle& GetHandle() const { return mHandle; }
    int32           GetRefCount() const { return mRefCount; }
    void            SetCanRemove(bool bCanRemove) { mCanRemove = bCanRemove; }
    bool            GetCanRemove() const { return mCanRemove; }

    virtual void    Update(float deltaTime) = 0;

private:
    void      _SetHandle(const KyHandle& rkHandle) { mHandle = rkHandle; }
    void      _IncRefCount() { ++mRefCount; }
    void      _DecRefCount() { --mRefCount; }

private:
    KyHandle     mHandle;
    int32        mRefCount{ 0 };
    bool         mCanRemove{ false };

    KyAttribute* mAttribute{ nullptr };
};

#endif
