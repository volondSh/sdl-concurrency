add_library(SDLWrapperWidgets STATIC
    src/Window.cpp
)

target_include_directories(SDLWrapperWidgets
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}
)

target_link_libraries(SDLWrapperWidgets
    PUBLIC
        spdlog::spdlog
        SDL3::SDL3
)