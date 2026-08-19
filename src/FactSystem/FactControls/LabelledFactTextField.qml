import QtQuick
import QtQuick.Layouts

import QUAV.Controls
import QUAV.ScreenTools
import QUAV.FactSystem
import QUAV.FactControls

RowLayout {
    property string label:                  fact.shortDescription
    property alias fact:                    m_factTextField.fact
    property real textFieldPreferredWidth:  -1

    spacing: ScreenTools.defaultFontPixelWidth*2

    QUAVLabel {
        Layout.fillWidth:   true
        text:               label
    }

    FactTextField {
        id:                    m_factTextField
        Layout.preferredWidth: textFieldPreferredWidth
    }
}
