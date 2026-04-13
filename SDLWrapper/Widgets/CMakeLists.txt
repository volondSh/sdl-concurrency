add_library(SDLWrapperWidgets STATIC
    src/Window.cpp
    src/Overlay.cpp
    src/Menu.cpp
)

target_include_directories(SDLWrapperWidgets
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${DearImGui_SOURCE_DIR}/backends
    PUBLIC ${CMAKE_SOURCE_DIR}
)

target_link_libraries(SDLWrapperWidgets
    PUBLIC
        spdlog::spdlog
        SDL3::SDL3
        nlohmann_json::nlohmann_json
        DearImGui
    PRIVATE
        SDLWrapperCore
)