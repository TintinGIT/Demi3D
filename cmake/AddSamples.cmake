
MACRO(DI_ADD_SAMPLE TARGETNAME)

project(${TARGETNAME})

file(GLOB SAMPLE_HEADERS "*.h")
file(GLOB SAMPLE_SRCS "*.cpp")

DI_ADD_EXECUTABLE(${TARGETNAME} ${SAMPLE_HEADERS} ${SAMPLE_SRCS})
include_directories(${OIS_INCLUDE_DIRS} ${DEMI_SOURCE_DIR}/src/demi
				 	${DEMI_SOURCE_DIR}/src/demi/demoFrame 
				 	${DEMI_SOURCE_DIR}/src/addons/K2
				 	${DEMI_SOURCE_DIR}/src/addons/K2/Fx 
				 	${DEMI_SOURCE_DIR}/src/demi/script)

if (NOT APPLE)
	set_property(TARGET ${TARGETNAME} APPEND PROPERTY DEBUG_POSTFIX "_d")
endif ()

set_property(TARGET ${TARGETNAME} PROPERTY FOLDER "Samples")

target_link_libraries(${TARGETNAME} DemoFramework DiMisc DiGfx DiK2 DiScript ${OIS_LIBRARIES})

ENDMACRO(DI_ADD_SAMPLE)


MACRO(DI_ADD_TEST TARGETNAME)

project(${TARGETNAME})

file(GLOB SAMPLE_HEADERS "*.h")
file(GLOB SAMPLE_SRCS "*.cpp")

DI_ADD_EXECUTABLE(${TARGETNAME} ${SAMPLE_HEADERS} ${SAMPLE_SRCS})
include_directories(${OIS_INCLUDE_DIRS} ${DEMI_SOURCE_DIR}/src/demi
				 	${DEMI_SOURCE_DIR}/src/demi/demoFrame 
				 	${DEMI_SOURCE_DIR}/src/addons/K2 
				 	${DEMI_SOURCE_DIR}/src/addons/K2/Fx 
				 	${DEMI_SOURCE_DIR}/src/demi/script)

if (NOT APPLE)
	set_property(TARGET ${TARGETNAME} APPEND PROPERTY DEBUG_POSTFIX "_d")
endif ()

set_property(TARGET ${TARGETNAME} PROPERTY FOLDER "Tests")

target_link_libraries(${TARGETNAME} DemoFramework DiMisc DiGfx DiK2 DiScript ${OIS_LIBRARIES})

ENDMACRO(DI_ADD_TEST)
