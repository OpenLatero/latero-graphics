cmake_minimum_required(VERSION 3.10...3.31)
Project(latero-example-vibrate-engine)

find_package(laterographics REQUIRED)

set(SOURCES
    vibrationgenerator.h
    vibrationgenerator.cpp
    main.cpp)

add_executable(latero-example-vibrate-engine ${SOURCES})
set_target_properties(latero-example-vibrate-engine PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
target_link_libraries(latero-example-vibrate-engine laterographics::laterographics)
install_programs(/bin latero-example-vibrate-engine)
