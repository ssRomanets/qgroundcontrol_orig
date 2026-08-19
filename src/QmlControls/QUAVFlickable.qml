import QtQuick
import QtQuick.Controls

import QUAV.Palette
import QUAV.ScreenTools

/// QUAV version of Flickable control that shows horizontal/vertial scroll indicators
Flickable {
    id:             root
    boundsBehavior: Flickable.StopAtBounds
    clip:           true
    maximumFlickVelocity: (ScreenTools.realPixelDensity*25.4)*8

    QUAVPalette { id: quavPal }

    property color indicatorColor: quavPal.text

    Component.onCompleted: {        
        var indicatorComponent = Qt.createComponent("QUAVFlickableScrollIndicator.qml")
        indicatorComponent.createObject(root, { orientation: QUAVFlickableScrollIndicator.Horizontal })
        indicatorComponent = Qt.createComponent("QUAVFlickableScrollIndicator.qml")
        indicatorComponent.createObject(root, { orientation: QUAVFlickableScrollIndicator.Vertical })
    }
}
