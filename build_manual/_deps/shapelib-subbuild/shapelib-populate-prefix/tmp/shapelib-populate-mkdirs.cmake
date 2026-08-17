# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/cpm_modules/shapelib/a995950de7c483318cb49c46205161ff77220e22"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/shapelib-build"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/shapelib-subbuild/shapelib-populate-prefix"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/shapelib-subbuild/shapelib-populate-prefix/tmp"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/shapelib-subbuild/shapelib-populate-prefix/src/shapelib-populate-stamp"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/shapelib-subbuild/shapelib-populate-prefix/src"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/shapelib-subbuild/shapelib-populate-prefix/src/shapelib-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/shapelib-subbuild/shapelib-populate-prefix/src/shapelib-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/shapelib-subbuild/shapelib-populate-prefix/src/shapelib-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
