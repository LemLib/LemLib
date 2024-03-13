# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/Mavericks/Desktop/LemLib/build/googletest-src"
  "C:/Users/Mavericks/Desktop/LemLib/build/googletest-build"
  "C:/Users/Mavericks/Desktop/LemLib/build/external-dependencies/googletest-prefix"
  "C:/Users/Mavericks/Desktop/LemLib/build/external-dependencies/googletest-prefix/tmp"
  "C:/Users/Mavericks/Desktop/LemLib/build/external-dependencies/googletest-prefix/src/googletest-stamp"
  "C:/Users/Mavericks/Desktop/LemLib/build/external-dependencies/googletest-prefix/src"
  "C:/Users/Mavericks/Desktop/LemLib/build/external-dependencies/googletest-prefix/src/googletest-stamp"
)

set(configSubDirs Debug;Release;MinSizeRel;RelWithDebInfo)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/Mavericks/Desktop/LemLib/build/external-dependencies/googletest-prefix/src/googletest-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/Mavericks/Desktop/LemLib/build/external-dependencies/googletest-prefix/src/googletest-stamp${cfgdir}") # cfgdir has leading slash
endif()
