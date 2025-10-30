# R-Type Game CMakeLists.txt
cmake_minimum_required(VERSION 3.21)

set(RTYPE_PROJECT_NAME "RType")

# --- gather sources (unchanged) ---
file(GLOB_RECURSE RTYPE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE GAME_SYSTEM_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Entity/Systems/*.cpp")
list(REMOVE_ITEM RTYPE_SOURCES ${GAME_SYSTEM_SOURCES})
list(FILTER RTYPE_SOURCES EXCLUDE REGEX ".*/main_client\\.cpp")
list(FILTER RTYPE_SOURCES EXCLUDE REGEX ".*/main_server\\.cpp")
list(FILTER RTYPE_SOURCES EXCLUDE REGEX ".*/Core/Server/.*")
list(APPEND RTYPE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Core/Server/Protocol/Protocol.cpp")
file(GLOB_RECURSE RTYPE_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
list(FILTER RTYPE_SOURCES EXCLUDE REGEX ".*/test/.*")
list(FILTER GAME_SYSTEM_SOURCES EXCLUDE REGEX ".*/test/.*")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RTYPE_SOURCES} ${RTYPE_HEADERS})

set(RTYPE_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Services
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/States
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Systems
    ${CMAKE_CURRENT_SOURCE_DIR}/Entity
    ${CMAKE_CURRENT_SOURCE_DIR}/UI
)

# --- RType static library ---
add_library(${RTYPE_PROJECT_NAME} STATIC ${RTYPE_SOURCES})
set_target_properties(${RTYPE_PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(${RTYPE_PROJECT_NAME} PUBLIC ${RTYPE_INCLUDE_DIRS})

# IMPORTANT: propagate Windows-safe defines (NOGDI, NOMINMAX, _WIN32_WINNT, etc.)
# via the interface target created in the root CMakeLists.txt
# This fixes Rectangle/CloseWindow/ShowCursor conflicts inside this library.
target_link_libraries(${RTYPE_PROJECT_NAME} PUBLIC
    raylib
    glfw
    win_sanity        # <<< add this
)

# Link ECS if present
if(TARGET ECS)
    target_link_libraries(${RTYPE_PROJECT_NAME} PUBLIC ECS)
    target_include_directories(${RTYPE_PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/ECS/include)
endif()

target_compile_features(${RTYPE_PROJECT_NAME} PUBLIC cxx_std_20)
target_compile_options(${RTYPE_PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# --- Build each game system as a separate shared lib (plugins) ---
foreach(SYSTEM_FILE ${GAME_SYSTEM_SOURCES})
    get_filename_component(SYSTEM_NAME ${SYSTEM_FILE} NAME_WE)
    set(GAME_SYSTEM_NAME "game_${SYSTEM_NAME}")

    add_library(${GAME_SYSTEM_NAME} SHARED ${SYSTEM_FILE})

    target_include_directories(${GAME_SYSTEM_NAME} PUBLIC ${RTYPE_INCLUDE_DIRS})

    target_link_libraries(${GAME_SYSTEM_NAME} PUBLIC
        raylib
        glfw
        ${RTYPE_PROJECT_NAME}
        win_sanity      # <<< add this so plugins also inherit the safe Windows defines
    )

    if(TARGET ECS)
        target_link_libraries(${GAME_SYSTEM_NAME} PUBLIC ECS)
        target_include_directories(${GAME_SYSTEM_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/ECS/include)
    endif()

    target_compile_features(${GAME_SYSTEM_NAME} PUBLIC cxx_std_20)

    set_target_properties(${GAME_SYSTEM_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/systems
    )

    message(STATUS "Building game system: ${GAME_SYSTEM_NAME} from ${SYSTEM_FILE}")
endforeach()

# --- Copy built systems next to the client exe for runtime loading ---
add_custom_target(copy_game_systems ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/lib/systems
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/lib/systems ${CMAKE_BINARY_DIR}/bin/lib/systems
    DEPENDS ${GAME_SYSTEM_SOURCES}
    COMMENT "Copying game system libraries to binary directory"
)

# --- Optional standalone exe if building this dir alone (unchanged) ---
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    find_package(raylib CONFIG REQUIRED)
    find_package(glfw3 CONFIG REQUIRED)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
        add_executable(${RTYPE_PROJECT_NAME}_exe ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
        target_link_libraries(${RTYPE_PROJECT_NAME}_exe PRIVATE ${RTYPE_PROJECT_NAME})
        set_target_properties(${RTYPE_PROJECT_NAME}_exe PROPERTIES
            OUTPUT_NAME "r-type_client"
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        )
    endif()
endif()

set(RTYPE_LIBRARIES ${RTYPE_PROJECT_NAME} PARENT_SCOPE)
set(RTYPE_INCLUDE_DIRS ${RTYPE_INCLUDE_DIRS} PARENT_SCOPE)

list(LENGTH RTYPE_SOURCES RTYPE_SOURCES_COUNT)
list(LENGTH GAME_SYSTEM_SOURCES GAME_SYSTEM_SOURCES_COUNT)
message(STATUS "RType Game Configuration: (sources=${RTYPE_SOURCES_COUNT}, systems=${GAME_SYSTEM_SOURCES_COUNT})")
