import QtQuick

import QUAV
import QUAV.Controls

SelectableControl {
    z: QUAV.zOrderWidgets
    selectionUIRightAnchor: true
    selectedControl:        QUAV.settingsManager.flyViewSettings.instrumentQmlFile2
}
