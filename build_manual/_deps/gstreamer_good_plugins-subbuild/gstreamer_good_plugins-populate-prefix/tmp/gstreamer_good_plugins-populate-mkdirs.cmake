# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/cpm_modules/gstreamer_good_plugins/4ee7a3c0aeebca164d994ed16e266c0e20abfea1"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/gstreamer_good_plugins-build"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/gstreamer_good_plugins-subbuild/gstreamer_good_plugins-populate-prefix"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/gstreamer_good_plugins-subbuild/gstreamer_good_plugins-populate-prefix/tmp"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/gstreamer_good_plugins-subbuild/gstreamer_good_plugins-populate-prefix/src/gstreamer_good_plugins-populate-stamp"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/gstreamer_good_plugins-subbuild/gstreamer_good_plugins-populate-prefix/src"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/gstreamer_good_plugins-subbuild/gstreamer_good_plugins-populate-prefix/src/gstreamer_good_plugins-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/gstreamer_good_plugins-subbuild/gstreamer_good_plugins-populate-prefix/src/gstreamer_good_plugins-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/gstreamer_good_plugins-subbuild/gstreamer_good_plugins-populate-prefix/src/gstreamer_good_plugins-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
