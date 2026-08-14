import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QUAV
import QUAV.Controls
import QUAV.ScreenTools

AnalyzePage {
    id: logDownloadPage
    pageComponent: pageComponent
    pageDescription: qsTr("Provides a connection to the vehicle's system shell.")
    allowPopout:     true

    Component {
        id: pageComponent

        ColumnLayout {
            height: availableHeight
            width:  availableWidth
        }
    }
}
