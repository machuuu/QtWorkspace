cmake_minimum_required(VERSION 3.22.0)

set(TARGET_NAME TestDevice)
set(TARGET_FOLDER "Modules")
project(${TARGET_NAME})

set(SOURCE_FILES
    src/TestDeviceAPI.cpp
)

set(PRIVATE_HEADER_FILES

)

set(PUBLIC_HEADER_FILES
    include/TestDeviceAPI.h
)

set(LIB_TYPE SHARED)
add_library(${TARGET_NAME} ${LIB_TYPE}
    ${SOURCE_FILES}
    ${PRIVATE_HEADER_FILES}
    ${PUBLIC_HEADER_FILES}
)

# Organize target into a group in Visual Studio IDE
IF (DEFINED TARGET_FOLDER)
	set_property(TARGET ${TARGET_NAME} PROPERTY FOLDER ${TARGET_FOLDER})
ENDIF()

target_include_directories(${TARGET_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)