import QtQuick
import QtQuick.Window
import QtQuick.Controls

import QUAV
import QUAV.Palette
import QUAV.Controls
import QUAV.ScreenTools

Rectangle {
    id:     m_root
    color:  quavPal.window
    z:      QUAV.zOrderTopMost

    signal popout()

    readonly property real m_defaultTextHeight: ScreenTools.defaultFontPixelHeight
    readonly property real m_defaultTextWidth:  ScreenTools.defaultFontPixelWidth
    readonly property real m_horizontalMargin:  m_defaultTextWidth/2
    readonly property real m_verticalMargin:    m_defaultTextHeight/2
    readonly property real m_buttonWidth:       m_defaultTextWidth*18

    QUAVPalette { id: quavPal }

    // This need to block click event leakage to underlying map.
    DeadMouseArea {
        anchors.fill: parent
    }

    QUAVFlickable {
        id:                 buttonScroll
        width:              buttonColumn.width
        anchors.topMargin:  m_defaultTextHeight/2
        anchors.top:        parent.top
        anchors.bottom:     parent.bottom
        anchors.leftMargin: m_horizontalMargin
        anchors.left:       parent.left
        contentHeight:      buttonColumn.height
        flickDeceleration:  Flickable.VerticalFlick
        clip:               true

        Column {
            id: buttonColumn
            width: m_maxButtonWidth
            spacing: m_defaultTextHeight/2

            property real m_maxButtonWidth: 0

            Component.onCompleted: reflowWidths()

            // I don't know why this does not work
            Connections {
                target: QUAV.settingsManager.appSettings.appFontPointSize
                function onValueChanged() {buttonColumn.reflowWidths()}
            }

            function reflowWidths() {
                buttonColumn.m_maxButtonWidth = 0
                for (var i = 0; i < children.length; i++) {
                    buttonColumn.m_maxButtonWidth = Math.max(buttonColumn.m_maxButtonWidth, children[i].width)
                }
                for (var j = 0; j < children.length; j++) {
                    children[j].width = buttonColumn.m_maxButtonWidth
                }
            }

            Repeater {
                id: buttonRepeater
                model : QUAV.corePlugin ? QUAV.corePlugin.analyzePages : []

                Component.onCompleted: itemAt(0).checked = true

                SubMenuButton {
                    id: subMenu
                    imageResource: modelData.icon
                    autoExclusive: true
                    text: modelData.title

                    onClicked: {
                        panelLoader.source = modelData.url
                        panelLoader.title  = modelData.title
                        checked = true
                    }
                }
            }
        }
    }

    Rectangle {
        id:                     divider
        anchors.topMargin:      m_verticalMargin
        anchors.bottomMargin:   m_verticalMargin
        anchors.leftMargin:     m_horizontalMargin
        anchors.left:           buttonScroll.right
        anchors.top:            parent.top
        anchors.bottom:         parent.bottom
        width:                  1
        color:                  quavPal.windowShade
    }

    Loader {
        id:                   panelLoader
        anchors.topMargin:    m_verticalMargin
        anchors.bottomMargin: m_verticalMargin
        anchors.leftMargin:   m_horizontalMargin
        anchors.rightMargin:  m_horizontalMargin
        anchors.left:         divider.right
        anchors.right:        parent.right
        anchors.top:          parent.top
        anchors.bottom:       parent.bottom
        source:               "LogDownloadPage.qml"

        property string title

        Connections {
            target: panelLoader.item 
            function onPopout() {mainWindow.createrWindowedAnalyzePage(panelLoader.title, panelLoader.source)}
        }
    }
}