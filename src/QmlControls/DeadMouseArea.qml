import QtQuick
import QtQuick.Controls

MouseArea {
    preventStealing: true
    hoverEnabled:    true
    onWheel: (wheel) => {wheel.accepted = true;}
    onPressed: (wheel) => {wheel.accepted = true;}
    onReleased: (wheel) => {wheel.accepted = true;}
}
