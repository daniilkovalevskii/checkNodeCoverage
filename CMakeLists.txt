cmake_minimum_required(VERSION 3.19)
project(checkNodeCoverage LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

qt_add_executable(checkNodeCoverage
    main.cpp
    node.h node.cpp
)

target_link_libraries(checkNodeCoverage
    PRIVATE
        Qt::Core
)

include(GNUInstallDirs)

install(TARGETS checkNodeCoverage
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
