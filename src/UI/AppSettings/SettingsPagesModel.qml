import QtQml.Models

import QUAV
import QUAV.ScreenTools

ListModel {
    ListElement {
        name: qsTr("Video")
        url: "qrc:/qml/QUAV/AppSettings/VideoSettings.qml"
        iconUrl: "qrc:/InstrumentValueIcons/camera.svg"
        pageVisible: function() { return QUAV.settingsManager.videoSettings.visible }
    }
    ListElement {
        name: qsTr("Maps")
        url: "qrc:/qml/QUAV/AppSettings/MapSettings.qml"
        iconUrl: "qrc:/InstrumentValueIcons/globe.svg"
        pageVisible: function() { return true }
    }
}
