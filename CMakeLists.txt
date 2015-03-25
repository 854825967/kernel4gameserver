#cmake file for project kernel
#author:T
#created:2015-01-12 15:07:04

cmake_minimum_required(VERSION 2.8)

PROJECT(T-Kernel)
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
ENDIF(WIN32)

IF(CMAKE_CL_64)
    MESSAGE(STATUS "x64")
ELSE()
    MESSAGE(STATUS "x86")
ENDIF(CMAKE_CL_64)

INCLUDE_DIRECTORIES(
    "${PROJECT_SOURCE_DIR}/Public"
    "${PROJECT_SOURCE_DIR}/API"
    "${PROJECT_SOURCE_DIR}/src/"
    "${PROJECT_SOURCE_DIR}/src/interface"
    "${PROJECT_SOURCE_DIR}/src/epoller"
    )

SET(TINYXML
    "${PROJECT_SOURCE_DIR}/Public/tinyxml/tinystr.cpp"
    "${PROJECT_SOURCE_DIR}/Public/tinyxml/tinystr.h"
    "${PROJECT_SOURCE_DIR}/Public/tinyxml/tinyxml.cpp"
    "${PROJECT_SOURCE_DIR}/Public/tinyxml/tinyxml.h"
    "${PROJECT_SOURCE_DIR}/Public/tinyxml/tinyxmlerror.cpp"
    "${PROJECT_SOURCE_DIR}/Public/tinyxml/tinyxmlparser.cpp"
    )

SET(PUBLIC
    "${PROJECT_SOURCE_DIR}/Public/CArgs.h"
    "${PROJECT_SOURCE_DIR}/Public/CData.h"
    "${PROJECT_SOURCE_DIR}/Public/CLock.h"
    "${PROJECT_SOURCE_DIR}/Public/CLockUnit.h"
    "${PROJECT_SOURCE_DIR}/Public/CRand.h"
    "${PROJECT_SOURCE_DIR}/Public/CThread.h"
    "${PROJECT_SOURCE_DIR}/Public/CThread.cpp"
    "${PROJECT_SOURCE_DIR}/Public/CHashMap.h"
    "${PROJECT_SOURCE_DIR}/Public/Linux.h"
    "${PROJECT_SOURCE_DIR}/Public/Winsys.h"
    "${PROJECT_SOURCE_DIR}/Public/MultiSys.h"
    "${PROJECT_SOURCE_DIR}/Public/MultiSys.cpp"
    "${PROJECT_SOURCE_DIR}/Public/Tools.h"
    "${PROJECT_SOURCE_DIR}/Public/TPool.h"
    "${PROJECT_SOURCE_DIR}/Public/TBundler.h"
    "${PROJECT_SOURCE_DIR}/Public/TCallBack.h"
    "${PROJECT_SOURCE_DIR}/Public/TQueue.h"
    "${PROJECT_SOURCE_DIR}/Public/TStream.h"
    )

SET(API
    "${PROJECT_SOURCE_DIR}/API/IKernel.h"
    "${PROJECT_SOURCE_DIR}/API/IModule.h"
    )

SET(KERNEL_INTERFACE
    "${PROJECT_SOURCE_DIR}/src/interface/ICore.h"
    "${PROJECT_SOURCE_DIR}/src/interface/IConfigmgr.h"
    "${PROJECT_SOURCE_DIR}/src/interface/ILogicmgr.h"
    "${PROJECT_SOURCE_DIR}/src/interface/INetengine.h"
    "${PROJECT_SOURCE_DIR}/src/interface/ITimermgr.h"
    )

SET(KERNEL_CONFIG_MGR
    "${PROJECT_SOURCE_DIR}/src/configmgr/Configmgr.h"
    "${PROJECT_SOURCE_DIR}/src/configmgr/Configmgr.cpp"
    )

