# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.28.3)

# Reject any attempt to use a toolchain file. We must not use one because
# we could be downloading it here. If the CMAKE_TOOLCHAIN_FILE environment
# variable is set, the cache variable will have been initialized from it.
unset(CMAKE_TOOLCHAIN_FILE CACHE)
unset(ENV{CMAKE_TOOLCHAIN_FILE})

# We name the project and the target for the ExternalProject_Add() call
# to something that will highlight to the user what we are working on if
# something goes wrong and an error message is produced.

project(gstreamer_good_plugins-populate NONE)



include(ExternalProject)
ExternalProject_Add(gstreamer_good_plugins-populate
                     "UPDATE_DISCONNECTED" "False" "DOWNLOAD_EXTRACT_TIMESTAMP" "FALSE" "EXTERNALPROJECT_INTERNAL_ARGUMENT_SEPARATOR" "URL" "https://gstreamer.freedesktop.org/src/gst-plugins-good/gst-plugins-good-1.24.12.tar.xz" "GIT_SHALLOW" "TRUE"
                    SOURCE_DIR          "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/cpm_modules/gstreamer_good_plugins/4ee7a3c0aeebca164d994ed16e266c0e20abfea1"
                    BINARY_DIR          "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/gstreamer_good_plugins-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
                    USES_TERMINAL_DOWNLOAD  YES
                    USES_TERMINAL_UPDATE    YES
                    USES_TERMINAL_PATCH     YES
)


