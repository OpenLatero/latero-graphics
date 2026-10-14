cmake_minimum_required(VERSION 3.10...3.31)
Project(latero-example-vibrate-gui)

find_package(laterographics REQUIRED)

set(SOURCES
    mainwindow.h
    mainwindow.cpp
    vibrationgenerator.h
    vibrationgenerator.cpp
    vibrationgeneratorwidget.h
    vibrationgeneratorwidget.cpp
    main.cpp)

add_executable(latero-example-vibrate-gui ${SOURCES})
set_target_properties(latero-example-vibrate-gui PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
target_link_libraries(latero-example-vibrate-gui laterographics::laterographics)
install_programs(/bin latero-example-vibrate-gui)
