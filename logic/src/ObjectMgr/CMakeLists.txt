#cmake file for project kernel
#author:T
#created:2015-01-12 15:07:04

INCLUDE_DIRECTORIES(
        "${PROJECT_SOURCE_DIR}/Public"
        "${PROJECT_SOURCE_DIR}/API"
        "${PROJECT_SOURCE_DIR}/logic/src/Interface"
        "${PROJECT_SOURCE_DIR}/logic/src/Define"
        )
        
SET(OBJECTMGR
    "${PROJECT_SOURCE_DIR}/logic/src/ObjectMgr/Attrib.h"
    "${PROJECT_SOURCE_DIR}/logic/src/ObjectMgr/Attrib.cpp"
    "${PROJECT_SOURCE_DIR}/logic/src/ObjectMgr/AttributeMgr.h"
    "${PROJECT_SOURCE_DIR}/logic/src/ObjectMgr/AttributeMgr.cpp"
    "${PROJECT_SOURCE_DIR}/logic/src/ObjectMgr/Object.h"
    "${PROJECT_SOURCE_DIR}/logic/src/ObjectMgr/Object.cpp"
    "${PROJECT_SOURCE_DIR}/logic/src/ObjectMgr/ObjectMgr.h"
    "${PROJECT_SOURCE_DIR}/logic/src/ObjectMgr/ObjectMgr.cpp"
    "${PROJECT_SOURCE_DIR}/logic/src/ObjectMgr/main.cpp"
    )
    
SOURCE_GROUP(API FILES ${API})
SOURCE_GROUP(interface FILES ${LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${LOGIC_DEFINE})
SOURCE_GROUP(Public\\tinyxml FILES ${TINYXML})
SOURCE_GROUP(Public FILES ${PUBLIC})
SOURCE_GROUP(\\ FILES ${OBJECTMGR})

ADD_LIBRARY(ObjectMgr SHARED
        ${OBJECTMGR}
        ${API}
        ${LOGIC_DEFINE}
        ${LOGIC_INTERFACE}
        ${TINYXML}
        ${PUBLIC}
        )
