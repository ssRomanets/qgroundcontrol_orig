import QtQuick
import QtQuick.Layouts

import QUAV.Controls
import QUAV.ScreenTools
import QUAV.FactSystem
import QUAV.FactControls

RowLayout {
    property alias  label:                  label.text
    property alias  fact:                   m_comboBox.fact
    property alias  indexModel:             m_comboBox.indexModel
    property var    combobox:               m_comboBox
    property real   comboboxPreferredWidth: -1

    spacing: ScreenTools.defaultFontPixelWidth*2

    signal activated(int index)

    QUAVLabel {
        id:                 label
        Layout.fillWidth:   true
    }

    FactComboBox {
        id: m_comboBox
        Layout.preferredWidth: comboboxPreferredWidth
        sizeToContents: true

        onActivated: (index) => {parent.activated(index)}
    }
}
