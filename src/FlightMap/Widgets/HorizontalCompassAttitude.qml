import QtQuick

import QUAV
import QUAV.Controls
import QUAV.ScreenTools
import QUAV.Palette

Rectangle {
    id: control
    width: 1.5*Math.min(m_defaultWidth, m_maxWidth)
    height: m_outerRadius*2
    radius: m_outerRadius
    color: quavPal.window

    property real m_defaultWidth:    mainWindow.width*0.2
    property real m_maxWidth:        ScreenTools.defaultFontPixelHeight*15
    property real m_innerRadius:     (width - (m_topBottomMargin*3))/4
    property real m_outerRadius:     m_innerRadius + m_topBottomMargin
    property real m_spacing:         ScreenTools.defaultFontPixelHeight*0.33
    property real m_topBottomMargin: (width*0.05)/2

    DeadMouseArea { anchors.fill: parent}

    QUAVPalette {id: quavPal}

    QUAVAttitudeWidget {
        id:                     attitude
        anchors.leftMargin:     control.m_topBottomMargin
        anchors.left:           parent.left
        size:                   control.m_innerRadius*2
        vehicle:                globals.activeVehicle
        anchors.verticalCenter: parent.verticalCenter
    }

    QUAVCompassWidget {
        id:                     compass
        anchors.leftMargin:     control.m_spacing
        anchors.left:           attitude.right
        size:                   control.m_innerRadius*2
        vehicle:                globals.activeVehicle
        anchors.verticalCenter: parent.verticalCenter
    }
}














