cmake_minimum_required(VERSION 3.22.0)

set(TARGET_NAME PlugTalk)
set(TARGET_FOLDER "Apps")
project(${TARGET_NAME} LANGUAGES CXX VERSION 1.0.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(PROJECT_UI_SOURCES
    mainwindow.ui
)
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    ${PROJECT_UI_SOURCES}
)

source_group("UI" FILES ${PROJECT_UI_SOURCES})

qt6_add_executable(${TARGET_NAME}
    ${PROJECT_SOURCES}
)

# Organize target into a group in Visual Studio IDE
IF (DEFINED TARGET_FOLDER)
	set_property(TARGET ${TARGET_NAME} PROPERTY FOLDER ${TARGET_FOLDER})
ENDIF()

target_link_libraries(${TARGET_NAME} PRIVATE Qt6::Widgets)

set_target_properties(${TARGET_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

qt6_setup_windows_environment(${TARGET_NAME})