cmake_minimum_required(VERSION 3.22.0)

################################################
# Common (Private)
################################################

set(TARGET_FOLDER "Modules")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(RENDERING_RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources" CACHE PATH "Path to Resources")
set(RENDERING_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data" CACHE PATH "Path to Data")
set(RENDERING_CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/config" CACHE PATH "Path to Rendering Config")
configure_file("${RENDERING_CONFIG_DIR}/rendering.configure.h.in" "${RENDERING_CONFIG_DIR}/rendering.configure.h")

################################################
# Vulkan (Public)
################################################

set(VULKAN_RENDERING VulkanRendering)
project(${VULKAN_RENDERING})

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Vulkan REQUIRED)

qt6_add_library(${VULKAN_RENDERING} STATIC
    # SOURCES
    src/vk/VulkanWidget.cpp
    src/config/rendering.configure.h.in
    src/config/rendering.configure.h
    
    # RESOURCES
    resources/vk/shader.frag
    resources/vk/shader.vert

    # PUBLIC HEADERS
    include/vk/VulkanWidget.h
)

# Organize target into a group in Visual Studio IDE
IF (DEFINED TARGET_FOLDER)
	set_property(TARGET ${VULKAN_RENDERING} PROPERTY FOLDER ${TARGET_FOLDER})
ENDIF()

target_link_libraries(${VULKAN_RENDERING} 
    PRIVATE 
        Qt6::Widgets
        Vulkan::Vulkan
        Logging
)

target_include_directories(${VULKAN_RENDERING} 
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include/vk
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src/config
)

################################################
# OpenGL (Public)
################################################

set(OPENGL_RENDERING OpenGLRendering)
project(${OPENGL_RENDERING})

find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(VTK CONFIG REQUIRED)

set(PRIVATE_SOURCES
    src/ogl/AirwayRenderWidget.cpp
    src/ogl/AirwayTreeEngine.cpp
    src/ogl/Camera6DoF.cpp

    src/config/rendering.configure.h.in
    src/config/rendering.configure.h

    src/ogl/mmUtilityTools.cpp

    src/ogl/CTVolume.cpp
    src/ogl/Crosshair.cpp
    src/ogl/SlicerCamera.cpp
    src/ogl/SlicerBase.cpp
    src/ogl/AxialSlicerWidget.cpp
    src/ogl/CoronalSlicerWidget.cpp
    src/ogl/SagittalSlicerWidget.cpp
    src/ogl/SlicerDisplay.cpp
)

set(PUBLIC_HEADERS
    include/ogl/AirwayRenderWidget.h
    include/ogl/AirwayTreeEngine.h
    include/ogl/Camera6DoF.h
    include/ogl/mmOpenGLErrorCheck.h
    include/ogl/mmUtilityTools.h
    include/ogl/CTVolume.h
    include/ogl/Crosshair.h
    include/ogl/SlicerCamera.h
    include/ogl/SlicerBase.h
    include/ogl/AxialSlicerWidget.h
    include/ogl/CoronalSlicerWidget.h
    include/ogl/SagittalSlicerWidget.h
    include/ogl/SlicerDisplay.h
)

set(GLSL_RESOURECS 
    resources/ogl/AirwayFragmentShader.glsl
    resources/ogl/AirwayVertexShader.glsl
    resources/ogl/AxialSlabCompute.glsl
    resources/ogl/AxialSliceCompute.glsl
    resources/ogl/CoronalSliceCompute.glsl
    resources/ogl/CrosshairFragment.glsl
    resources/ogl/CrosshairVertex.glsl
    resources/ogl/ObliqueSliceCompute.glsl
    resources/ogl/SagittalSliceCompute.glsl
    resources/ogl/SlicerFragment.glsl
    resources/ogl/SlicerVertex.glsl
)

set(QRC_RESOURCES
    resources/ogl/AirwayShaders.qrc
    resources/ogl/ComputeShaders.qrc
    resources/ogl/SlicerShaders.qrc
    resources/ogl/CrosshairShaders.qrc
)

source_group("QRC" FILES ${QRC_RESOURCES})
source_group("Shaders" FILES ${GLSL_RESOURECS})

qt6_add_library(${OPENGL_RENDERING} STATIC
    # SOURCES
    ${PRIVATE_SOURCES}
    
    # RESOURCES
    ${QRC_RESOURCES}
    ${GLSL_RESOURECS}

    # PUBLIC SOURCES
    ${PUBLIC_HEADERS}
)

# Organize target into a group in Visual Studio IDE
IF (DEFINED TARGET_FOLDER)
	set_property(TARGET ${OPENGL_RENDERING} PROPERTY FOLDER ${TARGET_FOLDER})
ENDIF()

target_link_libraries(${OPENGL_RENDERING} 
    PRIVATE 
        Logging
        ${VTK_LIBRARIES}
    PUBLIC
        Qt6::OpenGLWidgets
        glad::glad
        glm::glm-header-only
)

target_include_directories(${OPENGL_RENDERING} 
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include/ogl
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src/config
)