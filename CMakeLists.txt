cmake_minimum_required (VERSION 3.1)

project (nes_emulator
  VERSION 0.0.0
  HOMEPAGE_URL https://github.com/GoMarck/assembler
  DESCRIPTION "C implementation of the assembler emulator"
  LANGUAGES CXX
)

option (FAIL_ON_WARNING "Build fail if meets compile warning" ON)
option (WITH_TEST "Build unit test codes" ON)

# Add common flags
# -std=c++11: use C++ 11 standard
set (CMAKE_CXX_FLAGS "-std=c++11")
# -Wall: allow all warnings
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
if (FAIL_ON_WARNING)
  # -Werror: treat warning as error
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif ()

find_package(SFML 2 COMPONENTS graphics REQUIRED)
find_package(glog REQUIRED)

include_directories (${CMAKE_SOURCE_DIR}/include)

set (SRCS
  src/cpu.cc
  src/logging.cc
  src/main.cc
  src/ppu.cc
  src/sfml_screen.cc
)

add_executable (nes_emulator ${SRCS})
target_link_libraries(nes_emulator glog::glog sfml-graphics)