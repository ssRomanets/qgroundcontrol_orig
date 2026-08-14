import QtQuick
import QtQuick.Layouts

import QUAV
import QUAV.ScreenTools

RowLayout {
    id:         control
    spacing:    ScreenTools.defaultFontPixelWidth

    property bool       waitForParameters:  false

    property Component  contentComponent

    property bool   parametersReady:    QUAV.multiVehicleManager.parameterReadyVehicleAvailable

    property bool m_loadPages: !waitForParameters || parametersReady

    QUAVLabel {
        text:       qsTr("Waiting for parameters...")
        visible:    waitForParameters && !parametersReady
    }

    Loader {
        id:                 contentItemLoader
        Layout.alignment:   Qt.AlignTop
        sourceComponent:    m_loadPages ? contentComponent : undefined
    }
}