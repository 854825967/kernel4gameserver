#cmake file for project kernel
#author:T
#created:2015-01-12 15:07:04

IF(LINUX)
SET(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/linux")
ENDIF(LINUX)

IF(WIN32)
SET(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/windows")
ENDIF(WIN32)
        
SET(LOGIC_INTERFACE
    "${PROJECT_SOURCE_DIR}/logic/src/interface/IGate.h"
    "${PROJECT_SOURCE_DIR}/logic/src/interface/IObjectMgr.h"
    )
    
SET(LOGIC_DEFINE
    "${PROJECT_SOURCE_DIR}/logic/src/define/ProtocolID.h"
    )
	
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/src/Gate)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/src/ObjectMgr)