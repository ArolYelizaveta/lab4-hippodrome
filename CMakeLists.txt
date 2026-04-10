cmake_minimum_required(VERSION 3.10)
project(lab4-hippodrome)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SQLite3 REQUIRED)

add_executable(hippodrome
        src/main.cpp
        src/DatabaseManager.cpp
        src/BusinessLogic.cpp
        src/UI.cpp
)

target_include_directories(hippodrome PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/includes
        ${SQLite3_INCLUDE_DIRS}
)

target_link_libraries(hippodrome
        ${SQLite3_LIBRARIES}
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/hippodrome.db DESTINATION ${CMAKE_CURRENT_BINARY_DIR})