cmake_minimum_required(VERSION 3.16)

project(advanced-2048 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(advanced-2048
    src/main.cpp
    src/mainwindow.cpp
    src/KeyboardHandler.cpp
    include/mainwindow.h
    include/KeyboardHandler.h
    include/constants.h
)

target_link_libraries(advanced-2048 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(advanced-2048 PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS advanced-2048
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
