cmake_minimum_required(VERSION 3.22.0)

set(TARGET_NAME OtsuThreshold)
set(TARGET_NAME_APP OstuThresholdApp)
set(TARGET_FOLDER "Plugs")
project(${TARGET_NAME} LANGUAGES CXX VERSION 1.0.0.0)

# TODO : Organize the Qt files better instead of relying on generic AUTOUIC, AUTOMOC, AUTORCC and qt6_add_executable
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Charts)

set(PROJECT_SOURCES
    otsuthreshold.cpp
    otsuthreshold.h
    otsuthreshold.ui
    OtsuThresholdExport.h
    OtsuThresholdConfig.h.in
)

set(LIB_TYPE SHARED)
if (${LIB_TYPE} STREQUAL "SHARED") 
    set(OTSU_THRESHOLD_BUILD_SHARED ON)
endif()

configure_file(OtsuThresholdConfig.h.in OtsuThresholdConfig.h)

set(GENERATED_FILES
    ${PROJECT_BINARY_DIR}/OtsuThresholdConfig.h
)

# message(ERROR ${PROJECT_SOURCES})
# message(ERROR ${GENERATED_FILES})

qt6_add_library(${TARGET_NAME} ${LIB_TYPE}
    ${PROJECT_SOURCES}
    ${GENERATED_FILES}
)

if (${LIB_TYPE} STREQUAL "SHARED") 
    target_compile_definitions(${TARGET_NAME} PRIVATE OTSU_THRESHOLD_EXPORTS)
endif()

# Organize target into a group in Visual Studio IDE
IF (DEFINED TARGET_FOLDER)
	set_property(TARGET ${TARGET_NAME} PROPERTY FOLDER ${TARGET_FOLDER})
ENDIF()

target_include_directories(${TARGET_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
)

target_link_libraries(${TARGET_NAME} 
    PRIVATE 
        Qt6::Widgets
        Qt6::Charts
)

set_target_properties(${TARGET_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE FALSE
)

qt6_add_executable(${TARGET_NAME_APP} main.cpp)

# Organize target into a group in Visual Studio IDE
IF (DEFINED TARGET_FOLDER)
	set_property(TARGET ${TARGET_NAME_APP} PROPERTY FOLDER ${TARGET_FOLDER})
ENDIF()

target_link_libraries(${TARGET_NAME_APP}
    PUBLIC
        ${TARGET_NAME}
        Qt6::Widgets
        Qt6::Charts
)

set_target_properties(${TARGET_NAME_APP} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

qt6_setup_windows_environment(${TARGET_NAME_APP})