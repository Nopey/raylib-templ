cmake_minimum_required(VERSION 3.8)
project(test CXX)
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS YES) # probably don't need this? but might

# generate compile_commands.json, for consumption by clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# grab raylib
set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(raylib)
  endif()
endif()

add_executable(mygame main.cpp)
target_link_libraries(mygame PRIVATE raylib)

add_subdirectory(FluidSim)
target_link_libraries(mygame PRIVATE FluidSim)

# Web Configurations
if (${PLATFORM} STREQUAL "Web")
  set_target_properties(mygame PROPERTIES SUFFIX ".html") # Tell Emscripten to build an example.html file.
  target_compile_definitions(mygame PRIVATE WEB_PLATFORM)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -s GL_ENABLE_GET_PROC_ADDRESS=1")
endif()
