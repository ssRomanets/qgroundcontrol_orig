import QtQuick
import QtQuick.Layouts

import QUAV
import QUAV.Controls
import QUAV.ScreenTools
import QUAV.Palette

RowLayout {
    id:         control
    spacing:    ScreenTools.defaultFontPixelWidth

    property var  m_activeVehicle: QUAV.multiVehicleManager.activeVehicle
    property bool m_healthAndArmingChecksSupported: m_activeVehicle ? m_activeVehicle.healthAndArmingChecksSupported.supported : false

    QUAVLabel {
        id:                 mainStatusLabel
        Layout.fillHeight:  true
        Layout.preferredWidth: contentWidth + vehicleMessagesIcon.width + control.spacing
        verticalAlignment:  Text.AlignVCenter
        text:               mainStatusText()
        font.pointSize:     ScreenTools.largeFontPointSize

        property string m_notReadyToFlyText: qsTr("Not Ready")
        property string m_disconnectedText:  qsTr("Disconnected - Click to manually connect")

        function mainStatusText() {
            if (m_activeVehicle) {

                if (m_activeVehicle.armed) {

                } else {
                    if (m_healthAndArmingChecksSupported) {
                        m_mainStatusBGColor = "red"
                        return mainStatusLabel.m_notReadyToFlyText
                    } else {
                        m_mainStatusBGColor = "yellow"
                        return mainStatusLabel.m_notReadyToFlyText
                    }
                }

            } else {
                m_mainStatusBGColor = quavPal.brandingPurple
                return mainStatusLabel.m_disconnectedText
            }
        }

        QUAVColoredImage {
            id:                     vehicleMessagesIcon
            anchors.verticalCenter: parent.verticalCenter
            anchors.right:          parent.right
            width:                  ScreenTools.defaultFontPixelWidth * 2
            height:                 width
            source:                 "/res/VehicleMessages.png"
            color:                  getIconColor()
            sourceSize.width:       width
            fillMode:               Image.PreserveAspectFit
            visible:                m_activeVehicle

            function getIconColor() {
                let iconColor = quavPal.text
                return iconColor
            }
        }
    }
}
