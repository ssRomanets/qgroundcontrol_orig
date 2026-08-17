# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/cpm_modules/libevents/1ea2d34e1cc3658140fb45034b187e87d60c899b"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/libevents-build"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/libevents-subbuild/libevents-populate-prefix"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/libevents-subbuild/libevents-populate-prefix/tmp"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/libevents-subbuild/libevents-populate-prefix/src/libevents-populate-stamp"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/libevents-subbuild/libevents-populate-prefix/src"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/libevents-subbuild/libevents-populate-prefix/src/libevents-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/libevents-subbuild/libevents-populate-prefix/src/libevents-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/libevents-subbuild/libevents-populate-prefix/src/libevents-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
