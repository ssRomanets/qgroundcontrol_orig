import QtQuick
import QtQuick.Controls
import QtLocation
import QtPositioning
import QtQuick.Dialogs
import Qt.labs.animation

import QUAV
import QUAV.FactSystem
import QUAV.Controls
import QUAV.FlightMap
import QUAV.ScreenTools

Map {
    id: m_map

    plugin:     Plugin { name: "QGroundControl" }
    opacity:    0.99 // https://bugreports.qt.io/browse/QTBUG-82185

    property string mapName:                        'defaultMap'
    property bool   isSatelliteMap:                 activeMapType.name.indexOf("Satellite") > -1 || activeMapType.name.indexOf("Hybrid") > -1
    property bool   allowVehicleLocationCenter:     false
    property bool   firstVehiclePositionReceived:   false
    property bool   planView:                       false

    property var    m_activeVehicleCoordinate:   QtPositioning.coordinate()

    function setVisibleRegion(region) {
        // TODO: Is this still necessary with Qt 5.11?
        // This works around a bug on Qt where if you set a visibleRegion and then the user moves or zooms the map
        // and then you set the same visibleRegion the map will not move/scale appropriately since it thinks there
        // is nothing to do.
        let maxZoomLevel = 20
        m_map.visibleRegion = QtPositioning.rectangle(QtPositioning.coordinate(0, 0), QtPositioning.coordinate(0, 0))
        m_map.visibleRegion = region
        if (m_map.zoomLevel > maxZoomLevel) {
            m_map.zoomLevel = maxZoomLevel
        }
    }

    function m_possiblyCenterToVehiclePosition() {
        if (!firstVehiclePositionReceived && allowVehicleLocationCenter && m_activeVehicleCoordinate.isValid) {
            firstVehiclePositionReceived = true
            center = m_activeVehicleCoordinate
            zoomLevel = QUAV.flightMapInitialZoom
        }
    }

    function updateactiveMapType() {
        var settings    = QUAV.settingsManager.flightMapSettings
        var fullMapName = settings.mapProvider.value + " " + settings.mapType.value

        for (var i = 0; i < m_map.supportedMapTypes.length; i++) {
            if (fullMapName === m_map.supportedMapTypes[i].name) {
                m_map.activeMapType = m_map.supportedMapTypes[i]
                return
            }
        }
    }

    onMapReadyChanged: {
        if (m_map.mapReady) {
            updateactiveMapType()
            m_possiblyCenterToVehiclePosition()
        }
    }

    Connections {
        target: QUAV.settingsManager.flightMapSettings.mapType
        function onRawValueChanged() {updateactiveMapType()}
    }

    Connections {
        target: QUAV.settingsManager.flightMapSettings.mapProvider
        function onRawValueChanged() {updateactiveMapType()}
    }

    signal mapPanStart
    signal mapPanStop
    signal mapClicked(var position)

    PinchHandler {
        id:     pinchHandler
        target: null

        property var pinchStartCentroid

        onActiveChanged: {
            if (active) {
                pinchStartCentroid = m_map.toCoordinate(pinchHandler.centroid.position, false)
            }
        }
        onScaleChanged: (delta) => {
            let newZoomLevel = Math.max(m_map.zoomLevel + Math.log2(delta), 0)
            m_map.zoomLevel = newZoomLevel
            m_map.alignCoordinateToPoint(pinchStartCentroid, pinchHandler.centroid.position)
        }
    }

    WheelHandler {
        // workaround for QTBUG-87646 / QTBUG-112394 / QTBUG-112432:
        // Magic Mouse pretends to be a trackpad but doesn't work with PinchHandler
        // and we don't yet distinguish mice and trackpads on Wayland either
        acceptedDevices:    Qt.platform.pluginName === "cocoa" || Qt.platform.pluginName === "wayland" ?
                                PointerDevice.Mouse | PointerDevice.TouchPad : PointerDevice.Mouse
        rotationScale:      1 / 120
        property:           "zoomLevel"

    }

    // We specifically do not use a DragHandler for panning. It just causes too many problems if you overlay anything else like a Flickable above it.
    // Causes all sorts of crazy problems where dragging/scrolling  no longerr works on items above in the hierarchy.
    // Since we are using a MouseArea we also can't use TapHandler for clicks. So we handle that here as well.
    MultiPointTouchArea {
        anchors.fill: parent
        maximumTouchPoints: 1
        mouseEnabled: true

        property bool dragActive: false
        property real lastMouseX
        property real lastMouseY

        onPressed: (touchPoints) => {
            lastMouseX = touchPoints[0].x
            lastMouseY = touchPoints[0].y
        }

        onGestureStarted: (gesture) => {
            dragActive = true
            gesture.grab()
            mapPanStart()
        }

        onUpdated: (touchPoints) => {
            if (dragActive) {
                let deltaX = touchPoints[0].x - lastMouseX
                let deltaY = touchPoints[0].y - lastMouseY
                if (Math.abs(deltaX) >= 1.0 || Math.abs(deltaY) >= 1.0) {
                    m_map.pan(lastMouseX - touchPoints[0].x, lastMouseY - touchPoints[0].y)
                    lastMouseX = touchPoints[0].x
                    lastMouseY = touchPoints[0].y
                }
            }
        }

        onReleased: (touchPoints) => {
            if (dragActive) {
                m_map.pan(lastMouseX - touchPoints[0].x, lastMouseY - touchPoints[0].y)
                dragActive = false
                mapPanStop()
            } else {
                mapClicked(Qt.point(touchPoints[0].x, touchPoints[0].y))
            }
        }
    }

} // Map
