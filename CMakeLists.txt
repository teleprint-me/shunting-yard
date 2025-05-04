cmake_minimum_required(VERSION 3.17)
project("shunting-yard" LANGUAGES C)

set(CMAKE_C_STANDARD 17)

# Common warning and security flags
set(COMMON_WARNING_FLAGS "-Wall -Wextra -Wpedantic -Werror -Wformat-security -Wshadow -fexceptions")
set(DEBUG_SANITIZERS "-fsanitize=address,undefined -fno-omit-frame-pointer")
set(DEBUG_EXTRA_WARNINGS "-Wformat -Wnull-dereference -Wdouble-promotion")
set(DEBUG_ANALYSIS "-Wanalyzer-double-free -Wanalyzer-file-leak -Wanalyzer-malloc-leak -Wanalyzer-null-dereference -Wanalyzer-out-of-bounds -Wanalyzer-va-list-leak")
set(DEBUG_FLAGS "-g3")

# Combine flags based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_WARNING_FLAGS} ${DEBUG_SANITIZERS} ${DEBUG_EXTRA_WARNINGS} ${DEBUG_ANALYSIS} ${DEBUG_FLAGS}")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_WARNING_FLAGS}")
endif()

# Shared library option (enabled by default)
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

# Library sources (for now just arena + lexer)
add_library(shunting-yard
    src/lexer.c
)

target_include_directories(shunting-yard PUBLIC include)
target_link_libraries(shunting-yard PUBLIC m pcre2-8)

# Main program entry point (optional, useful for CLI testing)
add_executable(rpn src/main.c)
target_link_libraries(rpn PRIVATE shunting-yard)

# Optional: add_subdirectory(tests) for unit tests

# Custom clean target
add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/bin ${CMAKE_BINARY_DIR}/lib
    COMMENT "Cleaning all generated files"
)
