target_sources(${CMAKE_PROJECT_NAME}
    PRIVATE
        MavlinkCameraControl.cc
        MavlinkCameraControl.h
        QUAVCameraManager.cc
        QUAVCameraManager.h
        SimulatedCameraControl.cc
        SimulatedCameraControl.h
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})