import QtQuick
import QtQuick.Effects

import QUAV
import QUAV.Controls
import QUAV.ScreenTools
import QUAV.Palette

Item {
    id: root

    property var vehicle: null
    property real size

    property real m_rollAngle: vehicle ? vehicle.roll.rawValue : 0
    property real m_pitchAngle: vehicle ? vehicle.pitch.rawValue : 0

    width:      size
    height:     size

    QUAVPalette {id: quavPal; colorGroupEnabled: enabled}
    
    Item {
        id:             instrument
        anchors.fill:   parent
        visible:        false

        //-- Artificial Horizon
        QUAVArtificialHorizon {
            rollAngle: m_rollAngle
            pitchAngle: m_pitchAngle
            anchors.fill: parent
        }

        //-- Pointer
        Image {
            id: pointer
            source:  "/qmlimages/attitudePointer.svg"
            mipmap: true
            fillMode: Image.PreserveAspectFit
            anchors.fill: parent
            sourceSize.height: parent.height
        }

        //-- Instrument Dial
        Image {
            id:                 instrumentDial
            source:             "/qmlimages/attitudeDial.svg"
            mipmap:             true
            fillMode:           Image.PreserveAspectFit
            anchors.fill:       parent
            sourceSize.height:  parent.height
            transform: Rotation {
                origin.x:   root.width/2
                origin.y:   root.height/2
                angle:      -m_rollAngle
            }
        }

        QUAVPitchIndicator {
            id:                     pitchWidget
            visible:                true
            size:                   root.size*0.5
            anchors.verticalCenter: parent.verticalCenter
            pitchAngle:             m_pitchAngle
            rollAngle:              m_rollAngle
            color:                  Qt.rgba(0,0,0,0)
        }

        //-- Cross Hair
        Image {
            id:                 crossHair
            anchors.centerIn:   parent
            source:             "/qmlimages/crossHair.svg"
            mipmap:             true
            width:              size*0.75
            sourceSize.width:   width
            fillMode:           Image.PreserveAspectFit
        }
    }

    MultiEffect {
        source: instrument
        anchors.fill: instrument
        maskEnabled: true
        maskSource: mask
    }

    Item {
        id: mask
        width: instrument.width
        height:  instrument.height
        layer.enabled: true
        visible: false

        Rectangle {
            width: parent.width
            height: parent.height
            radius: width/2
            color: "black"
        }
    }

    Rectangle {
        id:             borderRect
        anchors.fill:   parent
        radius:         width/2
        color:          Qt.rgba(0,0,0,0)
        border.color:   quavPal.text
        border.width:   1
    }

    QUAVLabel {
        anchors.bottomMargin:       Math.round(ScreenTools.defaultFontPixelHeight*0.75)
        anchors.bottom:             parent.bottom
        anchors.horizontalCenter:   parent.horizontalCenter
        text:                       m_headingString3
        color:                      "white"
        visible:                    true

        property string m_headingString: vehicle ? vehicle.heading.rawValue.toFixed(0) : "OFF"
        property string m_headingString2: m_headingString.length === 1 ? "0" + m_headingString : m_headingString
        property string m_headingString3: m_headingString.length === 2 ? "0" + m_headingString2 : m_headingString2
    }
}
















