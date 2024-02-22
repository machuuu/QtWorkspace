cmake_minimum_required(VERSION 3.22.0)

project(QtWorkspace)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(SetupLibraries)
include(QtUtility)

# Use solution folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Use Qt Auto Generation and group in Generated Files folder
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set_property(GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP "Generated Files")

# IF( NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
#   SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin CACHE INTERNAL
#     "Single output directory for building all libraries." )
# ENDIF()

# IF( NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY )
#   SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin CACHE INTERNAL
#     "Single output directory for building all executables." )
# ENDIF()

option(QTWS_DEVELOPER "Developer Macro" OFF)
set(QTWS_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Data" CACHE PATH "Path to Data")

add_subdirectory(Modules)
add_subdirectory(Apps)