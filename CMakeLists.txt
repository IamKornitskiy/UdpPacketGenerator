cmake_minimum_required(VERSION 3.16)

project(UdpPacketGenerator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Gui UiTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Core Gui UiTools)

set(PROJECT_SOURCES
        main.cpp
        MainWindow.cpp
        MainWindow.h
        MainWindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(UdpPacketGenerator
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        PacketTemplate.h PacketTemplate.cpp
        FieldGenerator.h FieldGenerator.cpp
        TrafficGenerator.h TrafficGenerator.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET UdpPacketGenerator APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(UdpPacketGenerator SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(UdpPacketGenerator
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_compile_definitions(UdpPacketGenerator PRIVATE APP_VERSION="${PROJECT_VERSION}")
target_link_libraries(UdpPacketGenerator PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network Qt6::UiTools)

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

# === CPack ===

install(TARGETS UdpPacketGenerator RUNTIME DESTINATION bin)
install(FILES mainwindow.ui DESTINATION bin)

set(CPACK_PACKAGE_NAME "udp-packet-generator")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A flexible UDP packet generator configured through JSON templates.")
set(CPACK_PACKAGE_VENDOR "Oleg Kornitskiy")
set(CPACK_PACKAGE_CONTACT "iam@kornitskiy.ru")
set(CPACK_GENERATOR "DEB;RPM")

set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.28), libqt6core6a, libqt6gui6, libqt6widgets6, libqt6network6, libqt6uitools6")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Oleg Kornitskiy <iam@kornitskiy.ru>")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/IamKornitskiy/UdpPacketGenerator")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")

set(CPACK_RPM_PACKAGE_REQUIRES "qt6-qtbase, qt6-qtbase-gui, qt6-qtbase-network, qt6-qttools")
set(CPACK_RPM_PACKAGE_LICENSE "GPL-3.0")
set(CPACK_RPM_PACKAGE_GROUP "Development/Tools")
set(CPACK_RPM_PACKAGE_URL "https://github.com/IamKornitskiy/UdpPacketGenerator")

include(CPack)
