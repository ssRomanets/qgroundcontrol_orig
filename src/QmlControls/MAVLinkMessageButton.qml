import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QUAV.Palette
import QUAV.ScreenTools

Button {
    id:                 control
    autoExclusive:      true
    leftPadding:        ScreenTools.defaultFontPixelWidth
    rightPadding:       leftPadding

    property real m_compIDWidth: ScreenTools.defaultFontPixelWidth * 3
    property real m_hzWidth:     ScreenTools.defaultFontPixelWidth * 6
    property real m_nameWidth:   nameLabel.contentWidth


    QUAVPalette { id: quavPal }

    background: Rectangle {
        anchors.fill:   parent
        color:          checked ? quavPal.buttonHighlight : quavPal.button
    }

    property double messageHz:  0
    property int    compID:     0

    contentItem: RowLayout {
        id:         rowLayout
        spacing:    ScreenTools.defaultFontPixelWidth

        QUAVLabel {
            text:                   control.compID
            color:                  checked ? quavPal.buttonHighlightText : quavPal.buttonText
            verticalAlignment:      Text.AlignVCenter
            Layout.minimumHeight:   ScreenTools.isMobile ? (ScreenTools.defaultFontPixelHeight * 2) : (ScreenTools.defaultFontPixelHeight * 1.5)
            Layout.minimumWidth:    m_compIDWidth
        }

        QUAVLabel {
            id:                     nameLabel
            text:                   control.text
            color:                  checked ? quavPal.buttonHighlightText : quavPal.buttonText
            Layout.fillWidth:       true
            Layout.alignment:       Qt.AlignVCenter
        }

        QUAVLabel {
            color:                  checked ? quavPal.buttonHighlightText : quavPal.buttonText
            text:                   messageHz.toFixed(1) + 'Hz'
            horizontalAlignment:    Text.AlignRight
            Layout.minimumWidth:    m_hzWidth
            Layout.alignment:       Qt.AlignVCenter
        }
    }

    Component.onCompleted: {
        maxButtonWidth = Math.max(maxButtonWidth, m_compIDWidth + m_hzWidth + m_nameWidth + (rowLayout.spacing * 2) + (control.leftPadding * 2))
    }
}

