add_library(settings
    abstract_settings_manager.cpp
    abstract_settings_manager.h
    theme_settings_manager.cpp
    theme_settings_manager.h
)

target_include_directories(settings PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(settings PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets)
