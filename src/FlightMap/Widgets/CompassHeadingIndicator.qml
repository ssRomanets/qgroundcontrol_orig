import QtQuick

import QUAV
import QUAV.Controls
import QUAV.ScreenTools
import QUAV.Palette

Canvas {
    id:                 control
    anchors.centerIn:   parent
    width:              compassSize*1/3
    height:             width

    property real compassSize
    property real heading
    property bool simplified: false

    property var m_quavPal: QUAV.globalPalette

    Connections {
        target: m_quavPal
        function onGlobalThemeChanged() { control.requestPaint()}
    }

    onPaint: {
        var ctx = getContext("2d")
        ctx.strokeStyle = simplified ? "#EE3424" : m_quavPal.text
        ctx.fillStyle = "#EE3424"
        ctx.lineWidth = 1
        ctx.beginPath()
        ctx.moveTo(width / 2, 0)
        ctx.lineTo(width, height)
        ctx.lineTo(width / 2, height * 0.75)
        ctx.lineTo(width / 2, 0)
        ctx.fill()
        ctx.stroke()
        ctx.fillStyle = "#C72B27"
        ctx.beginPath()
        ctx.moveTo(width / 2, 0)
        ctx.lineTo(0, height)
        ctx.lineTo(width / 2, height * 0.75)
        ctx.lineTo(width / 2, 0)
        ctx.fill()
        ctx.stroke()
    }

    transform: Rotation {
        origin.x: control.width/2
        origin.y: control.height/2
        angle: heading
    }
}
