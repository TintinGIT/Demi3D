
/********************************************************************
    File:       GLPrerequisites.h
    Creator:    demiwangya
*********************************************************************/

#pragma once

namespace Demi
{
    class DiGLStateCache;
    class DiGLDriver;
    class DiGLContext;
    class DiGLUtil;
    class DiGLBufferManager;
    class DiGLTextureDrv;
    class DiGLFrameBuffer;
    class DiGLDepthBuffer;
    class DiGLRenderBuffer;
    class DiGLFBOManager;
    class DiGLWindowTarget;
    class DiGLGpuVariable;
    class DiGLShaderLinker;
    class DiGLShaderParam;

#ifdef WIN32
    class DiWin32GLUtil;
    class DiWin32GLContext;
#endif
}