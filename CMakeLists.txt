#cmake file for project kernel
#author:T
#created:2015-01-12 15:07:04

SET(CMAKE_CXX_FLAGS "-g -std=c++0x -Wall -lpthread -rdynamic -ldl -Dlinux -D_DEBUG")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -std=c++0x -lpthread -rdynamic -ldl -Dlinux -D_DEBUG")
SET(CMAKE_CXX_FLAGS_RELEASE "-g -std=c++0x -lpthread -rdynamic -ldl -Dlinux")
MESSAGE(STATUS "Linux")

INCLUDE_DIRECTORIES(
        "${PROJECT_SOURCE_DIR}/Public"
        "${PROJECT_SOURCE_DIR}/API"
        "${PROJECT_SOURCE_DIR}/src/"
        "${PROJECT_SOURCE_DIR}/src/interface"
        "${PROJECT_SOURCE_DIR}/src/epoller"
        )

SET(PUBLIC_FILES
        "${PROJECT_SOURCE_DIR}/Public/MultiSys.cc"
        "${PROJECT_SOURCE_DIR}/Public/CThread.cpp"
        "${PROJECT_SOURCE_DIR}/Public/tinyxml/tinystr.cpp"
        "${PROJECT_SOURCE_DIR}/Public/tinyxml/tinyxml.cpp"
        "${PROJECT_SOURCE_DIR}/Public/tinyxml/tinyxmlerror.cpp"
        "${PROJECT_SOURCE_DIR}/Public/tinyxml/tinyxmlparser.cpp"
   )

SET(KERNEL_FILES
        "${PROJECT_SOURCE_DIR}/src/epoller/ITcpServer.cpp"
        "${PROJECT_SOURCE_DIR}/src/epoller/epoll_waiter.cpp"
        "${PROJECT_SOURCE_DIR}/src/epoller/epoll_worker.cpp"
        "${PROJECT_SOURCE_DIR}/src/epoller/epoller.cpp"
        "${PROJECT_SOURCE_DIR}/src/logicmgr/Logicmgr.cpp"
        "${PROJECT_SOURCE_DIR}/src/configmgr/Configmgr.cpp"
        "${PROJECT_SOURCE_DIR}/src/timermgr/Timermgr.cpp"
        "${PROJECT_SOURCE_DIR}/src/Kernel.cpp"
        "${PROJECT_SOURCE_DIR}/src/main.cpp"
   )

SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/bin")

ADD_EXECUTABLE(kernel
        ${PUBLIC_FILES}
        ${KERNEL_FILES}
        )

TARGET_LINK_LIBRARIES(kernel ${STATIC_LIBRARYS})

ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/src/)

