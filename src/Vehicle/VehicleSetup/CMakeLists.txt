
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(NOT QGC_NO_SERIAL_LINK)

endif()

qt_add_library(VehicleSetupModule STATIC)

qt_add_qml_module(VehicleSetupModule
    URI QUAV.VehicleSetup
    VERSION 1.0
    RESOURCE_PREFIX /qml
    QML_FILES
        SetupView.qml
    NO_PLUGIN
)