add_library(SDLWrapperCore STATIC
    src/Context.cpp
    src/EventLoop.cpp
    src/Renderer.cpp
)

target_include_directories(SDLWrapperCore
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}
)

target_link_libraries(SDLWrapperCore
    PUBLIC
        spdlog::spdlog
        SDL3::SDL3
        EnTT::EnTT
        ECS
)