import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

import QUAV
import QUAV.Controls
import QUAV.ScreenTools

AnalyzePage {
    pageComponent: pageComponent
    pageDescription: qsTr("Used to tag a set of images from a survey mission with gps coordinates. You must provide the binary log from the flight as well as the directory which contains the images to tag.")

    readonly property real m_margin: ScreenTools.defaultFontPixelWidth*2

    Component {
        id: pageComponent

        GridLayout {
            columns:       2
            columnSpacing: m_margin
            rowSpacing:    ScreenTools.defaultFontPixelWidth*2
            width:         availableWidth
        }
    }
}