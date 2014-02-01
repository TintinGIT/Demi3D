
/********************************************************************
    File:       OSXGLUtil.h
    Creator:    demiwangya
*********************************************************************/

#ifndef OSXGLUtil_h__
#define OSXGLUtil_h__

#include "GLUtil.h"

namespace Demi
{
    class DI_GLDRV_API DiOSXGLUtil : public DiGLUtil
    {
    public:

        DiOSXGLUtil();

        ~DiOSXGLUtil();

    public:

        void*           GetProcAddress(const DiString& procname);

        DiWindow*       CreateNewWindow();

    private:

        DiVector<int>   mFSAALevels;

        bool            mHasPixelFormatARB;

        bool            mHasMultisample;

        bool            mHasHardwareGamma;
    };
}

#endif