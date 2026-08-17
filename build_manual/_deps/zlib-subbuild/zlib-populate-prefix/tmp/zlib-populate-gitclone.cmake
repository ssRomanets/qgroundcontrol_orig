# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

if(EXISTS "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/zlib-subbuild/zlib-populate-prefix/src/zlib-populate-stamp/zlib-populate-gitclone-lastrun.txt" AND EXISTS "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/zlib-subbuild/zlib-populate-prefix/src/zlib-populate-stamp/zlib-populate-gitinfo.txt" AND
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/zlib-subbuild/zlib-populate-prefix/src/zlib-populate-stamp/zlib-populate-gitclone-lastrun.txt" IS_NEWER_THAN "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/zlib-subbuild/zlib-populate-prefix/src/zlib-populate-stamp/zlib-populate-gitinfo.txt")
  message(STATUS
    "Avoiding repeated git clone, stamp file is up to date: "
    "'/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/zlib-subbuild/zlib-populate-prefix/src/zlib-populate-stamp/zlib-populate-gitclone-lastrun.txt'"
  )
  return()
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E rm -rf "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/cpm_modules/zlib/b6adfce2bc5987edc7cfab3490c73021e521991f"
  RESULT_VARIABLE error_code
)
if(error_code)
  message(FATAL_ERROR "Failed to remove directory: '/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/cpm_modules/zlib/b6adfce2bc5987edc7cfab3490c73021e521991f'")
endif()

# try the clone 3 times in case there is an odd git clone issue
set(error_code 1)
set(number_of_tries 0)
while(error_code AND number_of_tries LESS 3)
  execute_process(
    COMMAND "/usr/bin/git"
            clone --no-checkout --depth 1 --no-single-branch --config "advice.detachedHead=false" "https://github.com/madler/zlib.git" "b6adfce2bc5987edc7cfab3490c73021e521991f"
    WORKING_DIRECTORY "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/cpm_modules/zlib"
    RESULT_VARIABLE error_code
  )
  math(EXPR number_of_tries "${number_of_tries} + 1")
endwhile()
if(number_of_tries GREATER 1)
  message(STATUS "Had to git clone more than once: ${number_of_tries} times.")
endif()
if(error_code)
  message(FATAL_ERROR "Failed to clone repository: 'https://github.com/madler/zlib.git'")
endif()

execute_process(
  COMMAND "/usr/bin/git"
          checkout "develop" --
  WORKING_DIRECTORY "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/cpm_modules/zlib/b6adfce2bc5987edc7cfab3490c73021e521991f"
  RESULT_VARIABLE error_code
)
if(error_code)
  message(FATAL_ERROR "Failed to checkout tag: 'develop'")
endif()

set(init_submodules TRUE)
if(init_submodules)
  execute_process(
    COMMAND "/usr/bin/git" 
            submodule update --recursive --init 
    WORKING_DIRECTORY "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/cpm_modules/zlib/b6adfce2bc5987edc7cfab3490c73021e521991f"
    RESULT_VARIABLE error_code
  )
endif()
if(error_code)
  message(FATAL_ERROR "Failed to update submodules in: '/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/cpm_modules/zlib/b6adfce2bc5987edc7cfab3490c73021e521991f'")
endif()

# Complete success, update the script-last-run stamp file:
#
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/zlib-subbuild/zlib-populate-prefix/src/zlib-populate-stamp/zlib-populate-gitinfo.txt" "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/zlib-subbuild/zlib-populate-prefix/src/zlib-populate-stamp/zlib-populate-gitclone-lastrun.txt"
  RESULT_VARIABLE error_code
)
if(error_code)
  message(FATAL_ERROR "Failed to copy script-last-run stamp file: '/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/zlib-subbuild/zlib-populate-prefix/src/zlib-populate-stamp/zlib-populate-gitclone-lastrun.txt'")
endif()
