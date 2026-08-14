target_sources(${CMAKE_PROJECT_NAME}
    PRIVATE
        InitialConnectStateMachine.cc
        InitialConnectStateMachine.h
        MultiVehicleManager.cc
        MultiVehicleManager.h
        StandardModes.cc
        StandardModes.h
        Vehicle.cc
        Vehicle.h
        VehicleLinkManager.cc
        VehicleLinkManager.h
)

qt_add_library(VehicleModule STATIC)

qt_add_qml_module(VehicleModule
    URI QUAV.Vehicle
    VERSION 1.0
    RESOURCE_PREFIX /qml
    NO_PLUGIN # Required by Qt 6.6.3. TODO: Remove when no longer supported.
)

add_subdirectory(FactGroups)
add_subdirectory(VehicleSetup)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
