#cmake file for project kernel
#author:T
#created:2015-01-12 15:07:04

INCLUDE_DIRECTORIES(
        "${PROJECT_SOURCE_DIR}/Public"
        "${PROJECT_SOURCE_DIR}/API"
        "${PROJECT_SOURCE_DIR}/logic/src/interface"
        )
        
SET(GATE
    "${PROJECT_SOURCE_DIR}/logic/src/gate/Gate.h"
    "${PROJECT_SOURCE_DIR}/logic/src/gate/Gate.cpp"
    "${PROJECT_SOURCE_DIR}/logic/src/gate/ClientConnectSick.h"
    "${PROJECT_SOURCE_DIR}/logic/src/gate/ClientConnectSick.cpp"
    "${PROJECT_SOURCE_DIR}/logic/src/gate/main.cpp"
    )
    
SOURCE_GROUP(API FILES ${API})
SOURCE_GROUP(interface FILES ${LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${LOGIC_DEFINE})
SOURCE_GROUP(Public\\tinyxml FILES ${TINYXML})
SOURCE_GROUP(Public FILES ${PUBLIC})
SOURCE_GROUP(\\ FILES ${GATE})

ADD_LIBRARY(gate SHARED
        ${GATE}
        ${API}
        ${LOGIC_DEFINE}
        ${LOGIC_INTERFACE}
        ${TINYXML}
        ${PUBLIC}
        )
