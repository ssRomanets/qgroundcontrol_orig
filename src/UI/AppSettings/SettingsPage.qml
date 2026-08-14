import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

import QUAV
import QUAV.FactSystem
import QUAV.FactControls
import QUAV.Controls
import QUAV.ScreenTools
import QUAV.Palette

Item {
    id: root

    default property alias contentItem: mainLayout.data

    QUAVFlickable {
        anchors.fill:   parent
        contentWidth:   mainLayout.width
        contentHeight:  mainLayout.height

        ColumnLayout {
            id:         mainLayout
            x:          Math.max(0, root.width / 2 - width / 2)
            width:      Math.max(implicitWidth, ScreenTools.defaultFontPixelWidth * 50)
            spacing:    ScreenTools.defaultFontPixelHeight
        }
    }
}

