# Highlighter module for syntax highlighting
add_library(highlighters STATIC
    csv_highlighter.cpp
    json_highlighter.cpp
    nmea_highlighter.cpp
)

target_include_directories(highlighters PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(highlighters PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
)

# Export public headers for other modules to include
target_include_directories(highlighters INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
