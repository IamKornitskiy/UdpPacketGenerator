add_library(widgets STATIC
    custom_button.cpp
    custom_button.h
    custom_statusbar.cpp
    custom_statusbar.h
    titlebar.cpp
    titlebar.h
    text_field_editor_dialog.cpp
    text_field_editor_dialog.h
    text_field_editor_dialog.ui
    stream_config_widget.h stream_config_widget.cpp stream_config_widget.ui)

target_include_directories(widgets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(widgets PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
)
