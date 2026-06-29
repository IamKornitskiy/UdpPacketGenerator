add_library(packet
    base_packet_field.h base_packet_field.cpp
    packet_template.h packet_template.cpp
    packet_builder.cpp
    packet_builder.h
    integer_packet_field.h integer_packet_field.cpp
    float_packet_field.h float_packet_field.cpp
)

target_include_directories(packet PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(packet PUBLIC Qt${QT_VERSION_MAJOR}::Core)