add_library(SDLWrapperCore STATIC
    src/Context.cpp
    src/EventLoop.cpp
    src/ImGuiRenderer.cpp
    src/Profiler.cpp
    src/Renderer.cpp
    src/TextRenderer.cpp
)

target_include_directories(SDLWrapperCore
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${DearImGui_SOURCE_DIR}/backends
    PUBLIC ${CMAKE_SOURCE_DIR}
)

target_link_libraries(SDLWrapperCore
    PUBLIC
        spdlog::spdlog
        SDL3::SDL3
        SDL3_ttf::SDL3_ttf
        EnTT::EnTT
        ECS
        DearImGui
    PRIVATE
        SDLWrapperWidgets
)

target_include_directories(SDLWrapperCore
    PRIVATE ${DearImGui_SOURCE_DIR}/backends
)