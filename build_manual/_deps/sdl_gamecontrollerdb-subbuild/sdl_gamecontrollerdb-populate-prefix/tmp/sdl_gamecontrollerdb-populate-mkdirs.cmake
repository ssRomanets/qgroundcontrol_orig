# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/cpm_modules/sdl_gamecontrollerdb/df26e45208ba3957c7751b0f686509f2e22f9a5f"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/sdl_gamecontrollerdb-build"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/sdl_gamecontrollerdb-subbuild/sdl_gamecontrollerdb-populate-prefix"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/sdl_gamecontrollerdb-subbuild/sdl_gamecontrollerdb-populate-prefix/tmp"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/sdl_gamecontrollerdb-subbuild/sdl_gamecontrollerdb-populate-prefix/src/sdl_gamecontrollerdb-populate-stamp"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/sdl_gamecontrollerdb-subbuild/sdl_gamecontrollerdb-populate-prefix/src"
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/sdl_gamecontrollerdb-subbuild/sdl_gamecontrollerdb-populate-prefix/src/sdl_gamecontrollerdb-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/sdl_gamecontrollerdb-subbuild/sdl_gamecontrollerdb-populate-prefix/src/sdl_gamecontrollerdb-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/sdl_gamecontrollerdb-subbuild/sdl_gamecontrollerdb-populate-prefix/src/sdl_gamecontrollerdb-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
