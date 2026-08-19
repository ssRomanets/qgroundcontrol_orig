import QtQuick
import QtQuick.Controls

import QUAV.Palette
import QUAV.Controls
import QUAV.ScreenTools

Switch {
    id: control

    QUAVPalette {
        id: quavPal
        colorGroupEnabled: true
    }

    contentItem: QUAVLabel {
        text:  control.text
        verticalAlignment: Text.AlignVCenter
        rightPadding: control.indicator.width+control.spacing
    }

    indicator: Rectangle {
        implicitWidth: knob.width*2
        implicitHeight: knob.height
        x: control.width - width - control.rightPadding
        y: parent.height/2 - height/2
        radius: knob.radius
        color: control.checked ? quavPal.primaryButton : quavPal.button

        Rectangle {
            id: knob
            x: control.checked ? parent.width - width : 0
            width: ScreenTools.defaultFontPixelHeight
            height: ScreenTools.defaultFontPixelHeight
            radius: height/2
            color: quavPal.buttonText
        }
    }
}




