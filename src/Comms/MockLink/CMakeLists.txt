if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    return()
endif()

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
