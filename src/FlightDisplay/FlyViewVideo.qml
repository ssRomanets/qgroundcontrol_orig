import QtQuick

import QUAV
import QUAV.Controls
import QUAV.ScreenTools

Item {
    id:         m_root

    property Item pipView
    property Item pipState: videoPipState

    PipState {
        id:      videoPipState
        pipView: m_root.pipView
    }

    Loader {
        id: videoCameraLoader
        anchors.fill: parent
        visible: QUAV.videoManager.isUvc && QUAV.videoManager.uvcEnabled && globals.cameraStreamAct
        source: "qrc:/qml/QUAV/FlightDisplay/FlightDisplayViewUVC.qml"
    }

    Loader {
        id: videoFileLoader
        anchors.fill: parent
        visible:  !globals.cameraStreamAct && globals.videoFileName !== ""
        source: "qrc:/qml/QUAV/FlightDisplay/FlightDisplayViewUVF.qml"
    }
}