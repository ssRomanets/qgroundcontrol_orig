qt_add_library(FlightDisplayModule STATIC)

qt_add_qml_module(FlightDisplayModule
    URI QUAV.FlightDisplay
    VERSION 1.0
    RESOURCE_PREFIX /qml
    QML_FILES
        FlightDisplayViewUVC.qml
        FlightDisplayViewUVF.qml
        FlyView.qml
        FlyViewBottomRightRowLayout.qml
        FlyViewInstrumentPanel.qml
        FlyViewMap.qml
        FlyViewTopRightPanel.qml
        FlyViewTopRightColumnLayout.qml
        FlyViewVideo.qml
        FlyViewWidgetLayer.qml
    NO_PLUGIN

    # Required by Qt 6.6.3. TODO: Remove when no longer supported.
)
