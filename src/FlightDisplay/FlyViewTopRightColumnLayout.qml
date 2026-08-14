import QtQuick
import QtQuick.Layouts

import QUAV.FlightDisplay
import QUAV.FlightMap

ColumnLayout {
    spacing:            m_layoutSpacing
    width: m_rightPanelWidth

    // We use a Loader to load the photoVideoControlComponent only when the active vehicle is not null
    // This make it easier to implement PhotoVideoControl without having to check for the mavlink camera
    // to be null all over the place
    Loader {
        id:                 photoVideoControlLoader
        Layout.alignment:   Qt.AlignTop | Qt.AlignRight
        sourceComponent:    globals.activeVehicle ? photoVideoControlComponent : undefined

        property real rightEdgeCenterInset: visible ? parent.width - x : 0

        Component {
            id: photoVideoControlComponent

            PhotoVideoControl {   }
        }
    }

    Timer {
        id: timer
        interval: 1000;
        running:  true;
        repeat:   true;
        onTriggered: {
            if (!globals.activeVehicle)
            {
                globals.toggleVideoRecordingAct = !globals.toggleVideoRecordingAct
            }
        }
    }
}

