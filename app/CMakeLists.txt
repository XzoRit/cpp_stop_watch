add_executable(
  app_exe
)

target_sources(
  app_exe
  PRIVATE
  "main.cpp"
)

target_compile_features(
  app_exe
  PUBLIC
    cxx_std_11
)

target_link_libraries(
  app_exe
  PRIVATE
    lib::lib
    Boost::program_options
)

add_test(
  NAME
    app::exe
  COMMAND
    app_exe
    "--help"
)
