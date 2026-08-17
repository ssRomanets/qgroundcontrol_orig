# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/cpm_modules/px4-gpsdrivers/ace289105774a9d05d40a1d89798051a4e2937e7"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/px4-gpsdrivers-build"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/px4-gpsdrivers-subbuild/px4-gpsdrivers-populate-prefix"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/px4-gpsdrivers-subbuild/px4-gpsdrivers-populate-prefix/tmp"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/px4-gpsdrivers-subbuild/px4-gpsdrivers-populate-prefix/src/px4-gpsdrivers-populate-stamp"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/px4-gpsdrivers-subbuild/px4-gpsdrivers-populate-prefix/src"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/px4-gpsdrivers-subbuild/px4-gpsdrivers-populate-prefix/src/px4-gpsdrivers-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/px4-gpsdrivers-subbuild/px4-gpsdrivers-populate-prefix/src/px4-gpsdrivers-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/px4-gpsdrivers-subbuild/px4-gpsdrivers-populate-prefix/src/px4-gpsdrivers-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
