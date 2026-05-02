find_package(Qt6 REQUIRED COMPONENTS Test Multimedia)

qt_add_executable(test_advanced_2048_mainwindow
    test_mainwindow.cpp
    ${CMAKE_SOURCE_DIR}/include/mainwindow.h
    ${CMAKE_SOURCE_DIR}/include/KeyboardHandler.h
    ${CMAKE_SOURCE_DIR}/include/settingsdialog.h
    ${CMAKE_SOURCE_DIR}/include/constants.h
    ${CMAKE_SOURCE_DIR}/src/mainwindow.cpp
    ${CMAKE_SOURCE_DIR}/src/KeyboardHandler.cpp
    ${CMAKE_SOURCE_DIR}/src/settingsdialog.cpp
    ${CMAKE_SOURCE_DIR}/src/windowsetting.cpp
    ${CMAKE_SOURCE_DIR}/resources.qrc
)

qt_add_executable(test_advanced_2048_keyboardhandler
    test_keyboardhandler.cpp
    ${CMAKE_SOURCE_DIR}/include/mainwindow.h
    ${CMAKE_SOURCE_DIR}/include/KeyboardHandler.h
    ${CMAKE_SOURCE_DIR}/include/settingsdialog.h
    ${CMAKE_SOURCE_DIR}/include/constants.h
    ${CMAKE_SOURCE_DIR}/src/mainwindow.cpp
    ${CMAKE_SOURCE_DIR}/src/KeyboardHandler.cpp
    ${CMAKE_SOURCE_DIR}/src/settingsdialog.cpp
    ${CMAKE_SOURCE_DIR}/src/windowsetting.cpp
    ${CMAKE_SOURCE_DIR}/resources.qrc
)

qt_add_executable(test_advanced_2048_settingsdialog
    test_settingsdialog.cpp
    ${CMAKE_SOURCE_DIR}/include/mainwindow.h
    ${CMAKE_SOURCE_DIR}/include/KeyboardHandler.h
    ${CMAKE_SOURCE_DIR}/include/settingsdialog.h
    ${CMAKE_SOURCE_DIR}/include/constants.h
    ${CMAKE_SOURCE_DIR}/src/mainwindow.cpp
    ${CMAKE_SOURCE_DIR}/src/KeyboardHandler.cpp
    ${CMAKE_SOURCE_DIR}/src/settingsdialog.cpp
    ${CMAKE_SOURCE_DIR}/src/windowsetting.cpp
    ${CMAKE_SOURCE_DIR}/resources.qrc
)

target_include_directories(test_advanced_2048_mainwindow PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(test_advanced_2048_keyboardhandler PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(test_advanced_2048_settingsdialog PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_advanced_2048_mainwindow PRIVATE UNIT_TEST)
target_compile_definitions(test_advanced_2048_keyboardhandler PRIVATE UNIT_TEST)
target_compile_definitions(test_advanced_2048_settingsdialog PRIVATE UNIT_TEST)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(test_advanced_2048_mainwindow PRIVATE --coverage -O0 -g)
    target_compile_options(test_advanced_2048_keyboardhandler PRIVATE --coverage -O0 -g)
    target_link_options(test_advanced_2048_mainwindow PRIVATE --coverage)
    target_link_options(test_advanced_2048_keyboardhandler PRIVATE --coverage)
    target_compile_options(test_advanced_2048_settingsdialog PRIVATE --coverage -O0 -g)
    target_link_options(test_advanced_2048_settingsdialog PRIVATE --coverage)
endif()

target_link_libraries(test_advanced_2048_mainwindow PRIVATE Qt6::Test Qt6::Widgets Qt6::Multimedia)
target_link_libraries(test_advanced_2048_keyboardhandler PRIVATE Qt6::Test Qt6::Widgets Qt6::Multimedia)
target_link_libraries(test_advanced_2048_settingsdialog PRIVATE Qt6::Test Qt6::Widgets Qt6::Multimedia)

add_test(NAME test_advanced_2048_mainwindow COMMAND test_advanced_2048_mainwindow)
add_test(NAME test_advanced_2048_keyboardhandler COMMAND test_advanced_2048_keyboardhandler)
add_test(NAME test_advanced_2048_settingsdialog COMMAND test_advanced_2048_settingsdialog)