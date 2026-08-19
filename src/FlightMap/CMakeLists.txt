qt_add_library(FlightMapModule STATIC)

qt_add_qml_module(FlightMapModule
    URI QUAV.FlightMap
    VERSION 1.0
    RESOURCE_PREFIX /qml
    QML_FILES
        Widgets/CompasDial.qml
        Widgets/CompassHeadingIndicator.qml
        Widgets/HorizontalCompassAttitude.qml
        Widgets/IntegratedAttitudeIndicator.qml
        Widgets/IntegratedCompassAttitude.qml
        Widgets/PhotoVideoControl.qml
        Widgets/QUAVAttitudeWidget.qml
        Widgets/QUAVArtificialHorizon.qml
        Widgets/QUAVCompassWidget.qml
        Widgets/QUAVPitchIndicator.qml
        Widgets/VerticalCompassAttitude.qml
        FlightMap.qml
        MapScale.qml
        ProximityRadarValues.qml
        ProximityRadarVideoView.qml
    NO_PLUGIN

    # Required by Qt 6.6.3. TODO: Remove when no longer supported.
)
