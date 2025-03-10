cmake_minimum_required(VERSION 3.16)
project(ziga-pomodoro VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt packages - try Qt6 first, fallback to Qt5
find_package(Qt6 COMPONENTS Widgets Multimedia Sql QUIET)
if (Qt6_FOUND)
    set(QT_VERSION_MAJOR 6)
else ()
    find_package(Qt5 COMPONENTS Widgets Multimedia Sql REQUIRED)
    set(QT_VERSION_MAJOR 5)
endif ()

# Add definitions based on what was found
if (TARGET Qt${QT_VERSION_MAJOR}::Multimedia)
    add_definitions(-DHAVE_QT_MULTIMEDIA)
endif ()

# Define sources
set(SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/timer.cpp
        src/settings.cpp
        src/timerwindow.cpp
        src/databasemanager.cpp
)

set(HEADERS
        src/mainwindow.h
        src/timer.h
        src/settings.h
        src/timerwindow.h
        src/databasemanager.h
)

# Create resource file
set(RESOURCES
        resources/resources.qrc
        src/pomodoroactivitymap.cpp
        src/pomodoroactivitymap.h
)

# Ensure resources.qrc file exists
if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources/resources.qrc")
    file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/resources/resources.qrc"
            "<!DOCTYPE RCC>
<RCC version=\"1.0\">
    <qresource prefix=\"/icons\">
        <file>icons/tomato.png</file>
    </qresource>
    <qresource prefix=\"/sounds\">
        <file>sounds/alarm.wav</file>
    </qresource>
    <qresource prefix=\"/styles\">
        <file>styles/default.qss</file>
    </qresource>
</RCC>")
endif ()

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES})

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql)

# Add multimedia if found
if (TARGET Qt${QT_VERSION_MAJOR}::Multimedia)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Multimedia)
    message(STATUS "Qt Multimedia found. Sound notifications will be enabled.")
else ()
    message(STATUS "Qt Multimedia not found. Sound notifications will be disabled.")
endif ()

# Install targets
install(TARGETS ${PROJECT_NAME}
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Make sure the resources directory exists
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources/sounds
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources/styles
)

# CPack configuration for packaging
include(CPack)

# Define the package information
set(CPACK_PACKAGE_NAME "ziga-pomodoro")
set(CPACK_PACKAGE_VERSION "0.1")
set(CPACK_PACKAGE_DESCRIPTION "A Pomodoro timer application built with Qt5/Qt6.")
set(CPACK_PACKAGE_CONTACT "github.com/zigameni")
set(CPACK_PACKAGE_VENDOR "zigameni")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-Linux")
set(CPACK_GENERATOR "TGZ")  # You can switch this to DEB, RPM, ZIP, etc.

# Define dependencies for packaging
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Gazmend Shehu")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5core5a, libqt5widgets5, libqt5multimedia5")  # Adjust package dependencies as necessary
