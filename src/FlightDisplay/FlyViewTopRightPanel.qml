import QtQuick
import QtQuick.Layouts

import QUAV
import QUAV.Controls
import QUAV.FlightDisplay
import QUAV.FlightMap
import QUAV.Palette
import QUAV.ScreenTools

Rectangle {
    id: topRightPanel
    visible: !QUAV.videoManager.fullScreen && m_multipleVehicles && m_settingEnableMVPanel
    clip: true

    property bool m_settingEnableMVPanel:   QUAV.settingsManager.appSettings.enableMultiVehiclePanel.value
    property bool m_multipleVehicles:       QUAV.multiVehicleManager.vehicles.count>1
}