SET(KERNEL_EPOLLER
    "${PROJECT_SOURCE_DIR}/src/epoller/header.h"
    "${PROJECT_SOURCE_DIR}/src/epoller/epoll_waiter.h"
    "${PROJECT_SOURCE_DIR}/src/epoller/epoll_waiter.cpp"
    "${PROJECT_SOURCE_DIR}/src/epoller/epoll_worker.h"
    "${PROJECT_SOURCE_DIR}/src/epoller/epoll_worker.cpp"
    "${PROJECT_SOURCE_DIR}/src/epoller/epoller.h"
    "${PROJECT_SOURCE_DIR}/src/epoller/epoller.cpp"
    "${PROJECT_SOURCE_DIR}/src/epoller/ITcpServer.cpp"
    )

SET(KERNEL_IOCPER
    "${PROJECT_SOURCE_DIR}/src/iocper/iocper.h"
    "${PROJECT_SOURCE_DIR}/src/iocper/iocper.cpp"
    "${PROJECT_SOURCE_DIR}/src/iocper/iocpworker.h"
    "${PROJECT_SOURCE_DIR}/src/iocper/iocpworker.cpp"
    "${PROJECT_SOURCE_DIR}/src/iocper/iocphead.h"
    "${PROJECT_SOURCE_DIR}/src/iocper/iocphead.cpp"
    "${PROJECT_SOURCE_DIR}/src/iocper/ITcpServer.cpp"
    )


SET(KERNEL_LOGIC_MGR
    "${PROJECT_SOURCE_DIR}/src/logicmgr/Logicmgr.h"
    "${PROJECT_SOURCE_DIR}/src/logicmgr/Logicmgr.cpp"
    )

SET(KERNEL_TIMER_MGR
    "${PROJECT_SOURCE_DIR}/src/timermgr/TimerHandler.h"
    "${PROJECT_SOURCE_DIR}/src/timermgr/Timermgr.h"
    "${PROJECT_SOURCE_DIR}/src/timermgr/Timermgr.cpp"
    )

SET(KERNEL
        "${PROJECT_SOURCE_DIR}/src/Kernel.h"
        "${PROJECT_SOURCE_DIR}/src/Kernel.cpp"
        "${PROJECT_SOURCE_DIR}/src/main.cpp"
   )
   
SOURCE_GROUP(API FILES ${API})
SOURCE_GROUP(Public\\tinyxml FILES ${TINYXML})
SOURCE_GROUP(Public FILES ${PUBLIC})

SOURCE_GROUP(src\\interface FILES ${KERNEL_INTERFACE})
SOURCE_GROUP(src\\configmgr FILES ${KERNEL_CONFIG_MGR})
SOURCE_GROUP(src\\logicmgr FILES ${KERNEL_LOGIC_MGR})
SOURCE_GROUP(src\\timermgr FILES ${KERNEL_TIMER_MGR})
SOURCE_GROUP(src\\iocper FILES ${KERNEL_IOCPER})
SOURCE_GROUP(src\\epoller FILES ${KERNEL_EPOLLER})
SOURCE_GROUP(src FILES ${KERNEL})

IF(UNIX)
ADD_EXECUTABLE(Kernel
        ${API}
        ${TINYXML}
        ${PUBLIC}
        ${KERNEL_INTERFACE}
        ${KERNEL_CONFIG_MGR}
        ${KERNEL_LOGIC_MGR}
        ${KERNEL_TIMER_MGR}
        ${KERNEL_EPOLLER}
        ${KERNEL}
        )
ENDIF(UNIX)

IF(WIN32)
ADD_EXECUTABLE(Kernel
        ${API}
        ${TINYXML}
        ${PUBLIC}
        ${KERNEL_INTERFACE}
        ${KERNEL_CONFIG_MGR}
        ${KERNEL_LOGIC_MGR}
        ${KERNEL_TIMER_MGR}
        ${KERNEL_IOCPER}
        ${KERNEL}
        )
ENDIF(WIN32)
IF(UNIX)
SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/linux")
ENDIF(UNIX)

IF(WIN32)
SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/windows")
ENDIF(WIN32)

TARGET_LINK_LIBRARIES(Kernel ${STATIC_LIBRARYS})


ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/src/)

