cmake_minimum_required(VERSION 3.16)

project(UdpPacketGenerator VERSION 2.1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Gui UiTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Core Gui UiTools Test Concurrent)

add_subdirectory(packet)
add_subdirectory(settings)

set(PROJECT_SOURCES
        main.cpp
        main_window.cpp
        main_window.h
        version_checker.cpp
        version_checker.h
        main_window.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(UdpPacketGenerator
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}


        traffic_generator.h traffic_generator.cpp
        resources.qrc
        titlebar.h titlebar.cpp
        field_editor_factory.h field_editor_factory.cpp
        text_field_editor_dialog.h text_field_editor_dialog.cpp text_field_editor_dialog.ui
        json_highlighter.h json_highlighter.cpp
        nmea_highlighter.h nmea_highlighter.cpp
        csv_highlighter.h csv_highlighter.cpp
        custom_button.h custom_button.cpp
        custom_statusbar.h custom_statusbar.cpp
        theme_manager.h theme_manager.cpp
    )
endif()

target_compile_definitions(UdpPacketGenerator PRIVATE APP_VERSION="${PROJECT_VERSION}")
target_link_libraries(UdpPacketGenerator PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network Qt6::UiTools packet settings)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.UdpPacketGenerator)
endif()
set_target_properties(UdpPacketGenerator PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(UdpPacketGenerator)
endif()

option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# === CPack ===

install(TARGETS UdpPacketGenerator RUNTIME DESTINATION bin)
install(FILES main_window.ui DESTINATION bin)

set(CPACK_RPM_PACKAGE_AUTOREQ OFF)
set(CPACK_PACKAGE_NAME "udp-packet-generator")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A flexible UDP packet generator configured through JSON templates.")
set(CPACK_PACKAGE_VENDOR "Oleg Kornitskiy")
set(CPACK_PACKAGE_CONTACT "iam@kornitskiy.ru")
set(CPACK_GENERATOR "DEB;RPM")

set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.28),
    libqt6core6 (>= 6.8.2),
    libqt6gui6 (>= 6.8.2),
    libqt6widgets6 (>= 6.8.2),
    libqt6network6 (>= 6.8.2),
    libqt6uitools6 (>= 6.8.2),
    libqt6opengl6 (>= 6.8.2),
    libqt6openglwidgets6 (>= 6.8.2),
    libqt6dbus6 (>= 6.8.2),
    libgl1,
    libxkbcommon-x11-0,
    libxcb-cursor06")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Oleg Kornitskiy <iam@kornitskiy.ru>")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/UdpPacketGenerator/UdpPacketGenerator")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")

set(CPACK_RPM_PACKAGE_REQUIRES "qt6-qtbase >= 6.8.2, qt6-qttools >= 6.8.2, libxkbcommon, xcb-util-cursor, libglvnd-glx")
set(CPACK_RPM_PACKAGE_LICENSE "GPL-3.0")
set(CPACK_RPM_PACKAGE_GROUP "Development/Tools")
set(CPACK_RPM_PACKAGE_URL "https://github.com/UdpPacketGenerator/UdpPacketGenerator")

include(CPack)
