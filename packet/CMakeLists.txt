add_library(packet
    base_packet_field.h base_packet_field.cpp
    packet_template.h packet_template.cpp)

target_include_directories(packet PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(packet PUBLIC Qt${QT_VERSION_MAJOR}::Core)