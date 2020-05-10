set(CMAKE_INCLUDE_CURRENT_DIR ON)
cmake_minimum_required(VERSION 3.0.0)
project(client-server VERSION 0.1.0)

include(CTest)
enable_testing()

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(SRV_HEADERS server/server.h
    server/players.h
    server/rooms.h)
qt5_wrap_cpp(moc_sources_srv ${SRV_HEADERS})

add_executable(server_exec server/main.cpp ${moc_sources_srv})
add_executable(client client/main.cpp)

find_package(Qt5 COMPONENTS Core Network REQUIRED)
target_link_libraries(server_exec Qt5::Core Qt5::Network)
target_link_libraries(client Qt5::Core Qt5::Network)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
