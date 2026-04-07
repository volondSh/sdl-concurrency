add_library(ECS INTERFACE)

target_include_directories(ECS
    INTERFACE
        ${CMAKE_SOURCE_DIR}
)

target_link_libraries(ECS
    INTERFACE
        SDL3::SDL3
)