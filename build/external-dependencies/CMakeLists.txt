cmake_minimum_required(VERSION 3.6)

project(external-dependencies NONE)

include(ExternalProject)

ExternalProject_Add(googletest
  GIT_REPOSITORY    https://github.com/google/googletest.git
  GIT_TAG           release-1.12.1
  SOURCE_DIR        "C:/Users/Mavericks/Desktop/LemLib/build/googletest-src"
  BINARY_DIR        "C:/Users/Mavericks/Desktop/LemLib/build/googletest-build"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND     ""
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)
