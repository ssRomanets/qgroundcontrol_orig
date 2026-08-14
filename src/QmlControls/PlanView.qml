import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtLocation
import QtPositioning
import QtQuick.Layouts
import QtQuick.Window

import QUAV
import QUAV.FlightMap
import QUAV.ScreenTools
import QUAV.Controls

Item {
    id: m_root

    PlanViewToolBar {
        id:                     planToolBar
    }

    Item {
        id:             panel
        anchors.left:   parent.left
        anchors.right:  parent.right
        anchors.top:    planToolBar.bottom
        anchors.bottom: parent.bottom

        FlightMap {
            id:                         editorMap
            anchors.fill:               parent
            mapName:                    "MissionEditor"
            planView:                   true

            zoomLevel:                  QUAV.flightMapZoom
            center:                     QUAV.flightMapPosition
        }
    }
}
