macro(qt5_copy_dll APP DLL)
    # find the release *.dll file
    get_target_property(Qt5_${DLL}Location Qt5::${DLL} LOCATION)
    # find the debug *d.dll file
    get_target_property(Qt5_${DLL}LocationDebug Qt5::${DLL} IMPORTED_LOCATION_DEBUG)

    add_custom_command(TARGET ${APP} POST_BUILD
       COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:Debug>:${Qt5_${DLL}LocationDebug}> $<$<NOT:$<CONFIG:Debug>>:${Qt5_${DLL}Location}> $<TARGET_FILE_DIR:${APP}>)
endmacro()

macro(qt6_copy_dll APP DLL)
    # find the release *.dll file
    get_target_property(Qt6_${DLL}Location Qt6::${DLL} LOCATION)
    # find the debug *d.dll file
    get_target_property(Qt6_${DLL}LocationDebug Qt6::${DLL} IMPORTED_LOCATION_DEBUG)

    add_custom_command(TARGET ${APP} POST_BUILD
       COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:Debug>:${Qt6_${DLL}LocationDebug}> $<$<NOT:$<CONFIG:Debug>>:${Qt6_${DLL}Location}> $<TARGET_FILE_DIR:${APP}>
    )
endmacro()

function(qt6_setup_windows_environment _TARGET_NAME)

    set(Qt6_MSVC_SETUP_EXECUTABLE "${Qt6_MSVC_DIR}/bin/windeployqt.exe")
    add_custom_command(TARGET ${_TARGET_NAME} POST_BUILD
        COMMAND ${Qt6_MSVC_SETUP_EXECUTABLE} $<TARGET_FILE_DIR:${_TARGET_NAME}>
        COMMENT "Setting up ${TARGET_NAME}'s environment for its build directory" $<TARGET_FILE_DIR:${_TARGET_NAME}
    )

endfunction()
