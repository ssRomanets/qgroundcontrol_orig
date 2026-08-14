import QtQuick

/// Object which exposes vehicle distanceSensors FactGroup information for use in UI
QtObject {
    property var  vehicle
    property bool telemetryAvailable: vehicle && vehicle.telemetryAvailable
}
