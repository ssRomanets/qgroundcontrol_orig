target_sources(${CMAKE_PROJECT_NAME}
    PRIVATE
        AutoPilotPlugin.cc
        AutoPilotPlugin.h
        Generic/GenericAutoPilotPlugin.cc
        Generic/GenericAutoPilotPlugin.h
)

target_include_directories(${CMAKE_PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        Generic
)