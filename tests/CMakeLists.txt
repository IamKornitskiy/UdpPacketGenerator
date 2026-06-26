function(setup_test_target TARGET)
    target_include_directories(${TARGET} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ../packet
    )
    target_link_libraries(${TARGET} PRIVATE
        Qt6::Core
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
