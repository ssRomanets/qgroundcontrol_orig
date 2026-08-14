import QtQuick

import QUAV
import QUAV.Controls
import QUAV.ScreenTools
import QUAV.FactSystem
import QUAV.FlightMap
import QUAV.Palette

Rectangle {
    width: ScreenTools.defaultFontPixelHeight*15
    height: m_outerRadius*4
    radius: m_outerRadius
    color: QUAV.globalPalette.window

    property real m_outerMargin: (width*0.05)/2
    property real m_outerRadius: width/2
    property real m_innerRadius: m_outerRadius - m_outerMargin

    DeadMouseArea {
        anchors.fill: parent
    }

    QUAVAttitudeWidget {
        id:                         attitude
        anchors.horizontalCenter:   parent.horizontalCenter
        anchors.topMargin:          m_outerMargin
        anchors.top:                parent.top
        size:                       m_innerRadius*2
        vehicle:                    globals.activeVehicle
    }

    QUAVCompassWidget {
        id: compass
        anchors.horizontalCenter:   parent.horizontalCenter
        anchors.topMargin:          m_outerMargin*2
        anchors.top:                attitude.bottom
        size:                       m_innerRadius*2
        vehicle:                    globals.activeVehicle
    }
}












