#cmake file for project kernel
#author:T
#created:2015-01-12 15:07:04

INCLUDE_DIRECTORIES(
        "${PROJECT_SOURCE_DIR}/Public"
        "${PROJECT_SOURCE_DIR}/API"
        "${PROJECT_SOURCE_DIR}/logic/src/interface"
        )

SET(GATE_FILES
    "${PROJECT_SOURCE_DIR}/logic/src/gate/Gate.cpp"
    "${PROJECT_SOURCE_DIR}/logic/src/gate/ClientConnectSick.cpp"
    "${PROJECT_SOURCE_DIR}/logic/src/gate/main.cpp"
    )
    
SET(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/bin/modules")

ADD_LIBRARY(gate SHARED
        ${GATE_FILES}
        )
