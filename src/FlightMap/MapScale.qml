import QtQuick
import QtQuick.Controls

import QUAV
import QUAV.Controls
import QUAV.ScreenTools
import QUAV.SettingsManager

Item {
    id: scale
    width: buttonsOnLeft || !m_zoomButtonsVisible ? rightEnd.x + rightEnd.width : zoomDownButton.x + zoomDownButton.width
    height: rightEnd.y + rightEnd.height

    property var mapControl
    property bool zoomButtonsVisible: true
    property bool buttonsOnLeft:     true

    property var  m_scaleLengthsMeters: [5, 10, 25, 50, 100, 150, 250, 500, 1000, 2000, 5000, 10000, 20000, 50000, 100000, 200000, 500000, 1000000, 2000000]
    property var  m_scaleLengthsFeet:   [10, 25, 50, 100, 250, 500, 1000, 2000, 3000, 4000, 5280, 5280*2, 5280*5, 5280*10, 5280*25, 5280*50, 5280*100, 5280*250, 5280*500, 5280*1000]
    property bool m_zoomButtonsVisible: zoomButtonsVisible && !ScreenTools.isMobile
    property var  m_color:              mapControl.isSatelliteMap ? "white" : "black"

    function formatDistanceMeters(meters) {
        var dist = Math.round(meters)
        if (dist > 1000) {
            if (dist > 100000) {
                dist = Math.round(dist/1000)
            } else {
                dist = Math.round(dist/100)
                dist = dist/10
            }
            dist = dist + qsTr(" km")
        } else {
            dist = dist +  qsTr(" m")
        }
        return dist;
    }

    function formatDistanceFeet(Feet) {
        var dist = Math.round(feet)
        if (dist >= 5280) {
            dist = Math.round(dist / 5280)
            dist = dist
            if (dist == 1) {
                dist += qsTr(" mile")
            } else {
                dist += qsTr(" miles")
            }
        } else {
            dist = dist + qsTr(" ft")
        }
        return dist;
    }

    function calculateMetersRatio(scaleLineMeters, scaleLinePixelLength) {
        var scaleLineRatio = 0

        if (scaleLineMeters === 0) {
            //not visible
        } else {
            for (var i = 0; i < m_scaleLengthsMeters.length-1; i++) {
                if (scaleLineMeters < (m_scaleLengthsMeters[i] + m_scaleLengthsMeters[i+1])/2) {
                    scaleLineRatio  = m_scaleLengthsMeters[i] / scaleLineMeters
                    scaleLineMeters = m_scaleLengthsMeters[i]
                    break;
                }
            }
            if (scaleLineRatio === 0) {
                scaleLineRatio  = scaleLineMeters / m_scaleLengthsMeters[i]
                scaleLineMeters = m_scaleLengthsMeters[i]
            }
        }

        var text = formatDistanceMeters(scaleLineMeters)
        centerLine.width = (scaleLinePixelLength*scaleLineRatio) - (2*leftEnd.width)
        scaleText.text = text
    }

    function calculateFeetRatio(scaleLineMeters, scaleLinePixelLength) {
        var scaleLineRatio = 0
        var scaleLineFeet = scaleLineMeters * 3.2808399

        if (scaleLineFeet === 0) {
            //not visible
        } else {
            for (var i = 0; i < m_scaleLengthsFeet.length-1; i++) {
                if (scaleLineFeet < (m_scaleLengthsFeet[i] + m_scaleLengthsFeet[i+1])/2) {
                    scaleLineRatio  = m_scaleLengthsFeet[i] / scaleLineFeet
                    scaleLineFeet = m_scaleLengthsFeet[i]
                    break;
                }
            }
            if (scaleLineRatio === 0) {
                scaleLineRatio  = scaleLineMeters / m_scaleLengthsMeters[i]
                scaleLineMeters = m_scaleLengthsMeters[i]
            }
        }

        var text = formatDistanceMeters(scaleLineFeet)
        centerLine.width = (scaleLinePixelLength*scaleLineRatio) - (2*leftEnd.width)
        scaleText.text = text
    }

    function calculateScale() {
        if (mapControl) {
            var scaleLinePixelLength = 100
            var leftCoord            = mapControl.toCoordinate(Qt.point(0, scale.y), false)
            var rightCoord           = mapControl.toCoordinate(Qt.point(scaleLinePixelLength, scale.y), false)
            var scaleLineMeters      = Math.round(leftCoord.distanceTo(rightCoord))
            if (QUAV.settingsManager.unitsSettings.horizontalDistanceUnits.value === UnitsSettings.HorizontalDistanceUnitsFeet)  {
                calculateFeetRatio(scaleLineMeters, scaleLinePixelLength)
            } else {
                calculateMetersRatio(scaleLineMeters, scaleLinePixelLength)
            }
        }
    }

    Connections {
        target: mapControl
        function onWidthChanged()     {scaleTimer.restart()}
        function onHeightChanged()    {scaleTimer.restart()}
        function onZoomLevelChanged() {scaleTimer.restart()}
    }

    Timer {
        id:          scaleTimer
        interval:    100
        running:     false
        repeat:      false
        onTriggered: calculateScale()
    }

    QUAVMapLabel {
        id:                 scaleText
        map:                mapControl
        font.bold:          true
        anchors.left:       parent.left
        anchors.right:      rightEnd.right
        horizontalAlignment:Text.AlignRight
        text:               "0 m"
    }


    Rectangle {
        id:                 leftEnd
        anchors.top:        scaleText.bottom
        anchors.leftMargin: buttonsOnLeft && (m_zoomButtonsVisible) ? ScreenTools.defaultFontPixelWidth / 2 : 0
        anchors.left:       buttonsOnLeft ?  (m_zoomButtonsVisible ? zoomDownButton.right : parent.left) : parent.left
        width:              2
        height:             ScreenTools.defaultFontPixelHeight
        color:              m_color
    }

    Rectangle {
        id:                   centerLine
        anchors.bottomMargin: 2
        anchors.bottom:       leftEnd.bottom
        anchors.left:         leftEnd.right
        height:               2
        color:                m_color
    }

    Rectangle {
        id:          rightEnd
        anchors.top: leftEnd.top
        anchors.left: centerLine.right
        width:        2
        height:       ScreenTools.defaultFontPixelHeight
        color:        m_color
    }

    QUAVButton {
        id:                 zoomUpButton
        anchors.top:        scaleText.top
        anchors.bottom:     rightEnd.bottom
        anchors.leftMargin: 0
        anchors.left:       buttonsOnLeft ? parent.left : rightEnd.right
        text:               qsTr("+")
        width:              height
        opacity:            0.75
        visible:            m_zoomButtonsVisible
        onClicked:          mapControl.zoomLevel += 0.5
    }

    QUAVButton {
        id:                 zoomDownButton
        anchors.top:        scaleText.top
        anchors.bottom:     rightEnd.bottom
        anchors.leftMargin: ScreenTools.defaultFontPixelWidth / 2
        anchors.left:       zoomUpButton.right
        text:               qsTr("-")
        width:              height
        opacity:            0.75
        visible:            m_zoomButtonsVisible
        onClicked:          mapControl.zoomLevel -= 0.5
    }

    Component.onCompleted: { if (scale.visible) { calculateScale(); }}
} // Map
