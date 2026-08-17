# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/cpm_modules/ulog_cpp/7a69708ff89747582667f268eeaee7cd22ba9f41"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/ulog_cpp-build"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/ulog_cpp-subbuild/ulog_cpp-populate-prefix"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/ulog_cpp-subbuild/ulog_cpp-populate-prefix/tmp"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/ulog_cpp-subbuild/ulog_cpp-populate-prefix/src/ulog_cpp-populate-stamp"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/ulog_cpp-subbuild/ulog_cpp-populate-prefix/src"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/ulog_cpp-subbuild/ulog_cpp-populate-prefix/src/ulog_cpp-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/ulog_cpp-subbuild/ulog_cpp-populate-prefix/src/ulog_cpp-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/ulog_cpp-subbuild/ulog_cpp-populate-prefix/src/ulog_cpp-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
