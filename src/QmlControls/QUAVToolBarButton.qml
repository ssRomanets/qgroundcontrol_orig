import QtQuick
import QtQuick.Controls

import QUAV
import QUAV.Controls
import QUAV.Palette
import QUAV.ScreenTools

// Important Note: Toolbar buttons must manage their checked state manually in order to support
// view switch prevention. This means they can't be checkable or autoExclusive.

Button {
    id:                 button
    height:             ScreenTools.defaultFontPixelHeight * 3
    leftPadding:        m_horizontalMargin
    rightPadding:       m_horizontalMargin
    checkable:          false

    property bool logo: false

    property real m_horizontalMargin: ScreenTools.defaultFontPixelWidth

    onCheckedChanged: checkable = false

    QUAVPalette { id: quavPal }

    background: Rectangle {
        anchors.fill:   parent
        color:          button.checked ? quavPal.buttonHighlight : Qt.rgba(0,0,0,0)
        border.color:   "red"
        border.width:   QUAV.corePlugin.showTouchAreas ? 3 : 0
    }

    contentItem: Row {
        spacing:                ScreenTools.defaultFontPixelWidth
        anchors.verticalCenter: button.verticalCenter
        QUAVColoredImage {
            id:                     m_icon
            height:                 ScreenTools.defaultFontPixelHeight * 2
            width:                  height
            sourceSize.height:      parent.height
            fillMode:               Image.PreserveAspectFit
            color:                  logo ? "transparent" : (button.checked ? quavPal.buttonHighlightText : quavPal.buttonText)
            source:                 button.icon.source
            anchors.verticalCenter: parent.verticalCenter
        }
        Label {
            id:                     m_label
            visible:                text !== ""
            text:                   button.text
            color:                  button.checked ? quavPal.buttonHighlightText : quavPal.buttonText
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
