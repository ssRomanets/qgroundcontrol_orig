# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

if(EXISTS "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/xz-embedded-subbuild/xz-embedded-populate-prefix/src/xz-embedded-populate-stamp/xz-embedded-populate-gitclone-lastrun.txt" AND EXISTS "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/xz-embedded-subbuild/xz-embedded-populate-prefix/src/xz-embedded-populate-stamp/xz-embedded-populate-gitinfo.txt" AND
  "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/xz-embedded-subbuild/xz-embedded-populate-prefix/src/xz-embedded-populate-stamp/xz-embedded-populate-gitclone-lastrun.txt" IS_NEWER_THAN "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/xz-embedded-subbuild/xz-embedded-populate-prefix/src/xz-embedded-populate-stamp/xz-embedded-populate-gitinfo.txt")
  message(STATUS
    "Avoiding repeated git clone, stamp file is up to date: "
    "'/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/xz-embedded-subbuild/xz-embedded-populate-prefix/src/xz-embedded-populate-stamp/xz-embedded-populate-gitclone-lastrun.txt'"
  )
  return()
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E rm -rf "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/cpm_modules/xz-embedded/89e65918af62198be203b1131c88f91ffdee79cf"
  RESULT_VARIABLE error_code
)
if(error_code)
  message(FATAL_ERROR "Failed to remove directory: '/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/cpm_modules/xz-embedded/89e65918af62198be203b1131c88f91ffdee79cf'")
endif()

# try the clone 3 times in case there is an odd git clone issue
set(error_code 1)
set(number_of_tries 0)
while(error_code AND number_of_tries LESS 3)
  execute_process(
    COMMAND "/usr/bin/git"
            clone --no-checkout --depth 1 --no-single-branch --config "advice.detachedHead=false" "https://github.com/tukaani-project/xz-embedded.git" "89e65918af62198be203b1131c88f91ffdee79cf"
    WORKING_DIRECTORY "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/cpm_modules/xz-embedded"
    RESULT_VARIABLE error_code
  )
  math(EXPR number_of_tries "${number_of_tries} + 1")
endwhile()
if(number_of_tries GREATER 1)
  message(STATUS "Had to git clone more than once: ${number_of_tries} times.")
endif()
if(error_code)
  message(FATAL_ERROR "Failed to clone repository: 'https://github.com/tukaani-project/xz-embedded.git'")
endif()

execute_process(
  COMMAND "/usr/bin/git"
          checkout "v2024-12-30" --
  WORKING_DIRECTORY "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/cpm_modules/xz-embedded/89e65918af62198be203b1131c88f91ffdee79cf"
  RESULT_VARIABLE error_code
)
if(error_code)
  message(FATAL_ERROR "Failed to checkout tag: 'v2024-12-30'")
endif()

set(init_submodules TRUE)
if(init_submodules)
  execute_process(
    COMMAND "/usr/bin/git" 
            submodule update --recursive --init 
    WORKING_DIRECTORY "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/cpm_modules/xz-embedded/89e65918af62198be203b1131c88f91ffdee79cf"
    RESULT_VARIABLE error_code
  )
endif()
if(error_code)
  message(FATAL_ERROR "Failed to update submodules in: '/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/cpm_modules/xz-embedded/89e65918af62198be203b1131c88f91ffdee79cf'")
endif()

# Complete success, update the script-last-run stamp file:
#
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/xz-embedded-subbuild/xz-embedded-populate-prefix/src/xz-embedded-populate-stamp/xz-embedded-populate-gitinfo.txt" "/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/xz-embedded-subbuild/xz-embedded-populate-prefix/src/xz-embedded-populate-stamp/xz-embedded-populate-gitclone-lastrun.txt"
  RESULT_VARIABLE error_code
)
if(error_code)
  message(FATAL_ERROR "Failed to copy script-last-run stamp file: '/home/aleksandr/QGC/qgroundcontrol_orig/build_manual/_deps/xz-embedded-subbuild/xz-embedded-populate-prefix/src/xz-embedded-populate-stamp/xz-embedded-populate-gitclone-lastrun.txt'")
endif()
