import QtQuick
import QtQuick.Layouts

import QUAV
import QUAV.ScreenTools
import QUAV.Palette

ColumnLayout {
    id:                 control
    spacing:            m_margins / 2
    implicitWidth:      m_contentLayout.implicitWidth + (m_margins * 2)
    implicitHeight:     m_contentLayout.implicitHeight + (m_margins * 2)

    default property alias contentItem: m_contentLayout.data

    property alias contentSpacing: m_contentLayout.spacing

    property string defaultBorderColor  : QUAV.globalPalette.groupBorder
    property string outerBorderColor    : defaultBorderColor

    property string heading
    property string headingDescription
    property bool   showDividers:       true
    property bool   showBorder:         true

    property real m_margins: ScreenTools.defaultFontPixelHeight / 2

    ColumnLayout {
        Layout.leftMargin: m_margins
        Layout.fillWidth:  true
        spacing:           0
        visible:           heading !== ""

        QUAVLabel {
            text:           heading
            font.pointSize: ScreenTools.defaultFontPointSize+1
            font.bold:      true
        }

        QUAVLabel {
            Layout.fillWidth: true
            text: headingDescription
            wrapMode: Text.WordWrap
            font.pointSize: ScreenTools.smallFontPointSize
            visible: headingDescription !== ""
        }
    }

    Rectangle {
        id:                 outerRect
        Layout.fillWidth:   true
        implicitWidth:      m_contentLayout.implicitWidth + (showBorder ? m_margins * 2 : 0)
        implicitHeight:     m_contentLayout.implicitHeight + (showBorder ? m_margins * 2: 0)
        color:              "transparent"
        border.color:       outerBorderColor
        border.width:       showBorder ? 1 : 0
        radius:             ScreenTools.defaultFontPixelHeight / 2

        Repeater {
            model: showDividers? m_contentLayout.children.length : 0

            Rectangle {
                x:                  showBorder ? m_margins : 0
                y:                  m_contentItem.y + m_contentItem.height + m_margins + (showBorder ? m_margins : 0)
                width:              parent.width - (showBorder ? m_margins * 2 : 0)
                height:             1
                color:              QUAV.globalPalette.groupBorder
                visible:            m_contentItem.visible && m_contentItem.width !== 0 && m_contentItem.height !== 0 &&
                                    index < m_contentLayout.children.length - 1

                property var m_contentItem: m_contentLayout.children[index]
            }
        }

        ColumnLayout {
            id:                 m_contentLayout
            x:                  showBorder ? m_margins : 0
            y:                  showBorder ? m_margins : 0
            width:              parent.width - (showBorder ? m_margins * 2 : 0)
            spacing:            m_margins * 2
        }
    }
}


















