# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/cpm_modules/mavlink/8e683429ddf7832888da831c8fd3e2acf6945b8f"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/mavlink-build"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/mavlink-subbuild/mavlink-populate-prefix"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/mavlink-subbuild/mavlink-populate-prefix/tmp"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/mavlink-subbuild/mavlink-populate-prefix/src/mavlink-populate-stamp"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/mavlink-subbuild/mavlink-populate-prefix/src"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/mavlink-subbuild/mavlink-populate-prefix/src/mavlink-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/mavlink-subbuild/mavlink-populate-prefix/src/mavlink-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/mavlink-subbuild/mavlink-populate-prefix/src/mavlink-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
