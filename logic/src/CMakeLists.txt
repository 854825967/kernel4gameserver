#cmake file for project kernel
#author:T
#created:2015-01-12 15:07:04

IF(UNIX)
    SET(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/linux")
    SET(PROTO_LIBRARY_DEBUG
        "${PROJECT_SOURCE_DIR}/thirdlibrary/libprotobuf/libprotobuf_debug.a")

    SET(PROTO_LIBRARY_RELEASE
        "${PROJECT_SOURCE_DIR}/thirdlibrary/libprotobuf/libprotobuf_release.a")
ENDIF(UNIX)

IF(WIN32)
    SET(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/windows")
    SET(PROTO_LIBRARY_DEBUG
        "${PROJECT_SOURCE_DIR}/thirdlibrary/libprotobuf/libprotobuf_debug.lib")

    SET(PROTO_LIBRARY_RELEASE
        "${PROJECT_SOURCE_DIR}/thirdlibrary/libprotobuf/libprotobuf_release.lib")
ENDIF(WIN32)
        
SET(LOGIC_INTERFACE
    "${PROJECT_SOURCE_DIR}/logic/src/Interface/IGate.h"
    "${PROJECT_SOURCE_DIR}/logic/src/Interface/IObjectMgr.h"
    "${PROJECT_SOURCE_DIR}/logic/src/Interface/IConnectionSink.h"
    )
    
SET(LOGIC_DEFINE
    "${PROJECT_SOURCE_DIR}/logic/src/Define/ProtocolID.h"
    "${PROJECT_SOURCE_DIR}/logic/src/Define/demo.pb.h"
    "${PROJECT_SOURCE_DIR}/logic/src/Define/demo.pb.cc"
    )

ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/src/Gate)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/src/ObjectMgr)
