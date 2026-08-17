# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/cpm_modules/geographiclib/9b427b28c42ccd9514fdbdfc56310bccb9a90d0c"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/geographiclib-build"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/geographiclib-subbuild/geographiclib-populate-prefix"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/geographiclib-subbuild/geographiclib-populate-prefix/tmp"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/geographiclib-subbuild/geographiclib-populate-prefix/src/geographiclib-populate-stamp"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/geographiclib-subbuild/geographiclib-populate-prefix/src"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/geographiclib-subbuild/geographiclib-populate-prefix/src/geographiclib-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/geographiclib-subbuild/geographiclib-populate-prefix/src/geographiclib-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/geographiclib-subbuild/geographiclib-populate-prefix/src/geographiclib-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
