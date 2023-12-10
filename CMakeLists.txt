cmake_minimum_required (VERSION 3.10)

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
  src/cartridge.cc
  src/cpu.cc
  src/logging.cc
  src/ppu.cc
  src/sfml_screen.cc
)

add_library (nes STATIC ${SRCS})
target_link_libraries(nes glog::glog sfml-graphics)

add_executable (nes_emulator src/main.cc)
target_link_libraries(nes_emulator nes)

add_executable (sfml_test src/sfml-test.cc)
target_link_libraries(sfml_test sfml-graphics)

###################################################################
#                            TEST ZONE                            #                        
###################################################################

# Add a new test case
#
# TEST_NAME is the name of the test, it must be a global unique name.
function (ADD_NES_TEST TEST_NAME)
  add_executable("${TEST_NAME}" 
    "${CMAKE_SOURCE_DIR}/src/${TEST_NAME}.cc"
    "${CMAKE_SOURCE_DIR}/src/test_main.cc")
  target_link_libraries("${TEST_NAME}" PUBLIC nes GTest::gtest)
  gtest_discover_tests("${TEST_NAME}")
endfunction()

if (WITH_TEST)
  enable_testing()
  include(GoogleTest)
  find_package(GTest REQUIRED)
  add_nes_test(cartridge-test)
endif()