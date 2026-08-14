import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QUAV.ScreenTools
import QUAV.Palette
import QUAV.FactSystem
import QUAV.FactControls

Control {
    id: control
    topInset:       0
    bottomInset:    0
    leftInset:      0
    rightInset:     0
    topPadding:     m_showSelectionUI ? selectionUILayout.height : 0
    bottomPadding:  0
    leftPadding:    0
    rightPadding:   0

    property Fact selectedControl
    property bool selectionUIRightAnchor: false

    property bool m_showSelectionUI: false

    property var m_compassComboBoxId: compassComboBoxId

    background: Item {
        RowLayout {
            id: selectionUILayout
            anchors.right: control.selectionUIRightAnchor ? parent.right : undefined
            spacing: ScreenTools.defaultFontPixelWidth
            visible:  m_showSelectionUI

            QUAVButton {
                onClicked: m_showSelectionUI = false
                iconSource: "qrc:/InstrumentValueIcons/lock-open.svg"
            }

            FactComboBox {
                id: compassComboBoxId
                fact: selectedControl
                indexModel: false
                sizeToContents: true
            }
        }
    }

    contentItem: Item {
         implicitWidth:  loader.item.width
         implicitHeight: loader.item.height

         Loader {
             id: loader
             source:  selectedControl ? selectedControl.enumValues[compassComboBoxId.currentIndex] : ""
         }

         QUAVMouseArea {
             anchors.fill: parent
             acceptedButtons: Qt.LeftButton | Qt.RightButton

            onClicked: (mouse) => {
                if (!ScreenTools.isMobile && mouse.button === Qt.RightButton) {
                     m_showSelectionUI = true
                }
            }

            onPressAndHold: {m_showSelectionUI = true}
        }
    }
}













