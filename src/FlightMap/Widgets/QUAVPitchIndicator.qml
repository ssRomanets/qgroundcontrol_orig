import QtQuick
import QUAV.ScreenTools
import QUAV.Controls

Rectangle {
    property real pitchAngle:       0
    property real rollAngle:        0
    property real size:             100
    property real m_rectileHeight:  1
    property real m_reticleSpacing: size*0.15
    property real m_rectileSlot:    m_rectileSpacing + m_rectileHeight
    property real m_longDash:       size * 0.35
    property real m_shortDash:      size * 0.25
    property real m_fontSize:       ScreenTools.defaultFontPointSize*0.75

    height: size
    width:  size
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter:   parent.verticalCenter
    clip: true

    Item {
        height: parent.height
        width:  parent.width

        Column {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            spacing: m_reticleSpacing

            Repeater {
                model: 36

                Rectangle {
                    property int m_pitch: -(modelData*5 - 90)
                    anchors.horizontalCenter:   parent.horizontalCenter
                    width:                      (m_pitch % 10) === 0 ? m_longDash : m_shortDash
                    height:                     m_rectileHeight
                    color:                      "white"
                    antialiasing:               true
                    smooth:                     true

                    QUAVLabel {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.horizontalCenterOffset: -(m_longDash)
                        anchors.verticalCenter: parent.verticalCenter
                        smooth: true
                        font.bold: true
                        font.pointSize: m_fontSize
                        text: m_pitch
                        color: "white"
                        visible: (m_pitch != 0) && ((m_pitch % 10) === 0 )
                    }

                    QUAVLabel {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.horizontalCenterOffset: (m_longDash)
                        anchors.verticalCenter: parent.verticalCenter
                        smooth: true
                        font.bold: true
                        font.pointSize: m_fontSize
                        text: m_pitch
                        color: "white"
                        visible: (m_pitch != 0) && ((m_pitch % 10) === 0)
                    }
                }
            }
        }

        transform: [Translate {y: (pitchAngle*m_rectileSlot/5) - (m_rectileSlot/2)}]
    }

    transform: [
        Rotation {
            origin.x:   width/2
            origin.y:   height/2
            angle:      -rollAngle
        }

    ]
}




