find_package(
  Threads
  REQUIRED
)

set(
  inc_dir
  ${CMAKE_CURRENT_SOURCE_DIR}
)

set(
  src_dir
  ${inc_dir}/lib
)

add_library(
  lib
)

target_sources(
  lib
  PRIVATE
  "${src_dir}/lib.cpp"
  "${src_dir}/lib.hpp"
  "${src_dir}/boost_stop_watch.cpp"
  "${src_dir}/boost_stop_watch.hpp"
)

target_include_directories(
  lib
  PUBLIC
    ${inc_dir}
)

target_compile_features(
  lib
  PUBLIC
    cxx_std_11
)

target_link_libraries(
  lib
  PUBLIC
    Boost::boost
    Boost::chrono
)

add_library(
  lib::lib
  ALIAS
  lib
)

add_executable(
  lib_test
)

target_sources(
  lib_test
  PRIVATE
  "${src_dir}/lib.test.cpp"
  "${src_dir}/main.test.cpp"
)

target_compile_features(
  lib_test
  PUBLIC
    cxx_std_11
)

target_link_libraries(
  lib_test
  PRIVATE
    lib::lib
    Threads::Threads
    Boost::boost
    Boost::chrono
    Boost::unit_test_framework
)

add_test(
  NAME
    lib::test
  COMMAND
    lib_test
    "--color_output=false"
)
