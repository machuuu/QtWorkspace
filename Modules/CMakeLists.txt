cmake_minimum_required(VERSION 3.22.0)

project(Modules)

add_subdirectory(Logging)

add_subdirectory(Data)
add_subdirectory(Rendering)

add_subdirectory(TestDevice)
add_subdirectory(TestDeviceApp)