function(setup_test_target TARGET)
    target_include_directories(${TARGET} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ../packet
    )
    target_link_libraries(${TARGET} PRIVATE
        Qt6::Core
        Qt6::Concurrent
        Qt6::Test
    )
    set_target_properties(${TARGET} PROPERTIES WIN32_EXECUTABLE OFF)

    get_target_property(CURRENT_LINK_FLAGS ${TARGET} LINK_FLAGS)
    if(CURRENT_LINK_FLAGS)
        string(REPLACE "-mwindows" "" NEW_LINK_FLAGS "${CURRENT_LINK_FLAGS}")
        set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "${NEW_LINK_FLAGS}")
    endif()
endfunction()

# ---- Test BasePacketField ----
add_executable(test_base_packet_field
    tst_base_packet_field.cpp
    ../packet/base_packet_field.cpp    
)
setup_test_target(test_base_packet_field)
add_test(NAME BasePacketField COMMAND test_base_packet_field)

# ---- Test IntegerPacketField ----
add_executable(test_integer_packet_field
    tst_integer_packet_field.cpp
    ../packet/base_packet_field.cpp
    ../packet/integer_packet_field.cpp
)
setup_test_target(test_integer_packet_field)
add_test(NAME IntegerPacketField COMMAND test_integer_packet_field)

# ---- Test PacketTemplate ----
add_executable(test_packet_template
    tst_packet_template.cpp
    ../packet/base_packet_field.cpp
    ../packet/integer_packet_field.cpp
    ../packet/packet_template.cpp
)
setup_test_target(test_packet_template)
add_test(NAME PacketTemplate COMMAND test_packet_template)

# ---- Test PacketBuilder ----
add_executable(test_packet_builder
    tst_packet_builder.cpp
    ../packet/base_packet_field.cpp
    ../packet/integer_packet_field.cpp
    ../packet/packet_builder.cpp
)
setup_test_target(test_packet_builder)
add_test(NAME PacketBuilder COMMAND test_packet_builder)


