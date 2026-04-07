add_library(App STATIC
    src/Application.cpp
    src/ConfigManager.cpp
    src/Logger.cpp
)

target_include_directories(App
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}
)

target_link_libraries(App
    PUBLIC
        spdlog::spdlog
        SDL3::SDL3
        nlohmann_json::nlohmann_json
        SDLWrapper
)