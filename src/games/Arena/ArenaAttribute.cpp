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

#include "ArenaPch.h"
#include "ArenaAttribute.h"
#include "XMLFile.h"
#include "K2Configs.h"

namespace Demi
{
    void ArAttribute::LoadAttributeConfig(const DiString& configfile)
    {
        DI_LOG("Loading atribute[config = %s] for entity[id = %d]", configfile.c_str(), GetID());

        auto file = DiK2Configs::GetDataStream(configfile, K2_RES_XML);

        if (!file)
        {
            DI_WARNING("Failed to load the attribute");
            return;
        }

        shared_ptr<DiXMLFile> xmlfile(new DiXMLFile());
        xmlfile->Load(file->GetAsString());
        DiXMLElement root = xmlfile->GetRoot();

        LoadAttribute(configfile.ExtractDirName(), root);
    }
}