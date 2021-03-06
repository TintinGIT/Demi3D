
#-------------------------------------------------------------------
# Configure settings and install targets
if(APPLE)
  macro(set_xcode_property targ xc_prop_name xc_prop_val)
    set_property( TARGET ${targ} PROPERTY XCODE_ATTRIBUTE_${xc_prop_name} ${xc_prop_val} )
  endmacro(set_xcode_property)

  set(MIN_IOS_VERSION "6.0")

  if(NOT DEMI_BUILD_PLATFORM_ANDROID AND NOT DEMI_BUILD_PLATFORM_APPLE_IOS)
    set(PLATFORM_NAME "macosx")
  elseif(DEMI_BUILD_PLATFORM_APPLE_IOS)
    set(PLATFORM_NAME "$(PLATFORM_NAME)")
  endif()
endif()
#-------------------------------------------------------------------

# add a new library target
# usage: demi_add_library(TARGETNAME LIBTYPE SOURCE_FILES [SEPARATE SOURCE_FILES])
MACRO(DI_ADD_LIBRARY TARGETNAME)
  if(DEMI_STATIC)
    add_library(${TARGETNAME} STATIC ${ARGN})
  else(DEMI_STATIC)
    add_library(${TARGETNAME} SHARED ${ARGN})
  endif(DEMI_STATIC)
  
  include_directories(${DEMI_EXTERNAL_SRC_PATH}/LinearMath)
  include_directories(${DEMI_SOURCE_DIR}/src/addons/MyGUI)
  include_directories(${DEMI_SOURCE_DIR}/src/addons/MyGUI/Extends)

  if (DEMI_STATIC)
    # add static prefix, if compiling static version
    set_target_properties(${TARGETNAME} PROPERTIES OUTPUT_NAME ${TARGETNAME}Static)
    set_target_properties(${TARGETNAME} PROPERTIES COMPILE_DEFINITIONS DEMI_STATIC_API)

    if(DEMI_BUILD_PLATFORM_APPLE_IOS)
	    set_xcode_property( ${TARGETNAME} IPHONEOS_DEPLOYMENT_TARGET ${MIN_IOS_VERSION} )
	    set_property( TARGET ${TARGETNAME} PROPERTY XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET[arch=arm64] "7.0" )
	    set_target_properties(${TARGETNAME} PROPERTIES XCODE_ATTRIBUTE_GCC_THUMB_SUPPORT "NO")
	    set_target_properties(${TARGETNAME} PROPERTIES XCODE_ATTRIBUTE_GCC_UNROLL_LOOPS "YES")
	    set_target_properties(${TARGETNAME} PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer")
	    set_target_properties(${TARGETNAME} PROPERTIES XCODE_ATTRIBUTE_GCC_PRECOMPILE_PREFIX_HEADER "YES")
    endif(DEMI_BUILD_PLATFORM_APPLE_IOS)
  else (DEMI_STATIC)
    if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
      # add GCC visibility flags to shared library build
      set_target_properties(${TARGETNAME} PROPERTIES COMPILE_FLAGS "${DEMI_GCC_VISIBILITY_FLAGS}")
      set_target_properties(${TARGETNAME} PROPERTIES XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "${XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN}")
      set_target_properties(${TARGETNAME} PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "NO")
      set_target_properties(${TARGETNAME} PROPERTIES XCODE_ATTRIBUTE_GCC_INLINES_ARE_PRIVATE_EXTERN[arch=x86_64] "YES")
      # disable "lib" prefix on Unix
      set_target_properties(${TARGETNAME} PROPERTIES PREFIX "")
      if (APPLE)
        set_target_properties(${TARGETNAME} PROPERTIES BUILD_WITH_INSTALL_RPATH 1
          INSTALL_NAME_DIR "@executable_path/"
        )
      endif()
    endif (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
  endif (DEMI_STATIC)

ENDMACRO(DI_ADD_LIBRARY)


# add a new executable target
# usage: demi_add_executable(TARGETNAME [WIN32] [MACOSX_BUNDLE] SOURCE_FILES [SEPARATE SOURCE_FILES])
MACRO(DI_ADD_EXECUTABLE TARGETNAME)
  # test if WIN32 or MACOSX_BUNDLE options were provided
  set(_WIN32 "")
  set(_OSX "")
  list(FIND ARGN "WIN32" _W32_IDX)
  if (_W32_IDX GREATER "-1")
    set(_WIN32 "WIN32")
    list(REMOVE_AT ARGN ${_W32_IDX})
  endif ()
  list(FIND ARGN "MACOSX_BUNDLE" _OSX_IDX)
  if (_OSX_IDX GREATER "-1")
    set(_OSX "MACOSX_BUNDLE")
    list(REMOVE_AT ARGN ${_OSX_IDX})
  endif ()
  
  add_executable(${TARGETNAME} ${_WIN32} ${_OSX} ${ARGN})
  include_directories(${DEMI_EXTERNAL_SRC_PATH}/LinearMath)
  include_directories(${DEMI_SOURCE_DIR}/src/addons/MyGUI)
  include_directories(${DEMI_SOURCE_DIR}/src/addons/MyGUI/Extends)
  
  set(DEMI_BIN_NAME ${TARGETNAME})
  
  if (DEMI_STATIC)
	  
	target_link_libraries(${TARGETNAME} ${LUA_LIBRARIES} ${LinearMath_LIBRARIES} DiScript)
	  
    # add static prefix, if compiling static version
	if (NOT DEMI_BUILD_PLATFORM_APPLE_IOS)
		set_target_properties(${TARGETNAME} PROPERTIES OUTPUT_NAME ${TARGETNAME}Static)
		set(DEMI_BIN_NAME ${TARGETNAME}Static)
	else ()
		set_target_properties(${TARGETNAME} PROPERTIES OUTPUT_NAME ${TARGETNAME})		
	endif()
    set_target_properties(${TARGETNAME} PROPERTIES COMPILE_DEFINITIONS DEMI_STATIC_API)

	if (APPLE AND NOT DEMI_BUILD_PLATFORM_APPLE_IOS)
		target_link_libraries(${TARGETNAME} DiDrvGL)
	elseif (APPLE AND DEMI_BUILD_PLATFORM_APPLE_IOS)
		target_link_libraries(${TARGETNAME} DiDrvGLES2)
		target_link_libraries(${TARGETNAME} ${OPENGLES2_LIBRARIES} ${EGL_LIBRARIES})
	elseif (WIN32)
		target_link_libraries(${TARGETNAME} DiDrvD3D9 DiDrvGL DiDrvGLES2)
	endif()
  endif()

  if (APPLE)
    # On OS X, create .app bundle
	  set(MACOSX_BUNDLE_EXENAME ${DEMI_BIN_NAME})
    set_property(TARGET ${TARGETNAME} PROPERTY MACOSX_BUNDLE TRUE)
	  set_property(TARGET ${TARGETNAME} PROPERTY MACOSX_BUNDLE_INFO_PLIST ${DEMI_SOURCE_DIR}/cmake/iosres/Info.plist)
    set_target_properties(${TARGETNAME} PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "NO")
	
    if (NOT DEMI_BUILD_PLATFORM_APPLE_IOS)
        # Add the path where the Demi3D framework was found
        if(${DEMI_FRAMEWORK_PATH})
          set_target_properties(${TARGETNAME} PROPERTIES
            COMPILE_FLAGS "-F${DEMI_FRAMEWORK_PATH}"
            LINK_FLAGS "-F${DEMI_FRAMEWORK_PATH}"
          )
        endif()
	    
  	    set (DEMI_SAMPLE_PATH 
  	    	${DEMI_SOURCE_DIR}/bin/$(CONFIGURATION)/${DEMI_BIN_NAME}.app)
  	    set (DEMI_SAMPLE_BIN_PATH 
  	    	${DEMI_SOURCE_DIR}/bin/$(CONFIGURATION)/${DEMI_BIN_NAME}.app/Contents/MacOS)
	    	
  	    add_custom_command(TARGET ${TARGETNAME} POST_BUILD
  	    	COMMAND ln ARGS -s -f ${DEMI_SOURCE_DIR}/media ${DEMI_SAMPLE_PATH}/
  	    	COMMAND ln ARGS -s -f ${DEMI_SOURCE_DIR}/media_hon ${DEMI_SAMPLE_PATH}/
		    	COMMAND ln ARGS -s -f ${DEMI_SOURCE_DIR}/scripts ${DEMI_SAMPLE_PATH}/
  	    	)
  	    add_custom_command(TARGET ${TARGETNAME} POST_BUILD
  	    	COMMAND ln ARGS -s -f ${DEMI_SOURCE_DIR}/lib/$(CONFIGURATION)/DiMisc.dylib ${DEMI_SAMPLE_BIN_PATH}/
  	    	COMMAND ln ARGS -s -f ${DEMI_SOURCE_DIR}/lib/$(CONFIGURATION)/DiGfx.dylib ${DEMI_SAMPLE_BIN_PATH}/
  	    	COMMAND ln ARGS -s -f ${DEMI_SOURCE_DIR}/lib/$(CONFIGURATION)/DiDrvGL.dylib ${DEMI_SAMPLE_BIN_PATH}/
  	    	COMMAND ln ARGS -s -f ${DEMI_SOURCE_DIR}/lib/$(CONFIGURATION)/DemoFramework.dylib ${DEMI_SAMPLE_BIN_PATH}/
  	    	COMMAND ln ARGS -s -f ${DEMI_SOURCE_DIR}/lib/$(CONFIGURATION)/DiK2.dylib ${DEMI_SAMPLE_BIN_PATH}/
			    COMMAND ln ARGS -s -f ${DEMI_SOURCE_DIR}/lib/$(CONFIGURATION)/DiScript.dylib ${DEMI_SAMPLE_BIN_PATH}/
          COMMAND ln ARGS -s -f ${DEMI_SOURCE_DIR}/lib/$(CONFIGURATION)/MyGUI.dylib ${DEMI_SAMPLE_BIN_PATH}/
  	    	)		
    else()
	    set_xcode_property( ${TARGETNAME} IPHONEOS_DEPLOYMENT_TARGET ${MIN_IOS_VERSION} )
	    set_property( TARGET ${TARGETNAME} PROPERTY XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET[arch=arm64] "7.0" )
	    set_target_properties(${TARGETNAME} PROPERTIES XCODE_ATTRIBUTE_GCC_THUMB_SUPPORT "NO")
	    set_target_properties(${TARGETNAME} PROPERTIES XCODE_ATTRIBUTE_GCC_UNROLL_LOOPS "YES")
	    set_target_properties(${TARGETNAME} PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer")
	    set_target_properties(${TARGETNAME} PROPERTIES XCODE_ATTRIBUTE_GCC_PRECOMPILE_PREFIX_HEADER "YES")
      set_target_properties(${TARGETNAME} PROPERTIES XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2")
		
		set(DEMI_SAMPLE_CONTENTS_PATH ${DEMI_SOURCE_DIR}/bin/$(CONFIGURATION)/${DEMI_BIN_NAME}.app)
	       add_custom_command(TARGET ${TARGETNAME} POST_BUILD
         COMMAND ditto ${DEMI_SOURCE_DIR}/cmake/iosres/*.png ${DEMI_SAMPLE_CONTENTS_PATH}/
	    #  COMMAND ditto ${OGRE_BINARY_DIR}/bin/*.cfg ${OGRE_SAMPLE_CONTENTS_PATH}/
	    )
	    add_custom_command(TARGET ${TARGETNAME} POST_BUILD
	      COMMAND ditto ${DEMI_SOURCE_DIR}/media ${DEMI_SAMPLE_CONTENTS_PATH}/media
		    COMMAND ditto ${DEMI_SOURCE_DIR}/scripts ${DEMI_SAMPLE_CONTENTS_PATH}/scripts
	    )
    endif(NOT DEMI_BUILD_PLATFORM_APPLE_IOS)
	
  endif (APPLE)
  
  if (NOT DEMI_STATIC)
    if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
      # add GCC visibility flags to shared library build
      set_target_properties(${TARGETNAME} PROPERTIES COMPILE_FLAGS "${DEMI_GCC_VISIBILITY_FLAGS}")
      set_target_properties(${TARGETNAME} PROPERTIES XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "${XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN}")
      set_target_properties(${TARGETNAME} PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "NO")
      set_target_properties(${TARGETNAME} PROPERTIES XCODE_ATTRIBUTE_GCC_INLINES_ARE_PRIVATE_EXTERN[arch=x86_64] "YES")
      # disable "lib" prefix on Unix
      set_target_properties(${TARGETNAME} PROPERTIES PREFIX "")
    endif (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
  endif()
ENDMACRO(DI_ADD_EXECUTABLE)

MACRO(DI_CONFIG_CORE_LIB TARGETNAME PREFIX_HEADER)
	if(WIN32 AND MSVC10 AND CMAKE_CL_64)
	  set_target_properties(${TARGETNAME} PROPERTIES 
							LINK_FLAGS "/INCREMENTAL:NO"
						  )
	endif()

	if (APPLE)
	  if (DEMI_BUILD_PLATFORM_APPLE_IOS)
		set_target_properties(${TARGETNAME} PROPERTIES INSTALL_NAME_DIR ${TARGETNAME})
		set_target_properties(${TARGETNAME} PROPERTIES COMPILE_DEFINITIONS DEMI_STATIC_API)
	  else ()
		#set_target_properties(${TARGETNAME} PROPERTIES FRAMEWORK TRUE)
		## Set the INSTALL_PATH so that Frameworks can be local
		#et_target_properties(${TARGETNAME}
		#   PROPERTIES BUILD_WITH_INSTALL_RPATH 1
		#   INSTALL_NAME_DIR "@executable_path/../Frameworks"
		#)
		set_target_properties(${TARGETNAME} PROPERTIES XCODE_ATTRIBUTE_GCC_PRECOMPILE_PREFIX_HEADER "YES")
		set_target_properties(${TARGETNAME} PROPERTIES XCODE_ATTRIBUTE_GCC_PREFIX_HEADER ${PREFIX_HEADER})
		set_target_properties(${TARGETNAME} PROPERTIES XCODE_ATTRIBUTE_GCC_UNROLL_LOOPS "YES")
	  endif ()

	  # Framework is called 'DEMI'
	  set_target_properties(${TARGETNAME} PROPERTIES OUTPUT_NAME ${TARGETNAME})
	endif ()

	if (MINGW)
	  # may need winsock htons functions for FreeImage
	  target_link_libraries(${TARGETNAME} ws2_32)
	endif ()	
ENDMACRO(DI_CONFIG_CORE_LIB)