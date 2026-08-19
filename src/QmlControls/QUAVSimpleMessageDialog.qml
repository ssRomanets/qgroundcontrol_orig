import QtQuick
import QtQuick.Layouts

import QUAV.Controls
import QUAV.ScreenTools

QUAVPopupDialog {
    property alias text:  label.text
    property var acceptFunction: null
    property var closeFunction:  null

    onAccepted: { if (acceptFunction) { acceptFunction() } }

    onClosed:   { if (closeFunction) { closeFunction() } }

    ColumnLayout {
        QUAVLabel {
            id: label
            Layout.preferredWidth: Math.max(mainWindow.width / (ScreenTools.isMobile ? 2 : 3), headerMinWidth)
            wrapMode: Text.WordWrap
        }
    }
}
