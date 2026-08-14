target_sources(${CMAKE_PROJECT_NAME}
    PRIVATE
        MAVLinkLib.h
        QUAVMAVLink.cc
        QUAVMAVLink.h
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

#===========================================================================#

message(STATUS "Building MAVLink")

CPMAddPackage(
    NAME mavlink
    GIT_REPOSITORY ${QGC_MAVLINK_GIT_REPO}
    GIT_TAG ${QGC_MAVLINK_GIT_TAG}
)

# For QGC all dialects means common and development. Though use of development mavlink code should be restricted to debug builds only.
target_include_directories(${CMAKE_PROJECT_NAME}
    PRIVATE
        ${mavlink_SOURCE_DIR}
        ${mavlink_SOURCE_DIR}/all
        ${mavlink_SOURCE_DIR}/common
        ${mavlink_SOURCE_DIR}/development
)

add_subdirectory(LibEvents)