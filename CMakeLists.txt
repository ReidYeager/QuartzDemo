
cmake_minimum_required (VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)

project(QuartzSandbox)

add_subdirectory("vendor")
add_subdirectory("quartz")

file(GLOB_RECURSE HeaderFiles CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/**.h")
file(GLOB_RECURSE CppSourceFiles CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/**.cpp")

add_executable (Sandbox
  ${HeaderFiles}
  ${CppSourceFiles}
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