add_subdirectory(Core)
add_subdirectory(Widgets)

add_library(SDLWrapper INTERFACE)

target_link_libraries(SDLWrapper INTERFACE
    spdlog::spdlog
    SDL3::SDL3
    SDLWrapperCore
    SDLWrapperWidgets
)

target_include_directories(SDLWrapper INTERFACE
    ${CMAKE_SOURCE_DIR}
)