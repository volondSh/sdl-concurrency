cmake_minimum_required(VERSION 3.25)

project(
    sdl_concurrency
    VERSION 0.1.0
    DESCRIPTION "Modern C++ wrappers around SDL3 C API with concurrency experiments"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_CCACHE "Enable ccache compiler launcher when available" ON)
option(BUILD_TESTS "Build tests" ON)

if(ENABLE_CCACHE)
    find_program(CCACHE_PROGRAM ccache)
    if(CCACHE_PROGRAM)
        message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
        set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
        set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    else()
        message(STATUS "ccache not found, continuing without compiler launcher")
    endif()
endif()

include(cmake/CPM.cmake)

if(CMAKE_EXPORT_COMPILE_COMMANDS)
    add_custom_target(sync_compile_commands ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/compile_commands.json"
            "${CMAKE_SOURCE_DIR}/compile_commands.json"
        COMMENT "Copy compile_commands.json to project root"
        VERBATIM
    )
endif()

CPMAddPackage(
    NAME spdlog
    GITHUB_REPOSITORY gabime/spdlog
    VERSION 1.14.1
    OPTIONS
        "SPDLOG_BUILD_EXAMPLE OFF"
        "SPDLOG_BUILD_TESTS OFF"
        "SPDLOG_BUILD_BENCH OFF"
        "SPDLOG_BUILD_SHARED OFF"
)

CPMAddPackage(
    NAME SDL3
    GITHUB_REPOSITORY libsdl-org/SDL
    GIT_TAG release-3.4.4
    OPTIONS
        "SDL_TEST_LIBRARY OFF"
        "SDL_TESTS OFF"
)

CPMAddPackage(
    NAME nlohmann_json
    GITHUB_REPOSITORY nlohmann/json
    VERSION 3.12.0
)

CPMAddPackage(
    NAME EnTT
    GITHUB_REPOSITORY skypjack/entt
    VERSION 3.16.0
)

CPMAddPackage(
    NAME DearImGui
    GITHUB_REPOSITORY ocornut/imgui
    GIT_TAG v1.92.7
    DOWNLOAD_ONLY YES
)

if(DearImGui_ADDED)
    add_library(DearImGui STATIC
        ${DearImGui_SOURCE_DIR}/imgui.cpp
        ${DearImGui_SOURCE_DIR}/imgui_demo.cpp
        ${DearImGui_SOURCE_DIR}/imgui_draw.cpp
        ${DearImGui_SOURCE_DIR}/imgui_tables.cpp
        ${DearImGui_SOURCE_DIR}/imgui_widgets.cpp
        ${DearImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
        ${DearImGui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
    )
    target_include_directories(DearImGui PUBLIC ${DearImGui_SOURCE_DIR})
    target_link_libraries(DearImGui PRIVATE SDL3::SDL3)
endif()

if(BUILD_TESTS)
    CPMAddPackage(
        NAME GTest
        GITHUB_REPOSITORY google/googletest
        VERSION 1.17.0
        OPTIONS "INSTALL_GTEST OFF"
    )
endif()

add_subdirectory(App)
add_subdirectory(SDLWrapper)
add_subdirectory(ECS)

add_executable(sdlc_app
    src/main.cpp
)

target_link_libraries(sdlc_app 
    PRIVATE 
        App
        SDLWrapper
        ECS
)

if(TARGET sync_compile_commands)
    add_dependencies(sdlc_app sync_compile_commands)
endif()

if(WIN32)
    add_custom_command(TARGET sdlc_app POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:sdlc_app> $<TARGET_RUNTIME_DLLS:sdlc_app>
        COMMAND_EXPAND_LISTS
    )
endif()

if(BUILD_TESTS)
    include(CTest)
    enable_testing()

    add_executable(sdlc_tests
        tests/sdl_context_test.cpp
        tests/ecs_components_test.cpp
    )

    target_link_libraries(sdlc_tests
        PRIVATE
            SDLWrapper
            ECS
            GTest::gtest_main
    )

    if(WIN32)
        add_custom_command(TARGET sdlc_tests POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:sdlc_tests> $<TARGET_RUNTIME_DLLS:sdlc_tests>
            COMMAND_EXPAND_LISTS
        )
    endif()

    if(TARGET sync_compile_commands)
        add_dependencies(sdlc_tests sync_compile_commands)
    endif()

    include(GoogleTest)
    gtest_discover_tests(sdlc_tests)
endif()
