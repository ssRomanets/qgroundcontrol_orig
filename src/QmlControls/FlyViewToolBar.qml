import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

import QUAV
import QUAV.Controls
import QUAV.Palette
import QUAV.ScreenTools

Rectangle {
    id:     m_root
    width:  parent.width
    height: ScreenTools.toolbarHeight
    color:  quavPal.toolbarBackground

    property var    m_activeVehicle: QUAV.multiVehicleManager.activeVehicle
    property color  m_mainStatusBGColor: quavPal.brandingPurple

    QUAVPalette { id: quavPal }

    /// Bottom single pixel divider
    Rectangle {
        anchors.left:   parent.left
        anchors.right:  parent.right
        anchors.bottom: parent.bottom
        height:         1
        color:          "black"
        visible:        quavPal.globalTheme === QUAVPalette.Light
    }

    Rectangle {
        anchors.fill: viewButtonRow

        gradient: Gradient {
            orientation: Gradient.Horizontal
            GradientStop { position: 0;                                     color: m_mainStatusBGColor }
            GradientStop { position: currentButton.x + currentButton.width; color: m_mainStatusBGColor }
            GradientStop { position: 1;                                     color: m_root.color }
        }
    }

    RowLayout {
        id:                     viewButtonRow
        anchors.bottomMargin:   1
        anchors.top:            parent.top
        anchors.bottom:         parent.bottom
        spacing:                ScreenTools.defaultFontPixelWidth / 2

        QUAVToolBarButton {
            id:                     currentButton
            Layout.preferredHeight: viewButtonRow.height
            icon.source:            "/res/QGCLogoFull.svg"
            logo:                   true
            onClicked:              mainWindow.showToolSelectDialog()
        }

        MainStatusIndicator {
            id: mainStatusIndicator
            Layout.preferredHeight: viewButtonRow.height
        }
    }
    
    //-- Branding Logo
    Image {
        anchors.right:      parent.right
        anchors.top:        parent.top
        anchors.bottom:     parent.bottom
        anchors.margins:    ScreenTools.defaultFontPixelHeight*0.66
        visible:            m_activeVehicle
        fillMode:           Image.PreserveAspectFit
        source:             m_outdoorPalette ? m_brandImageOutdoor : m_brandImageIndoor
        mipmap:             true

        property bool   m_outdoorPalette: quavPal.globalTheme === QUAVPalette.Light
        property bool   m_corePluginBranding: QUAV.corePlugin.brandImageIndoor.length != 0

        property string m_userBrandImageIndoor: QUAV.settingsManager.brandImageSettings.userBrandImageIndoor.value
        property string m_userBrandImageOutdoor: QUAV.settingsManager.brandImageSettings.userBrandImageOutdoor.value
        property bool   m_userBrandIndoor: QUAV.settingsManager.brandImageSettings.visible && m_userBrandImageIndoor.length != 0
        property bool   m_userBrandOutdoor: QUAV.settingsManager.brandImageSettings.visible && m_userBrandImageOutdoor.length != 0

        property string m_brandImageIndoor: brandImageIndoor()
        property string m_brandImageOutdoor: brandImageOutdoor()

        function brandImageIndoor() {
            if (m_brandImageIndoor) {
                return m_userBrandImageIndoor;
            } else {
                if (m_userBrandImageOutdoor) {
                    return m_userBrandImageOutdoor;
                } else {
                    if (m_corePluginBranding) {
                        return QUAV.corePlugin.brandImageIndoor
                    } else {
                        return m_activeVehicle ? m_activeVehicle.brandImageIndoor : ""
                    }
                }
            }
        }

        function brandImageOutdoor() {
            if (m_userBrandOutdoor) {
                return m_userBrandImageOutdoor
            } else {
                if (m_brandImageIndoor) {
                    return m_userBrandImageIndoor
                } else {
                    if (m_corePluginBranding) {
                        return QUAV.corePlugin.brandImageOutdoor
                    } else {
                        return m_activeVehicle ? m_activeVehicle.brandImageOutdoor : ""
                    }
                }
            }
        }
    }
}

