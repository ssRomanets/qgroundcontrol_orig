import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QUAV
import QUAV.Controls
import QUAV.Palette
import QUAV.ScreenTools

Button {
    id:            control
    padding:       ScreenTools.defaultFontPixelWidth*0.75
    hoverEnabled:  !ScreenTools.isMobile
    autoExclusive: true
    icon.color:    textColor

    property color textColor: checked || pressed ? quavPal.buttonHighlightText : quavPal.buttonText

    QUAVPalette {
        id: quavPal
        colorGroupEnabled: control.enabled
    }

    background: Rectangle {
        color:   quavPal.buttonHighlight
        opacity: checked || pressed ? 1 : enabled && hovered ? 0.2 : 0
        radius:  ScreenTools.defaultFontPixelWidth/2
    }

    contentItem: RowLayout {
        spacing: ScreenTools.defaultFontPixelWidth

        QUAVColoredImage {
            source: control.icon.source
            color:  control.icon.color
            width:  ScreenTools.defaultFontPixelHeight
            height: ScreenTools.defaultFontPixelHeight
        }

        QUAVLabel {
            id:                  displayText
            Layout.fillWidth:    true
            text:                control.text
            color:               control.textColor
            horizontalAlignment: QUAVLabel.AlignLeft
        }
    }
}








