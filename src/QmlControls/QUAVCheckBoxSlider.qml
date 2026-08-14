import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QUAV.Palette
import QUAV.ScreenTools

AbstractButton {
    id:        control
    checkable: true
    padding:   0

    property bool m_showBorder: quavPal.globalTheme === QUAVPalette.Light

    QUAVPalette {id: quavPal; colorGroupEnabled: control.enabled}

    contentItem: Item {
        implicitWidth: (label.visible ? label.contentWidth + ScreenTools.defaultFontPixelWidth : 0) + indicator.width
        implicitHeight: label.contentHeight

        QUAVLabel {
            id:             label
            anchors.left:   parent.left
            text:           visible ? control.text : "X"
            visible:        control.text
        }

        Rectangle {
            id:                     indicator
            anchors.right:          parent.right
            anchors.verticalCenter: parent.verticalCenter
            height:                 ScreenTools.defaultFontPixelHeight
            width:                  height*2
            radius:                 height/2
            color:                  control.checkable ? quavPal.primaryButton : quavPal.button
            border.width:           m_showBorder ? 1 : 0
            border.color:           quavPal.buttonBorder

            Rectangle {
                anchors.verticalCenter: parent.verticalCenter
                x: checked ? indicator.width - width - 1 : 1
                height: parent.height - 2
                width: height
                radius: height/2
                color: quavPal.buttonText
            }
        }
    }
}
