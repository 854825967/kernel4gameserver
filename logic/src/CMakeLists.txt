#cmake file for project kernel
#author:T
#created:2015-01-12 15:07:04

IF(UNIX)
SET(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/linux")
ENDIF(UNIX)

IF(WIN32)
SET(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/windows")
ENDIF(WIN32)
        
SET(LOGIC_INTERFACE
    "${PROJECT_SOURCE_DIR}/logic/src/Interface/IGate.h"
    "${PROJECT_SOURCE_DIR}/logic/src/Interface/IObjectMgr.h"
    )
    
SET(LOGIC_DEFINE
    "${PROJECT_SOURCE_DIR}/logic/src/Define/ProtocolID.h"
    )
	
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/src/Gate)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/src/ObjectMgr)
