import QtQuick

import QUAV
import QUAV.Controls
import QUAV.ScreenTools
import QUAV.Palette

Rectangle {
    id:             root
    width:          size
    height:         size
    radius:         width / 2
    color:          quavPal.window
    border.color:   quavPal.text
    border.width:   0
    opacity:        vehicle && !vehicle.armed ? 0.5 : 1

    property real size:                         m_defaultSize
    property var  vehicle:                      null

    property real m_defaultSize:                ScreenTools.defaultFontPixelHeight * 10
    property real m_heading:                     vehicle ? vehicle.heading.rawValue : 0
    property real m_headingToHome:               /*vehicle ? vehicle.headingToHome.rawValue :*/ 0
    property real m_groundSpeed:                 /*vehicle ? vehicle.groundSpeed.rawValue :*/ 0

    property real m_headingToNextWP:             /*vehicle ? vehicle.headingToNextWP.rawValue :*/ 0
    property real m_courseOverGround:            /*vehicle ? vehicle.gps.courseOverGround.rawValue :*/ 0
    property var  m_flyViewSettings:             QUAV.settingsManager.flyViewSettings
    property bool m_showAdditionalIndicators:    m_flyViewSettings.showAdditionalIndicatorsCompass.value
    property bool m_lockNoseUpCompass:           m_flyViewSettings.lockNoseUpCompass.value

    function showCOG() {
        if (m_groundSpeed < 0.5) {  return false } else { return vehicle && m_showAdditionalIndicators }
    }

    function showHeadingHome() { return vehicle && m_showAdditionalIndicators && !isNaN(m_headingToHome)}

    function showHeadingToNextWP() { return vehicle && m_showAdditionalIndicators && !isNaN(m_headingToNextWP) }

    function translateCenterToAngleX(radius, angle) {   return radius * Math.sin(angle * (Math.PI / 180)) }

    function translateCenterToAngleY(radius, angle) {  return -radius * Math.cos(angle * (Math.PI / 180)) }

    QUAVPalette { id: quavPal; colorGroupEnabled: enabled }

    Item {
        id:             rotationParent
        anchors.fill:   parent

        transform: Rotation {
            origin.x:       rotationParent.width  / 2
            origin.y:       rotationParent.height / 2
            angle:         m_lockNoseUpCompass ? -m_heading : 0
        }

        CompasDial {
            anchors.fill: parent
            visible: true
        }

        CompassHeadingIndicator {
            compassSize:    size
            heading:        m_heading
            simplified:     false
        }

        Image {
            id:                  cogPointer
            source:              "/qmlimages/cOGPointer.svg"
            mipmap:              true
            fillMode:            Image.PreserveAspectFit
            anchors.fill:        parent
            sourceSize.height:   parent.height
            visible:             showCOG()

            transform: Rotation {
                origin.x: cogPointer.width/2
                origin.y: cogPointer.height/2
                angle: m_courseOverGround
            }
        }

        Image {
            id:                 nextWPPointer
            source:             "/qmlimages/compassDottedLine.svg"
            mipmap: true
            fillMode: Image.PreserveAspectFit
            anchors.fill: parent
            sourceSize.height: parent.height
            visible: showHeadingToNextWP()

            transform: Rotation {
                origin.x:   nextWPPointer.width/2
                origin.y:   nextWPPointer.height/2
                angle:      m_headingToNextWP
            }
        }

        // Launch location indicator
        Rectangle {
            width: Math.max(label.contentWidth, label.contentHeight)
            height: width
            color: quavPal.mapIndicator
            radius: width/2
            anchors.centerIn: parent
            visible: showHeadingHome()

            QUAVLabel {
                id:               label
                text:             qsTr("L")
                font.bold:        true
                color:            quavPal.text
                anchors.centerIn: parent
            }

            transform: Translate {
                property double m_angle: m_headingToHome

                x: translateCenterToAngleX(parent.width/2, m_angle)
                y: translateCenterToAngleY(parent.height/2, m_angle)
            }
        }
    }

    QUAVLabel {
        anchors.horizontalCenter:   parent.horizontalCenter
        y:                          size*0.74
        text:                       vehicle ? m_heading.toFixed(0) +  "°" : ""
        horizontalAlignment:        Text.AlignHCenter
    }
}
