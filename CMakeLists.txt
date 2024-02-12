
cmake_minimum_required (VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)

project(QuartzSandbox)

add_subdirectory("vendor")
add_subdirectory("quartz")

add_executable (Sandbox
  "src/main.cpp"
  "src/sandbox_imgui.cpp"
  "src/sandbox_init.cpp"
  "src/sandbox_update.cpp"
  "src/sandbox_shutdown.cpp"
)

target_include_directories (Sandbox PUBLIC
  ./src/
  ${PROJECT_SOURCE_DIR}/Quartz/src/
  VendorSandbox
)

target_link_libraries(Sandbox
  Quartz
  VendorSandbox
)