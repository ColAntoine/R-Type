cmake_minimum_required(VERSION 3.21)

# ------------------------------------------------------------------------------
# Toolchain (auto-setup vcpkg if not provided)
# ------------------------------------------------------------------------------
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(VCPKG_ROOT "${CMAKE_SOURCE_DIR}/external/vcpkg")
    if(NOT EXISTS "${VCPKG_ROOT}")
        find_package(Git REQUIRED)
        message(STATUS "Cloning vcpkg...")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} clone https://github.com/Microsoft/vcpkg.git ${VCPKG_ROOT}
            RESULT_VARIABLE git_result
        )
        if(NOT git_result EQUAL 0)
            message(FATAL_ERROR "Failed to clone vcpkg")
        endif()

        # Bootstrap vcpkg
        if(WIN32)
            execute_process(
                COMMAND ${VCPKG_ROOT}/bootstrap-vcpkg.bat
                WORKING_DIRECTORY ${VCPKG_ROOT}
                RESULT_VARIABLE bootstrap_result
            )
        else()
            execute_process(
                COMMAND ${VCPKG_ROOT}/bootstrap-vcpkg.sh
                WORKING_DIRECTORY ${VCPKG_ROOT}
                RESULT_VARIABLE bootstrap_result
            )
        endif()

        if(NOT bootstrap_result EQUAL 0)
            message(FATAL_ERROR "Failed to bootstrap vcpkg")
        endif()
    endif()

    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

# ------------------------------------------------------------------------------
# Project
# ------------------------------------------------------------------------------
project(R-Type)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Pretty ASCII banner (pure cosmetic)
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red "__________        ________________.___._____________________")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red "\\______   \\       \\__    ___/\\__  |   |\\______   \\_   _____/")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red " |       _/  ______ |    |    /   |   | |     ___|    __)_ ")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red " |    |   \\ /_____/ |    |    \\____   | |    |    |        \\")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red " |____|_  /         |____|    / ______| |____|   /_______  /")
execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red "        \\/                    \\/                         \\/ ")

add_library(win_sanity INTERFACE)

if(WIN32)
    target_compile_definitions(win_sanity INTERFACE
        _WIN32_WINNT=0x0A00
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        NOGDI
        NOUSER
    )
    if(MSVC)
        target_compile_options(win_sanity INTERFACE
            /Zc:__cplusplus
            /permissive-
            /U_WINSOCKAPI_
        )
    endif()
endif()

add_subdirectory(Games/RType)
