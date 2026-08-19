target_sources(${CMAKE_PROJECT_NAME}
    PRIVATE
        DeviceInfo.cc
        DeviceInfo.h
        JsonHelper.cc
        JsonHelper.h
        QUAV.cc
        QUAV.h
        StateMachine.cc
        StateMachine.h
)

if(LINUX)

elseif(IOS)

endif()

target_include_directories(${CMAKE_PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        FileSystem
)

