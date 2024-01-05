cmake_minimum_required(VERSION 3.14)
project(think_cell_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

if (MSVC)
    # warning level 4
    add_compile_options(/W4)
else()
    # additional warnings
    add_compile_options(-Wall -Wextra -Wpedantic -O0)
    include(CodeCoverage)
    append_coverage_compiler_flags()

endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  interval_map_test
  interval_map_test.cpp
)
target_link_libraries(
  interval_map_test
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(interval_map_test)

if(CMAKE_COMPILER_IS_GNUCXX)
    setup_target_for_coverage_lcov(NAME coverage EXECUTABLE testrunner)
    target_link_libraries(interval_map_test gcov)
endif()


