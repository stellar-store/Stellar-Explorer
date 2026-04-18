cmake_minimum_required(VERSION 3.16)
project(StellarExplorer VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/filetab.cpp
    src/breadcrumbs.cpp
    src/fileopworker.cpp
    src/configdialog.cpp
    src/utils.cpp
)

set(HEADERS
    src/mainwindow.h
    src/filetab.h
    src/breadcrumbs.h
    src/fileopworker.h
    src/configdialog.h
    src/utils.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Widgets
)

# Instalar el binario
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
# Instalar temas CSS
install(FILES themes.css DESTINATION share/${PROJECT_NAME})
