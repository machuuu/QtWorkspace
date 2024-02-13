cmake_minimum_required(VERSION 3.22.0)

set(TARGET_NAME TestDeviceApp)
set(TARGET_FOLDER "Modules")
project(${TARGET_NAME})

add_executable(${TARGET_NAME} main.cpp)

# Organize target into a group in Visual Studio IDE
IF (DEFINED TARGET_FOLDER)
	set_property(TARGET ${TARGET_NAME} PROPERTY FOLDER ${TARGET_FOLDER})
ENDIF()

target_link_libraries(${TARGET_NAME} PRIVATE TestDevice)