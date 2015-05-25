#cmake file for project kernel
#author:T
#created:2015-01-12 15:07:04

INCLUDE_DIRECTORIES(
        "${PROJECT_SOURCE_DIR}/src/Public"
        "${PROJECT_SOURCE_DIR}/src/API"
        "${PROJECT_SOURCE_DIR}/src/logic/Interface"
        "${PROJECT_SOURCE_DIR}/src/logic/Define"
        )
        
SET(GATE
    "${PROJECT_SOURCE_DIR}/src/logic/Gate/Gate.h"
    "${PROJECT_SOURCE_DIR}/src/logic/Gate/Gate.cpp"
    "${PROJECT_SOURCE_DIR}/src/logic/Gate/ClientConnectSink.h"
    "${PROJECT_SOURCE_DIR}/src/logic/Gate/ClientConnectSink.cpp"
    "${PROJECT_SOURCE_DIR}/src/logic/Gate/main.cpp"
    )
    
SOURCE_GROUP(API FILES ${API})
SOURCE_GROUP(Interface FILES ${LOGIC_INTERFACE})
SOURCE_GROUP(Define FILES ${LOGIC_DEFINE})
SOURCE_GROUP(Public\\tinyxml FILES ${TINYXML})
SOURCE_GROUP(Public FILES ${PUBLIC})
SOURCE_GROUP(\\ FILES ${GATE})

ADD_LIBRARY(Gate SHARED
        ${GATE}
        ${API}
        ${LOGIC_DEFINE}
        ${LOGIC_INTERFACE}
        ${TINYXML}
        ${PUBLIC}
        )

#TARGET_LINK_LIBRARIES(Gate 
#    debug ${PROTO_LIBRARY_DEBUG}
#    optimized ${PROTO_LIBRARY_RELEASE})