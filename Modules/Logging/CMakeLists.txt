cmake_minimum_required(VERSION 3.22.0)

set(TARGET_NAME Logging)
set(TARGET_FOLDER "Modules")
project(${TARGET_NAME})

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core)

set(SOURCE_FILES
    src/Logger.cpp
)

set(PRIVATE_HEADER_FILES
)

set(PUBLIC_HEADER_FILES
    include/Logger.h
)

set(LIB_TYPE STATIC)
qt6_add_library(${TARGET_NAME} ${LIB_TYPE}
    ${SOURCE_FILES}
    ${PRIVATE_HEADER_FILES}
    ${PUBLIC_HEADER_FILES}
)

# Organize target into a group in Visual Studio IDE
IF (DEFINED TARGET_FOLDER)
	set_property(TARGET ${TARGET_NAME} PROPERTY FOLDER ${TARGET_FOLDER})
ENDIF()

target_link_libraries(${TARGET_NAME} 
    PUBLIC 
        Qt6::Core
)

target_include_directories(${TARGET_NAME} 
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
)