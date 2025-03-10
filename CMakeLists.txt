cmake_minimum_required(VERSION 3.16)
project(ziga-pomodoro VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# (LINE 10) Find Qt packages - try Qt6 first, fallback to Qt5
find_package(Qt6 COMPONENTS Widgets Multimedia Sql QUIET)  # <-- ADDED Sql
if (Qt6_FOUND)
    set(QT_VERSION_MAJOR 6)
else ()
    find_package(Qt5 COMPONENTS Widgets Multimedia Sql REQUIRED)  # <-- ADDED Sql
    set(QT_VERSION_MAJOR 5)
endif ()

# (LINE 20) Add definitions based on what was found
if (TARGET Qt${QT_VERSION_MAJOR}::Multimedia)
    add_definitions(-DHAVE_QT_MULTIMEDIA)
endif ()

# (LINE 25) Define sources
set(SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/timer.cpp
        src/settings.cpp
        src/timerwindow.cpp
        src/databasemanager.cpp  # <-- ADD THIS FILE FOR DATABASE LOGIC
)

set(HEADERS
        src/mainwindow.h
        src/timer.h
        src/settings.h
        src/timerwindow.h
        src/databasemanager.h  # <-- ADD DATABASE HEADER
)

# (LINE 40) Create resource file
set(RESOURCES
        resources/resources.qrc
        src/pomodoroactivitymap.cpp
        src/pomodoroactivitymap.h
)

# (LINE 45) Check if the resources.qrc file exists, if not, create it
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

# (LINE 60) Add executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES})

# (LINE 65) Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql)  # <-- ADDED Sql

# (LINE 70) Add multimedia if found
if (TARGET Qt${QT_VERSION_MAJOR}::Multimedia)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Multimedia)
    message(STATUS "Qt Multimedia found. Sound notifications will be enabled.")
else ()
    message(STATUS "Qt Multimedia not found. Sound notifications will be disabled.")
endif ()

# (LINE 78) Install
install(TARGETS ${PROJECT_NAME}
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# (LINE 85) Make sure the resources directory exists
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources/sounds
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources/styles
)
