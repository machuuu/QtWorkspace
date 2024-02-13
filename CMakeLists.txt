cmake_minimum_required(VERSION 3.22.0)

project(QtWorkspace)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(SetupLibraries)
include(QtUtility)

# Use solution folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# IF( NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
#   SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin CACHE INTERNAL
#     "Single output directory for building all libraries." )
# ENDIF()

# IF( NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY )
#   SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin CACHE INTERNAL
#     "Single output directory for building all executables." )
# ENDIF()

add_subdirectory(Modules)
add_subdirectory(Apps)