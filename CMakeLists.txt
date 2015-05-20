#cmake file for project kernel
#author:T
#created:2015-01-12 15:07:04

cmake_minimum_required(VERSION 2.8)
PROJECT(T-Kernel)
option(USE_32BITS "Compile for 32bits processors(linux only)" OFF)
IF(UNIX)
    SET(CMAKE_CXX_FLAGS "-g -std=c++0x -Wall -lpthread -rdynamic -ldl -Dlinux -D_DEBUG")
    SET(CMAKE_CXX_FLAGS_DEBUG "-g -std=c++0x -lpthread -rdynamic -ldl -Dlinux -D_DEBUG")
    SET(CMAKE_CXX_FLAGS_RELEASE "-g -std=c++0x -lpthread -rdynamic -ldl -Dlinux")
    MESSAGE(STATUS "Linux")
ENDIF(UNIX)

IF(WIN32)
    SET(CMAKE_C_FLAGS_DEBUG_INIT "/D_DEBUG /Zm8192000 /MTd /Zi /Ob0 /Od /RTC1") 
    SET(CMAKE_C_FLAGS_MINSIZEREL_INIT "/MT /Zm8192000 /O1 /Ob1 /D NDEBUG") 
    SET(CMAKE_C_FLAGS_RELEASE_INIT "/MT /Zm8192000 /O2 /Ob2 /D NDEBUG") 
    SET(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "/MT /Zm8192000 /Zi /O2 /Ob1 /D NDEBUG")
    MESSAGE(STATUS "WINDOWS")
    IF(CMAKE_CL_64)
        MESSAGE(STATUS "x64")
    ELSE()
        MESSAGE(STATUS "x86")
    ENDIF(CMAKE_CL_64)
ENDIF(WIN32)

SET(TINYXML
    "${PROJECT_SOURCE_DIR}/src/Public/tinyxml/tinystr.cpp"
    "${PROJECT_SOURCE_DIR}/src/Public/tinyxml/tinystr.h"
    "${PROJECT_SOURCE_DIR}/src/Public/tinyxml/tinyxml.cpp"
    "${PROJECT_SOURCE_DIR}/src/Public/tinyxml/tinyxml.h"
    "${PROJECT_SOURCE_DIR}/src/Public/tinyxml/tinyxmlerror.cpp"
    "${PROJECT_SOURCE_DIR}/src/Public/tinyxml/tinyxmlparser.cpp"
    )

SET(PUBLIC
    "${PROJECT_SOURCE_DIR}/src/Public/CArgs.h"
    "${PROJECT_SOURCE_DIR}/src/Public/CDumper.h"
    "${PROJECT_SOURCE_DIR}/src/Public/CDumper.cpp"
    "${PROJECT_SOURCE_DIR}/src/Public/CData.h"
    "${PROJECT_SOURCE_DIR}/src/Public/CLock.h"
    "${PROJECT_SOURCE_DIR}/src/Public/CLockUnit.h"
    "${PROJECT_SOURCE_DIR}/src/Public/CRand.h"
    "${PROJECT_SOURCE_DIR}/src/Public/CThread.h"
    "${PROJECT_SOURCE_DIR}/src/Public/CThread.cpp"
    "${PROJECT_SOURCE_DIR}/src/Public/CHashMap.h"
    "${PROJECT_SOURCE_DIR}/src/Public/Linux.h"
    "${PROJECT_SOURCE_DIR}/src/Public/Winsys.h"
    "${PROJECT_SOURCE_DIR}/src/Public/MultiSys.h"
    "${PROJECT_SOURCE_DIR}/src/Public/MultiSys.cpp"
    "${PROJECT_SOURCE_DIR}/src/Public/Tools.h"
    "${PROJECT_SOURCE_DIR}/src/Public/TPool.h"
    "${PROJECT_SOURCE_DIR}/src/Public/TBundler.h"
    "${PROJECT_SOURCE_DIR}/src/Public/TCallBack.h"
    "${PROJECT_SOURCE_DIR}/src/Public/TQueue.h"
    "${PROJECT_SOURCE_DIR}/src/Public/TStream.h"
    )

SET(API
    "${PROJECT_SOURCE_DIR}/src/API/IKernel.h"
    "${PROJECT_SOURCE_DIR}/src/API/IModule.h"
    )
    
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/kernel/)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/logic/)

