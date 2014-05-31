
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

/****************************************************
  This module was originally from Particle Universe
  Repo: https://github.com/scrawl/particleuniverse
  License: MIT 
****************************************************/

#ifndef FxTokensParser_h__
#define FxTokensParser_h__

#include "FxPrerequisites.h"

#include "ParticleEmitter.h"
#include "BillboardRender.h"

namespace Demi
{
    class DiFxTokensParser
    {
    public:

        DiFxTokensParser();

        ~DiFxTokensParser();

        static DiFxTokensParser* Get() { return sParser;  }

    public:

        void    WriteRenderer(DiParticleRenderer* val, DiXMLElement& node);

        void    ReadRenderer(DiParticleRenderer* val, DiXMLElement& node);

        void    WriteBillboardRenderer(DiBillboardRenderer* val, DiXMLElement& node);

        void    ReadBillboardRenderer(DiBillboardRenderer* val, DiXMLElement& node);

        void    WriteElement(DiParticleElement* val, DiXMLElement& node);

        void    ReadElement(DiParticleElement* val, DiXMLElement& node);

        void    WriteSystem(DiParticleSystem* val, DiXMLElement& node);

        void    ReadSystem(DiParticleSystem* val, DiXMLElement& node);

    public:

        void    WriteBaseEmitter(DiParticleEmitter* val, DiXMLElement& node);

        void    ReadBaseEmitter(DiParticleEmitter* val, DiXMLElement& node);

        void    WriteBoxEmitter(DiParticleEmitter* val, DiXMLElement& node);

        void    ReadBoxEmitter(DiParticleEmitter* val, DiXMLElement& node);

        void    WriteCircleEmitter(DiParticleEmitter* val, DiXMLElement& node);

        void    ReadCircleEmitter(DiParticleEmitter* val, DiXMLElement& node);

        void    WriteLineEmitter(DiParticleEmitter* val, DiXMLElement& node);

        void    ReadLineEmitter(DiParticleEmitter* val, DiXMLElement& node);

    public:

        void    WriteController(DiParticleController* val, DiXMLElement& node);

        void    ReadController(DiParticleController* val, DiXMLElement& node);

        void    WriteBaseForceController(DiParticleController* val, DiXMLElement& node);

        void    ReadBaseForceController(DiParticleController* val, DiXMLElement& node);

        void    WriteLinearForceController(DiParticleController* val, DiXMLElement& node);

        void    ReadLinearForceController(DiParticleController* val, DiXMLElement& node);

        void    WriteSinForceController(DiParticleController* val, DiXMLElement& node);

        void    ReadSinForceController(DiParticleController* val, DiXMLElement& node);

        void    WriteGravityController(DiParticleController* val, DiXMLElement& node);

        void    ReadGravityController(DiParticleController* val, DiXMLElement& node);

        void    WriteJetController(DiParticleController* val, DiXMLElement& node);

        void    ReadJetController(DiParticleController* val, DiXMLElement& node);

        void    WriteRandomiserController(DiParticleController* val, DiXMLElement& node);

        void    ReadRandomiserController(DiParticleController* val, DiXMLElement& node);

        void    WriteVortexController(DiParticleController* val, DiXMLElement& node);

        void    ReadVortexController(DiParticleController* val, DiXMLElement& node);

    protected:

        DiVector<DiFloatRect*>  ReadFloatRectVector(const DiString& val);

        DiString                WriteFloatRectVector(const DiVector<DiFloatRect*>& val);

    protected:

        static DiFxTokensParser*               sParser;

        DiMap<DiString, BillboardType>         sStrBillboardType;
        DiMap<BillboardType, DiString>         sBillboardTypeStr;

        DiMap<DiString, BillboardOrigin>       sStrBillboardOrigin;
        DiMap<BillboardOrigin, DiString>       sBillboardOriginStr;

        DiMap<DiString, BillboardRotationType> sStrBillboardRotationType;
        DiMap<BillboardRotationType, DiString> sBillboardRotationTypeStr;
    };
}

#endif // PointEmitter_h__
