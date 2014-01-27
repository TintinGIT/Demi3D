
/********************************************************************
    File:       D3D9ShaderParam.h
    Creator:    demiwangya
*********************************************************************/

#pragma once

#include "GpuProgram.h"
#include "ShaderParam.h"
#include "D3D9Driver.h"
#include "D3D9StateCache.h"

namespace Demi
{
    class DI_D3D9DRV_API DiD3D9ShaderParam : public DiShaderParameter
    {
    public:

        DiD3D9ShaderParam(DiMaterial& mat);

        ~DiD3D9ShaderParam();

    public:

        void            LoadParameters();

        void            Bind() const;

        void            AddD3DParameter(const DiString& name, DiShaderType shadertype, uint32 regID);

    private:

        typedef DiHashMap<DiString, DiPair<DiShaderType, uint32>> D3DConstMaps;

        D3DConstMaps    mD3DConsts;
    };
}