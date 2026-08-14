import QtQuick

import QUAV
import QUAV.Controls
import QUAV.ScreenTools
import QUAV.FlightDisplay
import QUAV.FlightMap

Item {
    id: control
    implicitWidth:  (compasRadius*2) + attitudeSpacing+ attitudeSize
    implicitHeight: implicitWidth

    property alias attitudeSize:    rollIndicator.attitudeSize
    property alias attitudeSpacing: rollIndicator.attitudeSpacing

    property real defaultCompassRadius:         (mainWindow.width * 0.15) / 2
    property real maxCompassRadius:             ScreenTools.defaultFontPixelHeight * 7 / 2

    property real  compasRadius:                1.5*Math.min(defaultCompassRadius, maxCompassRadius)
    property real  compassBorder:               ScreenTools.defaultFontPixelHeight/2
    property var   vehicle:                     globals.activeVehicle
    property var   quavPal:                     QUAV.globalPalette

    property real m_totalAttitudeSize: attitudeSize + attitudeSpacing

    IntegratedAttitudeIndicator {
        id:                     rollIndicator
        x:                      - m_totalAttitudeSize
        attitudeAngleDegrees:   vehicle ? vehicle.roll.rawValue : 0
        compassRadius:          control.compasRadius
    }

    IntegratedAttitudeIndicator {
        x:                    - m_totalAttitudeSize
        attitudeAngleDegrees: vehicle ? vehicle.pitch.rawValue : 0
        compassRadius:        control.compasRadius
        attitudeSize:         control.attitudeSize
        attitudeSpacing:      control.attitudeSpacing
        transformOrigin:      Item.Center
        rotation:             90
    }

    Rectangle {
        y: m_totalAttitudeSize
        width: compasRadius*2
        height: width
        radius: width/2
        color: quavPal.window

        QUAVCompassWidget {
            size:                      parent.width - compassBorder
            vehicle:                   control.vehicle
            anchors.centerIn:          parent
        }
    }
}































