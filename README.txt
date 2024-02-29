Overview:

Requirements:
1. Installation of CMake (3.22.0+)
1. Visual Studio 2022 (v143 toolkit)
    1. Visual Studio Code can be used but needs MSBuild, there may be an extension for it.
1. Installation of VCPKG with $env:VCPKG_ROOT configured for CMake. (Eventually to support Qt6 requirements)
    1. See CMakePresets.json for specify the VCPKG toolchain file
3. Installation of Qt6 (6.4.2)
    1. See CMake/SetupLibraries
1. Windows OS

This repository is used for me to learn Qt for application development using the following concepts:
1. Vulkan Rendering
2. Plugin Applications
3. OpenGL Rendering
    1. .ply surface rendering (Specifically Airway Surface Rendering at the moment, hope to make general)
    2. .analyze img slice and slab rendering (Modified Analyze Image at the moment, hope to make general)
    3. mouse click synchronization
4. Networking
5. Hardware Interfacing

I chose to use Qt because I can learn how to apply each concept to multiple platforms. 
Finally, it encourages me to become more proficient in CMake to handle the build system for each platform.
    1. Note: At the moment I am only focusing on Windows OS. 

Tasks:
1. Implement dynamic load of Otsu Threshold plugin for PlugTalk application.
    1. Create a standalone .exe to test and use the Otsu Threshold plugin outside of the main PlugTalk application
1. (Vulkan) Remove Win-32 specific window surface creation. (https://gitlab.kitware.com/cmake/community/-/wikis/doc/tutorials/How-To-Write-Platform-Checks)
1. Figure out what Hardware to implement in the Hardware Interface project, at the moment it is imaginary.
1. Figure out what Networking task to implement in the Networking project.
    1. Online photo gallary with each client controlling a camera to view a wall of custom photos.
1. Migrate my existing OpenGL rendering
    1. Properly forward declare the class definitions and define an public / private interface
1. Create Install CMake Target for a Module with the VCPKG tool to help bundle dependencies