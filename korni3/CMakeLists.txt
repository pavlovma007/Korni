cmake_minimum_required(VERSION 3.5)

project(korni3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD 17) # no need to manually adjust the CXXFLAGS
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(korni3 main.cpp my_sha256.cpp my_crypto_utils.cpp
        my_filesystem.cpp my_command.cpp my_geo.cpp my_sqlite.cpp
)

# boost optins
FIND_PACKAGE(Boost COMPONENTS program_options REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(korni3
    ${Boost_LIBRARIES} # property_tree
    sqlite3 # sqlite
)
target_link_libraries(korni3 stdc++fs ssl crypto)

