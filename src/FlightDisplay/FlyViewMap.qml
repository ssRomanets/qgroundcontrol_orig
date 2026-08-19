import QtQuick
import QtQuick.Controls
import QtLocation
import QtPositioning
import QtQuick.Dialogs
import QtQuick.Layouts

import QUAV
import QUAV.Controls
import QUAV.FlightMap

FlightMap {
    id:                         m_root
    zoomLevel:                  QUAV.flightMapZoom
    center:                     QUAV.flightMapPosition

    property Item pipView
    property Item pipState:     m_pipState

    property bool m_saveZoomLevelSetting:      true

    onZoomLevelChanged: { if (m_saveZoomLevelSetting) { QUAV.flightMapZoom = m_root.zoomLevel } }
    onCenterChanged:    {  QUAV.flightMapPosition = m_root.center }

    PipState {
        id: m_pipState
        pipView: m_root.pipView
    }
}