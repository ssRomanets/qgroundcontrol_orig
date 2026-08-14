import QtQuick
import QtQuick.Controls

import QUAV.Controls
import QUAV.Palette

/// Text control used for displaying text of Maps
QUAVLabel {
    property var map

    QUAVMapPalette { id: mapPal; lightColors: map.isSatelliteMap }

    color:      mapPal.text
    style:      Text.Outline
    styleColor: mapPal.textOutline
}