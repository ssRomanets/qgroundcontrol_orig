import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

import QUAV
import QUAV.Palette
import QUAV.Controls
import QUAV.ScreenTools

AnalyzePage {
    id: vibrationPage
    pageComponent:      pageComponent
    pageDescription:    qsTr("Analyze vibration associated with your vehicle.")
    allowPopout: true

    QUAVPalette { id:quavPal; colorGroupEnabled: true }

    Component {
        id: pageComponent

        Item {
            width:  childrenRect.width
            height: childrenRect.height
        }
    }
}
