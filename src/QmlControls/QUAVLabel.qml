import QtQuick
import QtQuick.Controls

import QUAV.Palette
import QUAV.ScreenTools

Text {
    font.pointSize: ScreenTools.defaultFontPointSize
    font.family:    ScreenTools.normalFontFamily
    color:          quavPal.text
    antialiasing:   true

    QUAVPalette { id: quavPal; colorGroupEnabled: enabled }
}
