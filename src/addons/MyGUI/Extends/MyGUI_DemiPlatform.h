/*!
	@file
	@author		Losev Vasiliy aka bool
	@date		06/2009
*/

#ifndef __MYGUI_DEMI_PLATFORM_H__
#define __MYGUI_DEMI_PLATFORM_H__

#include "DebugAssert.h"
#include "MyGUI_Prerequest.h"
#include "MyGUI_DemiRenderManager.h"
#include "MyGUI_DemiDataManager.h"
#include "MyGUI_DemiTexture.h"
#include "MyGUI_DemiVertexBuffer.h"
#include "MyGUI_DemiDiagnostic.h"
#include "MyGUI_LogManager.h"

namespace MyGUI
{
	class DemiPlatform
	{
	public:
		DemiPlatform() :
			mIsInitialise(false)
		{
			mLogManager = new LogManager();
			mRenderManager = new DemiRenderManager();
			mDataManager = new DemiDataManager();
		}

		~DemiPlatform()
		{
			DI_ASSERT(!mIsInitialise);
			delete mRenderManager;
			delete mDataManager;
			delete mLogManager;
		}

		void initialise()
		{
            DI_ASSERT(!mIsInitialise);
			mIsInitialise = true;
			mRenderManager->initialise();
			mDataManager->initialise();
		}

		void shutdown()
		{
            DI_ASSERT(mIsInitialise);
			mIsInitialise = false;

			mRenderManager->shutdown();
			mDataManager->shutdown();
		}

		DemiRenderManager* getRenderManagerPtr()
		{
            DI_ASSERT(mIsInitialise);
			return mRenderManager;
		}

		DemiDataManager* getDataManagerPtr()
		{
            DI_ASSERT(mIsInitialise);
			return mDataManager;
		}

	private:
		bool mIsInitialise;
		DemiRenderManager* mRenderManager;
		DemiDataManager* mDataManager;
		LogManager* mLogManager;

	};

} // namespace MyGUI

#endif // __MYGUI_DIRECTX_PLATFORM_H__
