import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QUAV
import QUAV.Palette
import QUAV.Controls
import QUAV.ScreenTools
import QUAV.AppSettings

Rectangle {
    id:     settingsView
    color:  quavPal.window
    z:      QUAV.zOrderTopMost

    readonly property real m_defaultTextHeight: ScreenTools.defaultFontPixelHeight
    readonly property real m_defaultTextWidth:  ScreenTools.defaultFontPixelWidth
    readonly property real m_horizontalMargin:  m_defaultTextWidth/2
    readonly property real m_verticalMargin:    m_defaultTextHeight/2
    readonly property real m_buttonHeight:      ScreenTools.isTinyScreen ? ScreenTools.defaultFontPixelHeight * 3 :
                                                                           ScreenTools.defaultFontPixelHeight * 2

    property bool m_first: true
    property bool m_commingFromRIDSettings: false

    QUAVPalette { id: quavPal }

    Component.onCompleted: {
        rightPanel.source =  "qrc:/qml/QUAV/AppSettings/VideoSettings.qml"
    }

    SettingsPagesModel {id: settingsPagesModel}

    QUAVFlickable {
        id:                 buttonList
        width:              buttonColumn.width
        anchors.topMargin:  m_verticalMargin
        anchors.top:        parent.top
        anchors.bottom:     parent.bottom
        anchors.leftMargin: m_horizontalMargin
        anchors.left:       parent.left
        contentHeight:      buttonColumn.height + m_verticalMargin
        flickDeceleration:  Flickable.VerticalFlick
        clip:               true

        ColumnLayout {
            id:         buttonColumn
            spacing:    ScreenTools.defaultFontPixelHeight/4

            Repeater {
                id: buttonRepeater
                model: settingsPagesModel

                SettingsButton {
                    Layout.fillWidth: true
                    text:             name
                    icon.source:      iconUrl
                    visible:          pageVisible()

                    onClicked: {
                        if (mainWindow.allowViewSwitch()) {
                            if (rightPanel.source !== url) {
                                rightPanel.source = url
                            }
                            checked = true
                        }
                    }

                    Component.onCompleted: {
                        if (globals.commingFromRIDIndicator) {
                            m_commingFromRIDSettings = true
                        }
                        if (m_first)
                        {
                            m_first = false
                            checked = true
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        id: divider
        anchors.topMargin:    m_verticalMargin
        anchors.bottomMargin: m_verticalMargin
        anchors.leftMargin:   m_horizontalMargin
        anchors.left:         buttonList.right
        anchors.top:          parent.top
        anchors.bottom:       parent.bottom
        width:                1
        color:                quavPal.windowShade
    }

    //-- Panel Contents
    Loader {
        id: rightPanel
        anchors.leftMargin:   m_horizontalMargin
        anchors.rightMargin:  m_horizontalMargin
        anchors.topMargin:    m_verticalMargin
        anchors.bottomMargin: m_verticalMargin
        anchors.left:         divider.right
        anchors.right:        parent.right
        anchors.top:          parent.top
        anchors.bottom:       parent.bottom
    }
}




