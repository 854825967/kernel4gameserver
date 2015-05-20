#cmake file for project kernel
#author:T
#created:2015-01-12 15:07:04

IF(UNIX)
    SET(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/linux")
    SET(PROTO_LIBRARY_DEBUG
        "libprotobuf.a")

    SET(PROTO_LIBRARY_RELEASE
        "libprotobuf.a")

    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
ENDIF(UNIX)

IF(WIN32)
    SET(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/windows")
    SET(PROTO_LIBRARY_DEBUG
        "${PROJECT_SOURCE_DIR}/thirdlibrary/libprotobuf/libprotobuf_debug.lib")

    SET(PROTO_LIBRARY_RELEASE
        "${PROJECT_SOURCE_DIR}/thirdlibrary/libprotobuf/libprotobuf_release.lib")
ENDIF(WIN32)
        
SET(LOGIC_INTERFACE
    "${PROJECT_SOURCE_DIR}/src/logic/Interface/IGate.h"
    )
    
SET(LOGIC_DEFINE
    )

ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/logic/Gate)
