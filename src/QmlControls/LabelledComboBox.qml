import QtQuick
import QtQuick.Layouts

import QUAV.Controls
import QUAV.ScreenTools

RowLayout {
    property alias label:                   label.text
    property alias model:                   m_comboBox.model
    property alias currentIndex:            m_comboBox.currentIndex
    property alias currentText:             m_comboBox.currentText
    property var   comboBox:                m_comboBox
    property real  comboBoxPreferredWidth:  -1

    spacing: ScreenTools.defaultFontPixelWidth * 2

    signal activated(int index)

    QUAVLabel {
        id:                 label
        Layout.fillWidth:   true
    }

    QUAVComboBox {
        id:                     m_comboBox
        Layout.preferredWidth:  comboBoxPreferredWidth
        sizeToContents:         true
        onActivated: (index) => { parent.activated(index) }
    }
}


