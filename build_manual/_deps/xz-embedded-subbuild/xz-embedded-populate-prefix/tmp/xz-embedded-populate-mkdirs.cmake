# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/cpm_modules/xz-embedded/89e65918af62198be203b1131c88f91ffdee79cf"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/xz-embedded-build"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/xz-embedded-subbuild/xz-embedded-populate-prefix"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/xz-embedded-subbuild/xz-embedded-populate-prefix/tmp"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/xz-embedded-subbuild/xz-embedded-populate-prefix/src/xz-embedded-populate-stamp"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/xz-embedded-subbuild/xz-embedded-populate-prefix/src"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/xz-embedded-subbuild/xz-embedded-populate-prefix/src/xz-embedded-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/xz-embedded-subbuild/xz-embedded-populate-prefix/src/xz-embedded-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/xz-embedded-subbuild/xz-embedded-populate-prefix/src/xz-embedded-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
