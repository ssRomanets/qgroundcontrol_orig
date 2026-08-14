import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

import QUAV
import QUAV.Controls
import QUAV.Palette
import QUAV.ScreenTools

Rectangle {
    id:     m_root
    width:  parent.width
    height: ScreenTools.toolbarHeight
    color:  quavPal.toolbarBackground

    QUAVPalette { id: quavPal }

    /// Bottom single pixel divider
    Rectangle {
        anchors.left:   parent.left
        anchors.right:  parent.right
        anchors.bottom: parent.bottom
        height:         1
        color:          "black"
        visible:        quavPal.globalTheme === QUAVPalette.Light
    }

    RowLayout {
        id:                     viewButtonRow
        anchors.bottomMargin:   1
        anchors.top:            parent.top
        anchors.bottom:         parent.bottom
        spacing:                ScreenTools.defaultFontPixelWidth / 2

        QUAVLabel {
            font.pointSize: ScreenTools.largeFontPointSize
            text:           "<"
        }

        QUAVLabel {
            text:           qsTr("Exit Plan")
            font.pointSize: ScreenTools.largeFontPointSize
        }
    }

    QUAVMouseArea {
        anchors.fill:   viewButtonRow
        onClicked:      mainWindow.showFlyView()
    }
}