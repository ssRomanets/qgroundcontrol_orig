import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QUAV.Palette
import QUAV.ScreenTools

/// Standard push button control:
///     If there is both an icon and text the icon will be to the left of the text
///     If icon only, icon will be centered
Button {
    id:             control
    hoverEnabled:   !ScreenTools.isMobile
    topPadding:     m_verticalPadding
    bottomPadding:  m_verticalPadding
    leftPadding:    m_horizontalPadding
    rightPadding:   m_horizontalPadding
    focusPolicy:    Qt.ClickFocus
    font.family:    ScreenTools.normalFontFamily
    text:           ""

    property bool   primary:        false                               ///< primary button for a group of buttons
    property bool   showBorder:     quavPal.globalTheme === QUAVPalette.Light
    property real   backRadius:     ScreenTools.buttonBorderRadius
    property real   heightFactor:   0.5
    property string iconSource:     ""
    property real   fontWeight:     Font.Normal // default for qml Text
    property real   pointSize:      ScreenTools.defaultFontPointSize

    property alias wrapMode:            text.wrapMode
    property alias horizontalAlignment: text.horizontalAlignment
    property alias backgroundColor:     backRect.color
    property alias textColor:           text.color

    property bool  m_showHighlight:     enabled && (pressed | checked)

    property int m_horizontalPadding:    ScreenTools.defaultFontPixelWidth * 2
    property int m_verticalPadding:      Math.round(ScreenTools.defaultFontPixelHeight * heightFactor)

    QUAVPalette { id: quavPal; colorGroupEnabled: enabled }

    background: Rectangle {
        id:             backRect
        radius:         backRadius
        implicitWidth:  ScreenTools.implicitButtonWidth
        implicitHeight: ScreenTools.implicitButtonHeight
        border.width:   showBorder ? 1 : 0
        border.color:   quavPal.buttonBorder
        color:          primary ? quavPal.primaryButton : quavPal.button

        Rectangle {
            anchors.fill:   parent
            color:          quavPal.buttonHighlight
            opacity:        m_showHighlight ? 1 : control.enabled && control.hovered ? 0.2 : 0
            radius:         parent.radius
        }
    }

    contentItem: RowLayout {
            spacing: ScreenTools.defaultFontPixelWidth

            QUAVColoredImage {
                id:                     icon
                Layout.alignment:       Qt.AlignHCenter
                source:                 control.iconSource
                height:                 text.height
                width:                  height
                color:                  text.color
                fillMode:               Image.PreserveAspectFit
                sourceSize.height:      height
                visible:                control.iconSource !== ""
            }

            QUAVLabel {
                id:                     text
                Layout.alignment:       Qt.AlignHCenter
                text:                   control.text
                font.pointSize:         control.pointSize
                font.family:            control.font.family
                font.weight:            fontWeight
                color:                  m_showHighlight ? quavPal.buttonHighlightText : (primary ? quavPal.primaryButtonText : quavPal.buttonText)
                visible:                control.text !== ""
            }
    }
}