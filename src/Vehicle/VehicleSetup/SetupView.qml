import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QUAV
import QUAV.Palette
import QUAV.Controls
import QUAV.ScreenTools

Rectangle {
    id:     setupView
    color:  quavPal.window
    z:      QUAV.zOrderTopMost

    // This need to block click event leakage to underlying map.
    DeadMouseArea {
        anchors.fill: parent
    }

    QUAVPalette { id: quavPal; colorGroupEnabled: true }
}