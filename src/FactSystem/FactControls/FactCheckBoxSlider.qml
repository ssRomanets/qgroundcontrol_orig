import QtQuick
import QtQuick.Controls

import QUAV.FactSystem
import QUAV.Palette
import QUAV.Controls

QUAVCheckBoxSlider {
    property Fact fact: Fact{}

    property var checkedValue:   fact.typeIsBool ? true : 1
    property var uncheckedValue: fact.typeIsBool ? false: 0

    Binding on checked { value: fact ? (fact.value === uncheckedValue ? Qt.Unchecked : Qt.Checked) : Qt.Unchecked }

    onClicked : fact.value = (checked ? checkedValue : uncheckedValue)
}

