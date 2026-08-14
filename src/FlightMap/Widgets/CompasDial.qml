import QtQuick

import QUAV
import QUAV.Controls
import QUAV.ScreenTools
import QUAV.Palette

Item {
    id: control

    property real offsetRadius: width/2 - ScreenTools.defaultFontPixelHeight/2

    function translateCenterToAngleX(radius, angle) { return radius*Math.sin(angle*(Math.PI/180)) }

    function translateCenterToAngleY(radius, angle) { return -radius*Math.cos(angle*(Math.PI/180)) }

    QUAVLabel {
        anchors.centerIn: parent
        text:             "N"

        transform: Translate {
            x: translateCenterToAngleX(control.offsetRadius, 0)
            y: translateCenterToAngleY(control.offsetRadius, 0)
        }
    }

    QUAVLabel {
        anchors.centerIn: parent
        text:             "E"

        transform: Translate {
            x: translateCenterToAngleX(control.offsetRadius, 90)
            y: translateCenterToAngleY(control.offsetRadius, 90)
        }
    }

    QUAVLabel {
        anchors.centerIn: parent
        text:             "S"

        transform: Translate {
            x: translateCenterToAngleX(control.offsetRadius, 180)
            y: translateCenterToAngleY(control.offsetRadius, 180)
        }
    }

    QUAVLabel {
        anchors.centerIn: parent
        text:             "W"

        transform: Translate {
            x: translateCenterToAngleX(control.offsetRadius, 270)
            y: translateCenterToAngleY(control.offsetRadius, 270)
        }
    }

    // Major tick marks
    Repeater {
        model: 4

        Rectangle {
            id:         majorTick
            x:          size/2
            width:      1
            height:     ScreenTools.defaultFontPixelHeight*0.5
            color:      quavPal.text

            transform: Rotation {
                origin.x: 0
                origin.y: size/2
                angle:    45 + (90*index)
            }
        }
    }

    // Minor tick marks
    Repeater {
        model: 8

        Rectangle {
            id: minorTick
            x:  size/2
            y:  m_margin
            width: 1
            height: m_margin
            color: quavPal.text

            property real m_margin: ScreenTools.defaultFontPixelHeight*0.25

            transform: Rotation {
                origin.x: 0
                origin.y: size/2 - m_margin
                angle: 45/2 + (45*index)
            }
        }
    }
}


