set(UTMSP_RESOURCES)

if(NOT QGC_UTM_ADAPTER)
    return()
endif()

# CPMFindPackage(
#     NAME nlohmann_json
#     VERSION 3.11.3
#     GITHUB_REPOSITORY nlohmann/json
# )

find_package(Threads REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME}
    PRIVATE
        # nlohmann_json
        Threads::Threads
)


target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE QGC_UTM_ADAPTER)

# qt_add_qml_module(QGC
#     URI QGroundControl.UTMSP
#     VERSION 1.0
#     QML_FILES
#         UTMSPActivationStatusBar.qml
#         UTMSPAdapterEditor.qml
#         UTMSPMapPolygonVisuals.qml
#         UTMSPMapVisuals.qml
#         UTMSPStateStorage.qml
#     OUTPUT_TARGETS UTMSP_targets
#     RESOURCES
#         images/date.svg
#         images/green.png
#         images/load.gif
#         images/red.png
#         images/time.svg
#         images/timer.gif
#     IMPORT_PATH ${QT_QML_OUTPUT_DIRECTORY}
#     IMPORTS
#         QtQuick
#         QtQuick.Layouts
#         QtQuick.Dialogs
#         QtQuick.Controls
#         QtPositioning
#         QtLocation

#         QGroundControl
#         QGroundControl.FlightMap
#         QGroundControl.ScreenTools
#         QGroundControl.Controls
#         QGroundControl.FactSystem
#         QGroundControl.FactControls
#         QGroundControl.Palette
#         QGroundControl.Controllers
#         QGroundControl.ShapeFileHelper
#         QGroundControl.UTMSP
#         QGroundControl.FlightDisplay
#         QGroundControl.MultiVehicleManager
# )
